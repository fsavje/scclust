/* ==============================================================================
 * scclust -- A C library for size constrained clustering
 * https://github.com/fsavje/scclust
 *
 * Copyright (C) 2015-2016  Fredrik Savje -- http://fredriksavje.com
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see http://www.gnu.org/licenses/
 * ============================================================================== */

#include "nng_core.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../include/scclust.h"
#include "digraph_core.h"
#include "digraph_operations.h"
#include "dist_search.h"
#include "error.h"
#include "nng_findseeds.h"
#include "scclust_int.h"


// ==============================================================================
// Internal structs
// ==============================================================================

typedef struct iscc_TypeCount iscc_TypeCount;
struct iscc_TypeCount {
	size_t num_queries;
	uint32_t sum_type_constraints;
	size_t* type_group_size;
	iscc_Dpid* point_store;
	iscc_Dpid** type_groups;
};


// ==============================================================================
// Internal function prototypes
// ==============================================================================

static scc_ErrorCode iscc_make_nng(void* data_set_object,
                                   size_t len_search_indices,
                                   const iscc_Dpid search_indices[],
                                   size_t len_query_indicators,
                                   const bool query_indicators[],
                                   bool out_query_indicators[],
                                   uint32_t k,
                                   bool radius_search,
                                   double radius,
                                   bool accept_partial,
                                   uintmax_t max_arcs,
                                   iscc_Digraph* out_nng);

static scc_ErrorCode iscc_make_nng_from_search_object(iscc_NNSearchObject* nn_search_object,
                                                      size_t len_query_indicators,
                                                      const bool query_indicators[],
                                                      bool out_query_indicators[],
                                                      uint32_t k,
                                                      bool radius_search,
                                                      double radius,
                                                      bool accept_partial,
                                                      uintmax_t max_arcs,
                                                      iscc_Digraph* out_nng);

static inline void iscc_ensure_self_match(iscc_Digraph* nng,
                                          size_t len_search_indices,
                                          const iscc_Dpid search_indices[]);

static scc_ErrorCode iscc_type_count(size_t num_data_points,
                                     uint32_t size_constraint,
                                     uint_fast16_t num_types,
                                     const uint32_t type_size_constraints[static num_types],
                                     const scc_TypeLabel type_labels[static num_data_points],
                                     const bool main_data_points[],
                                     iscc_TypeCount* out_type_result);

static scc_ErrorCode iscc_assign_seeds_and_neighbors(scc_Clustering* clustering,
                                                     const iscc_SeedResult* seed_result,
                                                     iscc_Digraph* nng);

scc_ErrorCode iscc_estimate_avg_seed_dist(void* data_set_object,
                                          const iscc_SeedResult* seed_result,
                                          const iscc_Digraph* nng,
                                          uint32_t size_constraint,
                                          double* out_avg_seed_dist);

static size_t iscc_assign_by_nng(scc_Clustering* clustering,
                                 iscc_Digraph* nng,
                                 bool scratch[restrict static clustering->num_data_points]);

static scc_ErrorCode iscc_assign_by_nn_search(scc_Clustering* clustering,
                                              iscc_NNSearchObject* nn_search_object,
                                              size_t num_to_assign,
                                              bool to_assign[restrict static clustering->num_data_points],
                                              bool radius_constraint,
                                              double radius);


// ==============================================================================
// External function implementations
// ==============================================================================

scc_ErrorCode iscc_get_nng_with_size_constraint(void* const data_set_object,
                                                const size_t num_data_points,
                                                const uint32_t size_constraint,
                                                const bool main_data_points[const],
                                                const bool radius_constraint,
                                                const double radius,
                                                iscc_Digraph* const out_nng)
{
	assert(iscc_check_data_set_object(data_set_object, num_data_points));
	assert(num_data_points > 0);
	assert(size_constraint <= num_data_points);
	assert(size_constraint > 0);
	assert(!radius_constraint || (radius > 0.0));
	assert(out_nng != NULL);

	size_t num_queries;
	if (main_data_points == NULL) {
		num_queries = num_data_points;
	} else {
		num_queries = 0;
		for (size_t i = 0; i < num_data_points; ++i) {
			num_queries += main_data_points[i];
		}
	}

	scc_ErrorCode ec;
	if ((ec = iscc_make_nng(data_set_object,
	                        num_data_points,
	                        NULL,
	                        num_data_points,
	                        main_data_points,
	                        NULL,
	                        size_constraint,
	                        radius_constraint,
	                        radius,
	                        false,
	                        (size_constraint * num_queries),
	                        out_nng)) != SCC_ER_OK) {
		return ec;
	}

	iscc_ensure_self_match(out_nng, num_data_points, NULL);

	if ((ec = iscc_delete_loops(out_nng)) != SCC_ER_OK) {
		iscc_free_digraph(out_nng);
		return ec;
	}

	return iscc_no_error();
}


scc_ErrorCode iscc_get_nng_with_type_constraint(void* const data_set_object,
                                                const size_t num_data_points,
                                                const uint32_t size_constraint,
                                                const uint_fast16_t num_types,
                                                const uint32_t type_size_constraints[const static num_types],
                                                const scc_TypeLabel type_labels[const static num_data_points],
                                                const bool main_data_points[const],
                                                const bool radius_constraint,
                                                const double radius,
                                                iscc_Digraph* const out_nng)
{
	assert(iscc_check_data_set_object(data_set_object, num_data_points));
	assert(num_data_points > 0);
	assert(size_constraint <= num_data_points);
	assert(size_constraint > 0);
	assert(num_types >= 2);
	assert(num_types <= ISCC_TYPELABEL_MAX);
	assert(type_size_constraints != NULL);
	assert(type_labels != NULL);
	assert(!radius_constraint || (radius > 0.0));
	assert(out_nng != NULL);

	*out_nng = ISCC_NULL_DIGRAPH; // So `out_nng` can be freed without init below.

	bool* seedable;
	const bool* seedable_const;

	if (radius_constraint) {
		seedable = malloc(sizeof(bool[num_data_points]));
		if (seedable == NULL) return iscc_make_error(SCC_ER_NO_MEMORY);
		seedable_const = seedable;
		if (main_data_points == NULL) {
			for (size_t i = 0; i < num_data_points; ++i) {
				seedable[i] = true;
			}
		} else {
			memcpy(seedable, main_data_points, sizeof(bool[num_data_points]));
		}
	} else {
		seedable = NULL;
		seedable_const = main_data_points;
	}

	iscc_Digraph* const nng_by_type = malloc(sizeof(iscc_Digraph[num_types]));
	if (nng_by_type == NULL) {
		free(seedable);
		return iscc_make_error(SCC_ER_NO_MEMORY);
	}

	scc_ErrorCode ec;
	iscc_TypeCount tc;
	if ((ec = iscc_type_count(num_data_points,
	                          size_constraint,
	                          num_types,
	                          type_size_constraints,
	                          type_labels,
	                          main_data_points,
	                          &tc)) != SCC_ER_OK) {
		free(seedable);
		free(nng_by_type);
		return ec;
	}

	uint_fast16_t num_non_zero_type_constraints = 0;
	for (uint_fast16_t i = 0; i < num_types; ++i) {
		if (type_size_constraints[i] > 0) {
			if ((ec = iscc_make_nng(data_set_object,
			                        tc.type_group_size[i],
			                        tc.type_groups[i],
			                        num_data_points,
			                        seedable_const,
			                        seedable,
			                        type_size_constraints[i],
			                        radius_constraint,
			                        radius,
			                        false,
			                        (type_size_constraints[i] * tc.num_queries),
			                        &nng_by_type[num_non_zero_type_constraints])) != SCC_ER_OK) {
				break;
			}
			iscc_ensure_self_match(&nng_by_type[num_non_zero_type_constraints],
			                       tc.type_group_size[i],
			                       tc.type_groups[i]);
			++num_non_zero_type_constraints;
		}
	}

	free(tc.type_group_size);
	free(tc.point_store);
	free(tc.type_groups);

	if (ec == SCC_ER_OK) {
		ec = iscc_digraph_union_and_delete(num_non_zero_type_constraints, nng_by_type, seedable_const, out_nng);
	}

	for (uint_fast16_t i = 0; i < num_non_zero_type_constraints; ++i) {
		iscc_free_digraph(&nng_by_type[i]);
	}
	free(nng_by_type);

	if (ec != SCC_ER_OK) {
		free(seedable);
		iscc_free_digraph(out_nng);
		return ec;
	}

	if (size_constraint > tc.sum_type_constraints) {
		uint32_t additional_nn_needed = size_constraint - tc.sum_type_constraints;
		iscc_Digraph nng_sum[2];
		nng_sum[0] = *out_nng;
		*out_nng = ISCC_NULL_DIGRAPH;

		if ((ec = iscc_make_nng(data_set_object,
		                        num_data_points,
		                        NULL,
		                        num_data_points,
		                        seedable_const,
		                        seedable,
		                        size_constraint,
		                        radius_constraint,
		                        radius,
		                        false,
		                        (size_constraint * tc.num_queries),
		                        &nng_sum[1])) != SCC_ER_OK) {
			free(seedable);
			iscc_free_digraph(&nng_sum[0]);
			return ec;
		}

		ec = iscc_digraph_difference(&nng_sum[1], &nng_sum[0], additional_nn_needed);

		if (ec == SCC_ER_OK) {
			ec = iscc_digraph_union_and_delete(2, nng_sum, seedable_const, out_nng);
		}

		iscc_free_digraph(&nng_sum[0]);
		iscc_free_digraph(&nng_sum[1]);

		if (ec != SCC_ER_OK) {
			free(seedable);
			return ec;
		}
	}

	free(seedable);

	return iscc_no_error();
}


scc_ErrorCode iscc_make_nng_clusters_from_seeds(scc_Clustering* const clustering,
                                                void* const data_set_object,
                                                const iscc_SeedResult* const seed_result,
                                                iscc_Digraph* const nng,
                                                const bool nng_is_ordered,
                                                const uint32_t size_constraint,
                                                scc_UnassignedMethod main_unassigned_method,
                                                bool main_radius_constraint,
                                                double main_radius,
                                                const bool main_data_points[const],
                                                scc_UnassignedMethod secondary_unassigned_method,
                                                bool secondary_radius_constraint,
                                                double secondary_radius)
{
	assert(iscc_check_input_clustering(clustering));
	assert(iscc_check_data_set_object(data_set_object, clustering->num_data_points));
	assert(seed_result->count > 0);
	assert(seed_result->seeds != NULL);
	assert(iscc_digraph_is_initialized(nng));
	assert(size_constraint > 0);
	assert((seed_result->count * size_constraint) <= clustering->num_data_points);
	assert((main_unassigned_method == SCC_UM_IGNORE) ||
	       (main_unassigned_method == SCC_UM_ASSIGN_BY_NNG) ||
	       (main_unassigned_method == SCC_UM_CLOSEST_ASSIGNED) ||
	       (main_unassigned_method == SCC_UM_CLOSEST_SEED) ||
	       (main_unassigned_method == SCC_UM_CLOSEST_SEED_EST_RADIUS));
	assert(!main_radius_constraint || (main_radius > 0.0));
	assert((secondary_unassigned_method == SCC_UM_IGNORE) ||
	       (secondary_unassigned_method == SCC_UM_CLOSEST_ASSIGNED) ||
	       (secondary_unassigned_method == SCC_UM_CLOSEST_SEED) ||
	       (secondary_unassigned_method == SCC_UM_CLOSEST_SEED_EST_RADIUS));
	assert(!secondary_radius_constraint || (secondary_radius > 0.0));

	if (main_data_points == NULL) secondary_unassigned_method = SCC_UM_IGNORE;

	const size_t num_assigned_as_seed_or_neighbor = seed_result->count * size_constraint;

	// Assign seeds and their neighbors
	scc_ErrorCode ec;
	if ((ec = iscc_assign_seeds_and_neighbors(clustering, seed_result, nng)) != SCC_ER_OK) {
		return ec;
	}

	// Are we done?
	if ((num_assigned_as_seed_or_neighbor == clustering->num_data_points) ||
	        ((main_unassigned_method == SCC_UM_IGNORE) && (secondary_unassigned_method == SCC_UM_IGNORE))) {
		return iscc_no_error();
	}

	// Estimate assign radius if we need to, and modify options
	if ((main_unassigned_method == SCC_UM_CLOSEST_SEED_EST_RADIUS) || 
	        (secondary_unassigned_method == SCC_UM_CLOSEST_SEED_EST_RADIUS)) {
		double avg_seed_dist;
		if ((ec = iscc_estimate_avg_seed_dist(data_set_object,
		                                      seed_result,
		                                      nng,
		                                      size_constraint,
		                                      &avg_seed_dist)) != SCC_ER_OK) {
			return ec;
		}

		if (main_unassigned_method == SCC_UM_CLOSEST_SEED_EST_RADIUS) {
			main_unassigned_method = SCC_UM_CLOSEST_SEED;
			main_radius_constraint = true;
			main_radius = avg_seed_dist;
		}
		
		if (secondary_unassigned_method == SCC_UM_CLOSEST_SEED_EST_RADIUS) {
			secondary_unassigned_method = SCC_UM_CLOSEST_SEED;
			secondary_radius_constraint = true;
			secondary_radius = avg_seed_dist;
		}
	}

	// If we are running assign by closest assigned, construct array with seeds and their neighbors for the nn search object
	iscc_Dpid* seed_or_neighbor = NULL;
	if ((main_unassigned_method == SCC_UM_CLOSEST_ASSIGNED) ||
	        (secondary_unassigned_method == SCC_UM_CLOSEST_ASSIGNED)) {
		size_t search_count = 0;
		seed_or_neighbor = malloc(sizeof(iscc_Dpid[num_assigned_as_seed_or_neighbor]));
		if (seed_or_neighbor == NULL) return iscc_make_error(SCC_ER_NO_MEMORY);
		
		assert(clustering->num_data_points <= ISCC_DPID_MAX);
		const iscc_Dpid num_data_points_dpid = (iscc_Dpid) clustering->num_data_points; // If `iscc_Dpid` is signed.
		for (iscc_Dpid i = 0; i < num_data_points_dpid; ++i) {
			if (clustering->cluster_label[i] != SCC_CLABEL_NA) {
				seed_or_neighbor[search_count] = i;
				++search_count;
			}
		}
		assert(search_count == num_assigned_as_seed_or_neighbor);
	}

	// Indicators of main data points that are unassigned (first used as scratch)
	bool* main_assign = malloc(sizeof(bool[clustering->num_data_points]));
	if (main_assign == NULL) {
		free(seed_or_neighbor);
		return iscc_make_error(SCC_ER_NO_MEMORY);
	}

	// Run assignment by nng. When nng is ordered, we can use it for `SCC_UM_CLOSEST_ASSIGNED` as well.
	size_t num_assigned_by_nng = 0;
	if ((main_unassigned_method == SCC_UM_ASSIGN_BY_NNG) || (nng_is_ordered && (main_unassigned_method == SCC_UM_CLOSEST_ASSIGNED))) {
		num_assigned_by_nng = iscc_assign_by_nng(clustering, nng, main_assign); // Use `main_assign` as scratch
	
		// Are we done?
		if ((num_assigned_as_seed_or_neighbor + num_assigned_by_nng == clustering->num_data_points) ||
	            ((main_unassigned_method == SCC_UM_ASSIGN_BY_NNG) && (secondary_unassigned_method == SCC_UM_IGNORE))) {
			free(seed_or_neighbor);
			free(main_assign);
			return iscc_no_error();
		}
	}

	// No need for nng any more
	iscc_free_digraph(nng);

	// Derive which data points to assign
	bool* secondary_assign = NULL;
	size_t num_main_assign = 0;
	size_t num_secondary_assign = 0;
	if (main_data_points == NULL) {
		assert(secondary_unassigned_method == SCC_UM_IGNORE);
		#ifndef NDEBUG
			size_t dbg_main_count = 0;
		#endif
		for (size_t i = 0; i < clustering->num_data_points; ++i) {
			main_assign[i] = (clustering->cluster_label[i] == SCC_CLABEL_NA);
			#ifndef NDEBUG
				dbg_main_count += main_assign[i];
			#endif
		}
		assert(num_assigned_as_seed_or_neighbor + num_assigned_by_nng + dbg_main_count == clustering->num_data_points);
		num_main_assign = clustering->num_data_points - num_assigned_as_seed_or_neighbor - num_assigned_by_nng;

	} else if (secondary_unassigned_method == SCC_UM_IGNORE) {
		#ifndef NDEBUG
			size_t dbg_secondary_count = 0;
		#endif
		for (size_t i = 0; i < clustering->num_data_points; ++i) {
			main_assign[i] = main_data_points[i] && (clustering->cluster_label[i] == SCC_CLABEL_NA);
			num_main_assign += main_assign[i];
			#ifndef NDEBUG
				dbg_secondary_count += !main_data_points[i] && (clustering->cluster_label[i] != SCC_CLABEL_NA);
			#endif
		}
		assert(num_assigned_as_seed_or_neighbor + num_assigned_by_nng + num_main_assign + dbg_secondary_count == clustering->num_data_points);

	} else {
		secondary_assign = malloc(sizeof(bool[clustering->num_data_points]));
		if (secondary_assign == NULL) {
			free(seed_or_neighbor);
			free(main_assign);
			return iscc_make_error(SCC_ER_NO_MEMORY);
		}
		for (size_t i = 0; i < clustering->num_data_points; ++i) {
			main_assign[i] = main_data_points[i] && (clustering->cluster_label[i] == SCC_CLABEL_NA);
			num_main_assign += main_assign[i];
			secondary_assign[i] = !main_data_points[i] && (clustering->cluster_label[i] == SCC_CLABEL_NA);
			num_secondary_assign += secondary_assign[i];
		}
		assert(num_assigned_as_seed_or_neighbor + num_assigned_by_nng + num_main_assign + num_secondary_assign == clustering->num_data_points);
	}

	if ((num_main_assign == 0) || (main_unassigned_method == SCC_UM_IGNORE)) {
		assert(main_assign != NULL);
		free(main_assign);
		main_assign = NULL;
	}

	if ((num_secondary_assign == 0) && (secondary_unassigned_method != SCC_UM_IGNORE)) {
		// If `secondary_unassigned_method == SCC_UM_IGNORE`, then `secondary_assign` should already be NULL
		assert(secondary_assign != NULL);
		free(secondary_assign);
		secondary_assign = NULL;
	}

	assert(((num_main_assign == 0) && (main_assign == NULL)) ||
	       ((num_main_assign > 0) && (main_assign != NULL)));
	assert(((num_secondary_assign == 0) && (secondary_assign == NULL)) ||
	       ((num_secondary_assign > 0) && (secondary_assign != NULL)));

	// Run assign to closest assigned
	if (((main_unassigned_method == SCC_UM_CLOSEST_ASSIGNED) && (num_main_assign > 0)) ||
	        ((secondary_unassigned_method == SCC_UM_CLOSEST_ASSIGNED) && (num_secondary_assign > 0))) {
		
		assert(seed_or_neighbor != NULL);
		iscc_NNSearchObject* nn_search_object;
		if (!iscc_init_nn_search_object(data_set_object,
		                                num_assigned_as_seed_or_neighbor,
		                                seed_or_neighbor,
		                                &nn_search_object)) {
			ec = iscc_make_error(SCC_ER_DIST_SEARCH_ERROR);
		}

		if ((ec == SCC_ER_OK) && (main_unassigned_method == SCC_UM_CLOSEST_ASSIGNED) && (num_main_assign > 0)) {
			assert(main_assign != NULL);
			ec = iscc_assign_by_nn_search(clustering,
			                              nn_search_object,
			                              num_main_assign,
			                              main_assign,
			                              main_radius_constraint,
			                              main_radius);
			free(main_assign);
			main_assign = NULL;
		}

		if ((ec == SCC_ER_OK) && (secondary_unassigned_method == SCC_UM_CLOSEST_ASSIGNED) && (num_secondary_assign > 0)) {
			assert(secondary_assign != NULL);
			ec = iscc_assign_by_nn_search(clustering,
			                              nn_search_object,
			                              num_secondary_assign,
			                              secondary_assign,
			                              secondary_radius_constraint,
			                              secondary_radius);
			free(secondary_assign);
			secondary_assign = NULL;
		}

		if (!iscc_close_nn_search_object(&nn_search_object)) {
			if (ec == SCC_ER_OK) ec = iscc_make_error(SCC_ER_DIST_SEARCH_ERROR);
		}

		if (ec != SCC_ER_OK) {
			free(seed_or_neighbor);
			free(main_assign);
			free(secondary_assign);
			return ec;
		}
	}

	free(seed_or_neighbor);
	seed_or_neighbor = NULL;

	// Run assign to closest seed
	if (((main_unassigned_method == SCC_UM_CLOSEST_SEED) && (num_main_assign > 0)) ||
	        ((secondary_unassigned_method == SCC_UM_CLOSEST_SEED) && (num_secondary_assign > 0))) {

		iscc_NNSearchObject* nn_search_object;
		if (!iscc_init_nn_search_object(data_set_object,
		                                seed_result->count,
		                                seed_result->seeds,
		                                &nn_search_object)) {
			ec = iscc_make_error(SCC_ER_DIST_SEARCH_ERROR);
		}

		if ((ec == SCC_ER_OK) && (main_unassigned_method == SCC_UM_CLOSEST_SEED) && (num_main_assign > 0)) {
			assert(main_assign != NULL);
			ec = iscc_assign_by_nn_search(clustering,
			                              nn_search_object,
			                              num_main_assign,
			                              main_assign,
			                              main_radius_constraint,
			                              main_radius);
			free(main_assign);
			main_assign = NULL;
		}

		if ((ec == SCC_ER_OK) && (secondary_unassigned_method == SCC_UM_CLOSEST_SEED) && (num_secondary_assign > 0)) {
			assert(secondary_assign != NULL);
			ec = iscc_assign_by_nn_search(clustering,
			                              nn_search_object,
			                              num_secondary_assign,
			                              secondary_assign,
			                              secondary_radius_constraint,
			                              secondary_radius);
			free(secondary_assign);
			secondary_assign = NULL;
		}

		if (!iscc_close_nn_search_object(&nn_search_object)) {
			if (ec == SCC_ER_OK) ec = iscc_make_error(SCC_ER_DIST_SEARCH_ERROR);
		}

		if (ec != SCC_ER_OK) {
			free(main_assign);
			free(secondary_assign);
			return ec;
		}
	}

	assert(main_assign == NULL);
	assert(secondary_assign == NULL);

	return iscc_no_error();
}


// ==============================================================================
// Internal function implementations 
// ==============================================================================

static scc_ErrorCode iscc_make_nng(void* const data_set_object,
                                   const size_t len_search_indices,
                                   const iscc_Dpid search_indices[const],
                                   const size_t len_query_indicators,
                                   const bool query_indicators[const],
                                   bool out_query_indicators[const],
                                   const uint32_t k,
                                   const bool radius_search,
                                   const double radius,
                                   const bool accept_partial,
                                   const uintmax_t max_arcs,
                                   iscc_Digraph* const out_nng)
{
	assert(iscc_check_data_set_object(data_set_object, len_query_indicators));
	assert(len_search_indices > 0);
	assert(len_query_indicators > 0);
	assert(k > 0);
	assert(!radius_search || (radius > 0.0));
	assert(max_arcs >= k);
	assert(out_nng != NULL);

	iscc_NNSearchObject* nn_search_object;
	if (!iscc_init_nn_search_object(data_set_object,
	                                len_search_indices,
	                                search_indices,
	                                &nn_search_object)) {
		return iscc_make_error(SCC_ER_DIST_SEARCH_ERROR);
	}

	scc_ErrorCode ec;
	if ((ec = iscc_make_nng_from_search_object(nn_search_object,
                                               len_query_indicators,
                                               query_indicators,
                                               out_query_indicators,
                                               k,
                                               radius_search,
                                               radius,
                                               accept_partial,
                                               max_arcs,
                                               out_nng)) != SCC_ER_OK) {
		iscc_close_nn_search_object(&nn_search_object);
		return ec;
	}

	if (!iscc_close_nn_search_object(&nn_search_object)) {
		iscc_free_digraph(out_nng);
		return iscc_make_error(SCC_ER_DIST_SEARCH_ERROR);
	}

	return iscc_no_error();
}


static scc_ErrorCode iscc_make_nng_from_search_object(iscc_NNSearchObject* const nn_search_object,
                                                      const size_t len_query_indicators,
                                                      const bool query_indicators[const],
                                                      bool out_query_indicators[const],
                                                      const uint32_t k,
                                                      const bool radius_search,
                                                      const double radius,
                                                      const bool accept_partial,
                                                      const uintmax_t max_arcs,
                                                      iscc_Digraph* const out_nng)
{
	assert(nn_search_object != NULL);
	assert(len_query_indicators > 0);
	assert(k > 0);
	assert(!radius_search || (radius > 0.0));
	assert(max_arcs >= k);
	assert(out_nng != NULL);

	scc_ErrorCode ec;
	if ((ec = iscc_init_digraph(len_query_indicators, max_arcs, out_nng)) != SCC_ER_OK) return ec;

	if (!iscc_nearest_neighbor_search(nn_search_object,
	                                  len_query_indicators,
	                                  query_indicators,
	                                  out_query_indicators,
	                                  k,
	                                  radius_search,
	                                  radius,
	                                  accept_partial,
	                                  out_nng->tail_ptr,
	                                  out_nng->head)) {
		iscc_free_digraph(out_nng);
		return iscc_make_error(SCC_ER_DIST_SEARCH_ERROR);
	}

	if (max_arcs > out_nng->tail_ptr[out_nng->vertices]) {
		if ((ec = iscc_change_arc_storage(out_nng, out_nng->tail_ptr[out_nng->vertices])) != SCC_ER_OK) {
			iscc_free_digraph(out_nng);
			return ec;
		}
	}

	return iscc_no_error();
}


static inline void iscc_ensure_self_match(iscc_Digraph* const nng,
                                          const size_t len_search_indices,
                                          const iscc_Dpid search_indices[const])
{
	assert(iscc_digraph_is_initialized(nng));
	assert(len_search_indices > 0);

	/* When there's identical data points, `iscc_nearest_neighbor_search` may not
	 * return a self-loop when a query is a search point. The NNG clustering functions
	 * require this. However, if all data points are unique, or the query and search sets
	 * are disjoint, it's safe to call `iscc_make_nng` without `iscc_ensure_self_match`. */

	if (search_indices == NULL) {
		assert(len_search_indices <= ISCC_DPID_MAX);
		const iscc_Dpid len_search_indices_dpid = (iscc_Dpid) len_search_indices; // If `iscc_Dpid` is signed.
		for (iscc_Dpid search_point = 0; search_point < len_search_indices_dpid; ++search_point) {
			iscc_Dpid* v_arc = nng->head + nng->tail_ptr[search_point];
			const iscc_Dpid* const v_arc_stop = nng->head + nng->tail_ptr[search_point + 1];
			if ((*v_arc == search_point) || (v_arc == v_arc_stop)) continue;
			for (++v_arc; (*v_arc != search_point) && (v_arc != v_arc_stop); ++v_arc);
			if (v_arc == v_arc_stop) *(v_arc - 1) = search_point;
		}

	} else if (search_indices != NULL) {
		for (size_t s = 0; s < len_search_indices; ++s) {
			const iscc_Dpid search_point = search_indices[s];
			iscc_Dpid* v_arc = nng->head + nng->tail_ptr[search_point];
			const iscc_Dpid* const v_arc_stop = nng->head + nng->tail_ptr[search_point + 1];
			if ((*v_arc == search_point) || (v_arc == v_arc_stop)) continue;
			for (++v_arc; (*v_arc != search_point) && (v_arc != v_arc_stop); ++v_arc);
			if (v_arc == v_arc_stop) *(v_arc - 1) = search_point;
		}
	}
}


static scc_ErrorCode iscc_type_count(const size_t num_data_points,
                                     const uint32_t size_constraint,
                                     const uint_fast16_t num_types,
                                     const uint32_t type_size_constraints[const static num_types],
                                     const scc_TypeLabel type_labels[const static num_data_points],
                                     const bool main_data_points[const],
                                     iscc_TypeCount* const out_type_result)
{
	assert(num_data_points > 1);
	assert(size_constraint > 0);
	assert(num_types >= 2);
	assert(num_types <= ISCC_TYPELABEL_MAX);
	assert(type_size_constraints != NULL);
	assert(type_labels != NULL);
	assert(out_type_result != NULL);

	*out_type_result = (iscc_TypeCount) {
		.num_queries = 0,
		.sum_type_constraints = 0,
		.type_group_size = calloc(num_types, sizeof(size_t)),
		.point_store = malloc(sizeof(iscc_Dpid[num_data_points])),
		.type_groups = malloc(sizeof(iscc_Dpid*[num_types])),
	};

	if ((out_type_result->type_group_size == NULL) || (out_type_result->point_store == NULL) || (out_type_result->type_groups == NULL)) {
		free(out_type_result->type_group_size);
		free(out_type_result->point_store);
		free(out_type_result->type_groups);
		return iscc_make_error(SCC_ER_NO_MEMORY);
	}

	if (main_data_points == NULL) {
		out_type_result->num_queries = num_data_points;
		for (size_t i = 0; i < num_data_points; ++i) {
			++out_type_result->type_group_size[type_labels[i]];
		}
	} else {
		for (size_t i = 0; i < num_data_points; ++i) {
			out_type_result->num_queries += main_data_points[i];
			++out_type_result->type_group_size[type_labels[i]];
		}
	}

	for (uint_fast16_t i = 0; i < num_types; ++i) {
		if (out_type_result->type_group_size[i] < type_size_constraints[i]) {
			free(out_type_result->type_group_size);
			free(out_type_result->point_store);
			free(out_type_result->type_groups);
			return iscc_make_error(SCC_ER_INVALID_INPUT);
		}
		out_type_result->sum_type_constraints += type_size_constraints[i];
	}

	if (out_type_result->sum_type_constraints > size_constraint) {
		free(out_type_result->type_group_size);
		free(out_type_result->point_store);
		free(out_type_result->type_groups);
		return iscc_make_error(SCC_ER_INVALID_INPUT);
	}

	out_type_result->type_groups[0] = out_type_result->point_store + out_type_result->type_group_size[0];
	for (uint_fast16_t i = 1; i < num_types; ++i) {
		out_type_result->type_groups[i] = out_type_result->type_groups[i - 1] + out_type_result->type_group_size[i];
	}

	assert(num_data_points <= ISCC_DPID_MAX);
	const iscc_Dpid num_data_points_dpid = (iscc_Dpid) num_data_points; // if case `iscc_Dpid` is signed.
	for (iscc_Dpid i = 0; i < num_data_points_dpid; ++i) {
		--(out_type_result->type_groups[type_labels[i]]);
		*(out_type_result->type_groups[type_labels[i]]) = i;
	}

	return iscc_no_error();
}


static scc_ErrorCode iscc_assign_seeds_and_neighbors(scc_Clustering* const clustering,
                                                     const iscc_SeedResult* const seed_result,
                                                     iscc_Digraph* const nng)
{
	assert(iscc_check_input_clustering(clustering));
	assert(seed_result->count > 0);
	assert(seed_result->seeds != NULL);
	assert(iscc_digraph_is_initialized(nng));

	// Initialize clustering
	if (clustering->cluster_label == NULL) {
		clustering->external_labels = false;
		clustering->cluster_label = malloc(sizeof(scc_Clabel[clustering->num_data_points]));
		if (clustering->cluster_label == NULL) return iscc_make_error(SCC_ER_NO_MEMORY);
	}

	clustering->num_clusters = seed_result->count;

	for (size_t i = 0; i < clustering->num_data_points; ++i) {
		clustering->cluster_label[i] = SCC_CLABEL_NA;
	}

	scc_Clabel clabel = 0;
	const iscc_Dpid* const seed_stop = seed_result->seeds + seed_result->count;
	for (const iscc_Dpid* seed = seed_result->seeds;
	        seed != seed_stop; ++seed, ++clabel) {
		assert(clabel != SCC_CLABEL_NA);
		assert(clabel <= SCC_CLABEL_MAX);
		assert(clustering->cluster_label[*seed] == SCC_CLABEL_NA);

		const iscc_Dpid* const s_arc_stop = nng->head + nng->tail_ptr[*seed + 1];
		for (const iscc_Dpid* s_arc = nng->head + nng->tail_ptr[*seed];
		        s_arc != s_arc_stop; ++s_arc) {
			assert(clustering->cluster_label[*s_arc] == SCC_CLABEL_NA);
			clustering->cluster_label[*s_arc] = clabel;
		}
		clustering->cluster_label[*seed] = clabel; // Assign seed last so `assert` work also in case of self-loops
	}

	assert(clabel == (scc_Clabel) clustering->num_clusters);

	return iscc_no_error();
}


scc_ErrorCode iscc_estimate_avg_seed_dist(void* const data_set_object,
                                          const iscc_SeedResult* const seed_result,
                                          const iscc_Digraph* const nng,
                                          const uint32_t size_constraint,
                                          double* const out_avg_seed_dist) {
	assert(iscc_check_data_set_object(data_set_object, nng->vertices));
	assert(seed_result->count > 0);
	assert(seed_result->seeds != NULL);
	assert(iscc_digraph_is_initialized(nng));
	assert(size_constraint > 0);

	const size_t to_sample = (seed_result->count > 1000) ? 1000 : seed_result->count;
	const size_t step = ((seed_result->count - 1) / to_sample) + 1;

	#ifndef NDEBUG
		size_t dbg_count_seeds = 0;
	#endif

	double sum_dist = 0.0;
	const uint32_t num_neighbors = size_constraint - 1;
	double* const output_dists = malloc(sizeof(double[num_neighbors]));
	for (size_t s = 0; s < seed_result->count; s += step) {
		assert((nng->tail_ptr[s] + num_neighbors) == nng->tail_ptr[s + 1]);
		if (!iscc_get_dist_rows(data_set_object,
		                        1,
		                        (seed_result->seeds + s),
		                        num_neighbors,
		                        nng->head + nng->tail_ptr[s],
		                        output_dists)) {
			free(output_dists);
			return iscc_make_error(SCC_ER_DIST_SEARCH_ERROR);
		}

		for (size_t d = 0; d < num_neighbors; ++d) {
			sum_dist += output_dists[d];
		}

		#ifndef NDEBUG
			++dbg_count_seeds;
		#endif
	}

	free(output_dists);
	assert(dbg_count_seeds == to_sample);

	*out_avg_seed_dist = sum_dist / ((double) to_sample);

	return iscc_no_error();
}


static size_t iscc_assign_by_nng(scc_Clustering* const clustering,
                                 iscc_Digraph* const nng,
                                 bool scratch[restrict const static clustering->num_data_points])
{
	assert(iscc_check_input_clustering(clustering));
	assert(iscc_digraph_is_initialized(nng));
	assert(scratch != NULL);

	for (size_t i = 0; i < clustering->num_data_points; ++i) {
		scratch[i] = (clustering->cluster_label[i] == SCC_CLABEL_NA);
	}

	size_t num_assigned_by_nng = 0;
	for (size_t i = 0; i < clustering->num_data_points; ++i) {
		if (scratch[i]) {
			const iscc_Dpid* const v_arc_stop = nng->head + nng->tail_ptr[i + 1];
			for (const iscc_Dpid* v_arc = nng->head + nng->tail_ptr[i];
			        v_arc != v_arc_stop; ++v_arc) {
				if (!scratch[*v_arc]) {
					assert(clustering->cluster_label[*v_arc] != SCC_CLABEL_NA);
					clustering->cluster_label[i] = clustering->cluster_label[*v_arc];
					++num_assigned_by_nng;
					break;
				}
			}
		}
	}

	return num_assigned_by_nng;
}


static scc_ErrorCode iscc_assign_by_nn_search(scc_Clustering* const clustering,
                                              iscc_NNSearchObject* const nn_search_object,
                                              const size_t num_to_assign,
                                              bool to_assign[restrict const static clustering->num_data_points],
                                              const bool radius_constraint,
                                              const double radius)
{
	assert(iscc_check_input_clustering(clustering));
	assert(nn_search_object != NULL);
	assert(num_to_assign > 0);
	assert(to_assign != NULL);
	assert(!radius_constraint || (radius > 0.0));

	scc_ErrorCode ec;
	iscc_Digraph priority_graph;
	if ((ec = iscc_make_nng_from_search_object(nn_search_object,
	                                           clustering->num_data_points,
	                                           to_assign,
	                                           to_assign,
	                                           1,
	                                           radius_constraint,
	                                           radius,
	                                           false,
	                                           num_to_assign,
	                                           &priority_graph)) != SCC_ER_OK) {
		return ec;
	}

	for (size_t i = 0; i < clustering->num_data_points; ++i) {
		if (to_assign[i]) {
			assert(priority_graph.tail_ptr[i] != priority_graph.tail_ptr[i + 1]);
			assert(!to_assign[priority_graph.head[priority_graph.tail_ptr[i]]]);
			assert(clustering->cluster_label[priority_graph.head[priority_graph.tail_ptr[i]]] != SCC_CLABEL_NA);
			clustering->cluster_label[i] = clustering->cluster_label[priority_graph.head[priority_graph.tail_ptr[i]]];
		}
	}

	iscc_free_digraph(&priority_graph);

	return iscc_no_error();
}
