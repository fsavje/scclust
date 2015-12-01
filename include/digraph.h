/* Copyright 2015 Fredrik Savje

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
==============================================================================*/

#ifndef SCC_CORE_HG
#define SCC_CORE_HG

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
