#include "digraph.h"

#include <stdlib.h>

thm_Digraph thm_init_digraph(const thm_Vid vertices, const thm_Arcref max_arcs) {
	thm_Digraph dg = {
		.head = NULL,
		.tail_ptr = malloc(sizeof(thm_Arcref[vertices + 1])),
		.vertices = vertices,
		.max_arcs = max_arcs,
		.valid = true,
	};

	if (!dg.tail_ptr) return (thm_Digraph) { 0 };

	if (max_arcs > 0) {
		dg.head = malloc(sizeof(thm_Vid[max_arcs]));
		if (!dg.head) thm_free_digraph(&dg);
	}

	return dg;
}

void thm_free_digraph(thm_Digraph* const dg) {
	if (dg) {
		free(dg->head);
		free(dg->tail_ptr);
		*dg = (thm_Digraph) { 0 };
	}
}

bool thm_change_digraph_head_size(thm_Digraph* const dg, const thm_Arcref new_max_arcs) {
	if (dg->max_arcs == new_max_arcs) return true;
	if (dg->tail_ptr[dg->vertices] > new_max_arcs) return false;

	if (new_max_arcs == 0) {
		free(dg->head);
		dg->head = NULL;
		dg->max_arcs = 0;
	} else {
		thm_Vid* tmp_ptr = realloc(dg->head, sizeof(thm_Vid[new_max_arcs]));
		if (!tmp_ptr) return false;
		dg->head = tmp_ptr;
		dg->max_arcs = new_max_arcs;
	}

	return true;
}

thm_Digraph thm_empty_digraph(const thm_Vid vertices, const thm_Arcref max_arcs) {
	thm_Digraph dg = thm_init_digraph(vertices, max_arcs);
	if (!dg.valid) return dg;
	
	for (thm_Vid v = 0; v <= vertices; ++v) dg.tail_ptr[v] = 0;

	return dg;
}

thm_Digraph thm_balanced_digraph(const thm_Vid vertices, const thm_Vid arcs_per_vertex, thm_Vid* const heads) {
	thm_Digraph dg = thm_init_digraph(vertices, 0);
	if (!dg.valid) return dg;

	dg.max_arcs = vertices * arcs_per_vertex;
	dg.head = heads;
	for (thm_Vid v = 0; v <= vertices; ++v) {
		dg.tail_ptr[v] = v * arcs_per_vertex;
	}
	
	return dg;
}

}

//thm_Digraph thm_add_digraphs(const thm_Digraph* const a, const thm_Digraph* const b){

	
//}
