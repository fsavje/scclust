#ifndef THM_DIGRAPH_HG
#define THM_DIGRAPH_HG

#include <stddef.h>
#include "config.h"

typedef struct thm_Digraph thm_Digraph;
struct thm_Digraph {
	// Number of vertices in graph: vertices < MAX(thm_Vid).
	thm_Vid vertices;

	// Array of pointers: [0, ..., vertices].
	// For arcs for which vertex i is the tail, head[tail_ptr[i]] is the head of the first arc and head[tail_ptr[i+1] - 1] is the last arc.
	thm_Arcref* tail_ptr;

	// Maximum number of arcs in graph: max_arcs <= MAX(thm_Arcref).
	thm_Arcref max_arcs;

	// Array of unsigned longs: [0, ..., max_arcs - 1].
	// Contains the head of each arc, indicated by vertex ID.
	thm_Vid* head;
};

thm_Digraph thm_init_digraph(thm_Vid vertices);
thm_Digraph thm_empty_digraph(thm_Vid vertices, thm_Arcref max_arcs);
thm_Digraph thm_balanced_digraph(thm_Vid vertices, thm_Vid arcs_per_vertex, thm_Vid* heads);
void thm_free_digraph(thm_Digraph* a);

//thm_Digraph thm_add_digraphs(const thm_Digraph* a, const thm_Digraph* b);
//fm_digraph fm_transpose(fm_digraph a);
//fm_digraph fm_undirected(fm_digraph a);
//fm_digraph fm_exclusion_graph(fm_digraph a);

#endif
