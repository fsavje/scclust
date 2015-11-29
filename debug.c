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
	#include "debug.h"
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "digraph.h"
#include "core.h"


bool tbg_is_valid_digraph(const tbg_Digraph* const dg) {
	if (!dg || !dg->tail_ptr) return false;
	if (dg->max_arcs == 0 && dg->head) return false;
	if (dg->max_arcs > 0 && !dg->head) return false;
	return true;
}


bool tbg_is_sound_digraph(const tbg_Digraph* const dg) {
	if (!tbg_is_valid_digraph(dg)) return false;
	if (dg->tail_ptr[0] != 0) return false;
	if (dg->tail_ptr[dg->vertices] > dg->max_arcs) return false;
	for (size_t i = 0; i < dg->vertices; ++i) {
		if (dg->tail_ptr[i] > dg->tail_ptr[i + 1]) return false;
	}
	for (size_t i = 0; i < dg->tail_ptr[dg->vertices]; ++i) {
		if (dg->head[i] >= dg->vertices) return false;
	}
	return true;
}


bool tbg_is_empty_digraph(const tbg_Digraph* const dg) {
	if (!tbg_is_sound_digraph(dg)) return false;
	if (dg->tail_ptr[dg->vertices] != 0) return false;
	return true;
}


bool tbg_is_balanced_digraph(const tbg_Digraph* const dg, const tbg_Vid arcs_per_vertex) {
	if (!tbg_is_sound_digraph(dg)) return false;

	for (size_t i = 0; i <= dg->vertices; ++i) {
		if (dg->tail_ptr[i] != i * arcs_per_vertex) return false;
	}

	return true;
}


bool tbg_digraphs_equal(const tbg_Digraph* const dg_a, const tbg_Digraph* const dg_b){
	if (!dg_a) return !dg_b;
	if (!dg_a->tail_ptr) return !dg_b->tail_ptr;
	if (dg_a->vertices != dg_b->vertices) return false;

	int_fast8_t* const single_row = malloc(sizeof(int_fast8_t[dg_a->vertices]));

	for (tbg_Vid v = 0; v < dg_a->vertices; ++v) {

		for (tbg_Vid i = 0; i < dg_a->vertices; ++i) single_row[i] = 0;

		for (const tbg_Vid* arc_a = dg_a->head + dg_a->tail_ptr[v];
					arc_a != dg_a->head + dg_a->tail_ptr[v + 1];
					++arc_a) {
			single_row[*arc_a] = 1;
		}

		for (const tbg_Vid* arc_b = dg_b->head + dg_b->tail_ptr[v];
					arc_b != dg_b->head + dg_b->tail_ptr[v + 1];
					++arc_b) {
			if (single_row[*arc_b] == 0) {
				free(single_row);
				return false;
			} 
			single_row[*arc_b] = 2;
		}

		for (tbg_Vid i = 0; i < dg_a->vertices; ++i) {
			if (single_row[i] == 1) {
				free(single_row);
				return false;
			} 
		}
	}

	free(single_row);
	return true;
}


tbg_Digraph tbg_digraph_from_pieces(const tbg_Vid vertices, const tbg_Arcref max_arcs, const tbg_Arcref tail_ptr[const vertices], const tbg_Vid head[const max_arcs]) {
	if (!tail_ptr) return tbg_null_digraph();
	if (max_arcs > 0 && !head) return tbg_null_digraph();
	tbg_Digraph dg = tbg_init_digraph(vertices, max_arcs);
	if (!dg.tail_ptr) return dg;

	for (tbg_Vid v = 0; v <= vertices; ++v) dg.tail_ptr[v] = tail_ptr[v];
	for (tbg_Arcref a = 0; a < max_arcs; ++a) dg.head[a] = head[a];

	return dg;
}


tbg_Digraph tbg_digraph_from_string(const char dg_str[const]) {
	tbg_Vid vertices = 0;
	size_t all_arcs = 0;
	tbg_Arcref max_arcs = 0;

	for (size_t c = 0; dg_str[c] != '\0'; ++c) {
		if (dg_str[c] == '*' || dg_str[c] == '.') ++all_arcs;
		if (dg_str[c] == '*') ++max_arcs;
		if (dg_str[c] == '/' && vertices == 0) {
			if (all_arcs > TBG_VID_MAX) return tbg_null_digraph();
			vertices = (tbg_Vid) all_arcs;
		} 
		if (dg_str[c] == '/' && all_arcs % vertices != 0) return tbg_null_digraph();
	}

	tbg_Digraph dg_out = tbg_init_digraph(vertices, max_arcs);

	tbg_Arcref curr_array_pos = 0;
	tbg_Vid curr_row = 0;
	tbg_Vid curr_col = 0;
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


void tbg_print_digraph(const tbg_Digraph* const dg) {

	if (!dg || !dg->tail_ptr) {
		printf("Unvalid digraph.\n\n");
		return;
	} 
	if (dg->vertices == 0) {
		printf("[]\n\n");
		return;
	}

	bool* const single_row = calloc(dg->vertices, sizeof(bool));

	for (tbg_Vid v = 0; v < dg->vertices; ++v) {

		for (tbg_Vid i = 0; i < dg->vertices; ++i) single_row[i] = false;

		for (const tbg_Vid* a = dg->head + dg->tail_ptr[v];
					a != dg->head + dg->tail_ptr[v + 1];
					++a) {
			single_row[*a] = true;
		}

		for (tbg_Vid i = 0; i < dg->vertices; ++i) {
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
