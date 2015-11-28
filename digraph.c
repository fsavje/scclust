#include "digraph.h"

#include <stdlib.h>


thm_Digraph thm_init_digraph(const thm_Vid vertices, const thm_Arcref max_arcs) {
	thm_Digraph dg = {
		.vertices = vertices,
		.max_arcs = max_arcs,
		.head = NULL,
		.tail_ptr = malloc(sizeof(thm_Arcref[vertices + 1])),
	};

	if (!dg.tail_ptr) return (thm_Digraph) { 0 };

	if (max_arcs > 0) {
		dg.head = malloc(sizeof(thm_Vid[max_arcs]));
		if (!dg.head) thm_free_digraph(&dg);
	}

	return dg;
}


void thm_free_digraph(thm_Digraph* const dg) {
	if (dg) {
		free(dg->head);
		free(dg->tail_ptr);
		*dg = (thm_Digraph) { 0 };
	}
}


bool thm_change_digraph_head_size(thm_Digraph* const dg, const thm_Arcref new_max_arcs) {
	if (!dg || !dg->tail_ptr) return false;
	if (dg->max_arcs == new_max_arcs) return true;
	if (dg->tail_ptr[dg->vertices] > new_max_arcs) return false;

	if (new_max_arcs == 0) {
		free(dg->head);
		dg->head = NULL;
		dg->max_arcs = 0;
	} else {
		thm_Vid* tmp_ptr = realloc(dg->head, sizeof(thm_Vid[new_max_arcs]));
		if (!tmp_ptr) return false;
		dg->head = tmp_ptr;
		dg->max_arcs = new_max_arcs;
	}

	return true;
}


thm_Digraph thm_empty_digraph(const thm_Vid vertices, const thm_Arcref max_arcs) {
	thm_Digraph dg = thm_init_digraph(vertices, max_arcs);
	if (!dg.tail_ptr) return dg;
	
	for (thm_Vid v = 0; v <= vertices; ++v) dg.tail_ptr[v] = 0;

	return dg;
}


thm_Digraph thm_identity_digraph(const thm_Vid vertices) {
	thm_Digraph dg = thm_init_digraph(vertices, vertices);
	if (!dg.tail_ptr) return dg;
	
	for (thm_Vid v = 0; v < vertices; ++v) {
		dg.tail_ptr[v] = v;
		dg.head[v] = v;
	}
	dg.tail_ptr[vertices] = vertices;

	return dg;
}


thm_Digraph thm_balanced_digraph(const thm_Vid vertices, const thm_Vid arcs_per_vertex, thm_Vid* const heads) {
	thm_Digraph dg = thm_init_digraph(vertices, 0);
	if (!dg.tail_ptr) return dg;

	dg.max_arcs = vertices * arcs_per_vertex;
	dg.head = heads;
	for (thm_Vid v = 0; v <= vertices; ++v) {
		dg.tail_ptr[v] = v * arcs_per_vertex;
	}
	
	return dg;
}


thm_Digraph thm_copy_digraph(const thm_Digraph* const dg) {
	if (!dg || !dg->tail_ptr) return (thm_Digraph) { 0 };
	if (dg->vertices == 0) return thm_empty_digraph(0, 0);

	thm_Digraph dg_out = thm_init_digraph(dg->vertices, dg->tail_ptr[dg->vertices]);
	if (!dg_out.tail_ptr) return dg_out;

	for (thm_Vid v = 0; v <= dg->vertices; ++v) dg_out.tail_ptr[v] = dg->tail_ptr[v];
	for (thm_Arcref a = 0; a < dg->tail_ptr[dg->vertices]; ++a) dg_out.head[a] = dg->head[a];

	return dg_out;
}


thm_Digraph thm_digraph_union_two(const thm_Digraph* const dg_a, const thm_Digraph* const dg_b) {
	const thm_Digraph* dgs_ab[2] = {dg_a, dg_b};
	return thm_digraph_union(2, dgs_ab);
}


thm_Digraph thm_digraph_union(const size_t num_dgs, const thm_Digraph* dgs[num_dgs]) {
	if (num_dgs == 0) return thm_empty_digraph(0, 0);
	if (!dgs || !dgs[0]) return (thm_Digraph) { 0 };

	thm_Vid vertices = dgs[0]->vertices;
	thm_Arcref out_max_arcs = 0;
	for (size_t i = 0; i < num_dgs; ++i) {
		if (!dgs[i] || !dgs[i]->tail_ptr || dgs[i]->vertices != vertices) return (thm_Digraph) { 0 };
		out_max_arcs += dgs[i]->tail_ptr[vertices];
	}

	thm_Vid* row_markers = malloc(sizeof(thm_Vid[vertices]));
	if (!row_markers) return (thm_Digraph) { 0 };

	thm_Digraph dg_out = thm_init_digraph(vertices, out_max_arcs);

	if (!dg_out.tail_ptr) {
		// Could not allocate digraph with `row_markers' arcs.
		// Do correct (but slow) memory calculation and try again.

		out_max_arcs = 0;
		for (thm_Vid v = 0; v < vertices; ++v) row_markers[v] = THM_VID_MAX;

		for (thm_Vid v = 0; v < vertices; ++v) {
			for (size_t i = 0; i < num_dgs; ++i) {
				for (thm_Vid* arc_toadd = dgs[i]->head + dgs[i]->tail_ptr[v];
						arc_toadd != dgs[i]->head + dgs[i]->tail_ptr[v + 1];
						++arc_toadd) {
					if (row_markers[*arc_toadd] != v) {
						row_markers[*arc_toadd] = v;
						++out_max_arcs;
					}
				}
			}
		}

		dg_out = thm_init_digraph(vertices, out_max_arcs);
	}

	// Try again. If fail, give up.
	if (!dg_out.tail_ptr) {
		free(row_markers);
		return dg_out;
	}

	dg_out.tail_ptr[0] = 0;
	thm_Arcref out_arc_write = 0;
	for (thm_Vid v = 0; v < vertices; ++v) row_markers[v] = THM_VID_MAX;

	for (thm_Vid v = 0; v < vertices; ++v) {
		for (size_t i = 0; i < num_dgs; ++i) {
			for (thm_Vid* arc_toadd = dgs[i]->head + dgs[i]->tail_ptr[v];
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

	thm_change_digraph_head_size(&dg_out, out_arc_write);

	return dg_out;
}


thm_Digraph thm_digraph_transpose(const thm_Digraph* const dg) {
	if (!dg || !dg->tail_ptr) return (thm_Digraph) { 0 };
	if (dg->vertices == 0) return thm_empty_digraph(0, 0);

	thm_Vid* row_count = calloc(dg->vertices + 1, sizeof(thm_Arcref));
	if (!row_count) return (thm_Digraph) { 0 };

	thm_Digraph dg_out = thm_init_digraph(dg->vertices, dg->tail_ptr[dg->vertices]);
	if (!dg_out.tail_ptr) {
		free(row_count);
		return dg_out;
	}

	for (thm_Vid* arc = dg->head;
			arc != dg->head + dg->tail_ptr[dg->vertices];
			++arc) {
		++row_count[*arc + 1];
	}

	dg_out.tail_ptr[0] = 0;
	for (thm_Vid v = 1; v <= dg->vertices; ++v) {
		row_count[v] += row_count[v - 1];
		dg_out.tail_ptr[v] = row_count[v];
	}

	for (thm_Vid v = 0; v < dg->vertices; ++v) {
		for (thm_Vid* arc = dg->head + dg->tail_ptr[v];
				arc != dg->head + dg->tail_ptr[v + 1];
				++arc) {
			dg_out.head[row_count[*arc]] = v;
			++row_count[*arc];
		}
	}

	free(row_count);

	return dg_out;
}


//thm_Digraph thm_digraph_undirected(const thm_Digraph* const dg) {

//}


thm_Digraph thm_digraph_adjacency_product(const thm_Digraph* const dg_a, const thm_Digraph* const dg_b, const bool force_diagonal, const bool ignore_diagonal) {
	if (force_diagonal && ignore_diagonal) return (thm_Digraph) { 0 };
	if (!dg_a || !dg_b || !dg_a->tail_ptr || !dg_b->tail_ptr) return (thm_Digraph) { 0 };
	if (dg_a->vertices != dg_b->vertices) return (thm_Digraph) { 0 };
	if (dg_a->vertices == 0) return thm_empty_digraph(0, 0);

	thm_Vid* row_markers = malloc(sizeof(thm_Vid[dg_a->vertices]));
	if (!row_markers) return (thm_Digraph) { 0 };

	thm_Arcref out_max_arcs = 0;

	// Greedy memory calculation first
	for (thm_Vid v = 0; v < dg_a->vertices; ++v) {
		if (force_diagonal) {
			out_max_arcs += dg_b->tail_ptr[v + 1] - dg_b->tail_ptr[v];
		}
		for (thm_Vid* arc_a = dg_a->head + dg_a->tail_ptr[v];
				arc_a != dg_a->head + dg_a->tail_ptr[v + 1];
				++arc_a) {
			if (*arc_a == v && (force_diagonal || ignore_diagonal)) continue;
			out_max_arcs += dg_b->tail_ptr[*arc_a + 1] - dg_b->tail_ptr[*arc_a];
		}
	}

	thm_Digraph dg_out = thm_init_digraph(dg_a->vertices, out_max_arcs);

	if (!dg_out.tail_ptr) {
		// Could not allocate digraph with `row_markers' arcs.
		// Do correct (but slow) memory calculation and try again.

		out_max_arcs = 0;
		for (thm_Vid v = 0; v < dg_a->vertices; ++v) row_markers[v] = THM_VID_MAX;

		for (thm_Vid v = 0; v < dg_a->vertices; ++v) {
			if (force_diagonal) {
				for (thm_Vid* arc_b = dg_b->head + dg_b->tail_ptr[v];
						arc_b != dg_b->head + dg_b->tail_ptr[v + 1];
						++arc_b) {
					if (row_markers[*arc_b] != v) {
						row_markers[*arc_b] = v;
						++out_max_arcs;
					}
				}
			}
			for (thm_Vid* arc_a = dg_a->head + dg_a->tail_ptr[v];
					arc_a != dg_a->head + dg_a->tail_ptr[v + 1];
					++arc_a) {
				if (*arc_a == v && (force_diagonal || ignore_diagonal)) continue;
				for (thm_Vid* arc_b = dg_b->head + dg_b->tail_ptr[*arc_a];
						arc_b != dg_b->head + dg_b->tail_ptr[*arc_a + 1];
						++arc_b) {
					if (row_markers[*arc_b] != v) {
						row_markers[*arc_b] = v;
						++out_max_arcs;
					}
				}
			}
		}

		dg_out = thm_init_digraph(dg_a->vertices, out_max_arcs);
	}

	// Try again. If fail, give up.
	if (!dg_out.tail_ptr) {
		free(row_markers);
		return dg_out;
	}

	dg_out.tail_ptr[0] = 0;
	thm_Arcref out_arc_write = 0;
	for (thm_Vid v = 0; v < dg_a->vertices; ++v) row_markers[v] = THM_VID_MAX;

	for (thm_Vid v = 0; v < dg_a->vertices; ++v) {
		if (force_diagonal) {
			for (thm_Vid* arc_b = dg_b->head + dg_b->tail_ptr[v];
					arc_b != dg_b->head + dg_b->tail_ptr[v + 1];
					++arc_b) {
				if (row_markers[*arc_b] != v) {
					row_markers[*arc_b] = v;
					dg_out.head[out_arc_write] = *arc_b;
					++out_arc_write;
				}
			}
		}
		for (thm_Vid* arc_a = dg_a->head + dg_a->tail_ptr[v];
				arc_a != dg_a->head + dg_a->tail_ptr[v + 1];
				++arc_a) {
			if (*arc_a == v && (force_diagonal || ignore_diagonal)) continue;
			for (thm_Vid* arc_b = dg_b->head + dg_b->tail_ptr[*arc_a];
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

	thm_change_digraph_head_size(&dg_out, out_arc_write);

	return dg_out;
}

/*
thm_Digraph thm_digraph_walks(const thm_Digraph* const dg, const thm_Vid length) {
	return ithm_digraph_power_walk(dg, length, false);
}

thm_Digraph thm_digraph_power(const thm_Digraph* const dg, const thm_Vid exponent) {
	return ithm_digraph_power_walk(dg, length, true);
}

thm_Digraph ithm_digraph_power_walk(const thm_Digraph* const dg, thm_Vid exponent, const bool power) {
	if (!dg || !dg->tail_ptr) return (thm_Digraph) { 0 };
	if (dg->vertices == 0) return thm_empty_digraph(0, 0);
	if (exponent == 0) return thm_empty_digraph(dg->vertices, 0);
	if (exponent == 1) return thm_copy_digraph(dg);
	if (exponent == 2) return thm_digraph_adjacency_product(dg, dg, power, false);

	thm_Digraph dg_tmp;
	thm_Digraph dg_out = (thm_Digraph) { 0 };
	thm_Digraph dg_expo = thm_copy_digraph(dg);
	if (!dg_expo.tail_ptr) return (thm_Digraph) { 0 };

	for (; exponent > 0; exponent /= 2) {
		if (exponent % 2 == 1) {
			if (!dg_out.tail_ptr) {
				dg_out = thm_copy_digraph(&dg_expo);
			} else {
				dg_tmp = dg_out;
				dg_out = thm_digraph_adjacency_product(&dg_tmp, &dg_expo);
				thm_free_digraph(&dg_tmp);
			}
			if (!dg_out.tail_ptr) {
				thm_free_digraph(&dg_expo);
				return (thm_Digraph) { 0 };
			} 
		}
		dg_tmp = dg_expo;
		dg_expo = thm_digraph_adjacency_product(&dg_tmp, &dg_expo);
		thm_free_digraph(&dg_tmp);
		if (!dg_expo.tail_ptr) {
			thm_free_digraph(&dg_out);
			return (thm_Digraph) { 0 };
		}
	}

	thm_free_digraph(&dg_expo);

	return dg_out;
}*/

