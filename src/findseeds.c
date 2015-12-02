/*  scclust -- A C library for size constrained clustering
    https://github.com/fsavje/scclust

    Copyright (C) 2015  Fredrik Savje -- http://fredriksavje.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
==============================================================================*/

#include "findseeds.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../include/config.h"
#include "../include/digraph.h"
#include "../include/nng_clustering.h"


// Internal structs

typedef struct iscc_fs_Vinfo iscc_fs_Vinfo;
struct iscc_fs_Vinfo {
	scc_Vid vertex_id;
	scc_Vid count;
};

typedef struct iscc_fs_SortResult iscc_fs_SortResult;
struct iscc_fs_SortResult {
	iscc_fs_Vinfo* sorted_vinfo;
	iscc_fs_Vinfo** vertex_index;
	iscc_fs_Vinfo** firstX_index;
};

// Static function prototypes

static int iscc_fs_count_compare(const void* a, const void* b);
static iscc_fs_SortResult iscc_fs_sort_by_inwards(const scc_Digraph* nng, bool make_indices);
static void iscc_fs_free_SortResult(iscc_fs_SortResult* sr);
static inline bool iscc_fs_check_candidate_vertex(scc_Vid cv, const scc_Digraph* nng, const bool* assigned);
static inline void iscc_fs_decrease_v_in_sort(scc_Vid v_to_decrease, iscc_fs_Vinfo** restrict vertex_index, iscc_fs_Vinfo** restrict firstX_index, iscc_fs_Vinfo* current_pos);

// External function implementations

bool iscc_findseeds_lexical(const scc_Digraph* const nng, scc_Clustering* const clustering) {
	if (!nng || !nng->tail_ptr) return false;
	if (!clustering || !clustering->assigned || !clustering->seed || !clustering->cluster_label) return false;

	const scc_Vid vertices = nng->vertices;
	if (vertices != clustering->vertices || vertices == 0) return false;

	for (scc_Vid cv = 0; cv < vertices; ++cv) {
		if (iscc_fs_check_candidate_vertex(cv, nng, clustering->assigned)) {
			clustering->seed[cv] = true;
			clustering->assigned[cv] = true;
			clustering->cluster_label[cv] = clustering->num_clusters;

			const scc_Vid* s_arc = nng->head + nng->tail_ptr[cv];
			const scc_Vid* const s_arc_stop = nng->head + nng->tail_ptr[cv + 1];
			for (; s_arc != s_arc_stop; ++s_arc) {
				clustering->assigned[*s_arc] = true;
				clustering->cluster_label[*s_arc] = clustering->num_clusters;
			}

			++(clustering->num_clusters);
		}
	}

	return true;
}


bool iscc_findseeds_inwards(const scc_Digraph* const nng, scc_Clustering* const clustering, const bool updating) {
	if (!nng || !nng->tail_ptr) return false;
	if (!clustering || !clustering->assigned || !clustering->seed || !clustering->cluster_label) return false;

	const scc_Vid vertices = nng->vertices;
	if (vertices != clustering->vertices || vertices == 0) return false;

	bool* const assigned = clustering->assigned;

	iscc_fs_SortResult sort = iscc_fs_sort_by_inwards(nng, updating);
	if (!sort.sorted_vinfo) return false;

	const iscc_fs_Vinfo* const sorted_v_stop = sort.sorted_vinfo + vertices;
	for (iscc_fs_Vinfo* sorted_v = sort.sorted_vinfo;
		sorted_v != sorted_v_stop; ++sorted_v) {

		const scc_Vid cv = sorted_v->vertex_id;

		if (iscc_fs_check_candidate_vertex(cv, nng, assigned)) {

			clustering->seed[cv] = true;
			assigned[cv] = true;
			clustering->cluster_label[cv] = clustering->num_clusters;

			const scc_Vid* const v_arc_stop = nng->head + nng->tail_ptr[cv + 1];
			for (const scc_Vid* v_arc = nng->head + nng->tail_ptr[cv];
					v_arc != v_arc_stop; ++v_arc) {

				assigned[*v_arc] = true;
				clustering->cluster_label[*v_arc] = clustering->num_clusters;

				if (updating) {
					const scc_Vid* const v_arc2_stop = nng->head + nng->tail_ptr[*v_arc + 1];
					for (scc_Vid* v_arc2 = nng->head + nng->tail_ptr[*v_arc];
							v_arc2 != v_arc2_stop; ++v_arc2) {
						if (!assigned[*v_arc2]) {
							iscc_fs_decrease_v_in_sort(*v_arc2, sort.vertex_index, sort.firstX_index, sorted_v);
						}
					}
				}
			}

			++(clustering->num_clusters);
		}
	}

	iscc_fs_free_SortResult(&sort);

	return true;
}


bool iscc_findseeds_inwards_onearc(const scc_Digraph* const nng, scc_Clustering* const clustering, const bool updating) {
	if (!nng || !nng->tail_ptr) return false;
	if (!clustering || !clustering->assigned || !clustering->seed || !clustering->cluster_label) return false;

	const scc_Vid vertices = nng->vertices;
	if (vertices != clustering->vertices || vertices == 0) return false;

	bool* const assigned = clustering->assigned;

	iscc_fs_SortResult sort = iscc_fs_sort_by_inwards(nng, updating);
	if (!sort.sorted_vinfo) return false;

	const iscc_fs_Vinfo* const sorted_v_stop = sort.sorted_vinfo + vertices;
	for (iscc_fs_Vinfo* sorted_v = sort.sorted_vinfo;
		sorted_v != sorted_v_stop; ++sorted_v) {

		const scc_Vid cv = sorted_v->vertex_id;
		if (!assigned[cv]) {

			const scc_Vid cv_arc = nng->head[nng->tail_ptr[cv]];
			if (!assigned[cv_arc]) {

				// Found valid seed
				clustering->seed[cv] = true;
				assigned[cv] = true;
				clustering->cluster_label[cv] = clustering->num_clusters;

				assigned[cv_arc] = true;
				clustering->cluster_label[cv_arc] = clustering->num_clusters;

				++(clustering->num_clusters);

				if (updating) {
					const scc_Vid cv_arc_arc = nng->head[nng->tail_ptr[cv_arc]];
					if (!assigned[cv_arc_arc]) {
						iscc_fs_decrease_v_in_sort(cv_arc_arc, sort.vertex_index, sort.firstX_index, sorted_v);
					}
				}
			}
		}
	}

	iscc_fs_free_SortResult(&sort);

	return true;
}

bool iscc_findseeds_exclusion(const scc_Digraph* const nng, scc_Clustering* const clustering, const bool updating) {
	if (!nng || !nng->tail_ptr) return false;
	if (!clustering || !clustering->assigned || !clustering->seed || !clustering->cluster_label) return false;

	const scc_Vid vertices = nng->vertices;
	if (vertices != clustering->vertices || vertices == 0) return false;

	bool* const assigned = clustering->assigned;

	scc_Digraph tmp_digraph = scc_digraph_transpose(nng);
	if (!tmp_digraph.tail_ptr) return false;

	scc_Digraph nng_nng_transpose = scc_adjacency_product(nng, &tmp_digraph, true, false);
	scc_free_digraph(&tmp_digraph);
	if (!nng_nng_transpose.tail_ptr) return false;

	const scc_Digraph* nng_sum[2] = {nng, &nng_nng_transpose};
	scc_Digraph exclusion_graph = scc_digraph_union(2, nng_sum);
	scc_free_digraph(&nng_nng_transpose);
	if (!exclusion_graph.tail_ptr) return false;

	iscc_fs_SortResult sort = iscc_fs_sort_by_inwards(&exclusion_graph, updating);
	if (!sort.sorted_vinfo) {
		scc_free_digraph(&exclusion_graph);
		return false;
	}

	bool* const excluded = calloc(vertices, sizeof(bool));
	if (!excluded) {
		scc_free_digraph(&exclusion_graph);
		iscc_fs_free_SortResult(&sort);
		return false;
	}
	
	const iscc_fs_Vinfo* const sorted_v_stop = sort.sorted_vinfo + vertices;
	for (iscc_fs_Vinfo* sorted_v = sort.sorted_vinfo;
		sorted_v != sorted_v_stop; ++sorted_v) {

		const scc_Vid cv = sorted_v->vertex_id;

		if (!excluded[cv]) {

			clustering->seed[cv] = true;
			assigned[cv] = true;
			excluded[cv] = true;
			clustering->cluster_label[cv] = clustering->num_clusters;

			const scc_Vid* const v_arc_stop = nng->head + nng->tail_ptr[cv + 1];
			for (const scc_Vid* v_arc = nng->head + nng->tail_ptr[cv];
					v_arc != v_arc_stop; ++v_arc) {
				assigned[*v_arc] = true;
				clustering->cluster_label[*v_arc] = clustering->num_clusters;
			}

			++(clustering->num_clusters);

			const scc_Vid* const ex_arc_stop = exclusion_graph.head + exclusion_graph.tail_ptr[cv + 1];
			for (const scc_Vid* ex_arc = exclusion_graph.head + exclusion_graph.tail_ptr[cv];
					ex_arc != ex_arc_stop; ++ex_arc) {

				excluded[*ex_arc] = true;

				if (updating) {
					const scc_Vid* const ex_arc2_stop = exclusion_graph.head + exclusion_graph.tail_ptr[*ex_arc + 1];
					for (scc_Vid* ex_arc2 = exclusion_graph.head + exclusion_graph.tail_ptr[*ex_arc];
							ex_arc2 != ex_arc2_stop; ++ex_arc2) {
						if (!excluded[*ex_arc2]) {
							iscc_fs_decrease_v_in_sort(*ex_arc2, sort.vertex_index, sort.firstX_index, sorted_v);
						}
					}
				}
			}
		}
	}

	scc_free_digraph(&exclusion_graph);
	free(excluded);
	iscc_fs_free_SortResult(&sort);

	return true;
}


// Static function implementations 

static int iscc_fs_count_compare(const void* const a, const void* const b) {
    const iscc_fs_Vinfo v_a = *(const iscc_fs_Vinfo*)a;
    const iscc_fs_Vinfo v_b = *(const iscc_fs_Vinfo*)b;

    if (v_a.count < v_b.count) return -1;
    if (v_a.count > v_b.count) return 1;

    // Stable sorting
    #ifdef SCC_STABLE_SORTING
	    if (v_a.vertex_id < v_b.vertex_id) return -1;
	    if (v_a.vertex_id > v_b.vertex_id) return 1;
    #endif

    return 0;
}

static iscc_fs_SortResult iscc_fs_sort_by_inwards(const scc_Digraph* const nng, const bool make_indices) {

	const scc_Vid vertices = nng->vertices;

	iscc_fs_SortResult res = {
		.sorted_vinfo = malloc(sizeof(iscc_fs_Vinfo[vertices])),
		.vertex_index = NULL,
		.firstX_index = NULL,
	};
	if (!res.sorted_vinfo) return (iscc_fs_SortResult) { NULL, NULL, NULL };

	for (scc_Vid v = 0; v < vertices; ++v) {
		res.sorted_vinfo[v].vertex_id = v;
		res.sorted_vinfo[v].count = 0;
	}

	const scc_Vid* const arc_stop = nng->head + nng->tail_ptr[vertices];
	for (const scc_Vid* arc = nng->head;
			arc != arc_stop; ++arc) {
		++(res.sorted_vinfo[*arc].count);
	}

	qsort(res.sorted_vinfo, vertices,
		  sizeof(iscc_fs_Vinfo),
		  iscc_fs_count_compare);

	if (make_indices) {
		const scc_Vid max_inwards = res.sorted_vinfo[vertices - 1].count;
		res.vertex_index = malloc(sizeof(iscc_fs_Vinfo*[vertices]));
		res.firstX_index = malloc(sizeof(iscc_fs_Vinfo*[max_inwards + 1]));
		if (!res.vertex_index || !res.firstX_index) {
			free(res.sorted_vinfo);
			free(res.vertex_index);
			free(res.firstX_index);
			return (iscc_fs_SortResult) { NULL, NULL, NULL };
		}

		for (iscc_fs_Vinfo* pos = res.sorted_vinfo + vertices - 1;
				pos != res.sorted_vinfo; --pos) {
			res.vertex_index[pos->vertex_id] = pos;

			if (pos->count != (pos - 1)->count) {
				for (scc_Vid write_index = pos->count;
						write_index > pos->count; --write_index) {
					res.firstX_index[write_index] = pos;
				}
			}
		}
		for (scc_Vid write_index = res.sorted_vinfo->count;
				write_index > 0; --write_index) {
			res.firstX_index[write_index] = res.sorted_vinfo;
		}
		res.firstX_index[0] = res.sorted_vinfo;
	}

	return res;
}

static void iscc_fs_free_SortResult(iscc_fs_SortResult* const sr) {
	if (sr) {
		free(sr->sorted_vinfo);
		free(sr->vertex_index);
		free(sr->firstX_index);
		*sr = (iscc_fs_SortResult) { NULL, NULL, NULL };
	}
}

static inline bool iscc_fs_check_candidate_vertex(const scc_Vid cv, const scc_Digraph* const nng, const bool* const assigned) {
	if (assigned[cv]) return false;

	const scc_Vid* cv_arc = nng->head + nng->tail_ptr[cv];
	const scc_Vid* const cv_arc_stop = nng->head + nng->tail_ptr[cv + 1];
	for (; !assigned[*cv_arc] && cv_arc != cv_arc_stop; ++cv_arc) { }

	return (cv_arc == cv_arc_stop);
}

static inline void iscc_fs_decrease_v_in_sort(const scc_Vid v_to_decrease,
												iscc_fs_Vinfo** restrict const vertex_index,
												iscc_fs_Vinfo** restrict const firstX_index,
												iscc_fs_Vinfo* const current_pos) {
	// Assert that vertex index is correct
	assert(v_to_decrease == vertex_index[v_to_decrease]->vertex_id);

	// Find vertices to move
	iscc_fs_Vinfo* move_from = vertex_index[v_to_decrease];
	iscc_fs_Vinfo* move_to = firstX_index[move_from->count];
	if (move_to <= current_pos) move_to = current_pos + 1;

	// Assert that swap vertices have the same count
	assert(move_from->count == move_to->count);

	// Check so list not already sorted
	if (move_from != move_to) {
		// Do swap
		move_from->vertex_id = move_to->vertex_id;
		move_to->vertex_id = v_to_decrease;

		// Update vertex index
		vertex_index[v_to_decrease] = move_to;
		vertex_index[move_from->vertex_id] = move_from;
	}

	// Update firstX index
	++(firstX_index[move_from->count]);

	// Decrease count on moved vertex
	--(move_to->count);
}
