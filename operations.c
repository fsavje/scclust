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

#include "config.h"
#ifndef TBG_HEADER_ONLY_LIB
	#include "operations.h"
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "digraph.h"
#include "core.h"


tbg_Digraph tbg_digraph_union_two(const tbg_Digraph* dg_a, const tbg_Digraph* dg_b) {
	const tbg_Digraph* dgs_ab[2] = {dg_a, dg_b};
	return tbg_digraph_union(2, dgs_ab);
}


tbg_Digraph tbg_digraph_union(size_t num_dgs, const tbg_Digraph* dgs[const static num_dgs]) {
	if (num_dgs == 0) return tbg_empty_digraph(0, 0);

	tbg_Vid vertices = dgs[0]->vertices;
	tbg_Arcref out_max_arcs = 0;
	for (size_t i = 0; i < num_dgs; ++i) {
		if (!dgs[i] || !dgs[i]->tail_ptr || dgs[i]->vertices != vertices) return (tbg_Digraph) { 0 };
		out_max_arcs += dgs[i]->tail_ptr[vertices];
	}

	tbg_Vid* row_markers = malloc(sizeof(tbg_Vid[vertices]));
	if (!row_markers) return (tbg_Digraph) { 0 };

	tbg_Digraph dg_out = tbg_init_digraph(vertices, out_max_arcs);
	if (!dgs || !dgs[0]) return tbg_null_digraph();

	if (!dg_out.tail_ptr) {
		// Could not allocate digraph with `row_markers' arcs.
		// Do correct (but slow) memory calculation and try again.

		out_max_arcs = 0;
		for (tbg_Vid v = 0; v < vertices; ++v) row_markers[v] = TBG_VID_MAX;

		for (tbg_Vid v = 0; v < vertices; ++v) {
			for (size_t i = 0; i < num_dgs; ++i) {
				for (tbg_Vid* arc_toadd = dgs[i]->head + dgs[i]->tail_ptr[v];
						arc_toadd != dgs[i]->head + dgs[i]->tail_ptr[v + 1];
						++arc_toadd) {
					if (row_markers[*arc_toadd] != v) {
						row_markers[*arc_toadd] = v;
						++out_max_arcs;
					}
				}
			}
		}

		dg_out = tbg_init_digraph(vertices, out_max_arcs);
	}

	// Try again. If fail, give up.
	if (!dg_out.tail_ptr) {
		free(row_markers);
		return dg_out;
	}

	dg_out.tail_ptr[0] = 0;
	tbg_Arcref out_arc_write = 0;
	for (tbg_Vid v = 0; v < vertices; ++v) row_markers[v] = TBG_VID_MAX;

	for (tbg_Vid v = 0; v < vertices; ++v) {
		for (size_t i = 0; i < num_dgs; ++i) {
			for (tbg_Vid* arc_toadd = dgs[i]->head + dgs[i]->tail_ptr[v];
					arc_toadd != dgs[i]->head + dgs[i]->tail_ptr[v + 1];
					++arc_toadd) {
				if (row_markers[*arc_toadd] != v) {
					row_markers[*arc_toadd] = v;
					dg_out.head[out_arc_write] = *arc_toadd;
					++out_arc_write;
				}
			}
		}

		dg_out.tail_ptr[v + 1] = out_arc_write;
	}

	free(row_markers);

	tbg_change_arc_storage(&dg_out, out_arc_write);

	return dg_out;
}


tbg_Digraph tbg_digraph_transpose(const tbg_Digraph* dg) {
	if (!dg || !dg->tail_ptr) return tbg_null_digraph();
	if (dg->vertices == 0) return tbg_empty_digraph(0, 0);

	tbg_Vid* row_count = calloc(dg->vertices + 1, sizeof(tbg_Arcref));
	if (!row_count) return tbg_null_digraph();

	tbg_Digraph dg_out = tbg_init_digraph(dg->vertices, dg->tail_ptr[dg->vertices]);
	if (!dg_out.tail_ptr) {
		free(row_count);
		return dg_out;
	}

	for (tbg_Vid* arc = dg->head;
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
		for (tbg_Vid* arc = dg->head + dg->tail_ptr[v];
				arc != dg->head + dg->tail_ptr[v + 1];
				++arc) {
			dg_out.head[row_count[*arc]] = v;
			++row_count[*arc];
		}
	}

	free(row_count);

	return dg_out;
}


tbg_Digraph tbg_adjacency_product(const tbg_Digraph* dg_a, const tbg_Digraph* dg_b, bool force_diagonal, bool ignore_diagonal) {
	if (force_diagonal && ignore_diagonal) return (tbg_Digraph) { 0 };
	if (!dg_a || !dg_b || !dg_a->tail_ptr || !dg_b->tail_ptr) return (tbg_Digraph) { 0 };
	if (dg_a->vertices != dg_b->vertices) return (tbg_Digraph) { 0 };
	if (dg_a->vertices == 0) return tbg_empty_digraph(0, 0);

	tbg_Vid* row_markers = malloc(sizeof(tbg_Vid[dg_a->vertices]));
	if (!row_markers) return (tbg_Digraph) { 0 };

	tbg_Arcref out_max_arcs = 0;

	// Greedy memory calculation first
	for (tbg_Vid v = 0; v < dg_a->vertices; ++v) {
		if (force_diagonal) {
			out_max_arcs += dg_b->tail_ptr[v + 1] - dg_b->tail_ptr[v];
		}
		for (tbg_Vid* arc_a = dg_a->head + dg_a->tail_ptr[v];
				arc_a != dg_a->head + dg_a->tail_ptr[v + 1];
				++arc_a) {
			if (*arc_a == v && (force_diagonal || ignore_diagonal)) continue;
			out_max_arcs += dg_b->tail_ptr[*arc_a + 1] - dg_b->tail_ptr[*arc_a];
		}
	}

	tbg_Digraph dg_out = tbg_init_digraph(dg_a->vertices, out_max_arcs);

	if (!dg_out.tail_ptr) {
		// Could not allocate digraph with `row_markers' arcs.
		// Do correct (but slow) memory calculation and try again.

		out_max_arcs = 0;
		for (tbg_Vid v = 0; v < dg_a->vertices; ++v) row_markers[v] = TBG_VID_MAX;

		for (tbg_Vid v = 0; v < dg_a->vertices; ++v) {
			if (force_diagonal) {
				for (tbg_Vid* arc_b = dg_b->head + dg_b->tail_ptr[v];
						arc_b != dg_b->head + dg_b->tail_ptr[v + 1];
						++arc_b) {
					if (row_markers[*arc_b] != v) {
						row_markers[*arc_b] = v;
						++out_max_arcs;
					}
				}
			}
			for (tbg_Vid* arc_a = dg_a->head + dg_a->tail_ptr[v];
					arc_a != dg_a->head + dg_a->tail_ptr[v + 1];
					++arc_a) {
				if (*arc_a == v && (force_diagonal || ignore_diagonal)) continue;
				for (tbg_Vid* arc_b = dg_b->head + dg_b->tail_ptr[*arc_a];
						arc_b != dg_b->head + dg_b->tail_ptr[*arc_a + 1];
						++arc_b) {
					if (row_markers[*arc_b] != v) {
						row_markers[*arc_b] = v;
						++out_max_arcs;
					}
				}
			}
		}

		dg_out = tbg_init_digraph(dg_a->vertices, out_max_arcs);
	}

	// Try again. If fail, give up.
	if (!dg_out.tail_ptr) {
		free(row_markers);
		return dg_out;
	}

	dg_out.tail_ptr[0] = 0;
	tbg_Arcref out_arc_write = 0;
	for (tbg_Vid v = 0; v < dg_a->vertices; ++v) row_markers[v] = TBG_VID_MAX;

	for (tbg_Vid v = 0; v < dg_a->vertices; ++v) {
		if (force_diagonal) {
			for (tbg_Vid* arc_b = dg_b->head + dg_b->tail_ptr[v];
					arc_b != dg_b->head + dg_b->tail_ptr[v + 1];
					++arc_b) {
				if (row_markers[*arc_b] != v) {
					row_markers[*arc_b] = v;
					dg_out.head[out_arc_write] = *arc_b;
					++out_arc_write;
				}
			}
		}
		for (tbg_Vid* arc_a = dg_a->head + dg_a->tail_ptr[v];
				arc_a != dg_a->head + dg_a->tail_ptr[v + 1];
				++arc_a) {
			if (*arc_a == v && (force_diagonal || ignore_diagonal)) continue;
			for (tbg_Vid* arc_b = dg_b->head + dg_b->tail_ptr[*arc_a];
					arc_b != dg_b->head + dg_b->tail_ptr[*arc_a + 1];
					++arc_b) {
				if (row_markers[*arc_b] != v) {
					row_markers[*arc_b] = v;
					dg_out.head[out_arc_write] = *arc_b;
					++out_arc_write;
				}
			}
		}

		dg_out.tail_ptr[v + 1] = out_arc_write;
	}

	free(row_markers);

	tbg_change_arc_storage(&dg_out, out_arc_write);

	return dg_out;
}

/*

tbg_Digraph tbg_digraph_all_walks(const tbg_Digraph* dg, tbg_Vid length) {
	return itbg_digraph_power_and_walks(dg, length, false, false);
}
tbg_Digraph tbg_digraph_all_paths(const tbg_Digraph* dg, tbg_Vid length) {
	return itbg_digraph_power_and_walks(dg, length, false, true);
}

tbg_Digraph tbg_digraph_power(const tbg_Digraph* dg, tbg_Vid exponent) {
	return itbg_digraph_power_and_walks(dg, length, true, false);
}

tbg_Digraph itbg_digraph_power_and_walks(const tbg_Digraph* const dg, tbg_Vid exponent, const bool force_diagonal, const bool ignore_diagonal) {
	if (!dg || !dg->tail_ptr) return tbg_null_digraph();
	if (dg->vertices == 0) return tbg_empty_digraph(0, 0);
	if (exponent == 0) return tbg_empty_digraph(dg->vertices, 0);
	if (exponent == 1) return tbg_copy_digraph(dg);
	if (exponent == 2) return tbg_digraph_adjacency_product(dg, dg, power, false);

	tbg_Digraph dg_tmp;
	tbg_Digraph dg_out = (tbg_Digraph) { 0 };
	tbg_Digraph dg_expo = tbg_copy_digraph(dg);
	if (!dg_expo.tail_ptr) return (tbg_Digraph) { 0 };

	for (; exponent > 0; exponent /= 2) {
		if (exponent % 2 == 1) {
			if (!dg_out.tail_ptr) {
				dg_out = tbg_copy_digraph(&dg_expo);
			} else {
				dg_tmp = dg_out;
				dg_out = tbg_digraph_adjacency_product(&dg_tmp, &dg_expo);
				tbg_free_digraph(&dg_tmp);
			}
			if (!dg_out.tail_ptr) {
				tbg_free_digraph(&dg_expo);
				return tbg_null_digraph();
			} 
		}
		dg_tmp = dg_expo;
		dg_expo = tbg_digraph_adjacency_product(&dg_tmp, &dg_expo);
		tbg_free_digraph(&dg_tmp);
		if (!dg_expo.tail_ptr) {
			tbg_free_digraph(&dg_out);
			return tbg_null_digraph();
		}
	}

	tbg_free_digraph(&dg_expo);

	return dg_out;
}*/

