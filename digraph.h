#ifndef THM_DIGRAPH_HG
#define THM_DIGRAPH_HG

#include <stddef.h>
#include "config.h"

typedef struct thm_Digraph thm_Digraph;
struct thm_Digraph {
	thm_Vid num_vertices;
	thm_Arcref num_arcs;

	// Array of unsigned longs: [0, ..., num_arcs - 1], where num_arcs <= MAX(thm_Arcref).
	// Contains the head of each arc, indicated by vertex ID.
	thm_Vid* head;

	// Array of pointers: [0, ..., num_vertices], where num_vertices < MAX(thm_Vid).
	// For arcs for which vertex i is the tail, head[tail_ptr[i]] is the head of the first arc and head[tail_ptr[i+1] - 1] is the last arc.
	thm_Arcref* tail_ptr;
};

thm_Digraph thm_empty_digraph(thm_Vid num_vertices, thm_Arcref num_arcs);

thm_Digraph thm_balanced_digraph(thm_Vid num_vertices, thm_Vid arcs_per_vertex, thm_Vid* heads);

void thm_free_digraph(thm_Digraph* a);

thm_Digraph thm_add_digraphs(const thm_Digraph* a, const thm_Digraph* b);

//fm_digraph fm_transpose(fm_digraph a);
//fm_digraph fm_undirected(fm_digraph a);
//fm_digraph fm_exclusion_graph(fm_digraph a);

#endif
