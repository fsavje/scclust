#include "digraph.h"
#include <stdlib.h>

thm_Digraph thm_init_digraph(const thm_Vid vertices) {
	thm_Digraph dg = {
		.vertices = vertices,
		.tail_ptr = malloc(sizeof(thm_Arcref[vertices + 1])),
		.max_arcs = 0,
		.head = NULL,
	};

	return dg;
}

thm_Digraph thm_empty_digraph(const thm_Vid vertices, const thm_Arcref max_arcs) {
	thm_Digraph dg = thm_init_digraph(vertices);
	for (thm_Vid v = 0; v <= vertices; ++v) dg.tail_ptr[v] = 0;
	dg.max_arcs = max_arcs;
	dg.head = malloc(sizeof(thm_Vid[max_arcs]));

	return dg;
}

thm_Digraph thm_balanced_digraph(const thm_Vid vertices, const thm_Vid arcs_per_vertex, thm_Vid* const heads) {
	thm_Digraph dg = thm_init_digraph(vertices);
	for (thm_Vid v = 0; v <= vertices; ++v) {
		dg.tail_ptr[v] = v * arcs_per_vertex;
	}
	dg.max_arcs = vertices * arcs_per_vertex;
	dg.head = heads;

	return dg;
}

void thm_free_digraph(thm_Digraph* const a) {
	if (a) {
		free(a->head);
		free(a->tail_ptr);
		*a = (thm_Digraph) { 0 };
	}
}

//thm_Digraph thm_add_digraphs(const thm_Digraph* const a, const thm_Digraph* const b){

	
//}
