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


#ifndef THM_DIGRAPH_HG
#define THM_DIGRAPH_HG

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Vertex ID type (must be unsigned)
// # of vertices < MAX(thm_Vid)
typedef uint32_t thm_Vid;

// MAX(thm_Vid): maximum value of thm_Vid
#define THM_VID_MAX UINT32_MAX

// Arc reference type (must be unsigned)
// # of arcs in any digraph <= MAX(thm_Arcref)
typedef uint32_t thm_Arcref;


typedef struct thm_Digraph thm_Digraph;
struct thm_Digraph {
	// Number of vertices in graph: vertices < MAX(thm_Vid).
	thm_Vid vertices;

	// Maximum number of arcs in graph: max_arcs <= MAX(thm_Arcref).
	thm_Arcref max_arcs;

	// Array of unsigned longs: [0, ..., max_arcs - 1].
	// Contains the head of each arc, indicated by vertex ID.
	// May be NULL if max_arcs == 0.
	thm_Vid* head;

	// Array of pointers: [0, ..., vertices].
	// For arcs for which vertex i is the tail, head[tail_ptr[i]] is the head of the first arc and head[tail_ptr[i+1] - 1] is the last arc.
	// May never be NULL.
	thm_Arcref* tail_ptr;
};


thm_Digraph thm_init_digraph(thm_Vid vertices, thm_Arcref max_arcs);
void thm_free_digraph(thm_Digraph* dg);
bool thm_change_digraph_head_size(thm_Digraph* dg, thm_Arcref new_max_arcs);
thm_Digraph thm_empty_digraph(thm_Vid vertices, thm_Arcref max_arcs);
thm_Digraph thm_identity_digraph(thm_Vid vertices);
thm_Digraph thm_balanced_digraph(thm_Vid vertices, thm_Vid arcs_per_vertex, thm_Vid* heads);
thm_Digraph thm_copy_digraph(const thm_Digraph* dg);
thm_Digraph thm_digraph_union_two(const thm_Digraph* dg_a, const thm_Digraph* dg_b);
thm_Digraph thm_digraph_union(size_t num_dgs, const thm_Digraph* dgs[num_dgs]);
thm_Digraph thm_digraph_transpose(const thm_Digraph* dg);
thm_Digraph thm_digraph_undirected(const thm_Digraph* dg);
thm_Digraph thm_digraph_adjacency_product(const thm_Digraph* dg_a, const thm_Digraph* dg_b, bool force_diagonal, bool ignore_diagonal);
thm_Digraph thm_digraph_walks(const thm_Digraph* dg, thm_Vid length);
thm_Digraph thm_digraph_power(const thm_Digraph* dg, thm_Vid exponent);

#endif
