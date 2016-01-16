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


#include "findseeds.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "../include/config.h"
#include "../include/digraph.h"
#include "../include/nng_clustering.h"


// ==============================================================================
// Internal structs
// ==============================================================================

typedef struct iscc_fs_SortResult iscc_fs_SortResult;
struct iscc_fs_SortResult {
	scc_Vid* inwards_count;
	scc_Vid* sorted_vertices;
	scc_Vid** vertex_index;
	scc_Vid** bucket_index;
};


// ==============================================================================
// Internal function prototypes
// ==============================================================================

static scc_Digraph iscc_exclusion_graph(const scc_Digraph* nng,
                                        bool excluded[static nng->vertices]);

static scc_TempSeedClustering iscc_fs_init_clustering(size_t vertices,
                                                      size_t seed_init_capacity);

static void iscc_fs_shrink_seed_array(scc_TempSeedClustering* cl);

static inline bool iscc_fs_add_seed(scc_Vid s,
                                    scc_TempSeedClustering* cl);

static inline bool iscc_fs_check_neighbors_marks(scc_Vid cv,
                                                 const scc_Digraph* nng,
                                                 const bool marks[static nng->vertices]);

static inline void iscc_fs_mark_seed_neighbors(scc_Vid s,
                                               const scc_Digraph* nng,
                                               bool marks[static nng->vertices]);

static iscc_fs_SortResult iscc_fs_sort_by_inwards(const scc_Digraph* nng,
                                                  bool make_indices);

static void iscc_fs_free_SortResult(iscc_fs_SortResult* sr);

static inline void iscc_fs_decrease_v_in_sort(scc_Vid v_to_decrease,
                                              scc_Vid inwards_count[restrict],
                                              scc_Vid* vertex_index[restrict],
                                              scc_Vid* bucket_index[restrict],
                                              scc_Vid* current_pos);

#ifdef SCC_STABLE_CLUSTERING

	static inline void iscc_fs_debug_bucket_sort(const scc_Vid* bucket_start,
	                                             scc_Vid* pos,
	                                             const scc_Vid inwards_count[],
	                                             scc_Vid* vertex_index[]);

	static inline void iscc_fs_debug_check_sort(const scc_Vid* current_pos,
	                                            const scc_Vid* last_pos,
	                                            const scc_Vid inwards_count[]);

#endif


// ==============================================================================
// External function implementations
// ==============================================================================


scc_TempSeedClustering iscc_findseeds_lexical(const scc_Digraph* const nng,
                                              size_t seed_init_capacity)
{
	assert(scc_digraph_is_initialized(nng));

	bool* const marks = calloc(nng->vertices, sizeof(bool));

	scc_TempSeedClustering cl = iscc_fs_init_clustering(nng->vertices, seed_init_capacity);

	if (!marks || !cl.seeds) {
		free(marks);
		scc_free_TempSeedClustering(&cl);
		return SCC_NULL_TEMP_SEED_CLUSTERING;
	}

	for (scc_Vid cv = 0; cv < nng->vertices; ++cv) {
		if (iscc_fs_check_neighbors_marks(cv, nng, marks)) {
			assert(nng->tail_ptr[cv] != nng->tail_ptr[cv + 1]);

			if (!iscc_fs_add_seed(cv, &cl)) {
				free(marks);
				scc_free_TempSeedClustering(&cl);
				return SCC_NULL_TEMP_SEED_CLUSTERING;
			}

			iscc_fs_mark_seed_neighbors(cv, nng, marks);
		}
	}

	free(marks);

	iscc_fs_shrink_seed_array(&cl);

	return cl;
}


scc_TempSeedClustering iscc_findseeds_inwards(const scc_Digraph* const nng,
                                              const size_t seed_init_capacity,
                                              const bool updating)
{
	assert(scc_digraph_is_initialized(nng));

	iscc_fs_SortResult sort = iscc_fs_sort_by_inwards(nng, updating);

	bool* const marks = calloc(nng->vertices, sizeof(bool));

	scc_TempSeedClustering cl = iscc_fs_init_clustering(nng->vertices, seed_init_capacity);

	if (!sort.sorted_vertices || !marks || !cl.seeds) {
		iscc_fs_free_SortResult(&sort);
		free(marks);
		scc_free_TempSeedClustering(&cl);
		return SCC_NULL_TEMP_SEED_CLUSTERING;
	}

	const scc_Vid* const sorted_v_stop = sort.sorted_vertices + nng->vertices;
	for (scc_Vid* sorted_v = sort.sorted_vertices;
	        sorted_v != sorted_v_stop; ++sorted_v) {

		#if defined(SCC_STABLE_CLUSTERING) && !defined(NDEBUG)
			iscc_fs_debug_check_sort(sorted_v, sorted_v_stop - 1, sort.inwards_count);
		#endif

		if (iscc_fs_check_neighbors_marks(*sorted_v, nng, marks)) {
			assert(nng->tail_ptr[*sorted_v] != nng->tail_ptr[*sorted_v + 1]);

			if (!iscc_fs_add_seed(*sorted_v, &cl)) {
				iscc_fs_free_SortResult(&sort);
				free(marks);
				scc_free_TempSeedClustering(&cl);
				return SCC_NULL_TEMP_SEED_CLUSTERING;
			}

			iscc_fs_mark_seed_neighbors(*sorted_v, nng, marks);

			if (updating) {
				const scc_Vid* const v_arc_stop = nng->head + nng->tail_ptr[*sorted_v + 1];
				for (const scc_Vid* v_arc = nng->head + nng->tail_ptr[*sorted_v];
				        v_arc != v_arc_stop; ++v_arc) {
					const scc_Vid* const v_arc_arc_stop = nng->head + nng->tail_ptr[*v_arc + 1];
					for (scc_Vid* v_arc_arc = nng->head + nng->tail_ptr[*v_arc];
					        v_arc_arc != v_arc_arc_stop; ++v_arc_arc) {
						// Only decrease if vertex can be seed (i.e., not already assigned and not already considered)
						if (!marks[*v_arc_arc] && sorted_v < sort.vertex_index[*v_arc_arc]) {
							iscc_fs_decrease_v_in_sort(*v_arc_arc, sort.inwards_count, sort.vertex_index, sort.bucket_index, sorted_v);
						}
					}
				}
			}
		}
	}

	iscc_fs_free_SortResult(&sort);
	free(marks);

	iscc_fs_shrink_seed_array(&cl);

	return cl;
}


scc_TempSeedClustering iscc_findseeds_exclusion(const scc_Digraph* const nng,
                                                const size_t seed_init_capacity,
                                                const bool updating)
{
	assert(scc_digraph_is_initialized(nng));

	bool* const excluded = malloc(sizeof(bool[nng->vertices]));
	if (!excluded) return SCC_NULL_TEMP_SEED_CLUSTERING;

	scc_Digraph exclusion_graph = iscc_exclusion_graph(nng, excluded);
	if (!scc_digraph_is_initialized(&exclusion_graph)) {
		free(excluded);
		return SCC_NULL_TEMP_SEED_CLUSTERING;
	}

	iscc_fs_SortResult sort = iscc_fs_sort_by_inwards(&exclusion_graph, updating);

	scc_TempSeedClustering cl = iscc_fs_init_clustering(nng->vertices, seed_init_capacity);

	if (!sort.sorted_vertices || !cl.seeds) {
		free(excluded);
		scc_free_digraph(&exclusion_graph);
		iscc_fs_free_SortResult(&sort);
		scc_free_TempSeedClustering(&cl);
		return SCC_NULL_TEMP_SEED_CLUSTERING;
	}

	const scc_Vid* const sorted_v_stop = sort.sorted_vertices + nng->vertices;
	for (scc_Vid* sorted_v = sort.sorted_vertices;
	        sorted_v != sorted_v_stop; ++sorted_v) {

		#if defined(SCC_STABLE_CLUSTERING) && !defined(NDEBUG)
			iscc_fs_debug_check_sort(sorted_v, sorted_v_stop - 1, sort.inwards_count);
		#endif

		if (!excluded[*sorted_v]) {
			assert(nng->tail_ptr[*sorted_v] != nng->tail_ptr[*sorted_v + 1]);

			if (!iscc_fs_add_seed(*sorted_v, &cl)) {
				free(excluded);
				scc_free_digraph(&exclusion_graph);
				iscc_fs_free_SortResult(&sort);
				scc_free_TempSeedClustering(&cl);
				return SCC_NULL_TEMP_SEED_CLUSTERING;
			}

			excluded[*sorted_v] = true;
				
			const scc_Vid* const ex_arc_stop = exclusion_graph.head + exclusion_graph.tail_ptr[*sorted_v + 1];
			for (const scc_Vid* ex_arc = exclusion_graph.head + exclusion_graph.tail_ptr[*sorted_v];
			        ex_arc != ex_arc_stop; ++ex_arc) {
				if (!excluded[*ex_arc]) {
					excluded[*ex_arc] = true;

					if (updating) {
						const scc_Vid* const ex_arc_arc_stop = exclusion_graph.head + exclusion_graph.tail_ptr[*ex_arc + 1];
						for (scc_Vid* ex_arc_arc = exclusion_graph.head + exclusion_graph.tail_ptr[*ex_arc];
						        ex_arc_arc != ex_arc_arc_stop; ++ex_arc_arc) {
							if (!excluded[*ex_arc_arc]) {
								iscc_fs_decrease_v_in_sort(*ex_arc_arc, sort.inwards_count, sort.vertex_index, sort.bucket_index, sorted_v);
							}
						}
					}
				}
			}
		}
	}

	free(excluded);
	scc_free_digraph(&exclusion_graph);
	iscc_fs_free_SortResult(&sort);

	iscc_fs_shrink_seed_array(&cl);

	return cl;
}


/*

Exclusion graph does not give one arc optimality

     *            *
     |            |
     v            v
  *->*->*->*<->*<-*<-*<-*
     ^            ^
     |            |
     *            *

bool iscc_findseeds_onearc_updating(const scc_Digraph* const nng, ...) {
	//Among those with 0 inwards arcs, sort on exclusion graph 
}
*/


// ==============================================================================
// Internal function implementations 
// ==============================================================================


static scc_Digraph iscc_exclusion_graph(const scc_Digraph* const nng,
                                        bool excluded[const static nng->vertices])
{
	assert(scc_digraph_is_initialized(nng));
	assert(excluded);

	scc_Digraph nng_transpose = scc_digraph_transpose(nng);
	if (!scc_digraph_is_initialized(&nng_transpose)) return SCC_NULL_DIGRAPH;

	scc_Digraph nng_nng_transpose = scc_adjacency_product(nng, &nng_transpose, true, false);
	scc_free_digraph(&nng_transpose);
	if (!scc_digraph_is_initialized(&nng_nng_transpose)) return SCC_NULL_DIGRAPH;

	/* nng_nng_transpose contains two type of irrelevant arcs:
	 * 1. All vertices with non-zero outwards arcs in `nng` will have an arcs pointing
	 *    to themselves. Obviously, a vertex assigned as seed excludes itself.
	 * 2. All vertices with zero outwards arcs in `nng` will have arcs pointing to
	 *    vertices that points to them in `nng`. These vertices are excluded from the
	 *    the beginning (due to zero arcs), thus their outwards arcs are not necessary.
	 *
	 *  1 & 2 both lead to excessive memory use. 2 also leads to that the sorting on
	 *  inwards arcs by `iscc_fs_sort_by_inwards()` becomes wrong.
	 *
	 *  This snippet removes all arcs falling under 1 & 2 from `nng_nng_transpose`
	 *  (and thus from `exclusion_graph`). See `scc_delete_arcs_by_tails()` for
	 *  more details on implementation.
	 */
	scc_Arci head_write = 0;
	for (scc_Vid v = 0; v < nng->vertices; ++v) {
		if (nng->tail_ptr[v] == nng->tail_ptr[v + 1]) {
			excluded[v] = true;
			nng_nng_transpose.tail_ptr[v] = head_write;
		} else {
			excluded[v] = false;
			const scc_Vid* v_arc = nng_nng_transpose.head + nng_nng_transpose.tail_ptr[v];
			const scc_Vid* const v_arc_stop = nng_nng_transpose.head + nng_nng_transpose.tail_ptr[v + 1];
			nng_nng_transpose.tail_ptr[v] = head_write;

			for (; v_arc != v_arc_stop; ++v_arc) {
				if (*v_arc != v) {
					nng_nng_transpose.head[head_write] = *v_arc;
					++head_write;
				}
			}
		}
	}
	nng_nng_transpose.tail_ptr[nng->vertices] = head_write;
	scc_change_arc_storage(&nng_nng_transpose, head_write);

	const scc_Digraph* nng_sum[2] = {nng, &nng_nng_transpose};
	scc_Digraph exclusion_graph = scc_digraph_union(2, nng_sum);
	scc_free_digraph(&nng_nng_transpose);
	if (!scc_digraph_is_initialized(&exclusion_graph)) return SCC_NULL_DIGRAPH;

	return exclusion_graph;
}


static scc_TempSeedClustering iscc_fs_init_clustering(const size_t vertices,
                                                      size_t seed_init_capacity)
{
	if (seed_init_capacity < 128) seed_init_capacity = 128;

	scc_TempSeedClustering cl = {
		.vertices = vertices,
		.num_clusters = 0,
		.seed_capacity = seed_init_capacity,
		.external_labels = false,
		.assigned = NULL,
		.seeds = malloc(sizeof(scc_Vid[seed_init_capacity])),
		.cluster_label = NULL,
	};

	if (!cl.seeds) return SCC_NULL_TEMP_SEED_CLUSTERING;

	return cl;
}


static void iscc_fs_shrink_seed_array(scc_TempSeedClustering* const cl)
{
	if (cl && cl->seeds && (cl->seed_capacity > cl->num_clusters) && (cl->num_clusters > 0)) {
		scc_Vid* const tmp_ptr = realloc(cl->seeds, sizeof(scc_Vid[cl->num_clusters]));
		if (tmp_ptr) {
			cl->seeds = tmp_ptr;
			cl->seed_capacity = cl->num_clusters;
		}
	}
}


static inline bool iscc_fs_add_seed(const scc_Vid s,
                                    scc_TempSeedClustering* const cl)
{
	assert(cl->seeds);
	assert(cl->num_clusters <= cl->seed_capacity);
	
	if (cl->num_clusters == cl->seed_capacity) {
		cl->seed_capacity = cl->seed_capacity + (cl->seed_capacity >> 3) + 1024;
		if (cl->seed_capacity >= SCC_CLABEL_MAX) cl->seed_capacity = SCC_CLABEL_MAX - 1;
		scc_Vid* const tmp_ptr = realloc(cl->seeds, sizeof(scc_Vid[cl->seed_capacity]));
		if (!tmp_ptr) return false;
		cl->seeds = tmp_ptr;
	}
	cl->seeds[cl->num_clusters] = s;
	++(cl->num_clusters);
	assert(cl->num_clusters < SCC_CLABEL_MAX);

	return true;
}


static inline bool iscc_fs_check_neighbors_marks(const scc_Vid cv,
                                                 const scc_Digraph* const nng,
                                                 const bool marks[const static nng->vertices])
{
	if (marks[cv]) return false;

	const scc_Vid* cv_arc = nng->head + nng->tail_ptr[cv];
	const scc_Vid* const cv_arc_stop = nng->head + nng->tail_ptr[cv + 1];
	if (cv_arc == cv_arc_stop) return false;

	for (; cv_arc != cv_arc_stop; ++cv_arc) { 
		if (marks[*cv_arc]) return false;
	}

	return true;
}


static inline void iscc_fs_mark_seed_neighbors(const scc_Vid s,
                                               const scc_Digraph* const nng,
                                               bool marks[const static nng->vertices])
{
	assert(!marks[s]);
	marks[s] = true;

	const scc_Vid* const s_arc_stop = nng->head + nng->tail_ptr[s + 1];
	for (const scc_Vid* s_arc = nng->head + nng->tail_ptr[s];
	        s_arc != s_arc_stop; ++s_arc) {
		assert(!marks[*s_arc]);
		marks[*s_arc] = true;
	}
}


static iscc_fs_SortResult iscc_fs_sort_by_inwards(const scc_Digraph* const nng,
                                                  const bool make_indices)
{
	assert(scc_digraph_is_initialized(nng));

	const size_t vertices = nng->vertices;

	iscc_fs_SortResult res = {
		.inwards_count = calloc(vertices, sizeof(scc_Vid)),
		.sorted_vertices = malloc(sizeof(scc_Vid[vertices])),
		.vertex_index = NULL,
		.bucket_index = NULL,
	};

	if (!res.inwards_count || !res.sorted_vertices) {
		iscc_fs_free_SortResult(&res);
		return (iscc_fs_SortResult) { NULL, NULL, NULL, NULL };
	}

	const scc_Vid* const arc_stop = nng->head + nng->tail_ptr[vertices];
	for (const scc_Vid* arc = nng->head; arc != arc_stop; ++arc) {
		++res.inwards_count[*arc];
	}

	// Dynamic alloc is slightly faster but more error-prone
	// Add if turns out to be bottleneck
	scc_Vid max_inwards = 0;
	for (scc_Vid v = 0; v < vertices; ++v) {
		if (max_inwards < res.inwards_count[v]) max_inwards = res.inwards_count[v];
	}

	scc_Vid* bucket_count = calloc(max_inwards + 1, sizeof(scc_Vid));
	res.bucket_index = malloc(sizeof(scc_Vid*[max_inwards + 1]));
	if (!bucket_count || !res.bucket_index) {
		free(bucket_count);
		iscc_fs_free_SortResult(&res);
		return (iscc_fs_SortResult) { NULL, NULL, NULL, NULL };
	}

	for (scc_Vid v = 0; v < vertices; ++v) {
		++bucket_count[res.inwards_count[v]];
	}

	size_t bucket_cumsum = 0;
	for (size_t b = 0; b <= max_inwards; ++b) {
		bucket_cumsum += bucket_count[b];
		res.bucket_index[b] = res.sorted_vertices + bucket_cumsum;
	}
	free(bucket_count);

	if (make_indices) {
		res.vertex_index = malloc(sizeof(scc_Vid*[vertices]));
		if (!res.vertex_index) {
			iscc_fs_free_SortResult(&res);
			return (iscc_fs_SortResult) { NULL, NULL, NULL, NULL };
		}
		for (scc_Vid v = (scc_Vid) vertices; v > 0; ) {
			--v;
			--res.bucket_index[res.inwards_count[v]];
			*res.bucket_index[res.inwards_count[v]] = v;
			res.vertex_index[v] = res.bucket_index[res.inwards_count[v]];
		}
	} else {
		for (scc_Vid v = (scc_Vid) vertices; v > 0; ) {
			--v;
			--(res.bucket_index[res.inwards_count[v]]);
			*res.bucket_index[res.inwards_count[v]] = v;
		}

		free(res.inwards_count);
		free(res.bucket_index);
		res.inwards_count = NULL;
		res.bucket_index = NULL;
	}

	return res;
}


static void iscc_fs_free_SortResult(iscc_fs_SortResult* const sr)
{
	if (sr) {
		free(sr->inwards_count);
		free(sr->sorted_vertices);
		free(sr->vertex_index);
		free(sr->bucket_index);
		*sr = (iscc_fs_SortResult) { NULL, NULL, NULL, NULL };
	}
}


static inline void iscc_fs_decrease_v_in_sort(const scc_Vid v_to_decrease,
                                              scc_Vid inwards_count[restrict const],
                                              scc_Vid* vertex_index[restrict const],
                                              scc_Vid* bucket_index[restrict const],
                                              scc_Vid* const current_pos)
{
	// Assert that vertex index is correct
	assert(v_to_decrease == *vertex_index[v_to_decrease]);

	// Find vertices to move
	scc_Vid* const move_from = vertex_index[v_to_decrease];
	scc_Vid* move_to = bucket_index[inwards_count[v_to_decrease]];
	if (move_to <= current_pos) {
		move_to = current_pos + 1;
		bucket_index[inwards_count[v_to_decrease] - 1] = move_to;
	} 

	// Assert that swap vertices have the same count
	assert(inwards_count[*move_from] == inwards_count[*move_to]);

	// Update bucket index
	bucket_index[inwards_count[v_to_decrease]] = move_to + 1;

	// Decrease count on vertex
	--inwards_count[v_to_decrease];

	// Check so list not already sorted
	if (move_from != move_to) {
		// Do swap
		*move_from = *move_to;
		*move_to = v_to_decrease;

		// Update vertex index
		vertex_index[*move_to] = move_to;
		vertex_index[*move_from] = move_from;

		#ifdef SCC_STABLE_CLUSTERING
			// Sort old bucket by vertex ID
			iscc_fs_debug_bucket_sort(move_to + 1, move_from, inwards_count, vertex_index);
		#endif
	}

	#ifdef SCC_STABLE_CLUSTERING
		// If new bucket start on or before current_pos in the sorted vertices, move it to next in line
		if (bucket_index[inwards_count[v_to_decrease]] <= current_pos) {
			bucket_index[inwards_count[v_to_decrease]] = current_pos + 1;
		}
		// Sort new bucket by vertex ID
		iscc_fs_debug_bucket_sort(bucket_index[inwards_count[v_to_decrease]],
		                          move_to, inwards_count, vertex_index);
	#endif
}


#ifdef SCC_STABLE_CLUSTERING


	static inline void iscc_fs_debug_bucket_sort(const scc_Vid* const bucket_start,
	                                             scc_Vid* pos,
	                                             const scc_Vid inwards_count[const],
	                                             scc_Vid* vertex_index[const])
	{
		scc_Vid tmp_v = *pos;
		for (; pos != bucket_start; --pos) {
			assert(inwards_count[tmp_v] == inwards_count[*(pos - 1)]);
			if (tmp_v >= *(pos - 1)) break;
			*pos = *(pos - 1);
			vertex_index[*pos] = pos;
		}
		*pos = tmp_v;
		vertex_index[*pos] = pos;
	}


	static inline void iscc_fs_debug_check_sort(const scc_Vid* current_pos,
	                                            const scc_Vid* const last_pos,
	                                            const scc_Vid inwards_count[const])
	{
		for (; current_pos != last_pos; ++current_pos) {
			assert(inwards_count[*(current_pos)] <= inwards_count[*(current_pos + 1)]);
			if (inwards_count[*(current_pos)] == inwards_count[*(current_pos + 1)]) {
				assert(*(current_pos) < *(current_pos + 1));
			}
		}
	}


#endif
