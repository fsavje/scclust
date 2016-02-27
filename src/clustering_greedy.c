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


#include "../include/scclust.h"

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include "clustering.h"
#include "dist_search.h"
#include "error.h"


// ==============================================================================
// Internal structs
// ==============================================================================

typedef struct iscc_gr_DistanceEdge iscc_gr_DistanceEdge;
struct iscc_gr_DistanceEdge {
	scc_Dpid head;
	double distance;
	iscc_gr_DistanceEdge* next_dist;
};

typedef struct iscc_gr_ClusterItem iscc_gr_ClusterItem;
struct iscc_gr_ClusterItem {
	size_t size;
	uint_fast16_t marker;
	scc_Dpid* members;
};

typedef struct iscc_gr_ClusterStack iscc_gr_ClusterStack;
struct iscc_gr_ClusterStack {
	size_t capacity;
	size_t items;
	iscc_gr_ClusterItem* clusters;
};


// ==============================================================================
// Internal variables
// ==============================================================================

static const iscc_gr_ClusterItem ISCC_GR_NULL_CLUSTER_ITEM = { 0, 0, NULL };
static const iscc_gr_ClusterStack ISCC_GR_NULL_CLUSTER_STACK = { 0, 0, NULL };


// ==============================================================================
// Internal function prototypes
// ==============================================================================

static scc_ErrorCode iscc_gr_empty_cl_stack(size_t num_data_points,
                                            iscc_gr_ClusterStack* out_cstack);

static scc_ErrorCode iscc_gr_init_cl_stack(const scc_Clustering* in_cl,
                                           iscc_gr_ClusterStack* out_cstack);

static void iscc_gr_free_cl_strack(iscc_gr_ClusterStack* cl_stack);

static scc_ErrorCode iscc_gr_run_greedy_clustering(scc_DataSetObject* data_set_object,
                                                   iscc_gr_ClusterStack* cl_stack,
                                                   scc_Clustering* cl,
                                                   size_t size_constraint,
                                                   bool batch_assign);

static inline iscc_gr_ClusterItem* iscc_gr_peek_at_stack(const iscc_gr_ClusterStack* cl_stack);

static inline iscc_gr_ClusterItem iscc_gr_pop_from_stack(iscc_gr_ClusterStack* cl_stack);

static scc_ErrorCode iscc_gr_push_to_stack(iscc_gr_ClusterStack* cl_stack,
                                           iscc_gr_ClusterItem cl);

static scc_ErrorCode iscc_gr_break_cluster_into_two(scc_DataSetObject* data_set_object,
                                                    iscc_gr_ClusterItem* cluster_to_break,
                                                    size_t size_constraint,
                                                    bool batch_assign,
                                                    uint_fast16_t* vertex_markers,
                                                    iscc_gr_ClusterItem* out_new_cluster);

static scc_ErrorCode iscc_gr_find_centers(scc_DataSetObject* data_set_object,
                                          iscc_gr_ClusterItem* cl,
                                          scc_Dpid* center1,
                                          scc_Dpid* center2,
                                          uint_fast16_t* vertex_markers);

static inline uint_fast16_t iscc_gr_get_next_marker(iscc_gr_ClusterItem* cl,
                                                    uint_fast16_t* vertex_markers);

static scc_ErrorCode iscc_gr_populate_dist_lists(scc_DataSetObject* data_set_object,
                                                 const iscc_gr_ClusterItem* cl,
                                                 scc_Dpid center1,
                                                 scc_Dpid center2,
                                                 iscc_gr_DistanceEdge dist_store1[static cl->size],
                                                 iscc_gr_DistanceEdge dist_store2[static cl->size]);

static inline void iscc_gr_sort_dist_list(const iscc_gr_ClusterItem* cl,
                                          scc_Dpid center,
                                          const double output_dists[static cl->size],
                                          iscc_gr_DistanceEdge dist_edge_store[static cl->size]);

static int iscc_gr_compare_dist_edges(const void* a,
                                      const void* b);

static inline void iscc_gr_move_v_to_cluster(scc_Dpid v_id,
                                             iscc_gr_ClusterItem* cl,
                                             uint_fast16_t* vertex_markers,
                                             uint_fast16_t curr_marker);

static inline iscc_gr_DistanceEdge* iscc_gr_get_next_dist(iscc_gr_DistanceEdge* prev_dist,
                                                          const uint_fast16_t* vertex_markers,
                                                          uint_fast16_t curr_marker);

static inline iscc_gr_DistanceEdge* iscc_gr_get_next_k_nn(iscc_gr_DistanceEdge* prev_dist,
                                                          size_t k,
                                                          scc_Dpid out_dist_array[static k],
                                                          const uint_fast16_t* vertex_markers,
                                                          uint_fast16_t curr_marker);


// ==============================================================================
// External function implementations
// ==============================================================================

scc_ErrorCode scc_top_down_greedy_clustering(scc_Clustering* const clustering,
                                             scc_DataSetObject* const data_set_object,
                                             const size_t size_constraint,
                                             const bool batch_assign)
{
	if (!iscc_check_input_clustering(clustering)) return iscc_make_error(SCC_ER_INVALID_CLUSTERING);
	if (!iscc_check_data_set_object(data_set_object, clustering->num_data_points)) return iscc_make_error(SCC_ER_INVALID_DATA_OBJ);
	if (size_constraint < 2) return iscc_make_error(SCC_ER_INVALID_INPUT);

	scc_ErrorCode ec;
	iscc_gr_ClusterStack cl_stack;
	if (clustering->num_clusters == 0) {
		if ((ec = iscc_gr_empty_cl_stack(clustering->num_data_points, &cl_stack)) != SCC_ER_OK) return ec;

		if (clustering->cluster_label == NULL) {
			clustering->external_labels = false;
			clustering->cluster_label = malloc(sizeof(scc_Clabel[clustering->num_data_points]));
			if (clustering->cluster_label == NULL) {
				iscc_gr_free_cl_strack(&cl_stack);
				return iscc_make_error(SCC_ER_NO_MEMORY);
			} 
		}
	} else {
		if ((ec = iscc_gr_init_cl_stack(clustering, &cl_stack)) != SCC_ER_OK) return ec;
	}

	assert(cl_stack.clusters != NULL);

	if ((ec = iscc_gr_run_greedy_clustering(data_set_object, &cl_stack, clustering, size_constraint, batch_assign)) != SCC_ER_OK) {
		iscc_gr_free_cl_strack(&cl_stack);
		return ec;
	}

	iscc_gr_free_cl_strack(&cl_stack);

	return iscc_no_error();
}


// ==============================================================================
// Internal function implementations 
// ==============================================================================

static scc_ErrorCode iscc_gr_empty_cl_stack(const size_t num_data_points,
                                            iscc_gr_ClusterStack* const out_cl_stack)
{
	assert(num_data_points >= 2);
	assert(out_cl_stack != NULL);

	out_cl_stack->capacity = 1 + (size_t) (20 * log2((double) num_data_points));
	out_cl_stack->items = 1;
	out_cl_stack->clusters = malloc(sizeof(iscc_gr_ClusterItem[out_cl_stack->capacity]));

	scc_Dpid* const tmp_members = malloc(sizeof(scc_Dpid[num_data_points]));

	if ((out_cl_stack->clusters == NULL) || (tmp_members == NULL)) {
		free(out_cl_stack->clusters);
		free(tmp_members);
		*out_cl_stack = ISCC_GR_NULL_CLUSTER_STACK;
		return iscc_make_error(SCC_ER_NO_MEMORY);
	}

	assert(num_data_points < SCC_DPID_MAX);
	const scc_Dpid num_data_points_dpid = (scc_Dpid) num_data_points; // If `scc_Dpid` is signed
	for (scc_Dpid i = 0; i < num_data_points_dpid; ++i) {
		tmp_members[i] = i;
	}

	out_cl_stack->clusters[0] = (iscc_gr_ClusterItem) {
		.size = num_data_points,
		.marker = 0,
		.members = tmp_members,
	};

	return iscc_no_error();
}


static scc_ErrorCode iscc_gr_init_cl_stack(const scc_Clustering* const in_cl,
                                           iscc_gr_ClusterStack* const out_cl_stack)
{
	assert(iscc_check_input_clustering(in_cl));
	assert(in_cl->num_data_points >= 2);
	assert(out_cl_stack != NULL);

	out_cl_stack->capacity = in_cl->num_clusters + (size_t) (10 * log2((double) in_cl->num_data_points)),
	out_cl_stack->items = in_cl->num_clusters,
	out_cl_stack->clusters = calloc(out_cl_stack->capacity, sizeof(iscc_gr_ClusterItem));

	if (out_cl_stack->clusters == NULL) {
		*out_cl_stack = ISCC_GR_NULL_CLUSTER_STACK;
		return iscc_make_error(SCC_ER_NO_MEMORY);
	}

	for (size_t v = 0; v < in_cl->num_data_points; ++v) {
		if (in_cl->cluster_label[v] != SCC_CLABEL_NA) {
			++(out_cl_stack->clusters[in_cl->cluster_label[v]].size);
		}
	}

	for (size_t c = 0; c < out_cl_stack->items; ++c) {
		if (out_cl_stack->clusters[c].size == 0) continue;
		out_cl_stack->clusters[c].members = malloc(sizeof(scc_Dpid[out_cl_stack->clusters[c].size]));
		if (out_cl_stack->clusters[c].members == NULL) {
			for (size_t c_free = 0; c_free < c; ++c_free) {
				free(out_cl_stack->clusters[c_free].members);
			}
			free(out_cl_stack->clusters);
			*out_cl_stack = ISCC_GR_NULL_CLUSTER_STACK;
			return iscc_make_error(SCC_ER_NO_MEMORY);
		}
		out_cl_stack->clusters[c].size = 0;
	}

	assert(in_cl->num_data_points < SCC_DPID_MAX);
	const scc_Dpid num_data_points = (scc_Dpid) in_cl->num_data_points; // If `scc_Dpid` is signed
	for (scc_Dpid i = 0; i < num_data_points; ++i) {
		if (in_cl->cluster_label[i] != SCC_CLABEL_NA) {
			iscc_gr_ClusterItem* cl = &(out_cl_stack->clusters[in_cl->cluster_label[i]]);
			cl->members[cl->size] = i;
			++(cl->size);
		}
	}

	return iscc_no_error();
}


static void iscc_gr_free_cl_strack(iscc_gr_ClusterStack* const cl_stack) {
	if (cl_stack != NULL) {
		if (cl_stack->clusters != NULL) {
			for (size_t i = 0; i < cl_stack->items; ++i) {
				free(cl_stack->clusters[i].members);
			}
			free(cl_stack->clusters);
		}
		*cl_stack = ISCC_GR_NULL_CLUSTER_STACK;
	}
}


static scc_ErrorCode iscc_gr_run_greedy_clustering(scc_DataSetObject* const data_set_object,
                                                   iscc_gr_ClusterStack* const cl_stack,
                                                   scc_Clustering* const cl,
                                                   const size_t size_constraint,
                                                   const bool batch_assign)
{
	assert(iscc_check_data_set_object(data_set_object, cl->num_data_points));
	assert(cl_stack != NULL);
	assert(cl_stack->items > 0);
	assert(cl_stack->items <= cl_stack->capacity);
	assert(cl_stack->clusters != NULL);
	assert(cl != NULL);
	assert(cl->num_data_points > 0);
	assert(cl->num_data_points < SCC_DPID_MAX);
	assert(cl->cluster_label != NULL);
	assert(size_constraint >= 2);

	uint_fast16_t* const vertex_markers = calloc(cl->num_data_points, sizeof(uint_fast16_t));
	if (vertex_markers == NULL)  return iscc_make_error(SCC_ER_NO_MEMORY);

	scc_ErrorCode ec;
	scc_Clabel curr_label = 0;
	while (iscc_gr_peek_at_stack(cl_stack) != NULL) {
		if (iscc_gr_peek_at_stack(cl_stack)->size < 2 * size_constraint) {
			iscc_gr_ClusterItem unbreakable_cluster = iscc_gr_pop_from_stack(cl_stack);
			if (unbreakable_cluster.size > 0) {
				if (curr_label == SCC_CLABEL_MAX) {
					free(unbreakable_cluster.members);
					free(vertex_markers);
					return iscc_make_error(SCC_ER_TOO_LARGE_PROBLEM);
				}
				for (size_t v = 0; v < unbreakable_cluster.size; ++v) {
					cl->cluster_label[unbreakable_cluster.members[v]] = curr_label;
				}
				free(unbreakable_cluster.members);
				++curr_label;
			}
		} else {
			iscc_gr_ClusterItem new_cluster;
			if ((ec = iscc_gr_break_cluster_into_two(data_set_object, iscc_gr_peek_at_stack(cl_stack), size_constraint, batch_assign, vertex_markers, &new_cluster)) != SCC_ER_OK) {
				free(vertex_markers);
				return ec;
			}
			if ((ec = iscc_gr_push_to_stack(cl_stack, new_cluster)) != SCC_ER_OK) {
				free(new_cluster.members);
				free(vertex_markers);
				return ec;
			}
		}
	}

	free(vertex_markers);

	assert(curr_label >= 0);
	cl->num_clusters = (size_t) curr_label;

	assert(cl_stack->items == 0);

	return iscc_no_error();
}


static inline iscc_gr_ClusterItem* iscc_gr_peek_at_stack(const iscc_gr_ClusterStack* const cl_stack)
{
	assert(cl_stack != NULL);
	assert(cl_stack->clusters != NULL);

	if (cl_stack->items == 0) return NULL;
	return &cl_stack->clusters[cl_stack->items - 1];
}


static inline iscc_gr_ClusterItem iscc_gr_pop_from_stack(iscc_gr_ClusterStack* const cl_stack)
{
	assert(cl_stack != NULL);
	assert(cl_stack->items > 0);
	assert(cl_stack->clusters != NULL);

	--(cl_stack->items);
	return cl_stack->clusters[cl_stack->items];
}


static scc_ErrorCode iscc_gr_push_to_stack(iscc_gr_ClusterStack* const cl_stack,
                                           const iscc_gr_ClusterItem cl)
{
	assert(cl_stack != NULL);
	assert(cl_stack->clusters != NULL);
	assert(cl_stack->items <= cl_stack->capacity);

	if (cl_stack->items == cl_stack->capacity) {
		size_t new_capacity = cl_stack->capacity + 16 + (cl_stack->capacity >> 4);
		iscc_gr_ClusterItem* const clusters_tmp = realloc(cl_stack->clusters, sizeof(iscc_gr_ClusterItem[new_capacity]));
		if (clusters_tmp == NULL) return iscc_make_error(SCC_ER_NO_MEMORY);
		cl_stack->capacity = new_capacity;
		cl_stack->clusters = clusters_tmp;
	}

	cl_stack->clusters[cl_stack->items] = cl;
	++(cl_stack->items);

	return iscc_no_error();
}


static scc_ErrorCode iscc_gr_break_cluster_into_two(scc_DataSetObject* const data_set_object,
                                                    iscc_gr_ClusterItem* const cluster_to_break,
                                                    const size_t size_constraint,
                                                    const bool batch_assign,
                                                    uint_fast16_t* const vertex_markers,
                                                    iscc_gr_ClusterItem* const out_new_cluster)
{
	assert(iscc_check_data_set_object(data_set_object, 0));
	assert(cluster_to_break != NULL);
	assert(cluster_to_break->size >= 2 * size_constraint);
	assert(cluster_to_break->members != NULL);
	assert(size_constraint >= 2);
	assert(vertex_markers != NULL);
	assert(out_new_cluster != NULL);

	scc_ErrorCode ec;
	scc_Dpid center1, center2;
	const size_t old_cluster_size = cluster_to_break->size;

	// Most be first as `iscc_gr_get_next_marker` increases cluster markers
	if ((ec = iscc_gr_find_centers(data_set_object, cluster_to_break, &center1, &center2, vertex_markers)) != SCC_ER_OK) {
		*out_new_cluster = ISCC_GR_NULL_CLUSTER_ITEM;
		return ec;
	}

	const uint_fast16_t curr_marker = iscc_gr_get_next_marker(cluster_to_break, vertex_markers);

	iscc_gr_DistanceEdge* const dist_store1 = malloc(sizeof(iscc_gr_DistanceEdge[old_cluster_size]));
	iscc_gr_DistanceEdge* const dist_store2 = malloc(sizeof(iscc_gr_DistanceEdge[old_cluster_size]));

	scc_Dpid* const k_nn_array1 = malloc(sizeof(scc_Dpid[size_constraint]));
	scc_Dpid* const k_nn_array2 = malloc(sizeof(scc_Dpid[size_constraint]));

	*out_new_cluster = (iscc_gr_ClusterItem) {
		.size = 0,
		.marker = curr_marker,
		.members = malloc(sizeof(scc_Dpid[old_cluster_size])),
	};

	if ((dist_store1 == NULL) || (dist_store2 == NULL) || (k_nn_array1 == NULL) ||
	        (k_nn_array2 == NULL) || (out_new_cluster->members == NULL)) {
		free(dist_store1);
		free(dist_store2);
		free(k_nn_array1);
		free(k_nn_array2);
		free(out_new_cluster->members);
		*out_new_cluster = ISCC_GR_NULL_CLUSTER_ITEM;
		return iscc_make_error(SCC_ER_NO_MEMORY);
	}

	if ((ec = iscc_gr_populate_dist_lists(data_set_object, cluster_to_break, center1, center2, dist_store1, dist_store2)) != SCC_ER_OK) {
		free(dist_store1);
		free(dist_store2);
		free(k_nn_array1);
		free(k_nn_array2);
		free(out_new_cluster->members);
		*out_new_cluster = ISCC_GR_NULL_CLUSTER_ITEM;
		return ec;
	}

	iscc_gr_ClusterItem* const cluster1 = cluster_to_break;
	cluster1->size = 0;

	iscc_gr_ClusterItem* const cluster2 = out_new_cluster;

	iscc_gr_move_v_to_cluster(center1, cluster1, vertex_markers, curr_marker);
	iscc_gr_move_v_to_cluster(center2, cluster2, vertex_markers, curr_marker);

	iscc_gr_DistanceEdge* last_assigned_dist1 = dist_store1;
	iscc_gr_DistanceEdge* last_assigned_dist2 = dist_store2;

	iscc_gr_DistanceEdge* temp_dist1;
	iscc_gr_DistanceEdge* temp_dist2;

	temp_dist1 = iscc_gr_get_next_k_nn(last_assigned_dist1, size_constraint - 1, k_nn_array1, vertex_markers, curr_marker);
	temp_dist2 = iscc_gr_get_next_k_nn(last_assigned_dist2, size_constraint - 1, k_nn_array2, vertex_markers, curr_marker);

	if (temp_dist1->distance >= temp_dist2->distance) {
		for (size_t i = 0; i < size_constraint - 1; ++i) {
			iscc_gr_move_v_to_cluster(k_nn_array1[i], cluster1, vertex_markers, curr_marker);
		}
		last_assigned_dist1 = temp_dist1;

		last_assigned_dist2 = iscc_gr_get_next_k_nn(last_assigned_dist2, size_constraint - 1, k_nn_array2, vertex_markers, curr_marker);
		for (size_t i = 0; i < size_constraint - 1; ++i) {
			iscc_gr_move_v_to_cluster(k_nn_array2[i], cluster2, vertex_markers, curr_marker);
		}
	} else {
		for (size_t i = 0; i < size_constraint - 1; ++i) {
			iscc_gr_move_v_to_cluster(k_nn_array2[i], cluster2, vertex_markers, curr_marker);
		}
		last_assigned_dist2 = temp_dist2;

		last_assigned_dist1 = iscc_gr_get_next_k_nn(last_assigned_dist1, size_constraint - 1, k_nn_array1, vertex_markers, curr_marker);
		for (size_t i = 0; i < size_constraint - 1; ++i) {
			iscc_gr_move_v_to_cluster(k_nn_array1[i], cluster1, vertex_markers, curr_marker);
		}
	}

	assert(cluster1->size == size_constraint && cluster2->size == size_constraint);

	if (batch_assign) {
		size_t num_assign_in_batch = size_constraint;
		for (size_t assigned = 2 * size_constraint; assigned < old_cluster_size; assigned += num_assign_in_batch) {
			
			if (assigned + num_assign_in_batch > old_cluster_size) {
				num_assign_in_batch = old_cluster_size - assigned;
			}

			temp_dist1 = iscc_gr_get_next_k_nn(last_assigned_dist1, num_assign_in_batch, k_nn_array1, vertex_markers, curr_marker);
			temp_dist2 = iscc_gr_get_next_k_nn(last_assigned_dist2, num_assign_in_batch, k_nn_array2, vertex_markers, curr_marker);

			if (temp_dist1->distance <= temp_dist2->distance) {
				for (size_t i = 0; i < num_assign_in_batch; ++i) {
					iscc_gr_move_v_to_cluster(k_nn_array1[i], cluster1, vertex_markers, curr_marker);
				}
				last_assigned_dist1 = temp_dist1;
			} else {
				for (size_t i = 0; i < num_assign_in_batch; ++i) {
					iscc_gr_move_v_to_cluster(k_nn_array2[i], cluster2, vertex_markers, curr_marker);
				}
				last_assigned_dist2 = temp_dist2;
			}
		}
	} else {
		for (size_t assigned = 2 * size_constraint; assigned < old_cluster_size; ++assigned) {

			temp_dist1 = iscc_gr_get_next_dist(last_assigned_dist1, vertex_markers, curr_marker);
			temp_dist2 = iscc_gr_get_next_dist(last_assigned_dist2, vertex_markers, curr_marker);

			if (temp_dist1->distance <= temp_dist2->distance) {
				iscc_gr_move_v_to_cluster(temp_dist1->head, cluster1, vertex_markers, curr_marker);
				last_assigned_dist1 = temp_dist1;
			} else {
				iscc_gr_move_v_to_cluster(temp_dist2->head, cluster2, vertex_markers, curr_marker);
				last_assigned_dist2 = temp_dist2;
			}
		}
	}

	free(dist_store1);
	free(dist_store2);
	free(k_nn_array1);
	free(k_nn_array2);

	assert(cluster1->size >= size_constraint);
	assert(cluster2->size >= size_constraint);

	// "Must" be sequential 
	scc_Dpid* const members_tmp1 = realloc(cluster1->members, sizeof(scc_Dpid[cluster1->size]));
	if (members_tmp1 == NULL) {
		free(cluster2->members);
		*out_new_cluster = ISCC_GR_NULL_CLUSTER_ITEM;
		return iscc_make_error(SCC_ER_NO_MEMORY);
	}
	cluster1->members = members_tmp1;

	scc_Dpid* const members_tmp2 = realloc(cluster2->members, sizeof(scc_Dpid[cluster2->size]));
	if (members_tmp2 == NULL) {
		free(cluster2->members);
		*out_new_cluster = ISCC_GR_NULL_CLUSTER_ITEM;
		return iscc_make_error(SCC_ER_NO_MEMORY);
	}
	cluster2->members = members_tmp2;

	return iscc_no_error();
}


static scc_ErrorCode iscc_gr_find_centers(scc_DataSetObject* const data_set_object,
                                          iscc_gr_ClusterItem* const cl,
                                          scc_Dpid* const center1,
                                          scc_Dpid* const center2,
                                          uint_fast16_t* const vertex_markers)
{
	assert(iscc_check_data_set_object(data_set_object, 0));
	assert(cl != NULL);
	assert(cl->size >= 4);
	assert(cl->members != NULL);
	assert((center1 != NULL) && (center2 != NULL));
	assert(vertex_markers != NULL);

	const uint_fast16_t curr_marker = iscc_gr_get_next_marker(cl, vertex_markers);

	size_t step = cl->size / 1000;
	if (step < 2) step = 2;
	size_t num_to_check = 1 + (cl->size - 1) / step;

	iscc_MaxDistObject* max_dist_object;
	if (!iscc_init_max_dist_object(data_set_object, cl->size, cl->members, &max_dist_object)) {
		return iscc_make_error(SCC_ER_DIST_SEARCH_ERROR);
	}

	scc_Dpid* to_check = malloc(sizeof(scc_Dpid[num_to_check]));
	scc_Dpid* max_indices = malloc(sizeof(scc_Dpid[num_to_check]));
	double* max_dists = malloc(sizeof(double[num_to_check]));
	if ((to_check == NULL) || (max_indices == NULL) || (max_dists == NULL)) {
		free(to_check);
		free(max_indices);
		free(max_dists);
		iscc_close_max_dist_object(&max_dist_object);
		return iscc_make_error(SCC_ER_NO_MEMORY);
	}

	for (size_t i = 0; i < num_to_check; ++i) {
		to_check[i] = cl->members[i * step];
		vertex_markers[to_check[i]] = curr_marker;
	}

	double max_dist = -1.0;
	while (num_to_check > 0) {
		if (!iscc_get_max_dist(max_dist_object, num_to_check, to_check, max_indices, max_dists)) {
			free(to_check);
			free(max_indices);
			free(max_dists);
			iscc_close_max_dist_object(&max_dist_object);
			return iscc_make_error(SCC_ER_DIST_SEARCH_ERROR);
		}

		size_t write_in_to_check = 0;
		for (size_t i = 0; i < num_to_check; ++i) {
			if (max_dists[i] > max_dist) {
				max_dist = max_dists[i];
				*center1 = to_check[i];
				*center2 = max_indices[i];
			}

			if (vertex_markers[max_indices[i]] != curr_marker) {
				vertex_markers[max_indices[i]] = curr_marker;
				to_check[write_in_to_check] = max_indices[i];
				++write_in_to_check;
			}
		}

		num_to_check = write_in_to_check;
	}

	free(to_check);
	free(max_indices);
	free(max_dists);
	if (!iscc_close_max_dist_object(&max_dist_object)) return iscc_make_error(SCC_ER_DIST_SEARCH_ERROR);

	return iscc_no_error();
}


static inline uint_fast16_t iscc_gr_get_next_marker(iscc_gr_ClusterItem* const cl,
                                                    uint_fast16_t* const vertex_markers)
{
	assert(cl != NULL);
	assert(cl->members != NULL);
	assert(vertex_markers != NULL);

	if (cl->marker == UINT_FAST16_MAX) {
		for (size_t i = 0; i < cl->size; ++i) {
			vertex_markers[cl->members[i]] = 0;
			cl->marker = 0;
		}
	} 
	++(cl->marker);
	return cl->marker;
}


static scc_ErrorCode iscc_gr_populate_dist_lists(scc_DataSetObject* const data_set_object,
                                                 const iscc_gr_ClusterItem* const cl,
                                                 const scc_Dpid center1,
                                                 const scc_Dpid center2,
                                                 iscc_gr_DistanceEdge dist_store1[const static cl->size],
                                                 iscc_gr_DistanceEdge dist_store2[const static cl->size])
{
	assert(iscc_check_data_set_object(data_set_object, 0));
	assert(cl != NULL);
	assert(cl->size >= 2);
	assert(cl->members != NULL);
	assert(dist_store1 != NULL);
	assert(dist_store2 != NULL);

	double* const output_dists = malloc(sizeof(double[2 * cl->size]));
	if (output_dists == NULL) return iscc_make_error(SCC_ER_NO_MEMORY);

	scc_Dpid query_indices[2] = { center1, center2 };

	if (!iscc_get_dist_row(data_set_object,
	                       cl->size,
	                       cl->members,
	                       2,
	                       query_indices,
	                       output_dists)) {
		free(output_dists);
		return iscc_make_error(SCC_ER_DIST_SEARCH_ERROR);
	}

	iscc_gr_sort_dist_list(cl, center1, output_dists, dist_store1);
	iscc_gr_sort_dist_list(cl, center2, output_dists + cl->size, dist_store2);

	free(output_dists);

	return iscc_no_error();
}


static inline void iscc_gr_sort_dist_list(const iscc_gr_ClusterItem* const cl,
                                          const scc_Dpid center,
                                          const double output_dists[const static cl->size],
                                          iscc_gr_DistanceEdge dist_edge_store[const static cl->size])
{
	assert(cl != NULL);
	assert(cl->size >= 2);
	assert(cl->members != NULL);
	assert(output_dists != NULL);
	assert(dist_edge_store != NULL);

	iscc_gr_DistanceEdge* write_dist_edge = dist_edge_store + 1;
	for (size_t i = 0; i < cl->size; ++i) {
		if (cl->members[i] == center) continue;
		write_dist_edge->head = cl->members[i];
		write_dist_edge->distance = output_dists[i];
		++write_dist_edge;
	}

	assert(write_dist_edge == dist_edge_store + cl->size);

    qsort(dist_edge_store + 1, cl->size - 1, sizeof(iscc_gr_DistanceEdge), iscc_gr_compare_dist_edges);

    for (size_t i = 0; i < cl->size; ++i) {
    	dist_edge_store[i].next_dist = dist_edge_store + i + 1;
    }
	dist_edge_store[cl->size - 1].next_dist = NULL;
}


static int iscc_gr_compare_dist_edges(const void* const a,
                                      const void* const b)
{
    const double dist_a = ((const iscc_gr_DistanceEdge*)a)->distance;
    const double dist_b = ((const iscc_gr_DistanceEdge*)b)->distance;
 
    if (dist_a < dist_b) return -1;
    if (dist_a > dist_b) return 1;
    return 0;
}


static inline void iscc_gr_move_v_to_cluster(const scc_Dpid id,
                                             iscc_gr_ClusterItem* const cl,
                                             uint_fast16_t* const vertex_markers,
                                             const uint_fast16_t curr_marker)
{
	assert(cl != NULL);
	assert(cl->members != NULL);
	assert(vertex_markers != NULL);
	assert(vertex_markers[id] != curr_marker);

	vertex_markers[id] = curr_marker;
	cl->members[cl->size] = id;
	++(cl->size);
}


static inline iscc_gr_DistanceEdge* iscc_gr_get_next_dist(iscc_gr_DistanceEdge* const prev_dist,
                                                          const uint_fast16_t* const vertex_markers,
                                                          const uint_fast16_t curr_marker)
{	
	assert(prev_dist != NULL);
	assert(prev_dist->next_dist != NULL); // We should never reach the end!
	assert(vertex_markers != NULL);

	while(vertex_markers[prev_dist->next_dist->head] == curr_marker) { // Vertex has already been assigned to a new cluster
		prev_dist->next_dist = prev_dist->next_dist->next_dist;
		assert(prev_dist->next_dist != NULL); // We should never reach the end!
	}

	return prev_dist->next_dist;
}


static inline iscc_gr_DistanceEdge* iscc_gr_get_next_k_nn(iscc_gr_DistanceEdge* prev_dist,
                                                          const size_t k,
                                                          scc_Dpid out_dist_array[const static k],
                                                          const uint_fast16_t* const vertex_markers,
                                                          const uint_fast16_t curr_marker)
{
	assert(prev_dist != NULL);
	assert(prev_dist->next_dist != NULL); // We should never reach the end!
	assert(k > 0);
	assert(out_dist_array != NULL);
	assert(vertex_markers != NULL);

	for (size_t found = 0; found < k; ++found) {
		prev_dist = iscc_gr_get_next_dist(prev_dist, vertex_markers, curr_marker);
		out_dist_array[found] = prev_dist->head;
	}

	return prev_dist;
}
