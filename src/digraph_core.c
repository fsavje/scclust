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
 * License along with this library. If not, see http://www.gnu.org/licenses/
 * ============================================================================== */

#include "digraph_core.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include "../include/scclust.h"
#include "error.h"
#include "scclust_int.h"


// ==============================================================================
// External function implementations
// ============================================================================== 

scc_ErrorCode iscc_init_digraph(const size_t vertices,
                                const uintmax_t max_arcs,
                                iscc_Digraph* const out_dg)
{
	assert(vertices > 0);
	assert(vertices <= ISCC_DPID_MAX);
	assert(vertices < SIZE_MAX);
	assert(out_dg != NULL);
	if ((max_arcs > ISCC_ARCI_MAX) || (max_arcs > SIZE_MAX)) return iscc_make_error(SCC_ER_TOO_LARGE_DIGRAPH);

	*out_dg = (iscc_Digraph) {
		.vertices = vertices,
		.max_arcs = (size_t) max_arcs,
		.head = NULL,
		.tail_ptr = malloc(sizeof(iscc_Arci[vertices + 1])),
	};
	if (out_dg->tail_ptr == NULL) return iscc_make_error(SCC_ER_NO_MEMORY);

	if (max_arcs > 0) {
		out_dg->head = malloc(sizeof(iscc_Dpid[max_arcs]));
		if (out_dg->head == NULL) {
			iscc_free_digraph(out_dg);
			return iscc_make_error(SCC_ER_NO_MEMORY);
		}
	}

	assert(iscc_digraph_is_initialized(out_dg));

	return iscc_no_error();
}


scc_ErrorCode iscc_empty_digraph(const size_t vertices,
                                 const uintmax_t max_arcs,
                                 iscc_Digraph* const out_dg)
{
	assert(vertices > 0);
	assert(vertices <= ISCC_DPID_MAX);
	assert(vertices < SIZE_MAX);
	assert(out_dg != NULL);
	if ((max_arcs > ISCC_ARCI_MAX) || (max_arcs > SIZE_MAX)) return iscc_make_error(SCC_ER_TOO_LARGE_DIGRAPH);

	*out_dg = (iscc_Digraph) {
		.vertices = vertices,
		.max_arcs = (size_t) max_arcs,
		.head = NULL,
		.tail_ptr = calloc(vertices + 1, sizeof(iscc_Arci)),
	};
	if (out_dg->tail_ptr == NULL) return iscc_make_error(SCC_ER_NO_MEMORY);

	if (max_arcs > 0) {
		out_dg->head = malloc(sizeof(iscc_Dpid[max_arcs]));
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
	if ((dg->vertices > ISCC_DPID_MAX) || (dg->max_arcs > ISCC_ARCI_MAX)) return false;
	if ((dg->max_arcs == 0) && (dg->head != NULL)) return false;
	if ((dg->max_arcs > 0) && (dg->head == NULL)) return false;
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
                                      const uintmax_t new_max_arcs)
{
	assert(iscc_digraph_is_initialized(dg));
	assert(dg->tail_ptr[dg->vertices] <= new_max_arcs);
	if ((new_max_arcs > ISCC_ARCI_MAX) || (new_max_arcs > SIZE_MAX)) return iscc_make_error(SCC_ER_TOO_LARGE_DIGRAPH);
	if (dg->max_arcs == new_max_arcs) return iscc_no_error();

	if (new_max_arcs == 0) {
		free(dg->head);
		dg->head = NULL;
		dg->max_arcs = 0;
	} else {
		iscc_Dpid* const tmp_ptr = realloc(dg->head, sizeof(iscc_Dpid[new_max_arcs]));
		if (tmp_ptr == NULL) return iscc_make_error(SCC_ER_NO_MEMORY);
		dg->head = tmp_ptr;
		dg->max_arcs = (size_t) new_max_arcs;
	}

	return iscc_no_error();
}
