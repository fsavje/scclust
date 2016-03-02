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

#include "digraph_operations.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include "config.h"
#include "digraph_core.h"
#include "error.h"


// ==============================================================================
// Internal function prototypes
// ==============================================================================

static inline uint64_t iscc_do_union_and_delete(uint_fast16_t num_dgs,
                                                const iscc_Digraph dgs[restrict static num_dgs],
                                                iscc_Dpid row_markers[restrict],
                                                const bool tails_to_keep[restrict],
                                                bool write,
                                                iscc_Arci out_tail_ptr[restrict],
                                                iscc_Dpid out_head[restrict]);

static inline uint64_t iscc_do_adjacency_product(const iscc_Digraph* dg_a,
                                                 const iscc_Digraph* dg_b,
                                                 iscc_Dpid row_markers[restrict],
                                                 bool force_loops,
                                                 bool write,
                                                 iscc_Arci out_tail_ptr[restrict],
                                                 iscc_Dpid out_head[restrict]);


// ==============================================================================
// External function implementations
// ==============================================================================

scc_ErrorCode iscc_delete_loops(iscc_Digraph* const dg)
{
	assert(iscc_digraph_is_initialized(dg));

	iscc_Arci head_write = 0;
	assert(dg->vertices <= ISCC_DPID_MAX);
	const iscc_Dpid vertices = (iscc_Dpid) dg->vertices; // If `iscc_Dpid` is signed 
	for (iscc_Dpid v = 0; v < vertices; ++v) {
		const iscc_Dpid* v_arc = dg->head + dg->tail_ptr[v];
		const iscc_Dpid* const v_arc_stop = dg->head + dg->tail_ptr[v + 1];
		dg->tail_ptr[v] = head_write;

		for (; v_arc != v_arc_stop; ++v_arc) {
			if (*v_arc != v) {
				dg->head[head_write] = *v_arc;
				++head_write;
			}
		}
	}
	dg->tail_ptr[vertices] = head_write;

	return iscc_change_arc_storage(dg, head_write);
}


scc_ErrorCode iscc_digraph_union_and_delete(const uint_fast16_t num_in_dgs,
                                            const iscc_Digraph in_dgs[const static num_in_dgs],
                                            const bool tails_to_keep[const],
                                            iscc_Digraph* const out_dg)
{
	assert(num_in_dgs > 0);
	assert(in_dgs != NULL);
	assert(iscc_digraph_is_initialized(&in_dgs[0]));
	assert(out_dg != NULL);

	const size_t vertices = in_dgs[0].vertices;

	// Try greedy memory count first
	uint64_t out_arcs_write = 0;
	for (uint_fast16_t i = 0; i < num_in_dgs; ++i) {
		assert(iscc_digraph_is_initialized(&in_dgs[i]));
		assert(in_dgs[i].vertices == vertices);
		out_arcs_write += in_dgs[i].tail_ptr[vertices];
	}

	iscc_Dpid* const row_markers = malloc(sizeof(iscc_Dpid[vertices]));
	if (row_markers == NULL) return iscc_make_error(SCC_ER_NO_MEMORY);

	scc_ErrorCode ec;
	if (iscc_init_digraph(vertices, out_arcs_write, out_dg) != SCC_ER_OK) {
		// Could not allocate digraph with `out_arcs_write' arcs.
		// Do correct (but slow) memory count by doing
		// union without writing.
		iscc_reset_error();

		out_arcs_write = iscc_do_union_and_delete(num_in_dgs, in_dgs,
		                                          row_markers, tails_to_keep,
		                                          false, NULL, NULL);

		// Try again. If fail, give up.
		if ((ec = iscc_init_digraph(vertices, out_arcs_write, out_dg)) != SCC_ER_OK) {
			free(row_markers);
			return ec;
		}
	}

	out_arcs_write = iscc_do_union_and_delete(num_in_dgs, in_dgs,
	                                          row_markers, tails_to_keep,
	                                          true, out_dg->tail_ptr, out_dg->head);

	free(row_markers);

	if ((ec = iscc_change_arc_storage(out_dg, out_arcs_write)) != SCC_ER_OK) {
		iscc_free_digraph(out_dg);
		return ec;
	}

	return iscc_no_error();
}


scc_ErrorCode iscc_digraph_difference(iscc_Digraph* const minuend_dg,
                                      const iscc_Digraph* const subtrahend_dg,
                                      const uint32_t max_out_degree)
{
	assert(iscc_digraph_is_initialized(minuend_dg));
	assert(iscc_digraph_is_initialized(subtrahend_dg));
	assert(minuend_dg->vertices > 0);
	assert(minuend_dg->vertices == subtrahend_dg->vertices);
	assert(max_out_degree > 0);

	assert(minuend_dg->vertices <= ISCC_DPID_MAX);
	const iscc_Dpid vertices = (iscc_Dpid) minuend_dg->vertices; // If `iscc_Dpid` is signed

	iscc_Dpid* const row_markers = malloc(sizeof(iscc_Dpid[vertices]));
	if (row_markers == NULL) return iscc_make_error(SCC_ER_NO_MEMORY);
	
	for (iscc_Dpid v = 0; v < vertices; ++v) {
		row_markers[v] = ISCC_DPID_MAX;
	}

	uint32_t row_counter;
	iscc_Arci out_arcs_write = 0;
	for (iscc_Dpid v = 0; v < vertices; ++v) {
		row_markers[v] = v;
		const iscc_Dpid* const v_arc_s_stop = subtrahend_dg->head + subtrahend_dg->tail_ptr[v + 1];
		for (const iscc_Dpid* v_arc_s = subtrahend_dg->head + subtrahend_dg->tail_ptr[v];
		        v_arc_s != v_arc_s_stop; ++v_arc_s) {
			row_markers[*v_arc_s] = v;
		}

		row_counter = 0;
		const iscc_Dpid* const arc_m_stop = minuend_dg->head + minuend_dg->tail_ptr[v + 1];
		const iscc_Dpid* arc_m = minuend_dg->head + minuend_dg->tail_ptr[v];
		minuend_dg->tail_ptr[v] = out_arcs_write;
		for (; ((row_counter < max_out_degree) && (arc_m != arc_m_stop)); ++arc_m) {
			if (row_markers[*arc_m] != v) {
				minuend_dg->head[out_arcs_write] = *arc_m;
				++row_counter;
				++out_arcs_write;
			}
		}
	}
	minuend_dg->tail_ptr[vertices] = out_arcs_write;

	free(row_markers);

	return iscc_change_arc_storage(minuend_dg, out_arcs_write);
}


scc_ErrorCode iscc_digraph_transpose(const iscc_Digraph* const in_dg,
                                     iscc_Digraph* const out_dg)
{
	assert(iscc_digraph_is_initialized(in_dg));
	assert(in_dg->vertices > 0);
	assert(out_dg != NULL);

	scc_ErrorCode ec;
	if ((ec = iscc_empty_digraph(in_dg->vertices, in_dg->tail_ptr[in_dg->vertices], out_dg)) != SCC_ER_OK) {
		return ec;
	}

	const iscc_Dpid* const arc_c_stop = in_dg->head + in_dg->tail_ptr[in_dg->vertices];
	for (const iscc_Dpid* arc_c = in_dg->head;
	        arc_c != arc_c_stop; ++arc_c) {
		++out_dg->tail_ptr[*arc_c];
	}

	for (size_t v = 0; v < in_dg->vertices; ++v) {
		out_dg->tail_ptr[v + 1] += out_dg->tail_ptr[v];
	}

	assert(in_dg->vertices <= ISCC_DPID_MAX);
	const iscc_Dpid vertices = (iscc_Dpid) in_dg->vertices; // If `iscc_Dpid` is signed 
	for (iscc_Dpid v = 0; v < vertices; ++v) {
		const iscc_Dpid* const arc_stop = in_dg->head + in_dg->tail_ptr[v + 1];
		for (const iscc_Dpid* arc = in_dg->head + in_dg->tail_ptr[v];
		        arc != arc_stop; ++arc) {
			--out_dg->tail_ptr[*arc];
			out_dg->head[out_dg->tail_ptr[*arc]] = v;
		}
	}

	return iscc_no_error();
}


scc_ErrorCode iscc_adjacency_product(const iscc_Digraph* const in_dg_a,
                                     const iscc_Digraph* const in_dg_b,
                                     const bool force_loops,
                                     iscc_Digraph* const out_dg)
{
	assert(iscc_digraph_is_initialized(in_dg_a));
	assert(iscc_digraph_is_initialized(in_dg_b));
	assert(in_dg_a->vertices > 0);
	assert(in_dg_a->vertices == in_dg_b->vertices);
	assert(out_dg != NULL);

	const size_t vertices = in_dg_a->vertices;

	iscc_Dpid* const row_markers = malloc(sizeof(iscc_Dpid[vertices]));
	if (row_markers == NULL) return iscc_make_error(SCC_ER_NO_MEMORY);

	// Try greedy memory count first
	uint64_t out_arcs_write = 0;
	const iscc_Dpid* const arc_a_stop = in_dg_a->head + in_dg_a->tail_ptr[vertices];
	for (const iscc_Dpid* arc_a = in_dg_a->head; arc_a != arc_a_stop; ++arc_a) {
		out_arcs_write += in_dg_b->tail_ptr[*arc_a + 1] - in_dg_b->tail_ptr[*arc_a];
	}
	if (force_loops) out_arcs_write += in_dg_b->tail_ptr[vertices];

	scc_ErrorCode ec;
	if (iscc_init_digraph(vertices, out_arcs_write, out_dg) != SCC_ER_OK) {
		// Could not allocate digraph with `out_arcs_write' arcs.
		// Do correct (but slow) memory count by doing
		// doing product without writing.
		iscc_reset_error();

		out_arcs_write = iscc_do_adjacency_product(in_dg_a, in_dg_b,
		                                           row_markers, force_loops,
		                                           false, NULL, NULL);

		// Try again. If fail, give up.
		if ((ec = iscc_init_digraph(vertices, out_arcs_write, out_dg)) != SCC_ER_OK) {
			free(row_markers);
			return ec;
		}
	}

	out_arcs_write = iscc_do_adjacency_product(in_dg_a, in_dg_b,
	                                           row_markers, force_loops,
	                                           true, out_dg->tail_ptr, out_dg->head);

	free(row_markers);

	if ((ec = iscc_change_arc_storage(out_dg, out_arcs_write)) != SCC_ER_OK) {
		iscc_free_digraph(out_dg);
		return ec;
	}

	return iscc_no_error();
}


// ==============================================================================
// Internal function implementations 
// ==============================================================================

static inline uint64_t iscc_do_union_and_delete(const uint_fast16_t num_dgs,
                                                const iscc_Digraph dgs[restrict const static num_dgs],
                                                iscc_Dpid row_markers[restrict const],
                                                const bool tails_to_keep[restrict const],
                                                const bool write,
                                                iscc_Arci out_tail_ptr[restrict const],
                                                iscc_Dpid out_head[restrict const])
{
	assert(num_dgs > 0);
	assert(dgs != NULL);
	assert(iscc_digraph_is_initialized(&dgs[0]));
	assert(dgs[0].vertices > 0);
	assert(row_markers != NULL);

	uint64_t counter = 0;
	assert(dgs->vertices <= ISCC_DPID_MAX);
	const iscc_Dpid vertices = (iscc_Dpid) dgs->vertices; // If `iscc_Dpid` is signed 

	for (iscc_Dpid v = 0; v < vertices; ++v) {
		row_markers[v] = ISCC_DPID_MAX;
	}

	if ((tails_to_keep == NULL) && !write) {
		for (iscc_Dpid v = 0; v < vertices; ++v) {
			row_markers[v] = v;
			for (uint_fast16_t i = 0; i < num_dgs; ++i) {
				const iscc_Dpid* const arc_i_stop = dgs[i].head + dgs[i].tail_ptr[v + 1];
				for (const iscc_Dpid* arc_i = dgs[i].head + dgs[i].tail_ptr[v];
				        arc_i != arc_i_stop; ++arc_i) {
					if (row_markers[*arc_i] != v) {
						row_markers[*arc_i] = v;
						++counter;
					}
				}
			}
		}

	} else if ((tails_to_keep != NULL) && !write) {
		for (iscc_Dpid v = 0; v < vertices; ++v) {
			if (tails_to_keep[v]) {
				row_markers[v] = v;
				for (uint_fast16_t i = 0; i < num_dgs; ++i) {
					const iscc_Dpid* const arc_i_stop = dgs[i].head + dgs[i].tail_ptr[v + 1];
					for (const iscc_Dpid* arc_i = dgs[i].head + dgs[i].tail_ptr[v];
					        arc_i != arc_i_stop; ++arc_i) {
						if (row_markers[*arc_i] != v) {
							row_markers[*arc_i] = v;
							++counter;
						}
					}
				}
			}
		}

	} else if ((tails_to_keep == NULL) && write) {
		assert(out_tail_ptr != NULL);
		assert(out_head != NULL);
		out_tail_ptr[0] = 0;
		for (iscc_Dpid v = 0; v < vertices; ++v) {
			row_markers[v] = v;
			for (uint_fast16_t i = 0; i < num_dgs; ++i) {
				const iscc_Dpid* const arc_i_stop = dgs[i].head + dgs[i].tail_ptr[v + 1];
				for (const iscc_Dpid* arc_i = dgs[i].head + dgs[i].tail_ptr[v];
				        arc_i != arc_i_stop; ++arc_i) {
					if (row_markers[*arc_i] != v) {
						row_markers[*arc_i] = v;
						out_head[counter] = *arc_i;
						++counter;
					}
				}
			}
			out_tail_ptr[v + 1] = (iscc_Arci) counter;
		}

	} else if ((tails_to_keep != NULL) && write) {
		assert(out_tail_ptr != NULL);
		assert(out_head != NULL);
		out_tail_ptr[0] = 0;
		for (iscc_Dpid v = 0; v < vertices; ++v) {
			if (tails_to_keep[v]) {
				row_markers[v] = v;
				for (uint_fast16_t i = 0; i < num_dgs; ++i) {
					const iscc_Dpid* const arc_i_stop = dgs[i].head + dgs[i].tail_ptr[v + 1];
					for (const iscc_Dpid* arc_i = dgs[i].head + dgs[i].tail_ptr[v];
					        arc_i != arc_i_stop; ++arc_i) {
						if (row_markers[*arc_i] != v) {
							row_markers[*arc_i] = v;
							out_head[counter] = *arc_i;
							++counter;
						}
					}
				}
			}
			out_tail_ptr[v + 1] = (iscc_Arci) counter;
		}
	}

	return counter;
}


static inline uint64_t iscc_do_adjacency_product(const iscc_Digraph* const dg_a,
                                                 const iscc_Digraph* const dg_b,
                                                 iscc_Dpid row_markers[restrict const],
                                                 const bool force_loops,
                                                 const bool write,
                                                 iscc_Arci out_tail_ptr[restrict const],
                                                 iscc_Dpid out_head[restrict const])
{
	assert(iscc_digraph_is_initialized(dg_a));
	assert(iscc_digraph_is_initialized(dg_b));
	assert(dg_a->vertices > 0);
	assert(dg_a->vertices == dg_b->vertices);
	assert(row_markers != NULL);

	uint64_t counter = 0;
	assert(dg_a->vertices <= ISCC_DPID_MAX);
	const iscc_Dpid vertices = (iscc_Dpid) dg_a->vertices; // If `iscc_Dpid` is signed 

	const iscc_Arci* const dg_a_tail_ptr = dg_a->tail_ptr;
	const iscc_Dpid* const dg_a_head = dg_a->head;
	const iscc_Arci* const dg_b_tail_ptr = dg_b->tail_ptr;
	const iscc_Dpid* const dg_b_head = dg_b->head;

	for (iscc_Dpid v = 0; v < vertices; ++v) {
		row_markers[v] = ISCC_DPID_MAX;
	}

	if (!write) {
		for (iscc_Dpid v = 0; v < vertices; ++v) {
			row_markers[v] = v;
			if (force_loops) {
				const iscc_Dpid* const v_arc_b_stop = dg_b_head + dg_b_tail_ptr[v + 1];
				for (const iscc_Dpid* v_arc_b = dg_b_head + dg_b_tail_ptr[v];
				        v_arc_b != v_arc_b_stop; ++v_arc_b) {
					if (row_markers[*v_arc_b] != v) {
						row_markers[*v_arc_b] = v;
						++counter;
					}
				}
			}
			const iscc_Dpid* const arc_a_stop = dg_a_head + dg_a_tail_ptr[v + 1];
			for (const iscc_Dpid* arc_a = dg_a_head + dg_a_tail_ptr[v];
			        arc_a != arc_a_stop; ++arc_a) {
				const iscc_Dpid* const arc_b_stop = dg_b_head + dg_b_tail_ptr[*arc_a + 1];
				for (const iscc_Dpid* arc_b = dg_b_head + dg_b_tail_ptr[*arc_a];
				        arc_b != arc_b_stop; ++arc_b) {
					if (row_markers[*arc_b] != v) {
						row_markers[*arc_b] = v;
						++counter;
					}
				}
			}
		}

	} else if (write) {
		assert(out_tail_ptr != NULL);
		assert(out_head != NULL);

		out_tail_ptr[0] = 0;
		for (iscc_Dpid v = 0; v < vertices; ++v) {
			row_markers[v] = v;
			if (force_loops) {
				const iscc_Dpid* const v_arc_b_stop = dg_b_head + dg_b_tail_ptr[v + 1];
				for (const iscc_Dpid* v_arc_b = dg_b_head + dg_b_tail_ptr[v];
				        v_arc_b != v_arc_b_stop; ++v_arc_b) {
					if (row_markers[*v_arc_b] != v) {
						row_markers[*v_arc_b] = v;
						out_head[counter] = *v_arc_b;
						++counter;
					}
				}
			}
			const iscc_Dpid* const arc_a_stop = dg_a_head + dg_a_tail_ptr[v + 1];
			for (const iscc_Dpid* arc_a = dg_a_head + dg_a_tail_ptr[v];
			        arc_a != arc_a_stop; ++arc_a) {
				const iscc_Dpid* const arc_b_stop = dg_b_head + dg_b_tail_ptr[*arc_a + 1];
				for (const iscc_Dpid* arc_b = dg_b_head + dg_b_tail_ptr[*arc_a];
				        arc_b != arc_b_stop; ++arc_b) {
					if (row_markers[*arc_b] != v) {
						row_markers[*arc_b] = v;
						out_head[counter] = *arc_b;
						++counter;
					}
				}
			}
			out_tail_ptr[v + 1] = (iscc_Arci) counter;
		}
	}

	return counter;
}
