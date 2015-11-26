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

thm_Digraph thm_add_two_digraphs(const thm_Digraph* const dg_a, const thm_Digraph* const dg_b, const bool remove_duplicates) {
	const thm_Digraph* dgs_ab[2] = {dg_a, dg_b};
	return thm_add_digraphs(2, dgs_ab, remove_duplicates);
}

thm_Digraph thm_add_digraphs(const size_t num_dgs, const thm_Digraph** const dgs, const bool remove_duplicates) {
	if (num_dgs == 0) return (thm_Digraph) { 0 };
	if (!dgs[0] || !dgs[0]->valid) return (thm_Digraph) { 0 };
	thm_Vid vertices = dgs[0]->vertices;
	thm_Arcref max_arcs = dgs[0]->tail_ptr[vertices];
	for (size_t i = 1; i < num_dgs; ++i) {
		if (!dgs[i] || !dgs[i]->valid || dgs[i]->vertices != vertices) return (thm_Digraph) { 0 };
		max_arcs += dgs[i]->tail_ptr[vertices];
	}

	if (max_arcs == 0) return thm_empty_digraph(vertices, 0);

	thm_Digraph dg_out = thm_init_digraph(vertices, max_arcs);
	if (!dg_out.valid) return dg_out;

	thm_Arcref out_arc_write = 0;
	dg_out.tail_ptr[0] = 0;

	for (thm_Vid v = 0; v < vertices; ++v) {

		thm_Vid* prev_add_stop = NULL;

		for (size_t i = 0; i < num_dgs; ++i) {

			thm_Vid* toadd_stop = dgs[i]->head + dgs[i]->tail_ptr[v + 1];

			for (thm_Vid* arc_toadd = dgs[i]->head + dgs[i]->tail_ptr[v];
					arc_toadd != toadd_stop; ++arc_toadd) {
				dg_out.head[out_arc_write] = *arc_toadd;
				++out_arc_write;
				if (prev_add_stop) {
					for (thm_Vid* arc_prevadd = dg_out.head + dg_out.tail_ptr[v];
							arc_prevadd != prev_add_stop; ++arc_prevadd) {
						if (*arc_prevadd == *arc_toadd) {
							--out_arc_write;
							break;
						}
					}
				}
			}

			if (remove_duplicates) prev_add_stop = dg_out.head + out_arc_write;
		}

		dg_out.tail_ptr[v + 1] = out_arc_write;
	}

	if (remove_duplicates) thm_change_digraph_head_size(&dg_out, out_arc_write);

	return dg_out;
}
