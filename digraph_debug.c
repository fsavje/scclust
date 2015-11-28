#include "digraph_debug.h"

#include <stdlib.h>
#include <stdio.h>


bool thm_valid_digraph(const thm_Digraph* const test_dg, const thm_Vid vertices) {
	if (!test_dg->tail_ptr) return false;
	if (test_dg->vertices != vertices) return false;
	if (test_dg->max_arcs == 0 && test_dg->head) return false;
	if (test_dg->max_arcs > 0 && !test_dg->head) return false;
	return true;
}


bool thm_sound_digraph(const thm_Digraph* const test_dg, const thm_Vid vertices) {
	if (!thm_valid_digraph(test_dg, vertices)) return false;
	if (test_dg->tail_ptr[0] != 0) return false;
	if (test_dg->tail_ptr[vertices] > test_dg->max_arcs) return false;
	for (size_t i = 0; i < vertices; ++i) {
		if (test_dg->tail_ptr[i] > test_dg->tail_ptr[i + 1]) return false;
	}
	for (size_t i = 0; i < test_dg->tail_ptr[vertices]; ++i) {
		if (test_dg->head[i] >= vertices) return false;
	}
	return true;
}


bool thm_digraphs_equal(const thm_Digraph* const dg_a, const thm_Digraph* const dg_b) {
	if (!dg_a) return !dg_b;
	if (!dg_a->tail_ptr) return !dg_b->tail_ptr;
	if (dg_a->vertices != dg_b->vertices) return false;

	int* single_row = malloc(sizeof(int[dg_a->vertices]));

	for (thm_Vid v = 0; v < dg_a->vertices; ++v) {

		for (thm_Vid i = 0; i < dg_a->vertices; ++i) single_row[i] = 0;

		for (thm_Vid* arc_a = dg_a->head + dg_a->tail_ptr[v];
					arc_a != dg_a->head + dg_a->tail_ptr[v + 1];
					++arc_a) {
			single_row[*arc_a] = 1;
		}

		for (thm_Vid* arc_b = dg_b->head + dg_b->tail_ptr[v];
					arc_b != dg_b->head + dg_b->tail_ptr[v + 1];
					++arc_b) {
			if (single_row[*arc_b] == 0) {
				free(single_row);
				return false;
			} 
			single_row[*arc_b] = 2;
		}

		for (thm_Vid i = 0; i < dg_a->vertices; ++i) {
			if (single_row[i] == 1) {
				free(single_row);
				return false;
			} 
		}
	}

	free(single_row);
	return true;
}


thm_Digraph thm_digraph_from_pieces(const thm_Vid vertices, const thm_Arcref max_arcs, const thm_Arcref tail_ptr[vertices], const thm_Vid head[max_arcs]) {
	if (!tail_ptr) return (thm_Digraph) { 0 };
	if (max_arcs > 0 && !head) return (thm_Digraph) { 0 };
	thm_Digraph dg = thm_init_digraph(vertices, max_arcs);
	if (!dg.tail_ptr) return dg;

	for (thm_Vid v = 0; v <= vertices; ++v) dg.tail_ptr[v] = tail_ptr[v];
	for (thm_Arcref a = 0; a < max_arcs; ++a) dg.head[a] = head[a];

	return dg;
}


thm_Digraph thm_digraph_from_string(const char dg_str[]) {
	thm_Vid vertices = 0;
	size_t all_arcs = 0;
	thm_Arcref max_arcs = 0;

	for (size_t c = 0; dg_str[c] != '\0'; ++c) {
		if (dg_str[c] == '*' || dg_str[c] == '.') ++all_arcs;
		if (dg_str[c] == '*') ++max_arcs;
		if (dg_str[c] == '/' && vertices == 0) vertices = all_arcs;
		if (dg_str[c] == '/' && all_arcs % vertices != 0) return (thm_Digraph) { 0 };
	}

	thm_Digraph dg_out = thm_init_digraph(vertices, max_arcs);

	thm_Arcref curr_array_pos = 0;
	thm_Vid curr_row = 0;
	thm_Vid curr_col = 0;
	dg_out.tail_ptr[0] = 0;

	for (size_t c = 0; dg_str[c] != '\0'; ++c) {
		if (dg_str[c] == '*') {
			dg_out.head[curr_array_pos] = curr_col;
			++curr_array_pos;
		}
		if (dg_str[c] == '*' || dg_str[c] == '.') ++curr_col;
		if (dg_str[c] == '/') {
			++curr_row;
			curr_col = 0;
			dg_out.tail_ptr[curr_row] = curr_array_pos;
		}
	}
	dg_out.tail_ptr[vertices] = curr_array_pos;

	return dg_out;
}


void thm_print_digraph(const thm_Digraph* const dg) {

	if (!dg || !dg->tail_ptr) {
		printf("Unvalid digraph.\n\n");
		return;
	} 
	if (dg->vertices == 0) {
		printf("[]\n\n");
		return;
	}

	bool* single_row = calloc(dg->vertices, sizeof(bool));

	for (thm_Vid v = 0; v < dg->vertices; ++v) {

		for (thm_Vid i = 0; i < dg->vertices; ++i) single_row[i] = false;

		for (thm_Vid* a = dg->head + dg->tail_ptr[v];
					a != dg->head + dg->tail_ptr[v + 1];
					++a) {
			single_row[*a] = true;
		}

		for (thm_Vid i = 0; i < dg->vertices; ++i) {
			if (single_row[i]) {
				putchar('*');
			} else {
				putchar('.');
			}
		}

		putchar('\n');
	}
	putchar('\n');

	free(single_row);
}
