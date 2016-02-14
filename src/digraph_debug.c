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


#include "digraph_debug.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/scclust.h"
#include "digraph_core.h"


// ==============================================================================
// External function implementations
// ==============================================================================

bool iscc_is_valid_digraph(const iscc_Digraph* const dg)
{
	if (!iscc_digraph_is_initialized(dg)) return false;
	if (dg->tail_ptr[0] != 0) return false;
	if (dg->tail_ptr[dg->vertices] > dg->max_arcs) return false;
	for (size_t i = 0; i < dg->vertices; ++i) {
		if (dg->tail_ptr[i] > dg->tail_ptr[i + 1]) return false;
	}
	for (size_t i = 0; i < dg->tail_ptr[dg->vertices]; ++i) {
		if (((size_t) dg->head[i]) >= dg->vertices) return false;
	}
	return true;
}


bool iscc_is_empty_digraph(const iscc_Digraph* const dg)
{
	if (!iscc_is_valid_digraph(dg)) return false;
	if (dg->tail_ptr[dg->vertices] != 0) return false;
	return true;
}


bool iscc_is_balanced_digraph(const iscc_Digraph* const dg,
                              const size_t arcs_per_vertex)
{
	if (!iscc_is_valid_digraph(dg)) return false;

	for (size_t i = 0; i <= dg->vertices; ++i) {
		if (dg->tail_ptr[i] != i * arcs_per_vertex) return false;
	}

	return true;
}


bool iscc_digraphs_equal(const iscc_Digraph* const dg_a,
                         const iscc_Digraph* const dg_b)
{
	if (dg_a == NULL) return (dg_b == NULL);
	if (dg_a->tail_ptr == NULL) return (dg_b->tail_ptr == NULL);
	if (dg_a->vertices != dg_b->vertices) return false;

	int_fast8_t* const single_row = malloc(sizeof(int_fast8_t[dg_a->vertices]));

	for (size_t v = 0; v < dg_a->vertices; ++v) {

		for (size_t i = 0; i < dg_a->vertices; ++i) {
			single_row[i] = 0;
		}

		const scc_Dpid* const arc_a_stop = dg_a->head + dg_a->tail_ptr[v + 1];
		for (const scc_Dpid* arc_a = dg_a->head + dg_a->tail_ptr[v];
		        arc_a != arc_a_stop; ++arc_a) {
			single_row[*arc_a] = 1;
		}

		const scc_Dpid* const arc_b_stop = dg_b->head + dg_b->tail_ptr[v + 1];
		for (const scc_Dpid* arc_b = dg_b->head + dg_b->tail_ptr[v];
		        arc_b != arc_b_stop; ++arc_b) {
			if (single_row[*arc_b] == 0) {
				free(single_row);
				return false;
			} 
			single_row[*arc_b] = 2;
		}

		for (size_t i = 0; i < dg_a->vertices; ++i) {
			if (single_row[i] == 1) {
				free(single_row);
				return false;
			} 
		}
	}

	free(single_row);
	return true;
}


iscc_Digraph iscc_digraph_from_pieces(const size_t vertices,
                                      const size_t max_arcs,
                                      const scc_Arci tail_ptr[const static vertices + 1],
                                      const scc_Dpid head[const])
{
	assert(tail_ptr != NULL);
	assert((max_arcs == 0) || (head != NULL));
	iscc_Digraph dg = iscc_init_digraph(vertices, max_arcs);
	if (!iscc_digraph_is_initialized(&dg)) return ISCC_NULL_DIGRAPH;

	for (size_t v = 0; v <= vertices; ++v) {
		dg.tail_ptr[v] = tail_ptr[v];
	}
	for (size_t a = 0; a < max_arcs; ++a) {
		dg.head[a] = head[a];
	}

	return dg;
}


iscc_Digraph iscc_digraph_from_string(const char dg_str[const])
{
	assert(dg_str != NULL);
	
	size_t vertices = 0;
	size_t all_arcs = 0;
	size_t max_arcs = 0;

	for (size_t c = 0; dg_str[c] != '\0'; ++c) {
		if (dg_str[c] == '#' || dg_str[c] == '.') ++all_arcs;
		if (dg_str[c] == '#') ++max_arcs;
		if (dg_str[c] == '/' && vertices == 0) vertices = all_arcs;
		if (dg_str[c] == '/' && (all_arcs % vertices) != 0) return ISCC_NULL_DIGRAPH;
	}

	iscc_Digraph dg_out = iscc_init_digraph(vertices, max_arcs);
	if (!iscc_digraph_is_initialized(&dg_out)) return ISCC_NULL_DIGRAPH;

	scc_Arci curr_array_pos = 0;
	size_t curr_row = 0;
	scc_Dpid curr_col = 0;
	dg_out.tail_ptr[0] = 0;

	for (size_t c = 0; dg_str[c] != '\0'; ++c) {
		if (dg_str[c] == '#') {
			dg_out.head[curr_array_pos] = curr_col;
			++curr_array_pos;
		}
		if (dg_str[c] == '#' || dg_str[c] == '.') ++curr_col;
		if (dg_str[c] == '/') {
			++curr_row;
			curr_col = 0;
			dg_out.tail_ptr[curr_row] = curr_array_pos;
		}
	}
	dg_out.tail_ptr[vertices] = curr_array_pos;

	if (!iscc_is_valid_digraph(&dg_out)) {
		iscc_free_digraph(&dg_out);
		return ISCC_NULL_DIGRAPH;
	}

	return dg_out;
}


void iscc_print_digraph(const iscc_Digraph* const dg)
{
	assert(iscc_digraph_is_initialized(dg));
	
	if (dg->vertices == 0) {
		printf("[]\n\n");
		return;
	}

	bool* const single_row = calloc(dg->vertices, sizeof(bool));
	if (single_row == NULL) {
		printf("Out of memory.\n\n");
		return;
	}

	for (size_t v = 0; v < dg->vertices; ++v) {
		for (size_t i = 0; i < dg->vertices; ++i) {
			single_row[i] = false;
		}

		const scc_Dpid* const a_stop = dg->head + dg->tail_ptr[v + 1];
		for (const scc_Dpid* a = dg->head + dg->tail_ptr[v];
		        a != a_stop; ++a) {
			single_row[*a] = true;
		}

		for (size_t i = 0; i < dg->vertices; ++i) {
			if (single_row[i]) {
				putchar('#');
			} else {
				putchar('.');
			}
		}

		putchar('\n');
	}
	putchar('\n');

	free(single_row);
}
