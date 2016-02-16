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
#include <string.h>
#include "../include/scclust.h"
#include "error.h"


// ==============================================================================
// External function implementations
// ============================================================================== 

scc_ErrorCode iscc_init_digraph(const size_t vertices,
                                const size_t max_arcs,
                                iscc_Digraph* const out_dg)
{
	assert(out_dg != NULL);
	assert(vertices < SCC_DPID_MAX);
	if (max_arcs > SCC_ARCI_MAX) return iscc_make_error(SCC_ER_TOO_LARGE_DIGRAPH);

	*out_dg = (iscc_Digraph) {
		.vertices = vertices,
		.max_arcs = max_arcs,
		.head = NULL,
		.tail_ptr = malloc(sizeof(scc_Arci[vertices + 1])),
	};
	if (out_dg->tail_ptr == NULL) return iscc_make_error(SCC_ER_NO_MEMORY);

	if (max_arcs > 0) {
		out_dg->head = malloc(sizeof(scc_Dpid[max_arcs]));
		if (out_dg->head == NULL) {
			iscc_free_digraph(out_dg);
			return iscc_make_error(SCC_ER_NO_MEMORY);
		}
	}

	assert(iscc_digraph_is_initialized(out_dg));

	return iscc_no_error();
}


scc_ErrorCode iscc_empty_digraph(const size_t vertices,
                                 const size_t max_arcs,
                                 iscc_Digraph* const out_dg)
{
	assert(out_dg != NULL);
	assert(vertices < SCC_DPID_MAX);
	if (max_arcs > SCC_ARCI_MAX) return iscc_make_error(SCC_ER_TOO_LARGE_DIGRAPH);

	*out_dg = (iscc_Digraph) {
		.vertices = vertices,
		.max_arcs = max_arcs,
		.head = NULL,
		.tail_ptr = calloc(vertices + 1, sizeof(scc_Arci)),
	};
	if (out_dg->tail_ptr == NULL) return iscc_make_error(SCC_ER_NO_MEMORY);

	if (max_arcs > 0) {
		out_dg->head = malloc(sizeof(scc_Dpid[max_arcs]));
		if (out_dg->head == NULL) {
			iscc_free_digraph(out_dg);
			return iscc_make_error(SCC_ER_NO_MEMORY);
		}
	}

	assert(iscc_digraph_is_initialized(out_dg));

	return iscc_no_error();
}


bool iscc_digraph_is_initialized(const iscc_Digraph* const dg)
{
	if ((dg == NULL) || (dg->tail_ptr == NULL)) return false;
	if ((dg->vertices >= SCC_DPID_MAX) || (dg->max_arcs > SCC_ARCI_MAX)) return false;
	if (dg->max_arcs == 0 && dg->head) return false;
	if (dg->max_arcs > 0 && !dg->head) return false;
	return true;
}


void iscc_free_digraph(iscc_Digraph* const dg)
{
	if (dg != NULL) {
		free(dg->head);
		free(dg->tail_ptr);
		*dg = ISCC_NULL_DIGRAPH;
	}
}


scc_ErrorCode iscc_change_arc_storage(iscc_Digraph* const dg,
                                      const size_t new_max_arcs)
{
	assert(iscc_digraph_is_initialized(dg));
	assert(dg->tail_ptr[dg->vertices] > new_max_arcs);
	if (new_max_arcs > SCC_ARCI_MAX) return iscc_make_error(SCC_ER_TOO_LARGE_DIGRAPH);
	if (dg->max_arcs == new_max_arcs) return iscc_no_error();

	if (new_max_arcs == 0) {
		free(dg->head);
		dg->head = NULL;
		dg->max_arcs = 0;
	} else {
		scc_Dpid* const tmp_ptr = realloc(dg->head, sizeof(scc_Dpid[new_max_arcs]));
		if (tmp_ptr == NULL) return iscc_make_error(SCC_ER_NO_MEMORY);
		dg->head = tmp_ptr;
		dg->max_arcs = new_max_arcs;
	}

	return iscc_no_error();
}


scc_ErrorCode iscc_copy_digraph(const iscc_Digraph* const in_dg,
                                iscc_Digraph* const out_dg)
{
	scc_ErrorCode ec;
	assert(iscc_digraph_is_initialized(in_dg));
	assert(out_dg != NULL);
	if (in_dg->vertices == 0) return iscc_empty_digraph(0, 0, out_dg);

	const size_t num_vertices = in_dg->vertices;
	const size_t num_arcs = in_dg->tail_ptr[in_dg->vertices];

	if ((ec = iscc_init_digraph(num_vertices, num_arcs, out_dg)) != SCC_ER_OK) return ec;

	memcpy(out_dg->tail_ptr, in_dg->tail_ptr, (num_vertices + 1) * sizeof(scc_Arci));
	memcpy(out_dg->head, in_dg->head, num_arcs * sizeof(scc_Dpid));

	return iscc_no_error();
}


scc_ErrorCode iscc_delete_arcs_by_tails_check_error(iscc_Digraph* dg,
                                                    const bool to_delete[static dg->vertices])
{
	assert(iscc_digraph_is_initialized(dg));
	assert(to_delete != NULL);

	scc_Arci head_write = 0;
	for (size_t v = 0; v < dg->vertices; ++v) {
		if (to_delete[v]) {
			dg->tail_ptr[v] = head_write;
		} else if (dg->tail_ptr[v] == head_write) {
			// No vertex has been deleted yet (or deleted vertices originally had no arcs).
			head_write = dg->tail_ptr[v + 1];
		} else {
			const scc_Dpid* v_arc = dg->head + dg->tail_ptr[v];
			const scc_Dpid* const v_arc_stop = dg->head + dg->tail_ptr[v + 1];
			dg->tail_ptr[v] = head_write;

			for (; v_arc != v_arc_stop; ++v_arc) {
				dg->head[head_write] = *v_arc;
				++head_write;
			}
		}
	}
	dg->tail_ptr[dg->vertices] = head_write;

	return iscc_no_error();
}


scc_ErrorCode iscc_delete_loops_check_error(iscc_Digraph* dg)
{
	assert(iscc_digraph_is_initialized(dg));

	scc_Arci head_write = 0;
	assert(dg->vertices < SCC_DPID_MAX);
	const scc_Dpid vertices = (scc_Dpid) dg->vertices; // If `scc_Dpid` is signed 
	for (scc_Dpid v = 0; v < vertices; ++v) {
		const scc_Dpid* v_arc = dg->head + dg->tail_ptr[v];
		const scc_Dpid* const v_arc_stop = dg->head + dg->tail_ptr[v + 1];
		dg->tail_ptr[v] = head_write;

		for (; v_arc != v_arc_stop; ++v_arc) {
			if (*v_arc != v) {
				dg->head[head_write] = *v_arc;
				++head_write;
			}
		}
	}
	dg->tail_ptr[dg->vertices] = head_write;

	return iscc_no_error();
}
