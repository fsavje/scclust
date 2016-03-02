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
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * ============================================================================== */

#include "nng_core.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../include/scclust.h"
#include "clustering.h"
#include "digraph_core.h"
#include "digraph_operations.h"
#include "dist_search.h"
#include "error.h"
#include "nng_findseeds.h"


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

static scc_ErrorCode iscc_make_nng(scc_DataSetObject* data_set_object,
                                   size_t len_search_indices,
                                   const iscc_Dpid search_indices[],
                                   size_t len_query_indicators,
                                   const bool query_indicators[],
                                   bool out_query_indicators[],
                                   uint32_t k,
                                   bool radius_search,
                                   double radius,
                                   bool accept_partial,
                                   uint64_t max_arcs,
                                   iscc_Digraph* out_nng);

static scc_ErrorCode iscc_make_nng_from_search_object(iscc_NNSearchObject* nn_search_object,
                                                      size_t len_query_indicators,
                                                      const bool query_indicators[],
                                                      bool out_query_indicators[],
                                                      uint32_t k,
                                                      bool radius_search,
                                                      double radius,
                                                      bool accept_partial,
                                                      uint64_t max_arcs,
                                                      iscc_Digraph* out_nng);

static scc_ErrorCode iscc_type_count(size_t num_data_points,
                                     uint32_t size_constraint,
                                     uint_fast16_t num_types,
                                     const uint32_t type_size_constraints[static num_types],
                                     const scc_TypeLabel type_labels[static num_data_points],
                                     const bool main_data_points[],
                                     iscc_TypeCount* out_type_result);

static size_t iscc_assign_lexical(scc_Clustering* clustering,
                                  const iscc_Digraph* priority_graph,
                                  const bool unassigned[restrict static clustering->num_data_points]);

static void iscc_assign_to_first(scc_Clustering* clustering,
                                 const iscc_Digraph* priority_graph,
                                 const bool unassigned[restrict static clustering->num_data_points]);


// ==============================================================================
// External function implementations
// ==============================================================================

scc_ErrorCode iscc_get_nng_with_size_constraint(scc_DataSetObject* const data_set_object,
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
                            size_constraint * num_queries,
                            out_nng)) != SCC_ER_OK) {
		return ec;
	}

	if ((ec = iscc_delete_loops(out_nng)) != SCC_ER_OK) {
		iscc_free_digraph(out_nng);
		return ec;
	}

	return iscc_no_error();
}


scc_ErrorCode iscc_get_nng_with_type_constraint(scc_DataSetObject* const data_set_object,
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
	assert(num_types > 0);
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
                                    type_size_constraints[i] * tc.num_queries,
                                    &nng_by_type[num_non_zero_type_constraints])) != SCC_ER_OK) {
				break;
			}
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
                                size_constraint * tc.num_queries,
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


scc_ErrorCode iscc_assign_seed_neighbors(scc_Clustering* const clustering,
                                         const iscc_Digraph* const nng,
                                         const iscc_SeedResult* const sr,
                                         bool out_unassigned[restrict const static clustering->num_data_points])
{
	assert(iscc_check_input_clustering(clustering));
	assert(iscc_digraph_is_initialized(nng));
	assert(sr->count > 0);
	assert(sr->seeds != NULL);
	assert(out_unassigned != NULL);

	if (clustering->cluster_label == NULL) {
		clustering->external_labels = false;
		clustering->cluster_label = malloc(sizeof(scc_Clabel[clustering->num_data_points]));
		if (clustering->cluster_label == NULL) return iscc_make_error(SCC_ER_NO_MEMORY);
	}

	for (size_t i = 0; i < clustering->num_data_points; ++i) {
		out_unassigned[i] = true;
	}

	clustering->num_clusters = sr->count;

	scc_Clabel clabel = 0;
	const iscc_Dpid* const seed_stop = sr->seeds + sr->count;
	for (const iscc_Dpid* seed = sr->seeds;
	        seed != seed_stop; ++seed, ++clabel) {
		assert(clabel != SCC_CLABEL_NA);
		assert(clabel <= SCC_CLABEL_MAX);
		assert(out_unassigned[*seed]);

		const iscc_Dpid* const s_arc_stop = nng->head + nng->tail_ptr[*seed + 1];
		for (const iscc_Dpid* s_arc = nng->head + nng->tail_ptr[*seed];
		        s_arc != s_arc_stop; ++s_arc) {
			assert(out_unassigned[*s_arc]);
			out_unassigned[*s_arc] = false;
			clustering->cluster_label[*s_arc] = clabel;
		}
		out_unassigned[*seed] = false;
		clustering->cluster_label[*seed] = clabel;
	}

	assert((scc_Clabel) clustering->num_clusters == clabel);

	return iscc_no_error();
}


scc_ErrorCode iscc_remaining_ignore(scc_Clustering* const clustering,
                                    const bool unassigned[restrict const static clustering->num_data_points])
{
	assert(iscc_check_input_clustering(clustering));
	assert(unassigned != NULL);

	for (size_t v = 0; v < clustering->num_data_points; ++v) {
		if (unassigned[v]) clustering->cluster_label[v] = SCC_CLABEL_NA;
	}

	return iscc_no_error();
}


scc_ErrorCode iscc_remaining_by_nng(scc_Clustering* const clustering,
                                    iscc_Digraph* const nng,
                                    const bool unassigned[restrict const static clustering->num_data_points])
{
	assert(iscc_check_input_clustering(clustering));
	assert(iscc_digraph_is_initialized(nng));
	assert(unassigned != NULL);

	iscc_assign_lexical(clustering, nng, unassigned);

	return iscc_no_error();
}


scc_ErrorCode iscc_remaining_to_nearest_assigned(scc_Clustering* const clustering,
                                                 scc_DataSetObject* const data_set_object,
                                                 iscc_Digraph* const nng,
                                                 const size_t num_assigned,
                                                 bool unassigned[restrict const static clustering->num_data_points],
                                                 const bool main_data_points[restrict const],
                                                 const bool assign_secondary_points,
                                                 const bool main_radius_constraint,
                                                 const double main_radius,
                                                 const bool secondary_radius_constraint,
                                                 const double secondary_radius)
{
	assert(iscc_check_input_clustering(clustering));
	assert(iscc_check_data_set_object(data_set_object, clustering->num_data_points));
	assert((nng == NULL) || iscc_digraph_is_initialized(nng));
	assert(num_assigned > 0);
	assert(num_assigned < clustering->num_data_points);
	assert(unassigned != NULL);
	assert(!main_radius_constraint || (main_radius > 0.0));
	assert(!secondary_radius_constraint || (secondary_radius > 0.0));

	size_t assigned_by_nng = 0;
	if (nng != NULL) {
		assigned_by_nng = iscc_assign_lexical(clustering, nng, unassigned);
		iscc_free_digraph(nng);
		if (num_assigned + assigned_by_nng == clustering->num_data_points) {
			return iscc_no_error();
		}
	}

	size_t search_count = 0;
	iscc_Dpid* const assigned_search = malloc(sizeof(iscc_Dpid[num_assigned]));
	if (assigned_search == NULL) return iscc_make_error(SCC_ER_NO_MEMORY);
	assert(clustering->num_data_points <= ISCC_DPID_MAX);
	const iscc_Dpid num_data_points_dpid = (iscc_Dpid) clustering->num_data_points; // If `iscc_Dpid` is signed.
	for (iscc_Dpid i = 0; i < num_data_points_dpid; ++i) {
		if (!unassigned[i]) {
			assigned_search[search_count] = i;
			++search_count;
		}
	}
	assert(search_count == num_assigned);

	if (nng != NULL) {
		for (size_t i = 0; i < clustering->num_data_points; ++i) {
			unassigned[i] = (clustering->cluster_label[i] == SCC_CLABEL_NA);
		}
	}

	bool* first_assign;
	uint64_t num_first_assign = 0;
	bool* second_assign = NULL;
	uint64_t num_second_assign = 0;

	if (main_data_points == NULL) {
		first_assign = unassigned;
		num_first_assign = clustering->num_data_points - num_assigned - assigned_by_nng;
	} else {
		first_assign = malloc(sizeof(bool[clustering->num_data_points]));
		if (first_assign == NULL) {
			free(assigned_search);
			return iscc_make_error(SCC_ER_NO_MEMORY);
		}
		second_assign = unassigned;
		for (size_t i = 0; i < clustering->num_data_points; ++i) {
			first_assign[i] = main_data_points[i] && unassigned[i];
			second_assign[i] = !main_data_points[i] && unassigned[i];
			num_first_assign += first_assign[i];
			num_second_assign += second_assign[i];
		}
	}

	assert(num_assigned + assigned_by_nng + num_first_assign + num_second_assign == clustering->num_data_points);

	iscc_NNSearchObject* nn_search_object;
	if (!iscc_init_nn_search_object(data_set_object,
	                                num_assigned,
	                                assigned_search,
	                                &nn_search_object)) {
		free(assigned_search);
		if (second_assign != NULL) free(first_assign);
		return iscc_make_error(SCC_ER_DIST_SEARCH_ERROR);
	}

	scc_ErrorCode ec;
	iscc_Digraph nearest_data_point;
	if (num_first_assign > 0) {
		if ((ec = iscc_make_nng_from_search_object(nn_search_object,
		                                           clustering->num_data_points,
		                                           first_assign,
		                                           NULL,
		                                           1,
		                                           main_radius_constraint,
		                                           main_radius,
		                                           false,
		                                           num_first_assign,
		                                           &nearest_data_point)) != SCC_ER_OK) {
			free(assigned_search);
			if (second_assign != NULL) free(first_assign);
			iscc_close_nn_search_object(&nn_search_object);
			return ec;
		}

		iscc_assign_to_first(clustering, &nearest_data_point, first_assign);
		iscc_free_digraph(&nearest_data_point);
	}

	if (second_assign != NULL) free(first_assign);

	if (num_second_assign > 0) {
		assert(second_assign != NULL);
		if (assign_secondary_points) {
			if ((ec = iscc_make_nng_from_search_object(nn_search_object,
			                                           clustering->num_data_points,
			                                           second_assign,
			                                           NULL,
			                                           1,
			                                           secondary_radius_constraint,
			                                           secondary_radius,
			                                           false,
			                                           num_second_assign,
			                                           &nearest_data_point)) != SCC_ER_OK) {
				free(assigned_search);
				iscc_close_nn_search_object(&nn_search_object);
				return ec;
			}

			iscc_assign_to_first(clustering, &nearest_data_point, second_assign);
			iscc_free_digraph(&nearest_data_point);
		} else {
			if ((ec = iscc_remaining_ignore(clustering, second_assign)) != SCC_ER_OK) {
				iscc_close_nn_search_object(&nn_search_object);
				return ec;
			}
		}
	}

	free(assigned_search);
	if (!iscc_close_nn_search_object(&nn_search_object)) {
		return iscc_make_error(SCC_ER_DIST_SEARCH_ERROR);
	}

	return iscc_no_error();
}


scc_ErrorCode iscc_remaining_to_nearest_seed(scc_Clustering* const clustering,
                                             scc_DataSetObject* const data_set_object,
                                             const iscc_SeedResult* const sr,
                                             const size_t num_assigned,
                                             bool unassigned[restrict const static clustering->num_data_points],
                                             const bool main_data_points[restrict const],
                                             const bool assign_secondary_points,
                                             const bool main_radius_constraint,
                                             const double main_radius,
                                             const bool secondary_radius_constraint,
                                             const double secondary_radius)
{
	assert(iscc_check_input_clustering(clustering));
	assert(iscc_check_data_set_object(data_set_object, clustering->num_data_points));
	assert(sr->count > 0);
	assert(sr->seeds != NULL);
	assert(num_assigned > 0);
	assert(num_assigned < clustering->num_data_points);
	assert(unassigned != NULL);
	assert(!main_radius_constraint || (main_radius > 0.0));
	assert(!secondary_radius_constraint || (secondary_radius > 0.0));

	bool* first_assign;
	uint64_t num_first_assign = 0;
	bool* second_assign = NULL;
	uint64_t num_second_assign = 0;

	if (main_data_points == NULL) {
		first_assign = unassigned;
		num_first_assign = clustering->num_data_points - num_assigned;
	} else {
		first_assign = malloc(sizeof(bool[clustering->num_data_points]));
		if (first_assign == NULL) return iscc_make_error(SCC_ER_NO_MEMORY);
		second_assign = unassigned;
		for (size_t i = 0; i < clustering->num_data_points; ++i) {
			first_assign[i] = main_data_points[i] && unassigned[i];
			second_assign[i] = !main_data_points[i] && unassigned[i];
			num_first_assign += first_assign[i];
			num_second_assign += second_assign[i];
		}
	}

	assert(num_assigned + num_first_assign + num_second_assign == clustering->num_data_points);

	iscc_NNSearchObject* nn_search_object;
	if (!iscc_init_nn_search_object(data_set_object,
	                                sr->count,
	                                sr->seeds,
	                                &nn_search_object)) {
		if (second_assign != NULL) free(first_assign);
		return iscc_make_error(SCC_ER_DIST_SEARCH_ERROR);
	}

	scc_ErrorCode ec;
	iscc_Digraph nearest_seed;
	if (num_first_assign > 0) {
		if ((ec = iscc_make_nng_from_search_object(nn_search_object,
		                                           clustering->num_data_points,
		                                           first_assign,
		                                           NULL,
		                                           1,
		                                           main_radius_constraint,
		                                           main_radius,
		                                           false,
		                                           num_first_assign,
		                                           &nearest_seed)) != SCC_ER_OK) {
			if (second_assign != NULL) free(first_assign);
			iscc_close_nn_search_object(&nn_search_object);
			return ec;
		}

		iscc_assign_to_first(clustering, &nearest_seed, first_assign);
		iscc_free_digraph(&nearest_seed);
	}

	if (second_assign != NULL) free(first_assign);

	if (num_second_assign > 0) {
		assert(second_assign != NULL);
		if (assign_secondary_points) {
			if ((ec = iscc_make_nng_from_search_object(nn_search_object,
			                                           clustering->num_data_points,
			                                           second_assign,
			                                           NULL,
			                                           1,
			                                           secondary_radius_constraint,
			                                           secondary_radius,
			                                           false,
			                                           num_second_assign,
			                                           &nearest_seed)) != SCC_ER_OK) {
				iscc_close_nn_search_object(&nn_search_object);
				return ec;
			}
			
			iscc_assign_to_first(clustering, &nearest_seed, second_assign);
			iscc_free_digraph(&nearest_seed);
		} else {
			if ((ec = iscc_remaining_ignore(clustering, second_assign)) != SCC_ER_OK) {
				iscc_close_nn_search_object(&nn_search_object);
				return ec;
			}
		}
	}

	if (!iscc_close_nn_search_object(&nn_search_object)) {
		return iscc_make_error(SCC_ER_DIST_SEARCH_ERROR);
	}

	return iscc_no_error();
}


// ==============================================================================
// Internal function implementations 
// ==============================================================================

static scc_ErrorCode iscc_make_nng(scc_DataSetObject* const data_set_object,
                                   const size_t len_search_indices,
                                   const iscc_Dpid search_indices[const],
                                   const size_t len_query_indicators,
                                   const bool query_indicators[const],
                                   bool out_query_indicators[const],
                                   const uint32_t k,
                                   const bool radius_search,
                                   const double radius,
                                   const bool accept_partial,
                                   const uint64_t max_arcs,
                                   iscc_Digraph* const out_nng)
{
	assert(iscc_check_data_set_object(data_set_object, len_query_indicators));
	assert(len_search_indices > 0);
	assert(len_query_indicators > 0);
	assert(k > 0);
	assert(!radius_search || (radius > 0.0));
	assert(max_arcs >= k);
	assert(out_nng != NULL);

	scc_ErrorCode ec;
	if ((ec = iscc_init_digraph(len_query_indicators, max_arcs, out_nng)) != SCC_ER_OK) return ec;

	iscc_NNSearchObject* nn_search_object;
	if (!iscc_init_nn_search_object(data_set_object,
	                                len_search_indices,
	                                search_indices,
	                                &nn_search_object)) {
		iscc_free_digraph(out_nng);
		return iscc_make_error(SCC_ER_DIST_SEARCH_ERROR);
	}

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
		iscc_close_nn_search_object(&nn_search_object);
		return iscc_make_error(SCC_ER_DIST_SEARCH_ERROR);
	}

	if (!iscc_close_nn_search_object(&nn_search_object)) {
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


static scc_ErrorCode iscc_make_nng_from_search_object(iscc_NNSearchObject* const nn_search_object,
                                                      const size_t len_query_indicators,
                                                      const bool query_indicators[const],
                                                      bool out_query_indicators[const],
                                                      const uint32_t k,
                                                      const bool radius_search,
                                                      const double radius,
                                                      const bool accept_partial,
                                                      const uint64_t max_arcs,
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
	assert(num_types > 1);
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
	for (size_t i = 1; i < num_types; ++i) {
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


static size_t iscc_assign_lexical(scc_Clustering* const clustering,
                                  const iscc_Digraph* const priority_graph,
                                  const bool unassigned[restrict const static clustering->num_data_points])
{
	assert(iscc_check_input_clustering(clustering));
	assert(iscc_digraph_is_initialized(priority_graph));
	assert(unassigned != NULL);

	size_t assigned_count = 0;

	for (size_t i = 0; i < clustering->num_data_points; ++i) {
		if (unassigned[i]) {
			clustering->cluster_label[i] = SCC_CLABEL_NA;

			const iscc_Dpid* const v_arc_stop = priority_graph->head + priority_graph->tail_ptr[i + 1];
			for (const iscc_Dpid* v_arc = priority_graph->head + priority_graph->tail_ptr[i];
			        v_arc != v_arc_stop; ++v_arc) {
				if (!unassigned[*v_arc]) {
					assert(clustering->cluster_label[*v_arc] != SCC_CLABEL_NA);
					clustering->cluster_label[i] = clustering->cluster_label[*v_arc];
					++assigned_count;
					break;
				}
			}
		}
	}

	return assigned_count;
}


static void iscc_assign_to_first(scc_Clustering* const clustering,
                                 const iscc_Digraph* const priority_graph,
                                 const bool unassigned[restrict const static clustering->num_data_points])
{
	assert(iscc_check_input_clustering(clustering));
	assert(iscc_digraph_is_initialized(priority_graph));
	assert(unassigned != NULL);

	for (size_t i = 0; i < clustering->num_data_points; ++i) {
		if (!unassigned[i]) continue;
		if (priority_graph->tail_ptr[i] != priority_graph->tail_ptr[i + 1]) {
			assert(!unassigned[priority_graph->head[priority_graph->tail_ptr[i]]]);
			assert(clustering->cluster_label[priority_graph->head[priority_graph->tail_ptr[i]]] != SCC_CLABEL_NA);
			clustering->cluster_label[i] = clustering->cluster_label[priority_graph->head[priority_graph->tail_ptr[i]]];
		} else {
			clustering->cluster_label[i] = SCC_CLABEL_NA;
		}
	}
}
