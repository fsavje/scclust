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


#include "../include/digraph.h"

#include <stdbool.h>
#include <stdlib.h>

#include "../include/config.h"


tbg_Digraph tbg_init_digraph(const tbg_Vid vertices, const tbg_Arcref max_arcs) {
	tbg_Digraph dg = {
		.vertices = vertices,
		.max_arcs = max_arcs,
		.head = NULL,
		.tail_ptr = malloc(sizeof(tbg_Arcref[vertices + 1])),
	};

	if (!dg.tail_ptr) return tbg_null_digraph();

	if (max_arcs > 0) {
		dg.head = malloc(sizeof(tbg_Vid[max_arcs]));
		if (!dg.head) tbg_free_digraph(&dg);
	}

	return dg;
}


void tbg_free_digraph(tbg_Digraph* const dg) {
	if (dg) {
		free(dg->head);
		free(dg->tail_ptr);
		*dg = tbg_null_digraph();
	}
}


bool tbg_change_arc_storage(tbg_Digraph* const dg, const tbg_Arcref new_max_arcs) {
	if (!dg || !dg->tail_ptr) return false;
	if (dg->max_arcs == new_max_arcs) return true;
	if (dg->tail_ptr[dg->vertices] > new_max_arcs) return false;

	if (new_max_arcs == 0) {
		free(dg->head);
		dg->head = NULL;
		dg->max_arcs = 0;
	} else {
		tbg_Vid* const tmp_ptr = realloc(dg->head, sizeof(tbg_Vid[new_max_arcs]));
		if (!tmp_ptr) return false;
		dg->head = tmp_ptr;
		dg->max_arcs = new_max_arcs;
	}

	return true;
}


tbg_Digraph tbg_empty_digraph(const tbg_Vid vertices, const tbg_Arcref max_arcs) {
	tbg_Digraph dg = tbg_init_digraph(vertices, max_arcs);
	if (!dg.tail_ptr) return dg;
	
	for (tbg_Vid v = 0; v <= vertices; ++v) dg.tail_ptr[v] = 0;

	return dg;
}


tbg_Digraph tbg_balanced_digraph(const tbg_Vid vertices, const tbg_Vid arcs_per_vertex, tbg_Vid* const heads) {
	tbg_Digraph dg = tbg_init_digraph(vertices, 0);
	if (!dg.tail_ptr) return dg;

	dg.max_arcs = vertices * arcs_per_vertex;
	dg.head = heads;
	for (tbg_Vid v = 0; v <= vertices; ++v) {
		dg.tail_ptr[v] = v * arcs_per_vertex;
	}
	
	return dg;
}


tbg_Digraph tbg_copy_digraph(const tbg_Digraph* const dg) {
	if (!dg || !dg->tail_ptr) return tbg_null_digraph();
	if (dg->vertices == 0) return tbg_empty_digraph(0, 0);

	tbg_Digraph dg_out = tbg_init_digraph(dg->vertices, dg->tail_ptr[dg->vertices]);
	if (!dg_out.tail_ptr) return dg_out;

	for (tbg_Vid v = 0; v <= dg->vertices; ++v) dg_out.tail_ptr[v] = dg->tail_ptr[v];
	for (tbg_Arcref a = 0; a < dg->tail_ptr[dg->vertices]; ++a) dg_out.head[a] = dg->head[a];

	return dg_out;
}
