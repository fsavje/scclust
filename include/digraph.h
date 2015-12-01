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


#ifndef SCC_DIGRAPH_HG
#define SCC_DIGRAPH_HG

#include <stdbool.h>
#include <stddef.h>

#include "config.h"


// Main digraph struct
typedef struct scc_Digraph scc_Digraph;
struct scc_Digraph {
	// Number of vertices in graph.
	const scc_Vid vertices;

	// Maximum number of arcs in graph (i.e., length of `head').
	scc_Arcref max_arcs;

	// Array of scc_Vid: [0, ..., max_arcs - 1].
	// Contains the head of each arc, indicated by vertex ID.
	// May be NULL if max_arcs == 0.
	scc_Vid* head;

	// Array of scc_Arcref: [0, ..., vertices].
	// For arcs for which vertex i is the tail, head[tail_ptr[i]] is the head of the first arc and head[tail_ptr[i+1] - 1] is the last arc.
	// May never be NULL.
	scc_Arcref* const tail_ptr;
};

static inline scc_Digraph scc_null_digraph(void) {
  return (scc_Digraph) { 0, 0, NULL, NULL };
}

scc_Digraph scc_init_digraph(scc_Vid vertices, scc_Arcref max_arcs);
void scc_free_digraph(scc_Digraph* dg);
bool scc_change_arc_storage(scc_Digraph* dg, scc_Arcref new_max_arcs);
scc_Digraph scc_empty_digraph(scc_Vid vertices, scc_Arcref max_arcs);
scc_Digraph scc_balanced_digraph(scc_Vid vertices, scc_Vid arcs_per_vertex, scc_Vid* heads);
scc_Digraph scc_copy_digraph(const scc_Digraph* dg);
scc_Digraph scc_digraph_union(size_t num_dgs, const scc_Digraph* const dgs[static num_dgs]);
scc_Digraph scc_digraph_transpose(const scc_Digraph* dg);
scc_Digraph scc_adjacency_product(const scc_Digraph* dg_a, const scc_Digraph* dg_b, bool force_diagonal, bool ignore_diagonal);


#endif
