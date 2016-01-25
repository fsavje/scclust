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
#include "../include/search.h"


// ==============================================================================
// Internal structs
// ==============================================================================

typedef struct iscc_gr_DistanceEdge iscc_gr_DistanceEdge;
struct iscc_gr_DistanceEdge {
	scc_Vid head;
	scc_Distance distance;
	iscc_gr_DistanceEdge* prev_dist;
	iscc_gr_DistanceEdge* next_dist;
};


typedef struct iscc_gr_DistanceEdgeList iscc_gr_DistanceEdgeList;
struct iscc_gr_DistanceEdgeList {
	scc_Clabel mark;
	iscc_gr_DistanceEdge* distances_store;
	iscc_gr_DistanceEdge* first_dist;
	iscc_gr_DistanceEdge* last_dist;
};


typedef struct iscc_gr_Vertex iscc_gr_Vertex;
struct iscc_gr_Vertex {
	scc_Clabel sub_label;
	iscc_gr_DistanceEdgeList* dist_list;
};


typedef struct iscc_gr_Cluster iscc_gr_Cluster;
struct iscc_gr_Cluster {
	size_t size;
	scc_Vid* members;
};


typedef struct iscc_gr_ClusterStack iscc_gr_ClusterStack;
struct iscc_gr_ClusterStack {
	size_t capacity;
	size_t first_empty_pos;
	iscc_gr_Cluster* clusters;
};


// ==============================================================================
// Internal variables
// ==============================================================================

static const iscc_gr_Cluster ISCC_GR_NULL_CLUSTER = { 0, NULL };
static const iscc_gr_ClusterStack ISCC_GR_NULL_CLUSTER_STACK = { 0, 0, NULL };

static iscc_gr_Vertex* iscc_gr_vertex_store = NULL;
static void* iscc_gr_data_object = NULL;
static void* iscc_gr_search_object = NULL;
static scc_Vid* iscc_gr_index_scratch = NULL;
static scc_Distance* iscc_gr_dist_scratch = NULL;


// ==============================================================================
// Internal function prototypes
// ==============================================================================

static iscc_gr_ClusterStack iscc_gr_init_cl_stack(const scc_Clustering* input_clustering);

static iscc_gr_ClusterStack iscc_gr_empty_cl_stack(size_t num_vertices);

static inline iscc_gr_Cluster* iscc_gr_peek_at_stack(const iscc_gr_ClusterStack* cl_stack);

static inline iscc_gr_Cluster iscc_gr_pop_from_stack(iscc_gr_ClusterStack* cl_stack);

static bool iscc_gr_push_to_stack(iscc_gr_ClusterStack* cl_stack,
                                  iscc_gr_Cluster cl);

static void iscc_gr_panic_free_everything(iscc_gr_ClusterStack* cl_stack,
                                          size_t num_vertices);

static iscc_gr_Cluster iscc_gr_break_cluster_into_two(iscc_gr_Cluster* cluster_to_break,
                                                      size_t k,
                                                      bool batch_assign);

static bool iscc_gr_find_centers(const iscc_gr_Cluster* cl,
                                 size_t k,
                                 scc_Vid* center1,
                                 scc_Vid* center2);

static bool iscc_gr_init_search_object(const iscc_gr_Cluster* cl);

static bool iscc_gr_free_search_object();

static bool iscc_gr_populate_dist_list(scc_Vid v_id,
                                       const iscc_gr_Cluster* cl);

static inline iscc_gr_DistanceEdge* iscc_gr_get_first_dist(iscc_gr_DistanceEdgeList* dist_list,
                                                           scc_Clabel required_sub_label);

static inline iscc_gr_DistanceEdge* iscc_gr_get_last_dist(iscc_gr_DistanceEdgeList* dist_list,
                                                          scc_Clabel required_sub_label);

static inline iscc_gr_DistanceEdge* iscc_gr_get_next_dist(iscc_gr_DistanceEdge* de,
                                                          scc_Clabel required_sub_label);

static inline iscc_gr_DistanceEdge* iscc_gr_get_first_k_nn(iscc_gr_DistanceEdgeList* dist_list,
                                                           size_t k,
                                                           scc_Vid out_dist_array[static k],
                                                           scc_Clabel required_sub_label);

static inline iscc_gr_DistanceEdge* iscc_gr_get_next_k_nn(iscc_gr_DistanceEdge* prev_dist,
                                                          size_t k,
                                                          scc_Vid out_dist_array[static k],
                                                          scc_Clabel required_sub_label);

static inline void iscc_gr_move_v_to_cluster(scc_Vid v_id,
                                             iscc_gr_Cluster* cl,
                                             scc_Clabel sub_label);

static bool iscc_gr_adjust_cluster_memory(iscc_gr_Cluster* cl,
                                          size_t k);


// ==============================================================================
// External function implementations
// ==============================================================================

bool scc_get_greedy_clustering(void* const data_object,
                               const size_t k,
                               scc_Clustering* const input_clustering,
                               const bool batch_assign)
{
	if (data_object == NULL) return false;
	if (k < 2) return false;
	if (input_clustering == NULL) return false;
	if (input_clustering->vertices < 2) return false;

	assert(iscc_gr_vertex_store == NULL);
	assert(iscc_gr_data_object == NULL);
	assert(iscc_gr_search_object == NULL);
	assert(iscc_gr_index_scratch == NULL);
	assert(iscc_gr_dist_scratch == NULL);

	// Allocate global vertex info storage
	iscc_gr_vertex_store = calloc(input_clustering->vertices, sizeof(iscc_gr_Vertex));
	if (iscc_gr_vertex_store == NULL) return false;

	iscc_gr_ClusterStack cl_stack;
	if (input_clustering->cluster_label == NULL) {
		input_clustering->external_labels = false;
		input_clustering->cluster_label = malloc(sizeof(scc_Clabel[input_clustering->vertices]));
		if (input_clustering->cluster_label == NULL) {
			free(iscc_gr_vertex_store);
			return false;
		}
		cl_stack = iscc_gr_empty_cl_stack(input_clustering->vertices);
	} else {
		cl_stack = iscc_gr_init_cl_stack(input_clustering);
	}

	if (cl_stack.clusters == NULL) {
		free(iscc_gr_vertex_store);
		iscc_gr_vertex_store = NULL;
		return false;
	}

	assert(input_clustering->cluster_label != NULL);

	iscc_gr_data_object = data_object;

	scc_Clabel curr_label = 0;
	while (iscc_gr_peek_at_stack(&cl_stack) != NULL) {

		if (iscc_gr_peek_at_stack(&cl_stack)->size < 2 * k) {
			iscc_gr_Cluster unbreakable_cluster = iscc_gr_pop_from_stack(&cl_stack);
			for (size_t v = 0; v < unbreakable_cluster.size; ++v) {
				input_clustering->cluster_label[unbreakable_cluster.members[v]] = curr_label;
			}
			++curr_label;
			free(unbreakable_cluster.members);

		} else {
			iscc_gr_Cluster new_cluster = iscc_gr_break_cluster_into_two(iscc_gr_peek_at_stack(&cl_stack), k, batch_assign);
			if (new_cluster.members == NULL) iscc_gr_panic_free_everything(&cl_stack, input_clustering->vertices);
			if (!iscc_gr_push_to_stack(&cl_stack, new_cluster)) iscc_gr_panic_free_everything(&cl_stack, input_clustering->vertices);
		}
	}

	input_clustering->num_clusters = curr_label;

	assert(cl_stack.first_empty_pos == 0);

	free(cl_stack.clusters);
	free(iscc_gr_vertex_store);
	iscc_gr_vertex_store = NULL;
	iscc_gr_data_object = NULL;

	assert(iscc_gr_vertex_store == NULL);
	assert(iscc_gr_data_object == NULL);
	assert(iscc_gr_search_object == NULL);
	assert(iscc_gr_index_scratch == NULL);
	assert(iscc_gr_dist_scratch == NULL);

	return true;
}


// ==============================================================================
// Internal function implementations 
// ==============================================================================

static iscc_gr_ClusterStack iscc_gr_init_cl_stack(const scc_Clustering* const input_clustering)
{
	iscc_gr_ClusterStack cl_stack = {
		.capacity = input_clustering->num_clusters + (size_t) (10 * log2((double) input_clustering->vertices)),
		.first_empty_pos = input_clustering->num_clusters,
		.clusters = NULL,
	};
	cl_stack.clusters = calloc(cl_stack.capacity, sizeof(iscc_gr_Cluster));
	if (cl_stack.clusters == NULL) return ISCC_GR_NULL_CLUSTER_STACK;

	for (size_t v = 0; v < input_clustering->vertices; ++v) {
		if (input_clustering->cluster_label[v] != SCC_CLABEL_NA) {
			++(cl_stack.clusters[input_clustering->cluster_label[v]].size);
		}
	}

	for (size_t c = 0; c < cl_stack.first_empty_pos; ++c) {
		cl_stack.clusters[c].members = malloc(sizeof(scc_Vid[cl_stack.clusters[c].size]));
		if (cl_stack.clusters[c].members == NULL) {
			for (size_t c_free = 0; c_free < c; ++c_free) free(cl_stack.clusters[c_free].members);
			free(cl_stack.clusters);
			return ISCC_GR_NULL_CLUSTER_STACK;
		}
		cl_stack.clusters[c].size = 0;
	}

	for (scc_Vid v = 0; v < input_clustering->vertices; ++v) {
		if (input_clustering->cluster_label[v] != SCC_CLABEL_NA) {
			iscc_gr_Cluster* cl = &cl_stack.clusters[input_clustering->cluster_label[v]];
			cl->members[cl->size] = v;
			++(cl->size);
		}
	}

	return cl_stack;
}


static iscc_gr_ClusterStack iscc_gr_empty_cl_stack(const size_t num_vertices)
{
	iscc_gr_ClusterStack cl_stack = {
		.capacity = 1 + (size_t) (20 * log2((double) num_vertices)),
		.first_empty_pos = 1,
		.clusters = NULL,
	};
	cl_stack.clusters = malloc(sizeof(iscc_gr_Cluster[cl_stack.capacity]));
	if (cl_stack.clusters == NULL) return ISCC_GR_NULL_CLUSTER_STACK;

	cl_stack.clusters[0].size = num_vertices;
	cl_stack.clusters[0].members = malloc(sizeof(scc_Vid[num_vertices]));
	if (cl_stack.clusters[0].members == NULL) {
		free(cl_stack.clusters);
		return ISCC_GR_NULL_CLUSTER_STACK;
	}

	for (scc_Vid v = 0; v < num_vertices; ++v) {
		cl_stack.clusters[0].members[v] = v;
	}

	return cl_stack;
}


static inline iscc_gr_Cluster* iscc_gr_peek_at_stack(const iscc_gr_ClusterStack* const cl_stack)
{
	assert(cl_stack != NULL);
	assert(cl_stack->clusters != NULL);

	if (cl_stack->first_empty_pos == 0) return NULL;

	return &cl_stack->clusters[cl_stack->first_empty_pos - 1];
}


static inline iscc_gr_Cluster iscc_gr_pop_from_stack(iscc_gr_ClusterStack* const cl_stack)
{
	assert(cl_stack != NULL);
	assert(cl_stack->clusters != NULL);
	assert(cl_stack->first_empty_pos != 0);

	--(cl_stack->first_empty_pos);
	return cl_stack->clusters[cl_stack->first_empty_pos];
}


static bool iscc_gr_push_to_stack(iscc_gr_ClusterStack* const cl_stack,
                                  const iscc_gr_Cluster cl)
{
	assert(cl_stack != NULL);
	assert(cl_stack->clusters != NULL);

	if (cl_stack->first_empty_pos == cl_stack->capacity) {
		size_t new_capacity = cl_stack->capacity + 1 + (size_t) (20 * log2((double) cl.size));
		iscc_gr_Cluster* const clusters_tmp = realloc(cl_stack->clusters, sizeof(iscc_gr_Cluster[new_capacity]));
		if (!clusters_tmp) return false;
		cl_stack->capacity = new_capacity;
		cl_stack->clusters = clusters_tmp;
	}

	cl_stack->clusters[cl_stack->first_empty_pos] = cl;
	++(cl_stack->first_empty_pos);

	return true;
}


static void iscc_gr_panic_free_everything(iscc_gr_ClusterStack* cl_stack,
                                          size_t num_vertices)
{
	for (size_t i = 0; i < num_vertices; ++i) {
		if (iscc_gr_vertex_store[i].dist_list != NULL) {
			free(iscc_gr_vertex_store[i].dist_list->distances_store);
			free(iscc_gr_vertex_store[i].dist_list);
			iscc_gr_vertex_store[i].dist_list = NULL;
		}
	}

	for (size_t c = 0; c < cl_stack->first_empty_pos; ++c) free(cl_stack->clusters[c].members);
	free(cl_stack->clusters);

	free(iscc_gr_vertex_store);
	iscc_gr_vertex_store = NULL;
	iscc_gr_data_object = NULL;
}


static iscc_gr_Cluster iscc_gr_break_cluster_into_two(iscc_gr_Cluster* const cluster_to_break,
                                                      const size_t k,
                                                      const bool batch_assign)
{
	assert(k >= 2);
	assert(cluster_to_break->size >= 2 * k);
	assert(iscc_gr_vertex_store != NULL);

	scc_Vid center1, center2;
	if (!iscc_gr_find_centers(cluster_to_break, k, &center1, &center2)) return ISCC_GR_NULL_CLUSTER;

	iscc_gr_Vertex* vertex1 = &iscc_gr_vertex_store[center1];
	iscc_gr_Vertex* vertex2 = &iscc_gr_vertex_store[center2];

	assert(vertex1->dist_list != NULL);
	assert(vertex2->dist_list != NULL);

	size_t old_cluster_size = cluster_to_break->size;
	scc_Clabel old_sub_label = vertex1->sub_label;

	scc_Clabel sub_label1 = old_sub_label + 1;
	scc_Clabel sub_label2 = old_sub_label + 2;

	scc_Vid* k_nn_array1 = malloc(sizeof(scc_Vid[k]));
	scc_Vid* k_nn_array2 = malloc(sizeof(scc_Vid[k]));

	iscc_gr_Cluster new_cluster = {
		.size = 0,
		.members = malloc(sizeof(scc_Vid[old_cluster_size])),
	};

	if (!k_nn_array1 || !k_nn_array2 || !new_cluster.members) {
		free(new_cluster.members);
		free(k_nn_array1);
		free(k_nn_array2);
		return ISCC_GR_NULL_CLUSTER;
	}

	iscc_gr_Cluster* cluster1 = cluster_to_break;
	cluster1->size = 0;

	iscc_gr_Cluster* cluster2 = &new_cluster;


	iscc_gr_move_v_to_cluster(center1, cluster1, sub_label1);
	iscc_gr_move_v_to_cluster(center2, cluster2, sub_label2);

	iscc_gr_DistanceEdge* last_assigned_dist1;
	iscc_gr_DistanceEdge* last_assigned_dist2;

	iscc_gr_DistanceEdge* temp_dist1;
	iscc_gr_DistanceEdge* temp_dist2;


	temp_dist1 = iscc_gr_get_first_k_nn(vertex1->dist_list, k - 1, k_nn_array1, old_sub_label);
	temp_dist2 = iscc_gr_get_first_k_nn(vertex2->dist_list, k - 1, k_nn_array2, old_sub_label);
	if (temp_dist1->distance >= temp_dist2->distance) {
		for (size_t i = 0; i < k - 1; ++i) iscc_gr_move_v_to_cluster(k_nn_array1[i], cluster1, sub_label1);
		last_assigned_dist1 = temp_dist1;

		last_assigned_dist2 = iscc_gr_get_first_k_nn(vertex2->dist_list, k - 1, k_nn_array2, old_sub_label);
		for (size_t i = 0; i < k - 1; ++i) iscc_gr_move_v_to_cluster(k_nn_array2[i], cluster2, sub_label2);
	} else {
		for (size_t i = 0; i < k - 1; ++i) iscc_gr_move_v_to_cluster(k_nn_array2[i], cluster2, sub_label2);
		last_assigned_dist2 = temp_dist2;

		last_assigned_dist1 = iscc_gr_get_first_k_nn(vertex1->dist_list, k - 1, k_nn_array1, old_sub_label);
		for (size_t i = 0; i < k - 1; ++i) iscc_gr_move_v_to_cluster(k_nn_array1[i], cluster1, sub_label1);
	}

	assert(cluster1->size + cluster2->size == 2 * k);

	if (batch_assign) {
		size_t num_assign_in_batch = k;
		for (size_t assigned = cluster1->size + cluster2->size;
		        assigned < old_cluster_size; assigned += num_assign_in_batch) {
			
			if (assigned + num_assign_in_batch > old_cluster_size) {
				num_assign_in_batch = old_cluster_size - assigned;
			}

			temp_dist1 = iscc_gr_get_next_k_nn(last_assigned_dist1, num_assign_in_batch, k_nn_array1, old_sub_label);
			temp_dist2 = iscc_gr_get_next_k_nn(last_assigned_dist2, num_assign_in_batch, k_nn_array2, old_sub_label);

			if (temp_dist1->distance <= temp_dist2->distance) {
				for (size_t i = 0; i < num_assign_in_batch; ++i) iscc_gr_move_v_to_cluster(k_nn_array1[i], cluster1, sub_label1);
				last_assigned_dist1 = temp_dist1;
			} else {
				for (size_t i = 0; i < num_assign_in_batch; ++i) iscc_gr_move_v_to_cluster(k_nn_array2[i], cluster2, sub_label2);
				last_assigned_dist2 = temp_dist2;
			}
		}

	} else {
		for (size_t assigned = cluster1->size + cluster2->size;
		        assigned < old_cluster_size; ++assigned) {

			temp_dist1 = iscc_gr_get_next_dist(last_assigned_dist1, old_sub_label);
			temp_dist2 = iscc_gr_get_next_dist(last_assigned_dist2, old_sub_label);

			if (temp_dist1->distance <= temp_dist2->distance) {
				iscc_gr_move_v_to_cluster(temp_dist1->head, cluster1, sub_label1);
				last_assigned_dist1 = temp_dist1;
			} else {
				iscc_gr_move_v_to_cluster(temp_dist2->head, cluster2, sub_label2);
				last_assigned_dist2 = temp_dist2;
			}
		}

	}

	free(k_nn_array1);
	free(k_nn_array2);

	if (!iscc_gr_adjust_cluster_memory(cluster1, k) || !iscc_gr_adjust_cluster_memory(cluster2, k)) {
		free(new_cluster.members);
		return ISCC_GR_NULL_CLUSTER;
	}

	return new_cluster;
}


static bool iscc_gr_find_centers(const iscc_gr_Cluster* const cl,
                                 const size_t k,
                                 scc_Vid* const center1,
                                 scc_Vid* const center2)
{
	assert(cl != NULL);
	assert(k > 2);
	assert(cl->size >= 2 * k);
	assert(center1 != NULL && center2 != NULL);
	assert(iscc_gr_vertex_store != NULL);
	assert(iscc_gr_search_object == NULL);
	assert(iscc_gr_index_scratch == NULL);
	assert(iscc_gr_dist_scratch == NULL);

	const size_t check_at_least = 1 + (size_t) (100 * log2((double) k));
	size_t step = cl->size / check_at_least;
	if (step < 2) step = 2;

	for (size_t i = 0; i < cl->size; i += step) {
		if (iscc_gr_vertex_store[cl->members[i]].dist_list == NULL) {
			if (iscc_gr_search_object == NULL) {
				if (!iscc_gr_init_search_object(cl)) return false;
			}
			if (!iscc_gr_populate_dist_list(cl->members[i], cl)) {
				iscc_gr_free_search_object();
				return false;
			}
		}
	}

	scc_Distance max_dist = -1.0;
	for (size_t i = 0; i < cl->size; ++i) {
		iscc_gr_Vertex* ve_check = &iscc_gr_vertex_store[cl->members[i]];
		if (ve_check->dist_list != NULL) {
			while (ve_check->dist_list->mark != ve_check->sub_label) { // Have the vertex been checked before in this cluster?
				ve_check->dist_list->mark = ve_check->sub_label;
				iscc_gr_DistanceEdge* ve_max_dist = iscc_gr_get_last_dist(ve_check->dist_list, ve_check->sub_label);

				if (ve_max_dist->distance > max_dist) {
					max_dist = ve_max_dist->distance;
					*center1 = cl->members[i];
					*center2 = ve_max_dist->head;
				}

				ve_check = &iscc_gr_vertex_store[ve_max_dist->head];
				if (ve_check->dist_list == NULL) {
					if (iscc_gr_search_object == NULL) {
						if (!iscc_gr_init_search_object(cl)) return false;
					}
					if (!iscc_gr_populate_dist_list(ve_max_dist->head, cl)) {
						iscc_gr_free_search_object();
						return false;
					}
				}
			}
		}
	}

	if (!iscc_gr_free_search_object()) return false;

	return true;
}


static bool iscc_gr_init_search_object(const iscc_gr_Cluster* const cl)
{
	assert(iscc_gr_data_object != NULL);
	assert(iscc_gr_search_object == NULL);
	assert(iscc_gr_index_scratch == NULL);
	assert(iscc_gr_dist_scratch == NULL);

	iscc_gr_search_object = scc_init_search_object(iscc_gr_data_object, cl->size, false, 0.0, cl->size, cl->members);
	iscc_gr_index_scratch = malloc(sizeof(scc_Vid[cl->size]));
	iscc_gr_dist_scratch = malloc(sizeof(scc_Distance[cl->size]));
	if (iscc_gr_search_object == NULL || iscc_gr_index_scratch == NULL || iscc_gr_dist_scratch == NULL) {
		iscc_gr_free_search_object();
		return false;
	}

	return true;
}


static bool iscc_gr_free_search_object()
{
	bool success = scc_close_search_object(iscc_gr_search_object);
	free(iscc_gr_index_scratch);
	free(iscc_gr_dist_scratch);
	iscc_gr_search_object = NULL;
	iscc_gr_index_scratch = NULL;
	iscc_gr_dist_scratch = NULL;

	return success;
}


static bool iscc_gr_populate_dist_list(const scc_Vid v_id,
                                       const iscc_gr_Cluster* const cl)
{
	assert(cl->size > 2);
	assert(iscc_gr_vertex_store != NULL);
	assert(iscc_gr_search_object != NULL);
	assert(iscc_gr_index_scratch != NULL);
	assert(iscc_gr_dist_scratch != NULL);

	const size_t num_nn = cl->size - 1;
	iscc_gr_Vertex* const vertex = &iscc_gr_vertex_store[v_id];

	vertex->dist_list = malloc(sizeof(iscc_gr_DistanceEdgeList));
	iscc_gr_DistanceEdge* distances_store = malloc(sizeof(iscc_gr_DistanceEdge[num_nn]));
	if (vertex->dist_list == NULL || distances_store == NULL) {
		free(vertex->dist_list);
		free(distances_store);
		return false;
	}

	if (!scc_nearest_neighbor_search(iscc_gr_search_object, 1, &v_id, iscc_gr_index_scratch, iscc_gr_dist_scratch)) {
		free(vertex->dist_list);
		free(distances_store);
		return false;
	}

	*(vertex->dist_list) = (iscc_gr_DistanceEdgeList) { .mark = SCC_CLABEL_MAX,
	                                                    .distances_store = distances_store,
	                                                    .first_dist = distances_store,
	                                                    .last_dist = &distances_store[num_nn - 1] };

	size_t in_list = 0;
	const iscc_gr_DistanceEdge* const de_stop = distances_store + num_nn;
	for (iscc_gr_DistanceEdge* de = distances_store; de != de_stop; ++de, ++in_list) {

		if (iscc_gr_index_scratch[in_list] == v_id) ++in_list;
		assert(in_list < cl->size);
		assert(iscc_gr_index_scratch[in_list] != v_id);

		*de = (iscc_gr_DistanceEdge) { .head = iscc_gr_index_scratch[in_list],
	                                   .distance = iscc_gr_dist_scratch[in_list],
	                                   .prev_dist = de - 1,
	                                   .next_dist = de + 1 };
	}

	distances_store[0].prev_dist = NULL;
	distances_store[num_nn - 1].next_dist = NULL;

	return true;
}


static inline iscc_gr_DistanceEdge* iscc_gr_get_first_dist(iscc_gr_DistanceEdgeList* const dist_list,
                                                           const scc_Clabel required_sub_label)
{
	assert(dist_list != NULL);
	assert(dist_list->first_dist != NULL);
	assert(iscc_gr_vertex_store != NULL);

	while(iscc_gr_vertex_store[dist_list->first_dist->head].sub_label != required_sub_label) {
		assert(dist_list->first_dist->prev_dist == NULL);
		assert(dist_list->first_dist->next_dist != NULL);
		dist_list->first_dist = dist_list->first_dist->next_dist;
		dist_list->first_dist->prev_dist = NULL;
	}

	return dist_list->first_dist;
}


static inline iscc_gr_DistanceEdge* iscc_gr_get_last_dist(iscc_gr_DistanceEdgeList* const dist_list,
                                                          const scc_Clabel required_sub_label)
{
	assert(dist_list != NULL);
	assert(dist_list->last_dist != NULL);
	assert(iscc_gr_vertex_store != NULL);

	while(iscc_gr_vertex_store[dist_list->last_dist->head].sub_label != required_sub_label) {
		assert(dist_list->last_dist->next_dist == NULL);
		assert(dist_list->last_dist->prev_dist != NULL);
		dist_list->last_dist = dist_list->last_dist->prev_dist;
		dist_list->last_dist->next_dist = NULL;
	}

	return dist_list->last_dist;
}


static inline iscc_gr_DistanceEdge* iscc_gr_get_next_dist(iscc_gr_DistanceEdge* const de,
                                                          const scc_Clabel required_sub_label)
{	
	assert(de != NULL);
	assert(de->next_dist != NULL); // We should never reach the end!
	assert(iscc_gr_vertex_store != NULL);

	while(iscc_gr_vertex_store[de->next_dist->head].sub_label != required_sub_label) {
		iscc_gr_DistanceEdge* to_delete = de->next_dist;

		assert(to_delete->next_dist != NULL); // We should never reach the end!
		assert(to_delete->prev_dist == de);
		assert(to_delete->next_dist->prev_dist == to_delete);
		assert(iscc_gr_vertex_store[de->next_dist->head].sub_label != iscc_gr_vertex_store[de->head].sub_label);

		de->next_dist = to_delete->next_dist;
		to_delete->next_dist->prev_dist = de;
	}

	return de->next_dist;
}


static inline iscc_gr_DistanceEdge* iscc_gr_get_first_k_nn(iscc_gr_DistanceEdgeList* const dist_list,
                                                           const size_t k,
                                                           scc_Vid out_dist_array[const static k],
                                                           const scc_Clabel required_sub_label)
{
	assert(k > 0);

	iscc_gr_DistanceEdge* prev_dist = iscc_gr_get_first_dist(dist_list, required_sub_label);
	out_dist_array[0] = prev_dist->head;

	for (size_t found = 1; found < k; ++found) {
		prev_dist = iscc_gr_get_next_dist(prev_dist, required_sub_label);
		out_dist_array[found] = prev_dist->head;
	}

	return prev_dist;
}


static inline iscc_gr_DistanceEdge* iscc_gr_get_next_k_nn(iscc_gr_DistanceEdge* prev_dist,
                                                          const size_t k,
                                                          scc_Vid out_dist_array[const static k],
                                                          const scc_Clabel required_sub_label)
{
	for (size_t found = 0; found < k; ++found) {
		prev_dist = iscc_gr_get_next_dist(prev_dist, required_sub_label);
		out_dist_array[found] = prev_dist->head;
	}

	return prev_dist;
}


static inline void iscc_gr_move_v_to_cluster(const scc_Vid v_id,
                                             iscc_gr_Cluster* const cl,
                                             const scc_Clabel sub_label)
{
	assert(iscc_gr_vertex_store != NULL);
	assert(iscc_gr_vertex_store[v_id].sub_label != sub_label);
	cl->members[cl->size] = v_id;
	++(cl->size);
	iscc_gr_vertex_store[v_id].sub_label = sub_label;
}


static bool iscc_gr_adjust_cluster_memory(iscc_gr_Cluster* const cl,
                                          const size_t k)
{
	if (cl->size < 2 * k) {
		// Cluster cannot be split anymore, deallocate distance lists
		for (size_t i = 0; i < cl->size; ++i) {
			if (iscc_gr_vertex_store[cl->members[i]].dist_list != NULL) {
				free(iscc_gr_vertex_store[cl->members[i]].dist_list->distances_store);
				free(iscc_gr_vertex_store[cl->members[i]].dist_list);
				iscc_gr_vertex_store[cl->members[i]].dist_list = NULL;
			}
		}
	}

	scc_Vid* const members_tmp = realloc(cl->members, sizeof(scc_Vid[cl->size]));
	if (!members_tmp) return false;
	cl->members = members_tmp;

	return true;
}
