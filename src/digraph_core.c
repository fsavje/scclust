/* ==============================================================================
 * scclust -- A C library for size constrained clustering
 * https://github.com/fsavje/scclust
 *
 * Copyright (C) 2015-2016  Fredrik Savje -- http://fredriksavje.com
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * ============================================================================== */


#include "../include/digraph.h"

#include <stdbool.h>
#include <stdlib.h>
#include "../include/config.h"


// ==============================================================================
// External function implementations
// ============================================================================== 

scc_Digraph scc_init_digraph(const scc_Vid vertices,
                             const scc_Arci max_arcs)
{
	scc_Digraph dg = {
		.vertices = vertices,
		.max_arcs = max_arcs,
		.head = NULL,
		.tail_ptr = malloc(sizeof(scc_Arci[vertices + 1])),
	};

	if (!dg.tail_ptr) return SCC_NULL_DIGRAPH;

	if (max_arcs > 0) {
		dg.head = malloc(sizeof(scc_Vid[max_arcs]));
		if (!dg.head) scc_free_digraph(&dg);
	}

	return dg;
}


void scc_free_digraph(scc_Digraph* const dg)
{
	if (dg) {
		free(dg->head);
		free(dg->tail_ptr);
		*dg = SCC_NULL_DIGRAPH;
	}
}


bool scc_change_arc_storage(scc_Digraph* const dg,
                            const scc_Arci new_max_arcs)
{
	if (!dg || !dg->tail_ptr) return false;
	if (dg->max_arcs == new_max_arcs) return true;
	if (dg->tail_ptr[dg->vertices] > new_max_arcs) return false;

	if (new_max_arcs == 0) {
		free(dg->head);
		dg->head = NULL;
		dg->max_arcs = 0;
	} else {
		scc_Vid* const tmp_ptr = realloc(dg->head, sizeof(scc_Vid[new_max_arcs]));
		if (!tmp_ptr) return false;
		dg->head = tmp_ptr;
		dg->max_arcs = new_max_arcs;
	}

	return true;
}


scc_Digraph scc_empty_digraph(const scc_Vid vertices,
                              const scc_Arci max_arcs)
{
	scc_Digraph dg = scc_init_digraph(vertices, max_arcs);
	if (!dg.tail_ptr) return SCC_NULL_DIGRAPH;
	
	for (scc_Vid v = 0; v <= vertices; ++v) dg.tail_ptr[v] = 0;

	return dg;
}


scc_Digraph scc_copy_digraph(const scc_Digraph* const dg)
{
	if (!dg || !dg->tail_ptr) return SCC_NULL_DIGRAPH;
	if (dg->vertices == 0) return scc_empty_digraph(0, 0);

	scc_Digraph dg_out = scc_init_digraph(dg->vertices, dg->tail_ptr[dg->vertices]);
	if (!dg_out.tail_ptr) return SCC_NULL_DIGRAPH;

	for (scc_Vid v = 0; v <= dg->vertices; ++v) dg_out.tail_ptr[v] = dg->tail_ptr[v];
	for (scc_Arci a = 0; a < dg->tail_ptr[dg->vertices]; ++a) dg_out.head[a] = dg->head[a];

	return dg_out;
}
