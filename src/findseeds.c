/* scclust -- A C library for size constrained clustering
 * https://github.com/fsavje/scclust
 *
 * Copyright (C) 2015  Fredrik Savje -- http://fredriksavje.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
==============================================================================*/


#include "findseeds.h"

#include <assert.h>
#include <stdbool.h>
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


/*
 * Function:  iscc_fs_check_input 
 * --------------------
 * Checks the input to the seed finding functions.
 *
 *  nng: nearest neighbor graph to be used in the clustering
 *  clustering: struct for output clustering results (this function do not change this)
 *
 *  returns: Boolean whether input is OK.
 */
static inline bool iscc_fs_check_input(const scc_Digraph* nng, const scc_Clustering* clustering);


static scc_Digraph iscc_exclusion_graph(const scc_Digraph* nng);


static iscc_fs_SortResult iscc_fs_sort_by_inwards(const scc_Digraph* nng, bool make_indices);


static void iscc_fs_free_SortResult(iscc_fs_SortResult* sr);


static inline bool iscc_fs_check_candidate_vertex(scc_Vid cv, const scc_Digraph* nng, const bool* assigned);


static inline void iscc_fs_set_seed(scc_Vid s, const scc_Digraph* nng, scc_Clustering* clustering);


static inline void iscc_fs_decrease_v_in_sort(scc_Vid v_to_decrease, scc_Vid* restrict inwards_count, scc_Vid** restrict vertex_index, scc_Vid** restrict bucket_index, scc_Vid* current_pos);


// ==============================================================================
// External function implementations
// ==============================================================================


bool iscc_findseeds_lexical(const scc_Digraph* const nng, scc_Clustering* const clustering) {
	if (!iscc_fs_check_input(nng, clustering)) return false;

	for (scc_Vid cv = 0; cv < nng->vertices; ++cv) {
		if (iscc_fs_check_candidate_vertex(cv, nng, clustering->assigned)) {
			iscc_fs_set_seed(cv, nng, clustering);
		}
	}

	return true;
}

bool iscc_findseeds_inwards(const scc_Digraph* const nng, scc_Clustering* const clustering, const bool updating) {
	if (!iscc_fs_check_input(nng, clustering)) return false;

	bool* const assigned = clustering->assigned;

	iscc_fs_SortResult sort = iscc_fs_sort_by_inwards(nng, updating);
	if (!sort.sorted_vertices) return false;

	const scc_Vid* const sorted_v_stop = sort.sorted_vertices + nng->vertices;
	for (scc_Vid* sorted_v = sort.sorted_vertices;
	        sorted_v != sorted_v_stop; ++sorted_v) {

		if (iscc_fs_check_candidate_vertex(*sorted_v, nng, assigned)) {
			iscc_fs_set_seed(*sorted_v, nng, clustering);

			if (updating) {
				const scc_Vid* const v_arc_stop = nng->head + nng->tail_ptr[*sorted_v + 1];
				for (const scc_Vid* v_arc = nng->head + nng->tail_ptr[*sorted_v];
				        v_arc != v_arc_stop; ++v_arc) {
					const scc_Vid* const v_arc_arc_stop = nng->head + nng->tail_ptr[*v_arc + 1];
					for (scc_Vid* v_arc_arc = nng->head + nng->tail_ptr[*v_arc];
					        v_arc_arc != v_arc_arc_stop; ++v_arc_arc) {
						// Only decrease if vertex can be seed (i.e., not already assigned and not already considered)
						if (!assigned[*v_arc_arc] && sorted_v < sort.vertex_index[*v_arc_arc]) {
							iscc_fs_decrease_v_in_sort(*v_arc_arc, sort.inwards_count, sort.vertex_index, sort.bucket_index, sorted_v);
						}
					}
				}
			}
		}
	}

	iscc_fs_free_SortResult(&sort);

	return true;
}

bool iscc_findseeds_exclusion(const scc_Digraph* const nng, scc_Clustering* const clustering, const bool updating) {
	if (!iscc_fs_check_input(nng, clustering)) return false;

	scc_Digraph exclusion_graph = iscc_exclusion_graph(nng);
	if (!exclusion_graph.tail_ptr) return false;

	iscc_fs_SortResult sort = iscc_fs_sort_by_inwards(&exclusion_graph, updating);
	if (!sort.sorted_vertices) {
		scc_free_digraph(&exclusion_graph);
		return false;
	}

	bool* const excluded = calloc(nng->vertices, sizeof(bool));
	if (!excluded) {
		scc_free_digraph(&exclusion_graph);
		iscc_fs_free_SortResult(&sort);
		return false;
	}

	const scc_Vid* const sorted_v_stop = sort.sorted_vertices + nng->vertices;
	for (scc_Vid* sorted_v = sort.sorted_vertices;
	        sorted_v != sorted_v_stop; ++sorted_v) {

		if (!excluded[*sorted_v] && nng->tail_ptr[*sorted_v] != nng->tail_ptr[*sorted_v + 1]) {
			iscc_fs_set_seed(*sorted_v, nng, clustering);
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

	scc_free_digraph(&exclusion_graph);
	iscc_fs_free_SortResult(&sort);
	free(excluded);

	return true;
}


// ==============================================================================
// Internal function implementations 
// ==============================================================================


static inline bool iscc_fs_check_input(const scc_Digraph* const nng, const scc_Clustering* const clustering) {
	if (!nng || !nng->tail_ptr) return false;
	if (!clustering || !clustering->assigned || !clustering->seed || !clustering->cluster_label) return false;
	if (nng->vertices != clustering->vertices || nng->vertices == 0) return false;
	return true;
}

static scc_Digraph iscc_exclusion_graph(const scc_Digraph* const nng) {
	if (!nng || !nng->tail_ptr) return scc_null_digraph();

	scc_Digraph nng_transpose = scc_digraph_transpose(nng);
	if (!nng_transpose.tail_ptr) return scc_null_digraph();

	scc_Digraph nng_nng_transpose = scc_adjacency_product(nng, &nng_transpose, true, false);
	scc_free_digraph(&nng_transpose);
	if (!nng_nng_transpose.tail_ptr) return scc_null_digraph();

	const scc_Digraph* nng_sum[2] = {nng, &nng_nng_transpose};
	scc_Digraph exclusion_graph = scc_digraph_union(2, nng_sum);
	scc_free_digraph(&nng_nng_transpose);
	if (!exclusion_graph.tail_ptr) return scc_null_digraph();

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

static inline bool iscc_fs_check_candidate_vertex(const scc_Vid cv, const scc_Digraph* const nng, const bool* const assigned) {
	if (assigned[cv]) return false;

	const scc_Vid* cv_arc = nng->head + nng->tail_ptr[cv];
	const scc_Vid* const cv_arc_stop = nng->head + nng->tail_ptr[cv + 1];
	if (cv_arc == cv_arc_stop) return false;

	for (; cv_arc != cv_arc_stop; ++cv_arc) { 
		if (assigned[*cv_arc]) return false;
	}

	return true;
}

static inline void iscc_fs_set_seed(const scc_Vid s, const scc_Digraph* const nng, scc_Clustering* const clustering) {
	
	assert(!clustering->assigned[s]);
	clustering->seed[s] = true;
	clustering->assigned[s] = true;
	clustering->cluster_label[s] = clustering->num_clusters;

	const scc_Vid* const s_arc_stop = nng->head + nng->tail_ptr[s + 1];
	for (const scc_Vid* s_arc = nng->head + nng->tail_ptr[s];
	        s_arc != s_arc_stop; ++s_arc) {
		assert(!clustering->assigned[*s_arc]);
		clustering->assigned[*s_arc] = true;
		clustering->cluster_label[*s_arc] = clustering->num_clusters;
	}

	++(clustering->num_clusters);
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

	// Check so list not already sorted
	if (move_from != move_to) {
		// Do swap
		*move_from = *move_to;
		*move_to = v_to_decrease;

		// Update vertex index
		vertex_index[*move_to] = move_to;
		vertex_index[*move_from] = move_from;
	}

	// Update firstX index
	bucket_index[inwards_count[v_to_decrease]] = move_to + 1;

	// Decrease count on moved vertex
	--inwards_count[v_to_decrease];
}
