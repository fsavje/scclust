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

#include "../config.h"
#ifndef TBG_HEADER_ONLY_LIB
	#include "../operations.h"
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "../digraph.h"
#include "../core.h"


tbg_Digraph tbg_digraph_union_two(const tbg_Digraph* const dg_a, const tbg_Digraph* const dg_b) {
	const tbg_Digraph* const dgs_ab[2] = {dg_a, dg_b};
	return tbg_digraph_union(2, dgs_ab);
}

static inline tbg_Arcref itbg_do_union(const tbg_Vid vertices,
									   const size_t num_dgs,
									   const tbg_Digraph* const * const dgs,
									   tbg_Vid* restrict const row_markers,
									   const bool write,
									   tbg_Arcref* restrict const out_tail_ptr,
									   tbg_Vid* restrict const out_head) {
	tbg_Arcref counter = 0;
	if (write) out_tail_ptr[0] = 0;
	for (tbg_Vid v = 0; v < vertices; ++v) row_markers[v] = TBG_VID_MAX;

	for (tbg_Vid v = 0; v < vertices; ++v) {
		for (size_t i = 0; i < num_dgs; ++i) {
			for (const tbg_Vid* arc_i = dgs[i]->head + dgs[i]->tail_ptr[v];
					arc_i != dgs[i]->head + dgs[i]->tail_ptr[v + 1];
					++arc_i) {
				if (row_markers[*arc_i] != v) {
					row_markers[*arc_i] = v;
					if (write) out_head[counter] = *arc_i;
					++counter;
				}
			}
		}

		if (write) out_tail_ptr[v + 1] = counter;
	}

	return counter;
}

tbg_Digraph tbg_digraph_union(const size_t num_dgs, const tbg_Digraph* const dgs[const static num_dgs]) {
	if (num_dgs == 0) return tbg_empty_digraph(0, 0);
	if (!dgs || !dgs[0]) return tbg_null_digraph();

	const tbg_Vid vertices = dgs[0]->vertices;

	tbg_Vid* const row_markers = malloc(sizeof(tbg_Vid[vertices]));
	if (!row_markers) return tbg_null_digraph();

	tbg_Arcref out_arcs_write = 0;

	// Try greedy memory count first
	for (size_t i = 0; i < num_dgs; ++i) {
		if (!dgs[i] || !dgs[i]->tail_ptr || dgs[i]->vertices != vertices) return tbg_null_digraph();
		out_arcs_write += dgs[i]->tail_ptr[vertices];
	}

	tbg_Digraph dg_out = tbg_init_digraph(vertices, out_arcs_write);
	if (!dg_out.tail_ptr) {
		// Could not allocate digraph with `out_arcs_write' arcs.
		// Do correct (but slow) memory count by doing
		// doing union without writing.
		out_arcs_write = itbg_do_union(vertices,
									   num_dgs, dgs,
									   row_markers,
									   false, NULL, NULL);

		// Try again. If fail, give up.
		dg_out = tbg_init_digraph(vertices, out_arcs_write);
		if (!dg_out.tail_ptr) {
			free(row_markers);
			return dg_out;
		}
	}

	out_arcs_write = itbg_do_union(vertices,
								   num_dgs, dgs,
								   row_markers,
								   true, dg_out.tail_ptr, dg_out.head);

	free(row_markers);

	tbg_change_arc_storage(&dg_out, out_arcs_write);

	return dg_out;
}


tbg_Digraph tbg_digraph_transpose(const tbg_Digraph* const dg) {
	if (!dg || !dg->tail_ptr) return tbg_null_digraph();
	if (dg->vertices == 0) return tbg_empty_digraph(0, 0);

	tbg_Vid* const row_count = calloc(dg->vertices + 1, sizeof(tbg_Arcref));
	if (!row_count) return tbg_null_digraph();

	tbg_Digraph dg_out = tbg_init_digraph(dg->vertices, dg->tail_ptr[dg->vertices]);
	if (!dg_out.tail_ptr) {
		free(row_count);
		return dg_out;
	}

	for (const tbg_Vid* arc = dg->head;
			arc != dg->head + dg->tail_ptr[dg->vertices];
			++arc) {
		++row_count[*arc + 1];
	}

	dg_out.tail_ptr[0] = 0;
	for (tbg_Vid v = 1; v <= dg->vertices; ++v) {
		row_count[v] += row_count[v - 1];
		dg_out.tail_ptr[v] = row_count[v];
	}

	for (tbg_Vid v = 0; v < dg->vertices; ++v) {
		for (const tbg_Vid* arc = dg->head + dg->tail_ptr[v];
				arc != dg->head + dg->tail_ptr[v + 1];
				++arc) {
			dg_out.head[row_count[*arc]] = v;
			++row_count[*arc];
		}
	}

	free(row_count);

	return dg_out;
}


static inline tbg_Arcref itbg_do_adjacency_product(const tbg_Vid vertices,
												   const tbg_Arcref* const dg_a_tail_ptr,
												   const tbg_Vid* const dg_a_head,
												   const tbg_Arcref* const dg_b_tail_ptr,
												   const tbg_Vid* const dg_b_head,
												   tbg_Vid* restrict const row_markers,
												   const bool force_diagonal,
												   const bool ignore_diagonal,
												   const bool write,
												   tbg_Arcref* restrict const out_tail_ptr,
												   tbg_Vid* restrict const out_head) {
	tbg_Arcref counter = 0;
	if (write) out_tail_ptr[0] = 0;
	for (tbg_Vid v = 0; v < vertices; ++v) row_markers[v] = TBG_VID_MAX;

	for (tbg_Vid v = 0; v < vertices; ++v) {
		if (force_diagonal) {
			for (const tbg_Vid* arc_b = dg_b_head + dg_b_tail_ptr[v];
					arc_b != dg_b_head + dg_b_tail_ptr[v + 1];
					++arc_b) {
				if (row_markers[*arc_b] != v) {
					row_markers[*arc_b] = v;
					if (write) out_head[counter] = *arc_b;
					++counter;
				}
			}
		}
		for (const tbg_Vid* arc_a = dg_a_head + dg_a_tail_ptr[v];
				arc_a != dg_a_head + dg_a_tail_ptr[v + 1];
				++arc_a) {
			if (*arc_a == v && (force_diagonal || ignore_diagonal)) continue;
			for (const tbg_Vid* arc_b = dg_b_head + dg_b_tail_ptr[*arc_a];
					arc_b != dg_b_head + dg_b_tail_ptr[*arc_a + 1];
					++arc_b) {
				if (row_markers[*arc_b] != v) {
					row_markers[*arc_b] = v;
					if (write) out_head[counter] = *arc_b;
					++counter;
				}
			}
		}

		if (write) out_tail_ptr[v + 1] = counter;
	}

	return counter;
}

tbg_Digraph tbg_adjacency_product(const tbg_Digraph* const dg_a, const tbg_Digraph* const dg_b, const bool force_diagonal, const bool ignore_diagonal) {
	if (force_diagonal && ignore_diagonal) return tbg_null_digraph();
	if (!dg_a || !dg_b || !dg_a->tail_ptr || !dg_b->tail_ptr) return tbg_null_digraph();
	if (dg_a->vertices != dg_b->vertices) return tbg_null_digraph();
	if (dg_a->vertices == 0) return tbg_empty_digraph(0, 0);

	const tbg_Vid vertices = dg_a->vertices;

	tbg_Vid* const row_markers = malloc(sizeof(tbg_Vid[vertices]));
	if (!row_markers) return tbg_null_digraph();

	tbg_Arcref out_arcs_write = 0;

	// Try greedy memory count first
	for (tbg_Vid v = 0; v < vertices; ++v) {
		if (force_diagonal) {
			out_arcs_write += dg_b->tail_ptr[v + 1] - dg_b->tail_ptr[v];
		}
		for (const tbg_Vid* arc_a = dg_a->head + dg_a->tail_ptr[v];
				arc_a != dg_a->head + dg_a->tail_ptr[v + 1];
				++arc_a) {
			if (*arc_a == v && (force_diagonal || ignore_diagonal)) continue;
			out_arcs_write += dg_b->tail_ptr[*arc_a + 1] - dg_b->tail_ptr[*arc_a];
		}
	}

	tbg_Digraph dg_out = tbg_init_digraph(vertices, out_arcs_write);
	if (!dg_out.tail_ptr) {
		// Could not allocate digraph with `out_arcs_write' arcs.
		// Do correct (but slow) memory count by doing
		// doing product without writing.
		out_arcs_write = itbg_do_adjacency_product(vertices,
												   dg_a->tail_ptr, dg_a->head,
												   dg_b->tail_ptr, dg_b->head,
												   row_markers,
												   force_diagonal, ignore_diagonal,
												   false, NULL, NULL);

		// Try again. If fail, give up.
		dg_out = tbg_init_digraph(vertices, out_arcs_write);
		if (!dg_out.tail_ptr) {
			free(row_markers);
			return dg_out;
		}
	}

	out_arcs_write = itbg_do_adjacency_product(vertices,
											   dg_a->tail_ptr, dg_a->head,
											   dg_b->tail_ptr, dg_b->head,
											   row_markers,
											   force_diagonal, ignore_diagonal,
											   true, dg_out.tail_ptr, dg_out.head);

	free(row_markers);

	tbg_change_arc_storage(&dg_out, out_arcs_write);

	return dg_out;
}


static tbg_Digraph itbg_digraph_power_and_walks(const tbg_Digraph* const dg,
												const tbg_Vid exponent,
												const bool force_diagonal,
												const bool ignore_diagonal) {
	if (!dg || !dg->tail_ptr) return tbg_null_digraph();
	if (dg->vertices == 0) return tbg_empty_digraph(0, 0);
	if (exponent == 0) return tbg_empty_digraph(dg->vertices, 0);
	if (exponent == 1) return tbg_copy_digraph(dg);
	if (exponent == 2) return tbg_adjacency_product(dg, dg, force_diagonal, ignore_diagonal);

	tbg_Digraph dg_expo = tbg_adjacency_product(dg, dg, force_diagonal, ignore_diagonal);
	if (!dg_expo.tail_ptr) return tbg_null_digraph();
	
	tbg_Digraph dg_out = tbg_null_digraph();
	if (exponent % 2 == 1) {
		dg_out = tbg_copy_digraph(dg);
		if (!dg_out.tail_ptr) {
			tbg_free_digraph(&dg_expo);
			return tbg_null_digraph();
		} 
	}

	tbg_Digraph dg_tmp;
	for (tbg_Vid e = exponent / 2; e > 0; e /= 2) {
		if (e % 2 == 1) {
			if (!dg_out.tail_ptr) {
				dg_out = tbg_copy_digraph(&dg_expo);
			} else {
				dg_tmp = dg_out;
				dg_out = tbg_adjacency_product(&dg_tmp, &dg_expo, force_diagonal, ignore_diagonal);
				tbg_free_digraph(&dg_tmp);
			}
			if (!dg_out.tail_ptr) {
				tbg_free_digraph(&dg_expo);
				return tbg_null_digraph();
			} 
		}
		dg_tmp = dg_expo;
		dg_expo = tbg_adjacency_product(&dg_tmp, &dg_tmp, force_diagonal, ignore_diagonal);
		tbg_free_digraph(&dg_tmp);
		if (!dg_expo.tail_ptr) {
			tbg_free_digraph(&dg_out);
			return tbg_null_digraph();
		}
	}

	tbg_free_digraph(&dg_expo);

	return dg_out;
}

tbg_Digraph tbg_digraph_all_walks(const tbg_Digraph* const dg, const tbg_Vid length) {
	return itbg_digraph_power_and_walks(dg, length, false, false);
}

tbg_Digraph tbg_digraph_all_paths(const tbg_Digraph* const dg, const tbg_Vid length) {
	return itbg_digraph_power_and_walks(dg, length, false, true);
}

tbg_Digraph tbg_digraph_power(const tbg_Digraph* const dg, const tbg_Vid exponent) {
	return itbg_digraph_power_and_walks(dg, exponent, true, false);
}


