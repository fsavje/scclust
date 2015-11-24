#include "digraph.h"
#include <stdlib.h>

thm_Digraph thm_empty_digraph(const thm_Vid num_vertices, const thm_Arcref num_arcs){
	thm_Digraph dg = {
		.num_vertices = num_vertices,
		.num_arcs = num_arcs,
		.head = malloc(sizeof(thm_Vid[num_arcs])),
		.tail_ptr = calloc(num_vertices + 1, sizeof(thm_Arcref))
	};

	return dg;
}

thm_Digraph thm_balanced_digraph(const thm_Vid num_vertices, const thm_Vid arcs_per_vertex, thm_Vid* const heads){
	thm_Digraph dg = {
		.num_vertices = num_vertices,
		.num_arcs = num_vertices * arcs_per_vertex,
		.head = heads,
		.tail_ptr = malloc(sizeof(thm_Arcref[num_vertices + 1]))
	};

	for (thm_Vid v = 0; v <= num_vertices; ++v) {
		dg.tail_ptr[v] = v * arcs_per_vertex;
	}

	return dg;
}

void thm_free_digraph(thm_Digraph* const a){
	if (a) {
		free(a->head);
		free(a->tail_ptr);
		*a = (thm_Digraph) { 0 };
	}
}

//thm_Digraph thm_add_digraphs(const thm_Digraph* const a, const thm_Digraph* const b){

	
//}
