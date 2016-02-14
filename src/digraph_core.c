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


#include "digraph_core.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "../include/scclust.h"


// ==============================================================================
// External function implementations
// ============================================================================== 

scc_Digraph scc_init_digraph(const size_t vertices,
                             const size_t max_arcs)
{
	assert(vertices < SCC_VID_MAX);
	if (max_arcs > SCC_ARCI_MAX) return SCC_NULL_DIGRAPH;

	scc_Digraph dg = {
		.vertices = vertices,
		.max_arcs = max_arcs,
		.head = NULL,
		.tail_ptr = malloc(sizeof(scc_Arci[vertices + 1])),
	};

	if (dg.tail_ptr == NULL) return SCC_NULL_DIGRAPH;

	if (max_arcs > 0) {
		dg.head = malloc(sizeof(scc_Vid[max_arcs]));
		if (dg.head == NULL) {
			scc_free_digraph(&dg);
			return SCC_NULL_DIGRAPH;
		}
	}

	assert(scc_digraph_is_initialized(&dg));

	return dg;
}


bool scc_digraph_is_initialized(const scc_Digraph* const dg)
{
	if ((dg == NULL) || (dg->tail_ptr == NULL)) return false;
	if ((dg->vertices >= SCC_VID_MAX) || (dg->max_arcs > SCC_ARCI_MAX)) return false;
	if (dg->max_arcs == 0 && dg->head) return false;
	if (dg->max_arcs > 0 && !dg->head) return false;
	return true;
}


void scc_free_digraph(scc_Digraph* const dg)
{
	if (dg != NULL) {
		free(dg->head);
		free(dg->tail_ptr);
		*dg = SCC_NULL_DIGRAPH;
	}
}


bool scc_change_arc_storage(scc_Digraph* const dg,
                            const size_t new_max_arcs)
{
	assert(scc_digraph_is_initialized(dg));
	if (dg->max_arcs == new_max_arcs) return true;
	if (new_max_arcs > SCC_ARCI_MAX) return false;
	if (dg->tail_ptr[dg->vertices] > new_max_arcs) return false;

	if (new_max_arcs == 0) {
		free(dg->head);
		dg->head = NULL;
		dg->max_arcs = 0;
	} else {
		scc_Vid* const tmp_ptr = realloc(dg->head, sizeof(scc_Vid[new_max_arcs]));
		if (tmp_ptr == NULL) return false;
		dg->head = tmp_ptr;
		dg->max_arcs = new_max_arcs;
	}

	return true;
}


scc_Digraph scc_empty_digraph(const size_t vertices,
                              const size_t max_arcs)
{
	assert(vertices < SCC_VID_MAX);
	if (max_arcs > SCC_ARCI_MAX) return SCC_NULL_DIGRAPH;

	scc_Digraph dg = {
		.vertices = vertices,
		.max_arcs = max_arcs,
		.head = NULL,
		.tail_ptr = calloc(vertices + 1, sizeof(scc_Arci)),
	};

	if (dg.tail_ptr == NULL) return SCC_NULL_DIGRAPH;

	if (max_arcs > 0) {
		dg.head = malloc(sizeof(scc_Vid[max_arcs]));
		if (dg.head == NULL) {
			scc_free_digraph(&dg);
			return SCC_NULL_DIGRAPH;
		}
	}

	assert(scc_digraph_is_initialized(&dg));

	return dg;
}


scc_Digraph scc_copy_digraph(const scc_Digraph* const dg)
{
	assert(scc_digraph_is_initialized(dg));
	if (dg->vertices == 0) return scc_empty_digraph(0, 0);

	scc_Digraph dg_out = scc_init_digraph(dg->vertices, dg->tail_ptr[dg->vertices]);
	if (!scc_digraph_is_initialized(&dg_out)) return SCC_NULL_DIGRAPH;

	for (size_t v = 0; v <= dg->vertices; ++v) {
		dg_out.tail_ptr[v] = dg->tail_ptr[v];
	}
	for (scc_Arci a = 0; a < dg->tail_ptr[dg->vertices]; ++a) {
		dg_out.head[a] = dg->head[a];
	}

	return dg_out;
}


void scc_delete_arcs_by_tails(scc_Digraph* const dg,
                              const bool to_delete[const static dg->vertices])
{
	assert(scc_digraph_is_initialized(dg));
	assert(to_delete != NULL);

	scc_Arci head_write = 0;
	for (size_t v = 0; v < dg->vertices; ++v) {
		if (to_delete[v]) {
			dg->tail_ptr[v] = head_write;
		} else if (dg->tail_ptr[v] == head_write) {
			// No vertex has been deleted yet (or deleted vertices originally had no arcs).
			head_write = dg->tail_ptr[v + 1];
		} else {
			const scc_Vid* v_arc = dg->head + dg->tail_ptr[v];
			const scc_Vid* const v_arc_stop = dg->head + dg->tail_ptr[v + 1];
			dg->tail_ptr[v] = head_write;

			for (; v_arc != v_arc_stop; ++v_arc) {
				dg->head[head_write] = *v_arc;
				++head_write;
			}
		}
	}
	dg->tail_ptr[dg->vertices] = head_write;
}


void scc_delete_loops(scc_Digraph* const dg)
{
	assert(scc_digraph_is_initialized(dg));

	scc_Arci head_write = 0;
	assert(dg->vertices < SCC_VID_MAX);
	const scc_Vid vertices = (scc_Vid) dg->vertices; // If `scc_Vid` is signed 
	for (scc_Vid v = 0; v < vertices; ++v) {
		const scc_Vid* v_arc = dg->head + dg->tail_ptr[v];
		const scc_Vid* const v_arc_stop = dg->head + dg->tail_ptr[v + 1];
		dg->tail_ptr[v] = head_write;

		for (; v_arc != v_arc_stop; ++v_arc) {
			if (*v_arc != v) {
				dg->head[head_write] = *v_arc;
				++head_write;
			}
		}
	}
	dg->tail_ptr[dg->vertices] = head_write;
}
