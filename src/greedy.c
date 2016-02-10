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


#include "../include/greedy.h"

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "../include/config.h"
#include "../include/clustering.h"
#include "nn_search.h"


// ==============================================================================
// Internal structs
// ==============================================================================

typedef struct iscc_gr_DistanceEdge iscc_gr_DistanceEdge;
struct iscc_gr_DistanceEdge {
	scc_Vid head;
	scc_Distance distance;
	iscc_gr_DistanceEdge* next_dist;
};

typedef struct iscc_gr_ClusterItem iscc_gr_ClusterItem;
struct iscc_gr_ClusterItem {
	size_t size;
	scc_Vid* members;
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

static const iscc_gr_ClusterItem ISCC_GR_NULL_CLUSTER_ITEM = { 0, NULL };
static const iscc_gr_ClusterStack ISCC_GR_NULL_CLUSTER_STACK = { 0, 0, NULL };
static bool* iscc_gr_vertex_mark = NULL;


// ==============================================================================
// Internal function prototypes
// ==============================================================================

static iscc_gr_ClusterStack iscc_gr_init_cl_stack(const scc_Clustering* input_clustering);

static iscc_gr_ClusterStack iscc_gr_empty_cl_stack(size_t num_vertices);

static bool iscc_gr_run_greedy_clustering(scc_DataSetObject* data_set_object,
                                          iscc_gr_ClusterStack* cl_stack,
                                          scc_Clustering* input_clustering,
                                          size_t k,
                                          bool batch_assign);

static inline iscc_gr_ClusterItem* iscc_gr_peek_at_stack(const iscc_gr_ClusterStack* cl_stack);

static inline iscc_gr_ClusterItem iscc_gr_pop_from_stack(iscc_gr_ClusterStack* cl_stack);

static bool iscc_gr_push_to_stack(iscc_gr_ClusterStack* cl_stack,
                                  iscc_gr_ClusterItem cl);

static iscc_gr_ClusterItem iscc_gr_break_cluster_into_two(scc_DataSetObject* data_set_object,
                                                          iscc_gr_ClusterItem* cluster_to_break,
                                                          size_t k,
                                                          bool batch_assign);

static bool iscc_gr_find_centers(scc_DataSetObject* data_set_object,
                                 const iscc_gr_ClusterItem* cl,
                                 scc_Vid* center1,
                                 scc_Vid* center2);

static bool iscc_gr_populate_dist_lists(scc_DataSetObject* data_set_object,
                                        const iscc_gr_ClusterItem* cl,
                                        scc_Vid center1,
                                        scc_Vid center2,
                                        iscc_gr_DistanceEdge dist_store1[static cl->size],
                                        iscc_gr_DistanceEdge dist_store2[static cl->size]);

static inline void iscc_gr_sort_dist_list(const iscc_gr_ClusterItem* cl,
                                          scc_Vid center,
                                          const scc_Distance output_dists[static cl->size],
                                          iscc_gr_DistanceEdge dist_edge_store[static cl->size]);

static int iscc_gr_compare_dist_edges(const void* a,
                                      const void* b);

static inline void iscc_gr_move_v_to_cluster(scc_Vid v_id,
                                             iscc_gr_ClusterItem* cl);

static inline iscc_gr_DistanceEdge* iscc_gr_get_next_dist(iscc_gr_DistanceEdge* prev_dist);

static inline iscc_gr_DistanceEdge* iscc_gr_get_next_k_nn(iscc_gr_DistanceEdge* prev_dist,
                                                          size_t k,
                                                          scc_Vid out_dist_array[static k]);


// ==============================================================================
// External function implementations
// ==============================================================================

bool scc_greedy_break_clustering(scc_Clustering* const input_clustering,
                                 scc_DataSetObject* const data_set_object,
                                 const size_t k,
                                 const bool batch_assign)
{
	if (data_set_object == NULL) return false;
	if (k < 2) return false;
	if (input_clustering == NULL) return false;
	if (input_clustering->num_clusters == 0) return false;
	if (input_clustering->cluster_label == NULL) return false;
	if (input_clustering->vertices < 2) return false;

	assert(iscc_gr_vertex_mark == NULL);

	// Allocate global vertex mark storage
	iscc_gr_vertex_mark = calloc(input_clustering->vertices, sizeof(bool));

	iscc_gr_ClusterStack cl_stack = iscc_gr_init_cl_stack(input_clustering);

	if ((iscc_gr_vertex_mark == NULL) || (cl_stack.clusters == NULL)) {
		free(iscc_gr_vertex_mark);
		iscc_gr_vertex_mark = NULL;
		free(cl_stack.clusters);
		return false;
	}

	if (!iscc_gr_run_greedy_clustering(data_set_object, &cl_stack, input_clustering, k, batch_assign)) {
		for (size_t i = 0; i < cl_stack.items; ++i) {
			free(cl_stack.clusters[i].members);
		}
		free(iscc_gr_vertex_mark);
		iscc_gr_vertex_mark = NULL;
		free(cl_stack.clusters);
		return false;
	}

	free(iscc_gr_vertex_mark);
	iscc_gr_vertex_mark = NULL;
	free(cl_stack.clusters);

	return true;
}


scc_Clustering scc_get_greedy_clustering(scc_DataSetObject* const data_set_object,
                                         const size_t k,
                                         const bool batch_assign)
{
	if (data_set_object == NULL) return SCC_NULL_CLUSTERING;
	if (k < 2) return SCC_NULL_CLUSTERING;

	size_t num_vertices = scc_get_data_point_count(data_set_object);
	if (num_vertices < 2) return SCC_NULL_CLUSTERING;

	assert(iscc_gr_vertex_mark == NULL);

	// Allocate global vertex mark storage
	iscc_gr_vertex_mark = calloc(num_vertices, sizeof(bool));

	scc_Clustering out_cl =  {
		.vertices = num_vertices,
		.num_clusters = 0,
		.external_labels = false,
		.cluster_label = malloc(sizeof(scc_Clabel[num_vertices])),
	};

	iscc_gr_ClusterStack cl_stack = iscc_gr_empty_cl_stack(num_vertices);

	if ((iscc_gr_vertex_mark == NULL) || (out_cl.cluster_label == NULL) || (cl_stack.clusters == NULL)) {
		free(iscc_gr_vertex_mark);
		iscc_gr_vertex_mark = NULL;
		free(out_cl.cluster_label);
		free(cl_stack.clusters);
		return SCC_NULL_CLUSTERING;
	}

	if (!iscc_gr_run_greedy_clustering(data_set_object, &cl_stack, &out_cl, k, batch_assign)) {
		for (size_t i = 0; i < cl_stack.items; ++i) {
			free(cl_stack.clusters[i].members);
		}
		free(iscc_gr_vertex_mark);
		iscc_gr_vertex_mark = NULL;
		free(out_cl.cluster_label);
		free(cl_stack.clusters);
		return SCC_NULL_CLUSTERING;
	}

	free(iscc_gr_vertex_mark);
	iscc_gr_vertex_mark = NULL;
	free(cl_stack.clusters);

	return out_cl;
}


// ==============================================================================
// Internal function implementations 
// ==============================================================================

static iscc_gr_ClusterStack iscc_gr_init_cl_stack(const scc_Clustering* const input_clustering)
{
	assert(input_clustering != NULL);
	assert(input_clustering->num_clusters > 0);
	assert(input_clustering->cluster_label != NULL);
	assert(input_clustering->vertices >= 2);

	iscc_gr_ClusterStack cl_stack;
	cl_stack.capacity = input_clustering->num_clusters + (size_t) (10 * log2((double) input_clustering->vertices)),
	cl_stack.items = input_clustering->num_clusters,
	cl_stack.clusters = calloc(cl_stack.capacity, sizeof(iscc_gr_ClusterItem));
	if (cl_stack.clusters == NULL) return ISCC_GR_NULL_CLUSTER_STACK;

	for (size_t v = 0; v < input_clustering->vertices; ++v) {
		if (input_clustering->cluster_label[v] != SCC_CLABEL_NA) {
			++(cl_stack.clusters[input_clustering->cluster_label[v]].size);
		}
	}

	for (size_t c = 0; c < cl_stack.items; ++c) {
		if (cl_stack.clusters[c].size == 0) continue;
		cl_stack.clusters[c].members = malloc(sizeof(scc_Vid[cl_stack.clusters[c].size]));
		if (cl_stack.clusters[c].members == NULL) {
			for (size_t c_free = 0; c_free < c; ++c_free) {
				free(cl_stack.clusters[c_free].members);
			}
			free(cl_stack.clusters);
			return ISCC_GR_NULL_CLUSTER_STACK;
		}
		cl_stack.clusters[c].size = 0;
	}

	for (scc_Vid v = 0; v < input_clustering->vertices; ++v) {
		if (input_clustering->cluster_label[v] != SCC_CLABEL_NA) {
			iscc_gr_ClusterItem* cl = &cl_stack.clusters[input_clustering->cluster_label[v]];
			cl->members[cl->size] = v;
			++(cl->size);
		}
	}

	return cl_stack;
}


static iscc_gr_ClusterStack iscc_gr_empty_cl_stack(const size_t num_vertices)
{
	assert(num_vertices >= 2);

	iscc_gr_ClusterStack cl_stack;
	cl_stack.capacity = 1 + (size_t) (20 * log2((double) num_vertices));
	cl_stack.items = 1;
	cl_stack.clusters = malloc(sizeof(iscc_gr_ClusterItem[cl_stack.capacity]));

	scc_Vid* const tmp_members = malloc(sizeof(scc_Vid[num_vertices]));

	if ((cl_stack.clusters == NULL) || (tmp_members == NULL)) {
		free(cl_stack.clusters);
		free(tmp_members);
		return ISCC_GR_NULL_CLUSTER_STACK;
	}

	for (scc_Vid v = 0; v < num_vertices; ++v) {
		tmp_members[v] = v;
	}

	cl_stack.clusters[0].size = num_vertices;
	cl_stack.clusters[0].members = tmp_members;

	return cl_stack;
}


static bool iscc_gr_run_greedy_clustering(scc_DataSetObject* const data_set_object,
                                          iscc_gr_ClusterStack* const cl_stack,
                                          scc_Clustering* const input_clustering,
                                          const size_t k,
                                          const bool batch_assign)
{
	assert(iscc_gr_vertex_mark != NULL);
	assert(cl_stack->items > 0);
	assert(cl_stack->clusters != NULL);
	assert(input_clustering->cluster_label != NULL);

	scc_Clabel curr_label = 0;
	while (iscc_gr_peek_at_stack(cl_stack) != NULL) {

		if (iscc_gr_peek_at_stack(cl_stack)->size < 2 * k) {
			iscc_gr_ClusterItem unbreakable_cluster = iscc_gr_pop_from_stack(cl_stack);
			if (unbreakable_cluster.size > 0) {
				for (size_t v = 0; v < unbreakable_cluster.size; ++v) {
					assert(unbreakable_cluster.members[v] < input_clustering->vertices);
					input_clustering->cluster_label[unbreakable_cluster.members[v]] = curr_label;
				}
				++curr_label;
				free(unbreakable_cluster.members);
			}
		} else {
			iscc_gr_ClusterItem new_cluster = iscc_gr_break_cluster_into_two(data_set_object, iscc_gr_peek_at_stack(cl_stack), k, batch_assign);
			if (new_cluster.members == NULL) return false;
			if (!iscc_gr_push_to_stack(cl_stack, new_cluster)) {
				free(new_cluster.members);
				return false;
			}
		}
	}

	input_clustering->num_clusters = curr_label;

	assert(cl_stack->items == 0);

	return true;
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
	assert(cl_stack->clusters != NULL);
	assert(cl_stack->items != 0);

	--(cl_stack->items);
	return cl_stack->clusters[cl_stack->items];
}


static bool iscc_gr_push_to_stack(iscc_gr_ClusterStack* const cl_stack,
                                  const iscc_gr_ClusterItem cl)
{
	assert(cl_stack != NULL);
	assert(cl_stack->clusters != NULL);

	if (cl_stack->items == cl_stack->capacity) {
		size_t new_capacity = cl_stack->capacity + 1 + (size_t) (20 * log2((double) cl.size));
		iscc_gr_ClusterItem* const clusters_tmp = realloc(cl_stack->clusters, sizeof(iscc_gr_ClusterItem[new_capacity]));
		if (!clusters_tmp) return false;
		cl_stack->capacity = new_capacity;
		cl_stack->clusters = clusters_tmp;
	}

	cl_stack->clusters[cl_stack->items] = cl;
	++(cl_stack->items);

	return true;
}


static iscc_gr_ClusterItem iscc_gr_break_cluster_into_two(scc_DataSetObject* const data_set_object,
                                                          iscc_gr_ClusterItem* const cluster_to_break,
                                                          const size_t k,
                                                          const bool batch_assign)
{
	assert(cluster_to_break != NULL);
	assert(k >= 2);
	assert(cluster_to_break->size >= 2 * k);
	assert(iscc_gr_vertex_mark != NULL);

	const size_t old_cluster_size = cluster_to_break->size;

	scc_Vid center1, center2;
	if (!iscc_gr_find_centers(data_set_object, cluster_to_break, &center1, &center2)) return ISCC_GR_NULL_CLUSTER_ITEM;

	for (size_t i = 0; i < old_cluster_size; ++i) {
		assert(iscc_gr_vertex_mark[cluster_to_break->members[i]]);
	}

	iscc_gr_DistanceEdge* const dist_store1 = malloc(sizeof(iscc_gr_DistanceEdge[old_cluster_size]));
	iscc_gr_DistanceEdge* const dist_store2 = malloc(sizeof(iscc_gr_DistanceEdge[old_cluster_size]));

	scc_Vid* const k_nn_array1 = malloc(sizeof(scc_Vid[k]));
	scc_Vid* const k_nn_array2 = malloc(sizeof(scc_Vid[k]));

	iscc_gr_ClusterItem new_cluster = {
		.size = 0,
		.members = malloc(sizeof(scc_Vid[old_cluster_size])),
	};

	if ((dist_store1 == NULL) || (dist_store2 == NULL) || (k_nn_array1 == NULL) ||
	        (k_nn_array2 == NULL) || (new_cluster.members == NULL) ||
	        !iscc_gr_populate_dist_lists(data_set_object, cluster_to_break, center1, center2, dist_store1, dist_store2)) {
		free(dist_store1);
		free(dist_store2);
		free(k_nn_array1);
		free(k_nn_array2);
		free(new_cluster.members);
		return ISCC_GR_NULL_CLUSTER_ITEM;
	}

	iscc_gr_ClusterItem* const cluster1 = cluster_to_break;
	cluster1->size = 0;

	iscc_gr_ClusterItem* const cluster2 = &new_cluster;

	iscc_gr_move_v_to_cluster(center1, cluster1);
	iscc_gr_move_v_to_cluster(center2, cluster2);

	iscc_gr_DistanceEdge* last_assigned_dist1 = dist_store1;
	iscc_gr_DistanceEdge* last_assigned_dist2 = dist_store2;

	iscc_gr_DistanceEdge* temp_dist1;
	iscc_gr_DistanceEdge* temp_dist2;

	temp_dist1 = iscc_gr_get_next_k_nn(last_assigned_dist1, k - 1, k_nn_array1);
	temp_dist2 = iscc_gr_get_next_k_nn(last_assigned_dist2, k - 1, k_nn_array2);

	if (temp_dist1->distance >= temp_dist2->distance) {
		for (size_t i = 0; i < k - 1; ++i) {
			iscc_gr_move_v_to_cluster(k_nn_array1[i], cluster1);
		}
		last_assigned_dist1 = temp_dist1;

		last_assigned_dist2 = iscc_gr_get_next_k_nn(last_assigned_dist2, k - 1, k_nn_array2);
		for (size_t i = 0; i < k - 1; ++i) {
			iscc_gr_move_v_to_cluster(k_nn_array2[i], cluster2);
		}
	} else {
		for (size_t i = 0; i < k - 1; ++i) {
			iscc_gr_move_v_to_cluster(k_nn_array2[i], cluster2);
		}
		last_assigned_dist2 = temp_dist2;

		last_assigned_dist1 = iscc_gr_get_next_k_nn(last_assigned_dist1, k - 1, k_nn_array1);
		for (size_t i = 0; i < k - 1; ++i) {
			iscc_gr_move_v_to_cluster(k_nn_array1[i], cluster1);
		}
	}

	assert(cluster1->size == k && cluster2->size == k);

	if (batch_assign) {
		size_t num_assign_in_batch = k;
		for (size_t assigned = 2 * k; assigned < old_cluster_size; assigned += num_assign_in_batch) {
			
			if (assigned + num_assign_in_batch > old_cluster_size) {
				num_assign_in_batch = old_cluster_size - assigned;
			}

			temp_dist1 = iscc_gr_get_next_k_nn(last_assigned_dist1, num_assign_in_batch, k_nn_array1);
			temp_dist2 = iscc_gr_get_next_k_nn(last_assigned_dist2, num_assign_in_batch, k_nn_array2);

			if (temp_dist1->distance <= temp_dist2->distance) {
				for (size_t i = 0; i < num_assign_in_batch; ++i) {
					iscc_gr_move_v_to_cluster(k_nn_array1[i], cluster1);
				}
				last_assigned_dist1 = temp_dist1;
			} else {
				for (size_t i = 0; i < num_assign_in_batch; ++i) {
					iscc_gr_move_v_to_cluster(k_nn_array2[i], cluster2);
				}
				last_assigned_dist2 = temp_dist2;
			}
		}

	} else {
		for (size_t assigned = 2 * k; assigned < old_cluster_size; ++assigned) {

			temp_dist1 = iscc_gr_get_next_dist(last_assigned_dist1);
			temp_dist2 = iscc_gr_get_next_dist(last_assigned_dist2);

			if (temp_dist1->distance <= temp_dist2->distance) {
				iscc_gr_move_v_to_cluster(temp_dist1->head, cluster1);
				last_assigned_dist1 = temp_dist1;
			} else {
				iscc_gr_move_v_to_cluster(temp_dist2->head, cluster2);
				last_assigned_dist2 = temp_dist2;
			}
		}

	}

	free(dist_store1);
	free(dist_store2);
	free(k_nn_array1);
	free(k_nn_array2);

	assert(cluster1->size >= k);
	assert(cluster2->size >= k);

	scc_Vid* const members_tmp1 = realloc(cluster1->members, sizeof(scc_Vid[cluster1->size]));
	scc_Vid* const members_tmp2 = realloc(cluster2->members, sizeof(scc_Vid[cluster2->size]));
	if ((members_tmp1 == NULL) || (members_tmp2 == NULL)) {
		free(members_tmp1);
		free(members_tmp2);
		if (cluster2->members != members_tmp2) free(cluster2->members);
		return ISCC_GR_NULL_CLUSTER_ITEM;
	}
	cluster1->members = members_tmp1;
	cluster2->members = members_tmp2;

	return new_cluster;
}


static bool iscc_gr_find_centers(scc_DataSetObject* const data_set_object,
                                 const iscc_gr_ClusterItem* const cl,
                                 scc_Vid* const center1,
                                 scc_Vid* const center2)
{
	assert(cl != NULL);
	assert(cl->size >= 4);
	assert(center1 != NULL && center2 != NULL);
	assert(iscc_gr_vertex_mark != NULL);

	for (size_t i = 0; i < cl->size; ++i) {
		assert(!iscc_gr_vertex_mark[cl->members[i]]);
	}

	size_t step = cl->size / 1000;
	if (step < 2) step = 2;
	size_t num_to_check = 1 + (cl->size - 1) / step;

	scc_Vid* to_check = malloc(sizeof(scc_Vid[num_to_check]));
	scc_Vid* max_indices = malloc(sizeof(scc_Vid[num_to_check]));
	scc_Distance* max_dists = malloc(sizeof(scc_Distance[num_to_check]));
	scc_MaxDistObject* max_dist_object = scc_init_max_dist_object(data_set_object, cl->size, cl->members, 2 * num_to_check);
	if ((to_check == NULL) || (max_indices == NULL) ||
	        (max_dists == NULL) || (max_dist_object == NULL)) {
		free(to_check);
		free(max_indices);
		free(max_dists);
		scc_close_max_dist_object(max_dist_object);
		return false;
	}

	for (size_t i = 0; i < num_to_check; ++i) {
		to_check[i] = cl->members[i * step];
		iscc_gr_vertex_mark[to_check[i]] = true;
	}

	scc_Distance max_dist = -1.0;
	while (num_to_check > 0) {
		if (!scc_get_max_dist(max_dist_object, num_to_check, to_check, max_indices, max_dists)) {
			free(to_check);
			free(max_indices);
			free(max_dists);
			scc_close_max_dist_object(max_dist_object);
			return false;
		}

		size_t write_in_to_check = 0;
		for (size_t i = 0; i < num_to_check; ++i) {
			if (max_dists[i] > max_dist) {
				max_dist = max_dists[i];
				*center1 = to_check[i];
				*center2 = max_indices[i];
			}

			if (!iscc_gr_vertex_mark[max_indices[i]]) {
				iscc_gr_vertex_mark[max_indices[i]] = true;
				to_check[write_in_to_check] = max_indices[i];
				++write_in_to_check;
			}
		}

		num_to_check = write_in_to_check;
	}

	for (size_t i = 0; i < cl->size; ++i) {
		iscc_gr_vertex_mark[cl->members[i]] = true;
	}

	free(to_check);
	free(max_indices);
	free(max_dists);
	if (!scc_close_max_dist_object(max_dist_object)) return false;

	#ifdef SCC_STABLE_CLUSTERING
		if (*center1 > *center2) {
			const scc_Vid tmp_center = *center1;
			*center1 = *center2;
			*center2 = tmp_center;
		}
	#endif

	return true;
}


static bool iscc_gr_populate_dist_lists(scc_DataSetObject* const data_set_object,
                                        const iscc_gr_ClusterItem* const cl,
                                        const scc_Vid center1,
                                        const scc_Vid center2,
                                        iscc_gr_DistanceEdge dist_store1[const static cl->size],
                                        iscc_gr_DistanceEdge dist_store2[const static cl->size])
{
	assert(cl->size > 2);
	assert(dist_store1 != NULL);
	assert(dist_store2 != NULL);

	scc_Distance* const output_dists = malloc(sizeof(scc_Distance[2 * cl->size]));
	scc_DistColObject* const dist_column_object = scc_init_dist_column_object(data_set_object, cl->size, cl->members, 2);
	if ((output_dists == NULL) || (dist_column_object == NULL)) {
		free(output_dists);
		scc_close_dist_column_object(dist_column_object);
		return false;
	}

	scc_Vid query_indices[2] = { center1, center2 };
	if (!scc_get_dist_row(dist_column_object, 2, query_indices, output_dists)) {
		free(output_dists);
		scc_close_dist_column_object(dist_column_object);
		return false;
	}

	if (!scc_close_dist_column_object(dist_column_object)) {
		free(output_dists);
		return false;
	}

	iscc_gr_sort_dist_list(cl, center1, output_dists, dist_store1);
	iscc_gr_sort_dist_list(cl, center2, output_dists + cl->size, dist_store2);

	free(output_dists);

	return true;
}


static inline void iscc_gr_sort_dist_list(const iscc_gr_ClusterItem* const cl,
                                          const scc_Vid center,
                                          const scc_Distance output_dists[const static cl->size],
                                          iscc_gr_DistanceEdge dist_edge_store[const static cl->size])
{
	assert(cl->size > 2);
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
    const scc_Distance dist_a = ((const iscc_gr_DistanceEdge*)a)->distance;
    const scc_Distance dist_b = ((const iscc_gr_DistanceEdge*)b)->distance;
 
    if (dist_a < dist_b) return -1;
    if (dist_a > dist_b) return 1;
    return 0;
}


static inline void iscc_gr_move_v_to_cluster(const scc_Vid v_id,
                                             iscc_gr_ClusterItem* const cl)
{
	assert(iscc_gr_vertex_mark != NULL);
	assert(iscc_gr_vertex_mark[v_id]);
	iscc_gr_vertex_mark[v_id] = false;
	cl->members[cl->size] = v_id;
	++(cl->size);
}


static inline iscc_gr_DistanceEdge* iscc_gr_get_next_dist(iscc_gr_DistanceEdge* const prev_dist)
{	
	assert(prev_dist != NULL);
	assert(prev_dist->next_dist != NULL); // We should never reach the end!
	assert(iscc_gr_vertex_mark != NULL);

	while(!iscc_gr_vertex_mark[prev_dist->next_dist->head]) { // Vertex has already been assigned to a new cluster
		prev_dist->next_dist = prev_dist->next_dist->next_dist;
		assert(prev_dist->next_dist != NULL); // We should never reach the end!
	}

	return prev_dist->next_dist;
}


static inline iscc_gr_DistanceEdge* iscc_gr_get_next_k_nn(iscc_gr_DistanceEdge* prev_dist,
                                                          const size_t k,
                                                          scc_Vid out_dist_array[const static k])
{
	assert(k > 0);
	assert(out_dist_array != NULL);

	for (size_t found = 0; found < k; ++found) {
		prev_dist = iscc_gr_get_next_dist(prev_dist);
		out_dist_array[found] = prev_dist->head;
	}

	return prev_dist;
}
