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

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * ============================================================================== */


#ifndef SCC_UT_ASSERT_DIGRAPH_HG
#define SCC_UT_ASSERT_DIGRAPH_HG

#include "test_suite.h"

#include <stdbool.h>
#include <stddef.h>

#define assert_valid_digraph(test_dg, vertices) _assert_valid_digraph(test_dg, vertices, #test_dg, __FILE__, __LINE__)
#define assert_free_digraph(test_dg) _assert_free_digraph(test_dg, #test_dg, __FILE__, __LINE__)
#define assert_sound_digraph(test_dg, vertices) _assert_sound_digraph(test_dg, vertices, #test_dg, __FILE__, __LINE__)
#define assert_equal_digraph(test_dg1, test_dg2) _assert_equal_digraph(test_dg1, test_dg2, #test_dg1, #test_dg2, __FILE__, __LINE__)
#define assert_identical_digraph(test_dg1, test_dg2) _assert_identical_digraph(test_dg1, test_dg2, #test_dg1, #test_dg2, __FILE__, __LINE__)
#define assert_empty_digraph(test_dg, vertices) _assert_empty_digraph(test_dg, vertices, #test_dg, __FILE__, __LINE__)
#define assert_balanced_digraph(test_dg, vertices, arcs_per_vertex) _assert_balanced_digraph(test_dg, vertices, arcs_per_vertex, #test_dg, __FILE__, __LINE__)

#include "../include/config.h"
#include "../include/digraph.h"
#include "../include/digraph_debug.h"


void _assert_valid_digraph(const scc_Digraph* test_dg,
                           const scc_Vid vertices,
                           const char* const name_dg,
                           const char* const file,
                           const int line) {
	if (!scc_is_valid_digraph(test_dg) || test_dg->vertices != vertices) {
		print_error("%s is not valid\n", name_dg);
		_fail(file, line);
	}
}

void _assert_free_digraph(scc_Digraph* test_dg,
                          const char* const name_dg,
                          const char* const file,
                          const int line) {
	if (!test_dg || !test_dg->tail_ptr) {
		print_error("%s is already freed\n", name_dg);
		_fail(file, line);
	} else {
		for (size_t i = 0; i <= test_dg->vertices; ++i) test_dg->tail_ptr[i] = 1;
		for (size_t i = 0; i < test_dg->max_arcs; ++i) test_dg->head[i] = 1;
	}
	scc_free_digraph(test_dg);
}

void _assert_sound_digraph(const scc_Digraph* test_dg,
                           const scc_Vid vertices,
                           const char* const name_dg,
                           const char* const file,
                           const int line) {
	if (!scc_is_sound_digraph(test_dg) || test_dg->vertices != vertices) {
		print_error("%s is not sound\n", name_dg);
		_fail(file, line);
	}
}

void _assert_empty_digraph(const scc_Digraph* test_dg,
                           const scc_Vid vertices,
                           const char* const name_dg,
                           const char* const file,
                           const int line) {
	if (!scc_is_empty_digraph(test_dg) || test_dg->vertices != vertices) {
		print_error("%s is not empty\n", name_dg);
		_fail(file, line);
	}
}

void _assert_balanced_digraph(const scc_Digraph* test_dg,
                              const scc_Vid vertices,
                              const scc_Vid arcs_per_vertex,
                              const char* const name_dg,
                              const char* const file,
                              const int line) {
	if (!scc_is_balanced_digraph(test_dg, arcs_per_vertex) || test_dg->vertices != vertices) {
		print_error("%s is not balanced\n", name_dg);
		_fail(file, line);
	}
}

void _assert_equal_digraph(const scc_Digraph* test_dg1,
                           const scc_Digraph* test_dg2,
                           const char* const name_dg1,
                           const char* const name_dg2,
                           const char* const file,
                           const int line) {
	if (!scc_digraphs_equal(test_dg1, test_dg2)) {
		print_error("%s and %s are not equal\n", name_dg1, name_dg2);
		_fail(file, line);
	}
}

void _assert_identical_digraph(const scc_Digraph* test_dg1,
                               const scc_Digraph* test_dg2,
                               const char* const name_dg1,
                               const char* const name_dg2,
                               const char* const file,
                               const int line) {
	bool is_identical = true;
	if (is_identical && test_dg1->max_arcs != test_dg2->max_arcs) is_identical = false;
	if (is_identical && test_dg1->vertices != test_dg2->vertices) is_identical = false;

	if (is_identical) {
		if (!test_dg1->tail_ptr != !test_dg2->tail_ptr) {
			is_identical = false;
		} else if (test_dg1->tail_ptr) {
			for (size_t i = 0; i < test_dg1->vertices + 1; ++i) {
				if (test_dg1->tail_ptr[i] != test_dg2->tail_ptr[i]) {
					is_identical = false;
					break;
				}
			}
		}
	}

	if (is_identical) {
		if (!test_dg1->head != !test_dg2->head) {
			is_identical = false;
		} else if (test_dg1->head) {
			for (size_t i = 0; i < test_dg1->max_arcs; ++i) {
				if (test_dg1->head[i] != test_dg2->head[i]) {
					is_identical = false;
					break;
				}
			}
		}
	}

	if (!is_identical) {
		print_error("%s and %s are not identical\n", name_dg1, name_dg2);
		_fail(file, line);
	}
}

#endif
