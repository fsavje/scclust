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
	size_t sum_type_constraints;
	size_t* type_group_size;
	scc_Dpid* point_store;
	scc_Dpid** type_groups;
};


// ==============================================================================
// Internal function prototypes
// ==============================================================================

scc_ErrorCode iscc_make_nng(scc_DataSetObject* data_set_object,
                            size_t n_search_points,
                            const scc_Dpid search_indices[],
                            size_t num_query_indicators,
                            const bool query_indicators[],
                            size_t k,
                            bool radius_search,
                            double radius,
                            bool accept_partial,
                            bool out_query_indicators[],
                            size_t max_arcs,
                            iscc_Digraph* out_nng);

scc_ErrorCode iscc_assign_lexical(scc_Clustering* clustering,
                                  const iscc_Digraph* priority_graph,
                                  const bool unassigned[static clustering->num_data_points]);

scc_ErrorCode iscc_type_count(size_t num_data_points,
                              size_t num_types,
                              const scc_TypeLabel type_labels[],
                              const size_t type_constraints[],
                              size_t size_constraint,
                              const bool must_be_assigned[],
                              iscc_TypeCount* out_type_result);


// ==============================================================================
// External function implementations
// ==============================================================================

scc_ErrorCode iscc_get_nng_with_size_constraint(scc_DataSetObject* const data_set_object,
                                                const size_t num_data_points,
                                                const size_t size_constraint,
                                                const bool must_be_assigned[const],
                                                const bool radius_constraint,
                                                const double radius,
                                                iscc_Digraph* const out_nng)
{
	assert(iscc_check_data_set_object(data_set_object, num_data_points));
	assert(num_data_points > 0);
	assert(size_constraint > 0);
	assert(size_constraint <= num_data_points);
	assert(!radius_constraint || (radius > 0.0));
	assert(out_nng != NULL);

	size_t num_queries;
	if (must_be_assigned == NULL) {
		num_queries = num_data_points;
	} else {
		num_queries = 0;
		for (size_t i = 0; i < num_data_points; ++i) {
			num_queries += must_be_assigned[i];
		}
	}

	scc_ErrorCode ec;
	if ((ec = iscc_make_nng(data_set_object,
                            num_data_points,
                            NULL,
                            num_data_points,
                            must_be_assigned,
                            size_constraint,
                            radius_constraint,
                            radius,
                            false,
                            NULL,
                            size_constraint * num_queries,
                            out_nng)) != SCC_ER_OK) {
		return ec;
	}

	ec = iscc_delete_loops_check_error(out_nng);
	if (ec != SCC_ER_OK) {
		iscc_free_digraph(out_nng);
		return ec;
	}

	return iscc_no_error();
}


scc_ErrorCode iscc_get_nng_with_type_constraint(scc_DataSetObject* const data_set_object,
                                                const size_t num_data_points,
                                                const size_t num_types,
                                                const scc_TypeLabel type_labels[const],
                                                const size_t type_constraints[const],
                                                const size_t size_constraint,
                                                const bool must_be_assigned[const],
                                                const bool radius_constraint,
                                                const double radius,
                                                iscc_Digraph* const out_nng)
{
	assert(iscc_check_data_set_object(data_set_object, num_data_points));
	assert(num_data_points > 0);
	assert(num_types > 0);
	assert(type_labels != NULL);
	assert(type_constraints != NULL);
	assert(size_constraint > 0);
	assert(size_constraint <= num_data_points);
	assert(!radius_constraint || (radius > 0.0));
	assert(out_nng != NULL);

	bool* seedable;
	const bool* seedable_const;

	if (radius_constraint) {
		seedable = malloc(sizeof(bool[num_data_points]));
		if (seedable == NULL) return iscc_make_error(SCC_ER_NO_MEMORY);
		seedable_const = seedable;
		if (must_be_assigned == NULL) {
			for (size_t i = 0; i < num_data_points; ++i) {
				seedable[i] = true;
			}
		} else {
			memcpy(seedable, must_be_assigned, sizeof(bool[num_data_points]));
		}
	} else {
		seedable = NULL;
		seedable_const = must_be_assigned;
	}

	iscc_Digraph* const nng_by_type = malloc(sizeof(iscc_Digraph[num_types]));
	if (nng_by_type == NULL) {
		free(seedable);
		return iscc_make_error(SCC_ER_NO_MEMORY);
	}

	scc_ErrorCode ec;
	iscc_TypeCount tc;
	if ((ec = iscc_type_count(num_data_points,
                              num_types,
                              type_labels,
                              type_constraints,
                              size_constraint,
                              seedable_const,
                              &tc)) != SCC_ER_OK) {
		free(seedable);
		free(nng_by_type);
		return ec;
	}

	size_t num_non_zero_type_constraints = 0;
	for (size_t i = 0; i < num_types; ++i) {
		if (type_constraints[i] > 0) {
			iscc_Digraph* const tmp_dg = &nng_by_type[num_non_zero_type_constraints];
			if ((ec = iscc_make_nng(data_set_object,
		                            tc.type_group_size[i],
		                            tc.type_groups[i],
		                            num_data_points,
		                            seedable_const,
		                            type_constraints[i],
		                            radius_constraint,
		                            radius,
		                            false,
		                            seedable,
		                            type_constraints[i] * tc.num_queries,
		                            tmp_dg)) != SCC_ER_OK) {
				break;
			}
			++num_non_zero_type_constraints;
		}
	}

	free(tc.type_group_size);
	free(tc.point_store);
	free(tc.type_groups);

	if (ec == SCC_ER_OK) {
		if (radius_constraint) {
			ec = iscc_digraph_union_and_delete(num_non_zero_type_constraints, nng_by_type, seedable_const, out_nng);
		} else {
			ec = iscc_digraph_union(num_non_zero_type_constraints, nng_by_type, out_nng);
		}
	}

	for (size_t i = 0; i < num_non_zero_type_constraints; ++i) {
		iscc_free_digraph(&nng_by_type[i]);
	}
	free(nng_by_type);

	if (ec != SCC_ER_OK) {
		free(seedable);
		iscc_free_digraph(out_nng);
		return ec;
	}

	if (size_constraint > tc.sum_type_constraints) {
		size_t additional_nn_needed = size_constraint - tc.sum_type_constraints;
		iscc_Digraph nng_sum[2];
		nng_sum[0] = *out_nng;

		if ((ec = iscc_make_nng(data_set_object,
	                            num_data_points,
	                            NULL,
	                            num_data_points,
	                            seedable_const,
	                            size_constraint,
	                            radius_constraint,
	                            radius,
	                            false,
	                            seedable,
	                            size_constraint * tc.num_queries,
	                            &nng_sum[1])) != SCC_ER_OK) {
			free(seedable);
			iscc_free_digraph(&nng_sum[0]);
			return ec;
		}

		ec = iscc_digraph_difference(&nng_sum[1], &nng_sum[0], additional_nn_needed);

		if (ec == SCC_ER_OK) {
			if (radius_constraint) {
				ec = iscc_digraph_union_and_delete(2, nng_sum, seedable_const, out_nng);
			} else {
				ec = iscc_digraph_union(2, nng_sum, out_nng);
			}
		}

		free(seedable);
		iscc_free_digraph(&nng_sum[0]);
		iscc_free_digraph(&nng_sum[1]);

		if (ec != SCC_ER_OK) return ec;
	}

	return iscc_no_error();
}


scc_ErrorCode iscc_assign_seed_neighbors(scc_Clustering* const clustering,
                                         const iscc_Digraph* const nng,
                                         const iscc_SeedResult* const sr,
                                         bool* const out_unassigned)
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
	const scc_Dpid* seed_stop = sr->seeds + sr->count;
	for (const scc_Dpid* seed = sr->seeds;
	        seed != seed_stop; ++seed, ++clabel) {

		assert(clabel != SCC_CLABEL_NA);
		assert(clabel < SCC_CLABEL_MAX);
		assert(out_unassigned[*seed]);
		out_unassigned[*seed] = false;
		clustering->cluster_label[*seed] = clabel;

		const scc_Dpid* const s_arc_stop = nng->head + nng->tail_ptr[*seed + 1];
		for (const scc_Dpid* s_arc = nng->head + nng->tail_ptr[*seed];
		        s_arc != s_arc_stop; ++s_arc) {
			assert(out_unassigned[*s_arc] || (*s_arc == *seed));
			out_unassigned[*s_arc] = false;
			clustering->cluster_label[*s_arc] = clabel;
		}
	}

	assert(clustering->num_clusters == clabel);

	return iscc_no_error();
}


scc_ErrorCode iscc_remaining_ignore(scc_Clustering* const clustering,
                                    const bool* const unassigned)
{
	assert(iscc_check_input_clustering(clustering));
	assert(unassigned != NULL);

	for (size_t v = 0; v < clustering->num_data_points; ++v) {
		if (unassigned[v]) clustering->cluster_label[v] = SCC_CLABEL_NA;
	}

	return iscc_no_error();
}


scc_ErrorCode iscc_remaining_to_nearest_assigned(scc_DataSetObject* const data_set_object,
                                                 scc_Clustering* const clustering,
                                                 iscc_Digraph* const nng,
                                                 const size_t num_assigned, // sr->count * size_constraint
                                                 bool* const unassigned,
                                                 const bool main_data_points[const],
                                                 const bool assign_secondary_points,
                                                 const bool secondary_radius_constraint,
                                                 const double secondary_radius)
{
	assert(iscc_check_input_clustering(clustering));
	assert(iscc_check_data_set_object(data_set_object, clustering->num_data_points));
	assert(iscc_digraph_is_initialized(nng));
	assert(num_assigned > 0);
	assert(unassigned != NULL);
	assert(!secondary_radius_constraint || (secondary_radius > 0.0));

	scc_ErrorCode ec;

	ec = iscc_assign_lexical(clustering, nng, unassigned);
	iscc_free_digraph(nng);
	if (ec != SCC_ER_OK) {
		return ec;
	}

	if ((main_data_points != NULL) && assign_secondary_points) {
		size_t query_count = 0;
		size_t search_count = 0;
		scc_Dpid* const assigned_search = malloc(sizeof(scc_Dpid[num_assigned]));
		if (assigned_search == NULL) {
			return iscc_make_error(SCC_ER_NO_MEMORY);
		}

		assert(clustering->num_data_points < SCC_DPID_MAX);
		scc_Dpid num_data_points_dpid = (scc_Dpid) clustering->num_data_points; // If `scc_Dpid` is signed.
		for (scc_Dpid i = 0; i < num_data_points_dpid; ++i) {
			if (!unassigned[i]) {
				assigned_search[search_count] = i;
				++search_count;
			} else {
				unassigned[i] = !main_data_points[i];
				query_count += unassigned[i];
			}
		}

		assert(search_count == num_assigned);

		ec = iscc_make_nng(data_set_object,
                           search_count,
                           assigned_search,
                           clustering->num_data_points,
                           unassigned,
                           1,
                           secondary_radius_constraint,
                           secondary_radius,
                           false,
                           NULL,
                           query_count,
                           nng);
		free(assigned_search);
		if (ec != SCC_ER_OK) return ec;

		ec = iscc_assign_lexical(clustering, nng, unassigned);
		iscc_free_digraph(nng);
		if (ec != SCC_ER_OK) return ec;
	}

	return iscc_no_error();
}


scc_ErrorCode iscc_remaining_to_nearest_seed(scc_DataSetObject* const data_set_object,
                                             scc_Clustering* const clustering,
                                             const iscc_SeedResult* const sr,
                                             const size_t num_assigned, // sr->count * size_constraint
                                             bool* const unassigned,
                                             const bool main_data_points[const],
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
	assert(unassigned != NULL);
	assert(!main_radius_constraint || (main_radius > 0.0));
	assert(!secondary_radius_constraint || (secondary_radius > 0.0));

	bool* first_assign;
	size_t num_first_assign = 0;
	bool* second_assign = NULL;
	size_t num_second_assign = 0;

	if (main_data_points == NULL) {
		first_assign = unassigned;
		num_first_assign = clustering->num_data_points - num_assigned;
	} else {
		first_assign = malloc(sizeof(bool[clustering->num_data_points]));
		if (first_assign == NULL) return iscc_make_error(SCC_ER_NO_MEMORY);
		second_assign = unassigned;
		for (size_t i = 0; i < clustering->num_data_points; ++i) {
			first_assign[i] = main_data_points[i] && second_assign[i];
			second_assign[i] = !main_data_points[i] && second_assign[i];
			num_first_assign += first_assign[i];
			num_second_assign += second_assign[i];
		}
	}

	assert(num_assigned + num_first_assign + num_second_assign == clustering->num_data_points);

	scc_ErrorCode ec;
	iscc_Digraph nearest_seed;
	if ((ec = iscc_make_nng(data_set_object,
                            sr->count,
                            sr->seeds,
                            clustering->num_data_points,
                            first_assign,
                            1,
                            main_radius_constraint,
                            main_radius,
                            false,
                            NULL,
                            num_first_assign,
                            &nearest_seed)) != SCC_ER_OK) {
		if (second_assign != NULL) free(first_assign);
		return ec;
	}

	ec = iscc_assign_lexical(clustering, &nearest_seed, first_assign);
	iscc_free_digraph(&nearest_seed);
	if (second_assign != NULL) free(first_assign);
	if (ec != SCC_ER_OK) return ec;

	if (num_second_assign > 0) {
		if (assign_secondary_points) {
			if ((ec = iscc_make_nng(data_set_object,
		                            sr->count,
		                            sr->seeds,
		                            clustering->num_data_points,
		                            second_assign,
		                            1,
		                            secondary_radius_constraint,
		                            secondary_radius,
		                            false,
		                            NULL,
		                            num_second_assign,
		                            &nearest_seed)) != SCC_ER_OK) {
				return ec;
			}

			ec = iscc_assign_lexical(clustering, &nearest_seed, second_assign);
			iscc_free_digraph(&nearest_seed);
			if (ec != SCC_ER_OK) return ec;
		} else {
			if ((ec = iscc_remaining_ignore(clustering, second_assign)) != SCC_ER_OK) return ec;
		}
	}

	return iscc_no_error();
}


/*
scc_ErrorCode iscc_assign_remaining_desired_size(scc_Clustering* const clustering,
                                                 const iscc_Digraph* const priority_graph,
                                                 const bool unassigned[const static clustering->num_data_points],
                                                 const size_t desired_size)
{
	assert(iscc_check_input_clustering(clustering));
	assert(iscc_digraph_is_initialized(priority_graph));
	assert(unassigned != NULL);
	assert(desired_size > 0);

	scc_Dpid* cluster_size = calloc(clustering->num_clusters, sizeof(scc_Dpid));
	if (cluster_size == NULL) return iscc_make_error(SCC_ER_NO_MEMORY);

	for (size_t v = 0; v < clustering->num_data_points; ++v) {
		if (unassigned[v]) {
			scc_Clabel best_cluster = SCC_CLABEL_NA;
			scc_Dpid best_size = 0;

			const scc_Dpid* const v_arc_stop = priority_graph->head + priority_graph->tail_ptr[v + 1];
			for (const scc_Dpid* v_arc = priority_graph->head + priority_graph->tail_ptr[v];
			        v_arc != v_arc_stop; ++v_arc) {
				if (!unassigned[*v_arc]) {
					assert(clustering->cluster_label[*v_arc] < clustering->num_clusters);
					if ((best_cluster == SCC_CLABEL_NA) || (best_size < cluster_size[clustering->cluster_label[*v_arc]])) {
						best_cluster = out_cl.cluster_label[*v_arc];
						best_size = cluster_size[best_cluster];
					}
				}
			}

			clustering->cluster_label[v] = best_cluster;
			if (best_cluster != SCC_CLABEL_NA) {
				++cluster_size[best_cluster];
				if (cluster_size[best_cluster] % desired_size == 0) {
					cluster_size[best_cluster] = 0;
				} 
			}
		}
	}

	free(cluster_size);

	return iscc_no_error();
}*/


// ==============================================================================
// Internal function implementations 
// ==============================================================================

scc_ErrorCode iscc_make_nng(scc_DataSetObject* const data_set_object,
                            const size_t n_search_points,
                            const scc_Dpid search_indices[const],
                            const size_t num_query_indicators,
                            const bool query_indicators[const],
                            const size_t k,
                            const bool radius_search,
                            const double radius,
                            const bool accept_partial,
                            bool out_query_indicators[const],
                            const size_t max_arcs,
                            iscc_Digraph* const out_nng)
{
	scc_ErrorCode ec;

	if ((ec = iscc_init_digraph(num_query_indicators, max_arcs, out_nng)) != SCC_ER_OK) return ec;

	if (!iscc_nearest_neighbor_search(data_set_object,
	                                  n_search_points,
	                                  search_indices,
	                                  num_query_indicators,
	                                  query_indicators,
	                                  k,
	                                  radius_search,
	                                  radius,
	                                  accept_partial,
	                                  out_query_indicators,
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


scc_ErrorCode iscc_assign_lexical(scc_Clustering* const clustering,
                                  const iscc_Digraph* const priority_graph,
                                  const bool unassigned[const static clustering->num_data_points])
{
	assert(iscc_check_input_clustering(clustering));
	assert(iscc_digraph_is_initialized(priority_graph));
	assert(unassigned != NULL);

	for (size_t v = 0; v < clustering->num_data_points; ++v) {
		if (unassigned[v]) {
			clustering->cluster_label[v] = SCC_CLABEL_NA;

			const scc_Dpid* const v_arc_stop = priority_graph->head + priority_graph->tail_ptr[v + 1];
			for (const scc_Dpid* v_arc = priority_graph->head + priority_graph->tail_ptr[v];
			        v_arc != v_arc_stop; ++v_arc) {
				if (unassigned[*v_arc]) {
					clustering->cluster_label[v] = clustering->cluster_label[*v_arc];
					break;
				}
			}
		}
	}

	return iscc_no_error();
}


scc_ErrorCode iscc_type_count(const size_t num_data_points,
                              const size_t num_types,
                              const scc_TypeLabel type_labels[const],
                              const size_t type_constraints[const],
                              const size_t size_constraint,
                              const bool must_be_assigned[const],
                              iscc_TypeCount* const out_type_result)
{
	assert(num_data_points > 1);
	assert(num_types > 1);
	assert(type_labels != NULL);
	assert(type_constraints != NULL);
	assert(size_constraint > 0);

	*out_type_result = (iscc_TypeCount) {
		.num_queries = 0,
		.sum_type_constraints = 0,
		.type_group_size = calloc(num_types, sizeof(size_t)),
		.point_store = NULL,
		.type_groups = NULL,
	};

	if (out_type_result->type_group_size == NULL) return iscc_make_error(SCC_ER_NO_MEMORY);

	if (must_be_assigned == NULL) {
		out_type_result->num_queries = num_data_points;
		for (size_t i = 0; i < num_data_points; ++i) {
			++out_type_result->type_group_size[type_labels[i]];
		}
	} else {
		for (size_t i = 0; i < num_data_points; ++i) {
			out_type_result->num_queries += must_be_assigned[i];
			++out_type_result->type_group_size[type_labels[i]];
		}
	}

	for (size_t i = 0; i < num_types; ++i) {
		if (out_type_result->type_group_size[i] < type_constraints[i]) {
			free(out_type_result->type_group_size);
			return iscc_make_error(SCC_ER_INVALID_INPUT);
		}
		out_type_result->sum_type_constraints += type_constraints[i];
	}

	if (out_type_result->sum_type_constraints > size_constraint) {
		free(out_type_result->type_group_size);
		return iscc_make_error(SCC_ER_INVALID_INPUT);
	}

	out_type_result->point_store = malloc(sizeof(scc_Dpid[num_data_points]));
	out_type_result->type_groups = malloc(sizeof(scc_Dpid*[num_types]));
	if ((out_type_result->point_store == NULL) || (out_type_result->type_groups == NULL)) {
		free(out_type_result->type_group_size);
		free(out_type_result->point_store);
		free(out_type_result->type_groups);
		return iscc_make_error(SCC_ER_NO_MEMORY);
	}

	out_type_result->type_groups[0] = out_type_result->point_store + out_type_result->type_group_size[0];
	for (size_t i = 1; i < num_types; ++i) {
		out_type_result->type_groups[i] = out_type_result->type_groups[i - 1] + out_type_result->type_group_size[i];
	}

	assert(num_data_points < SCC_DPID_MAX);
	const scc_Dpid num_data_points_dpid = (scc_Dpid) num_data_points; // if case `scc_Dpid` is signed.
	for (scc_Dpid i = 0; i < num_data_points_dpid; ++i) {
		--(out_type_result->type_groups[type_labels[i]]);
		*(out_type_result->type_groups[type_labels[i]]) = i;
	}

	return iscc_no_error();
}
