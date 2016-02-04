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
#include <stddef.h>
#include <stdlib.h>
#include "../include/config.h"


// ==============================================================================
// Internal function prototypes
// ==============================================================================

static inline size_t iscc_do_union(size_t vertices,
                                   size_t num_dgs,
                                   const scc_Digraph dgs[static num_dgs],
                                   scc_Vid row_markers[restrict static vertices],
                                   bool ignore_loops,
                                   bool write,
                                   scc_Arci out_tail_ptr[restrict],
                                   scc_Vid out_head[restrict]);

static inline size_t iscc_do_adjacency_product(size_t vertices,
                                               const scc_Arci dg_a_tail_ptr[static vertices + 1],
                                               const scc_Vid* dg_a_head,
                                               const scc_Arci dg_b_tail_ptr[static vertices + 1],
                                               const scc_Vid* dg_b_head,
                                               scc_Vid row_markers[restrict static vertices],
                                               bool force_loops,
                                               bool ignore_loops,
                                               bool write,
                                               scc_Arci out_tail_ptr[restrict],
                                               scc_Vid out_head[restrict]);


// ==============================================================================
// External function implementations
// ==============================================================================

scc_Digraph scc_digraph_union(const size_t num_dgs,
                              const scc_Digraph dgs[const static num_dgs],
                              const bool ignore_loops)
{
	if (num_dgs == 0) return scc_empty_digraph(0, 0);
	if ((dgs == NULL) || !scc_digraph_is_initialized(dgs)) return SCC_NULL_DIGRAPH;

	const size_t vertices = dgs[0].vertices;

	size_t out_arcs_write = 0;

	// Try greedy memory count first
	for (size_t i = 0; i < num_dgs; ++i) {
		if (!scc_digraph_is_initialized(&dgs[i]) || dgs[i].vertices != vertices) return SCC_NULL_DIGRAPH;
		out_arcs_write += dgs[i].tail_ptr[vertices];
	}

	scc_Vid* const row_markers = malloc(sizeof(scc_Vid[vertices]));
	if (row_markers == NULL) return SCC_NULL_DIGRAPH;

	scc_Digraph dg_out = scc_init_digraph(vertices, out_arcs_write);
	if (!scc_digraph_is_initialized(&dg_out)) {
		// Could not allocate digraph with `out_arcs_write' arcs.
		// Do correct (but slow) memory count by doing
		// union without writing.
		out_arcs_write = iscc_do_union(vertices, num_dgs, dgs, row_markers,
		                               ignore_loops, false, NULL, NULL);

		// Try again. If fail, give up.
		dg_out = scc_init_digraph(vertices, out_arcs_write);
		if (!scc_digraph_is_initialized(&dg_out)) {
			free(row_markers);
			return SCC_NULL_DIGRAPH;
		}
	}

	out_arcs_write = iscc_do_union(vertices, num_dgs, dgs, row_markers,
	                               ignore_loops, true, dg_out.tail_ptr, dg_out.head);

	free(row_markers);

	scc_change_arc_storage(&dg_out, out_arcs_write);

	return dg_out;
}


scc_Digraph scc_digraph_transpose(const scc_Digraph* const dg)
{
	if (!scc_digraph_is_initialized(dg)) return SCC_NULL_DIGRAPH;
	if (dg->vertices == 0) return scc_empty_digraph(0, 0);

	scc_Vid* const row_count = calloc(dg->vertices + 1, sizeof(scc_Vid));
	scc_Digraph dg_out = scc_init_digraph(dg->vertices, dg->tail_ptr[dg->vertices]);
	if ((row_count == NULL) || !scc_digraph_is_initialized(&dg_out)) {
		free(row_count);
		scc_free_digraph(&dg_out);
		return SCC_NULL_DIGRAPH;
	}

	const scc_Vid* const arc_c_stop = dg->head + dg->tail_ptr[dg->vertices];
	for (const scc_Vid* arc_c = dg->head;
	        arc_c != arc_c_stop; ++arc_c) {
		++row_count[*arc_c + 1];
	}

	dg_out.tail_ptr[0] = 0;
	for (scc_Vid v = 1; v <= dg->vertices; ++v) {
		row_count[v] += row_count[v - 1];
		dg_out.tail_ptr[v] = row_count[v];
	}

	for (scc_Vid v = 0; v < dg->vertices; ++v) {
		const scc_Vid* const arc_stop = dg->head + dg->tail_ptr[v + 1];
		for (const scc_Vid* arc = dg->head + dg->tail_ptr[v];
		        arc != arc_stop; ++arc) {
			dg_out.head[row_count[*arc]] = v;
			++row_count[*arc];
		}
	}

	free(row_count);

	return dg_out;
}


scc_Digraph scc_adjacency_product(const scc_Digraph* const dg_a,
                                  const scc_Digraph* const dg_b,
                                  const bool force_loops,
                                  const bool ignore_loops)
{
	if (force_loops && ignore_loops) return SCC_NULL_DIGRAPH;
	if (!scc_digraph_is_initialized(dg_a) || !scc_digraph_is_initialized(dg_b)) return SCC_NULL_DIGRAPH;
	if (dg_a->vertices != dg_b->vertices) return SCC_NULL_DIGRAPH;
	if (dg_a->vertices == 0) return scc_empty_digraph(0, 0);

	const size_t vertices = dg_a->vertices;

	scc_Vid* const row_markers = malloc(sizeof(scc_Vid[vertices]));
	if (row_markers == NULL) return SCC_NULL_DIGRAPH;

	size_t out_arcs_write = 0;

	// Try greedy memory count first
	for (scc_Vid v = 0; v < vertices; ++v) {
		if (force_loops) {
			out_arcs_write += dg_b->tail_ptr[v + 1] - dg_b->tail_ptr[v];
		}
		const scc_Vid* const arc_a_stop = dg_a->head + dg_a->tail_ptr[v + 1];
		for (const scc_Vid* arc_a = dg_a->head + dg_a->tail_ptr[v];
		        arc_a != arc_a_stop; ++arc_a) {
			if (*arc_a == v && (force_loops || ignore_loops)) continue;
			out_arcs_write += dg_b->tail_ptr[*arc_a + 1] - dg_b->tail_ptr[*arc_a];
		}
	}

	scc_Digraph dg_out = scc_init_digraph(vertices, out_arcs_write);
	if (!scc_digraph_is_initialized(&dg_out)) {
		// Could not allocate digraph with `out_arcs_write' arcs.
		// Do correct (but slow) memory count by doing
		// doing product without writing.
		out_arcs_write = iscc_do_adjacency_product(vertices,
		                                           dg_a->tail_ptr, dg_a->head,
		                                           dg_b->tail_ptr, dg_b->head,
		                                           row_markers,
		                                           force_loops, ignore_loops,
		                                           false, NULL, NULL);

		// Try again. If fail, give up.
		dg_out = scc_init_digraph(vertices, out_arcs_write);
		if (!scc_digraph_is_initialized(&dg_out)) {
			free(row_markers);
			return SCC_NULL_DIGRAPH;
		}
	}

	out_arcs_write = iscc_do_adjacency_product(vertices,
	                                           dg_a->tail_ptr, dg_a->head,
	                                           dg_b->tail_ptr, dg_b->head,
	                                           row_markers,
	                                           force_loops, ignore_loops,
	                                           true, dg_out.tail_ptr, dg_out.head);

	free(row_markers);

	scc_change_arc_storage(&dg_out, out_arcs_write);

	return dg_out;
}


// ==============================================================================
// Internal function implementations 
// ==============================================================================

static inline size_t iscc_do_union(const size_t vertices,
                                   const size_t num_dgs,
                                   const scc_Digraph dgs[const static num_dgs],
                                   scc_Vid row_markers[restrict const static vertices],
                                   const bool ignore_loops,
                                   const bool write,
                                   scc_Arci out_tail_ptr[restrict const],
                                   scc_Vid out_head[restrict const])
{
	size_t counter = 0;
	if (write) out_tail_ptr[0] = 0;
	for (scc_Vid v = 0; v < vertices; ++v) {
		row_markers[v] = SCC_VID_MAX;
	}

	for (scc_Vid v = 0; v < vertices; ++v) {
		if (ignore_loops) row_markers[v] = v;
		for (size_t i = 0; i < num_dgs; ++i) {
			const scc_Vid* const arc_i_stop = dgs[i].head + dgs[i].tail_ptr[v + 1];
			for (const scc_Vid* arc_i = dgs[i].head + dgs[i].tail_ptr[v];
			        arc_i != arc_i_stop; ++arc_i) {
				if (row_markers[*arc_i] != v) {
					row_markers[*arc_i] = v;
					if (write) out_head[counter] = *arc_i;
					++counter;
				}
			}
		}

		if (write) out_tail_ptr[v + 1] = (scc_Arci) counter;
	}

	return counter;
}


static inline size_t iscc_do_adjacency_product(const size_t vertices,
                                               const scc_Arci dg_a_tail_ptr[const static vertices + 1],
                                               const scc_Vid* const dg_a_head,
                                               const scc_Arci dg_b_tail_ptr[const static vertices + 1],
                                               const scc_Vid* const dg_b_head,
                                               scc_Vid row_markers[restrict const static vertices],
                                               const bool force_loops,
                                               const bool ignore_loops,
                                               const bool write,
                                               scc_Arci out_tail_ptr[restrict const],
                                               scc_Vid out_head[restrict const])
{
	size_t counter = 0;
	if (write) out_tail_ptr[0] = 0;
	for (scc_Vid v = 0; v < vertices; ++v) {
		row_markers[v] = SCC_VID_MAX;
	}

	for (scc_Vid v = 0; v < vertices; ++v) {
		if (force_loops) {
			const scc_Vid* const v_arc_b_stop = dg_b_head + dg_b_tail_ptr[v + 1];
			for (const scc_Vid* v_arc_b = dg_b_head + dg_b_tail_ptr[v];
			        v_arc_b != v_arc_b_stop; ++v_arc_b) {
				if (row_markers[*v_arc_b] != v) {
					row_markers[*v_arc_b] = v;
					if (write) out_head[counter] = *v_arc_b;
					++counter;
				}
			}
		}
		const scc_Vid* const arc_a_stop = dg_a_head + dg_a_tail_ptr[v + 1];
		for (const scc_Vid* arc_a = dg_a_head + dg_a_tail_ptr[v];
		        arc_a != arc_a_stop; ++arc_a) {
			if (*arc_a == v && (force_loops || ignore_loops)) continue;
			const scc_Vid* const arc_b_stop = dg_b_head + dg_b_tail_ptr[*arc_a + 1];
			for (const scc_Vid* arc_b = dg_b_head + dg_b_tail_ptr[*arc_a];
			        arc_b != arc_b_stop; ++arc_b) {
				if (row_markers[*arc_b] != v) {
					row_markers[*arc_b] = v;
					if (write) out_head[counter] = *arc_b;
					++counter;
				}
			}
		}

		if (write) out_tail_ptr[v + 1] = (scc_Arci) counter;
	}

	return counter;
}
