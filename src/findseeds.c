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
#include <stdlib.h>
#include "../include/config.h"
#include "../include/digraph.h"
#include "../include/nng_clustering.h"

#ifdef SCC_STABLE_CLUSTERING
	#include "findseeds_debug.h"
#endif


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

static inline bool iscc_fs_check_neighbors_marks(scc_Vid cv,
                                                 const scc_Digraph* nng,
                                                 const bool* assigned);

static inline void iscc_fs_mark_seed_neighbors(scc_Vid s,
                                               const scc_Digraph* nng,
                                               bool* assigned);

static inline bool iscc_fs_add_seed(scc_Vid s,
                                    iscc_SeedArray* sa);

static inline void iscc_fs_assign_neighbors(scc_Vid s,
                                            scc_Clabel new_label,
                                            const scc_Digraph* restrict nng,
                                            bool* restrict assigned,
                                            scc_Clabel* restrict cluster_label);

static inline void iscc_fs_assign_cl_labels(scc_Vid s,
                                            scc_Clabel new_label,
                                            const scc_Digraph* nng,
                                            scc_Clabel* cluster_label);

static void iscc_fs_shrink_SeedArray(iscc_SeedArray* sa);

static iscc_SeedArray iscc_fs_make_seed_array(scc_Vid seed_init_capacity);

static scc_Digraph iscc_exclusion_graph(const scc_Digraph* nng);

static iscc_fs_SortResult iscc_fs_sort_by_inwards(const scc_Digraph* nng,
                                                  bool make_indices);

static void iscc_fs_free_SortResult(iscc_fs_SortResult* sr);

static inline void iscc_fs_decrease_v_in_sort(scc_Vid v_to_decrease,
                                              scc_Vid* restrict inwards_count,
                                              scc_Vid** restrict vertex_index,
                                              scc_Vid** restrict bucket_index,
                                              scc_Vid* current_pos);


// ==============================================================================
// External function implementations
// ==============================================================================


iscc_SeedArray iscc_findseeds_lexical(const scc_Digraph* nng,
                                      scc_Vid seed_init_capacity) {
	
	if (!nng || !nng->tail_ptr) return ISCC_NULL_SEED_ARRAY;

	bool* const marks = calloc(nng->vertices, sizeof(bool));

	iscc_SeedArray seed_array = iscc_fs_make_seed_array(seed_init_capacity);

	if (!marks || !seed_array.seeds) {
		free(marks);
		iscc_free_SeedArray(&seed_array);
		return ISCC_NULL_SEED_ARRAY;
	}

	for (scc_Vid cv = 0; cv < nng->vertices; ++cv) {
		if (iscc_fs_check_neighbors_marks(cv, nng, marks)) {
			if (!iscc_fs_add_seed(cv, &seed_array)) {
				free(marks);
				iscc_free_SeedArray(&seed_array);
				return ISCC_NULL_SEED_ARRAY;
			}

			iscc_fs_mark_seed_neighbors(cv, nng, marks);
		}
	}

	free(marks);

	iscc_fs_shrink_SeedArray(&seed_array);

	return seed_array;
}


iscc_SeedArray iscc_findseeds_inwards(const scc_Digraph* const nng,
                                      const scc_Vid seed_init_capacity,
                                      const bool updating) {

	if (!nng || !nng->tail_ptr) return ISCC_NULL_SEED_ARRAY;

	iscc_fs_SortResult sort = iscc_fs_sort_by_inwards(nng, updating);

	bool* const marks = calloc(nng->vertices, sizeof(bool));

	iscc_SeedArray seed_array = iscc_fs_make_seed_array(seed_init_capacity);

	if (!sort.sorted_vertices || !marks || !seed_array.seeds) {
		iscc_fs_free_SortResult(&sort);
		free(marks);
		iscc_free_SeedArray(&seed_array);
		return ISCC_NULL_SEED_ARRAY;
	}

	const scc_Vid* const sorted_v_stop = sort.sorted_vertices + nng->vertices;
	for (scc_Vid* sorted_v = sort.sorted_vertices;
	        sorted_v != sorted_v_stop; ++sorted_v) {

		#ifdef SCC_STABLE_CLUSTERING
			iscc_fs_debug_check_sort(sorted_v, sorted_v_stop - 1, sort.inwards_count);
		#endif

		if (iscc_fs_check_neighbors_marks(*sorted_v, nng, marks)) {
			if (!iscc_fs_add_seed(*sorted_v, &seed_array)) {
				iscc_fs_free_SortResult(&sort);
				free(marks);
				iscc_free_SeedArray(&seed_array);
				return ISCC_NULL_SEED_ARRAY;
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

	iscc_fs_shrink_SeedArray(&seed_array);

	return seed_array;
}


iscc_SeedArray iscc_findseeds_exclusion(const scc_Digraph* const nng,
                                        const scc_Vid seed_init_capacity,
                                        const bool updating) {

	if (!nng || !nng->tail_ptr) return ISCC_NULL_SEED_ARRAY;

	scc_Digraph exclusion_graph = iscc_exclusion_graph(nng);

	bool* const excluded = calloc(nng->vertices, sizeof(bool));

	if (!exclusion_graph.tail_ptr || !excluded) {
		scc_free_digraph(&exclusion_graph);
		free(excluded);
		return ISCC_NULL_SEED_ARRAY;
	}

	// Remove edges to vertices that cannot be seeds
	for (scc_Vid v = 0; v < nng->vertices; ++v) {
		if (nng->tail_ptr[v] == nng->tail_ptr[v + 1]) {
			excluded[v] = true;
			const scc_Vid* const ex_arc_stop = exclusion_graph.head + exclusion_graph.tail_ptr[v + 1];
			for (scc_Vid* ex_arc = exclusion_graph.head + exclusion_graph.tail_ptr[v];
			        ex_arc != ex_arc_stop; ++ex_arc) {
				*ex_arc = SCC_VID_MAX;
			}
		}
	}

	iscc_fs_SortResult sort = iscc_fs_sort_by_inwards(&exclusion_graph, updating);

	iscc_SeedArray seed_array = iscc_fs_make_seed_array(seed_init_capacity);

	if (!sort.sorted_vertices || !seed_array.seeds) {
		scc_free_digraph(&exclusion_graph);
		free(excluded);
		iscc_fs_free_SortResult(&sort);
		iscc_free_SeedArray(&seed_array);
		return ISCC_NULL_SEED_ARRAY;
	}

	const scc_Vid* const sorted_v_stop = sort.sorted_vertices + nng->vertices;
	for (scc_Vid* sorted_v = sort.sorted_vertices;
	        sorted_v != sorted_v_stop; ++sorted_v) {

		#ifdef SCC_STABLE_CLUSTERING
			iscc_fs_debug_check_sort(sorted_v, sorted_v_stop - 1, sort.inwards_count);
		#endif

		if (!excluded[*sorted_v]) {
			if (!iscc_fs_add_seed(*sorted_v, &seed_array)) {
				scc_free_digraph(&exclusion_graph);
				free(excluded);
				iscc_fs_free_SortResult(&sort);
				iscc_free_SeedArray(&seed_array);
				return ISCC_NULL_SEED_ARRAY;
			}

			excluded[*sorted_v] = true;
				
			const scc_Vid* const ex_arc_stop = exclusion_graph.head + exclusion_graph.tail_ptr[*sorted_v + 1];
			for (const scc_Vid* ex_arc = exclusion_graph.head + exclusion_graph.tail_ptr[*sorted_v];
			        ex_arc != ex_arc_stop; ++ex_arc) {
				assert(*ex_arc != SCC_VID_MAX);
				if (!excluded[*ex_arc]) {
					excluded[*ex_arc] = true;

					if (updating) {
						const scc_Vid* const ex_arc_arc_stop = exclusion_graph.head + exclusion_graph.tail_ptr[*ex_arc + 1];
						for (scc_Vid* ex_arc_arc = exclusion_graph.head + exclusion_graph.tail_ptr[*ex_arc];
						        ex_arc_arc != ex_arc_arc_stop; ++ex_arc_arc) {
							assert(*ex_arc_arc != SCC_VID_MAX);
							if (!excluded[*ex_arc_arc]) {
								iscc_fs_decrease_v_in_sort(*ex_arc_arc, sort.inwards_count, sort.vertex_index, sort.bucket_index, sorted_v);
							}
						}
					}
				}
			}
		}
	}

	scc_free_digraph(&exclusion_graph);
	free(excluded);
	iscc_fs_free_SortResult(&sort);

	iscc_fs_shrink_SeedArray(&seed_array);

	return seed_array;
}


void iscc_free_SeedArray(iscc_SeedArray* const sa) {
	if (sa) {
		free(sa->seeds);
		*sa = ISCC_NULL_SEED_ARRAY;
	}
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

static inline bool iscc_fs_check_neighbors_marks(const scc_Vid cv,
                                                 const scc_Digraph* const nng,
                                                 const bool* const marks) {
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
                                               bool* const marks) {
	assert(!marks[s]);
	marks[s] = true;

	const scc_Vid* const s_arc_stop = nng->head + nng->tail_ptr[s + 1];
	for (const scc_Vid* s_arc = nng->head + nng->tail_ptr[s];
	        s_arc != s_arc_stop; ++s_arc) {
		assert(!marks[*s_arc]);
		marks[*s_arc] = true;
	}
}


static inline bool iscc_fs_add_seed(const scc_Vid s,
                                    iscc_SeedArray* const sa) {
	assert(sa->num_seeds <= sa->seed_capacity);
	if (sa->num_seeds == sa->seed_capacity) {
		sa->seed_capacity = sa->seed_capacity + (sa->seed_capacity >> 3) + 512;
		scc_Vid* const tmp_ptr = realloc(sa->seeds, sizeof(scc_Vid[sa->seed_capacity]));
		if (!tmp_ptr) return false;
		sa->seeds = tmp_ptr;
	}
	sa->seeds[sa->num_seeds] = s;
	++(sa->num_seeds);
	return true;
}


static inline void iscc_fs_assign_neighbors(const scc_Vid s,
                                            const scc_Clabel new_label,
                                            const scc_Digraph* restrict const nng,
                                            bool* restrict const assigned,
                                            scc_Clabel* restrict const cluster_label) {
	assert(!assigned[s]);
	assigned[s] = true;
	cluster_label[s] = new_label;

	const scc_Vid* const s_arc_stop = nng->head + nng->tail_ptr[s + 1];
	for (const scc_Vid* s_arc = nng->head + nng->tail_ptr[s];
	        s_arc != s_arc_stop; ++s_arc) {
		assert(!assigned[*s_arc]);
		assigned[*s_arc] = true;
		cluster_label[*s_arc] = new_label;
	}
}


static inline void iscc_fs_assign_cl_labels(const scc_Vid s,
                                            const scc_Clabel new_label,
                                            const scc_Digraph* const nng,
                                            scc_Clabel* const cluster_label) {
	cluster_label[s] = new_label;

	const scc_Vid* const s_arc_stop = nng->head + nng->tail_ptr[s + 1];
	for (const scc_Vid* s_arc = nng->head + nng->tail_ptr[s];
	        s_arc != s_arc_stop; ++s_arc) {
		cluster_label[*s_arc] = new_label;
	}
}


static void iscc_fs_shrink_SeedArray(iscc_SeedArray* const sa) {
	if (sa && sa->seeds && (sa->seed_capacity > sa->num_seeds)) {
		scc_Vid* const tmp_ptr = realloc(sa->seeds, sizeof(scc_Vid[sa->num_seeds]));
		if (tmp_ptr) {
			sa->seeds = tmp_ptr;
			sa->seed_capacity = sa->num_seeds;
		}
	}
}


static iscc_SeedArray iscc_fs_make_seed_array(scc_Vid seed_init_capacity) {
	
	if (seed_init_capacity < 128) seed_init_capacity = 128;

	iscc_SeedArray sa = {
		.seed_capacity = seed_init_capacity,
		.num_seeds = 0,
		.seeds = malloc(sizeof(scc_Vid[seed_init_capacity])),
	};

	if (!sa.seeds) return ISCC_NULL_SEED_ARRAY;

	return sa;
}


static scc_Digraph iscc_exclusion_graph(const scc_Digraph* const nng) {
	if (!nng || !nng->tail_ptr) return SCC_NULL_DIGRAPH;

	scc_Digraph nng_transpose = scc_digraph_transpose(nng);
	if (!nng_transpose.tail_ptr) return SCC_NULL_DIGRAPH;

	scc_Digraph nng_nng_transpose = scc_adjacency_product(nng, &nng_transpose, true, false);
	scc_free_digraph(&nng_transpose);
	if (!nng_nng_transpose.tail_ptr) return SCC_NULL_DIGRAPH;

	const scc_Digraph* nng_sum[2] = {nng, &nng_nng_transpose};
	scc_Digraph exclusion_graph = scc_digraph_union(2, nng_sum);
	scc_free_digraph(&nng_nng_transpose);
	if (!exclusion_graph.tail_ptr) return SCC_NULL_DIGRAPH;

	return exclusion_graph;
}


static iscc_fs_SortResult iscc_fs_sort_by_inwards(const scc_Digraph* const nng, const bool make_indices) {

	const scc_Vid vertices = nng->vertices;

	iscc_fs_SortResult res = {
		.inwards_count = calloc(vertices, sizeof(scc_Vid)),
		.sorted_vertices = malloc(sizeof(scc_Vid[vertices])),
		.vertex_index = NULL,
		.bucket_index = NULL,
	};

	if (!res.inwards_count || !res.sorted_vertices) {
		iscc_fs_free_SortResult(&res);
		return res;
	}

	if (make_indices) {
		res.vertex_index = malloc(sizeof(scc_Vid*[vertices]));
		if (!res.vertex_index) {
			iscc_fs_free_SortResult(&res);
			return res;
		}
	}

	const scc_Vid* const arc_stop = nng->head + nng->tail_ptr[vertices];
	for (const scc_Vid* arc = nng->head; arc != arc_stop; ++arc) {
		if (*arc != SCC_VID_MAX) ++res.inwards_count[*arc];
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
		return res;
	}

	for (scc_Vid v = 0; v < vertices; ++v) {
		++bucket_count[res.inwards_count[v]];
	}

	scc_Vid bucket_cumsum = 0;
	for (scc_Vid b = 0; b <= max_inwards; ++b) {
		bucket_cumsum += bucket_count[b];
		res.bucket_index[b] = res.sorted_vertices + bucket_cumsum;
	}
	free(bucket_count);

	if (make_indices) {
		for (scc_Vid v = vertices; v > 0; ) {
			--v;
			--res.bucket_index[res.inwards_count[v]];
			*res.bucket_index[res.inwards_count[v]] = v;
			res.vertex_index[v] = res.bucket_index[res.inwards_count[v]];
		}
	} else {
		for (scc_Vid v = vertices; v > 0; ) {
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


static void iscc_fs_free_SortResult(iscc_fs_SortResult* const sr) {
	if (sr) {
		free(sr->inwards_count);
		free(sr->sorted_vertices);
		free(sr->vertex_index);
		free(sr->bucket_index);
		*sr = (iscc_fs_SortResult) { NULL, NULL, NULL, NULL };
	}
}


static inline void iscc_fs_decrease_v_in_sort(const scc_Vid v_to_decrease,
                                              scc_Vid* restrict const inwards_count,
                                              scc_Vid** restrict const vertex_index,
                                              scc_Vid** restrict const bucket_index,
                                              scc_Vid* const current_pos) {
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
			iscc_fs_debug_bucket_sort(move_to + 1, move_from, inwards_count, vertex_index);
		#endif
	}

	#ifdef SCC_STABLE_CLUSTERING
		if (bucket_index[inwards_count[v_to_decrease]] <= current_pos) {
			bucket_index[inwards_count[v_to_decrease]] = current_pos + 1;
		}
		iscc_fs_debug_bucket_sort(bucket_index[inwards_count[v_to_decrease]],
		                          move_to, inwards_count, vertex_index);
	#endif
}
