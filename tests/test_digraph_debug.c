/*  scclust -- A C library for size constrained clustering
    https://github.com/fsavje/scclust

    Copyright (C) 2015  Fredrik Savje -- http://fredriksavje.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
==============================================================================*/


#include "test_suite.h"
#include "assert_digraph.h"

#include <stddef.h>

#include "../include/config.h"
#include "../include/digraph.h"
#include "../include/digraph_debug.h"


void scc_ut_is_valid_digraph(void** state) {
	(void) state;

	scc_Digraph dg1 = scc_init_digraph(4, 10);
	scc_Digraph dg2 = scc_null_digraph();
	scc_Digraph dg3 = scc_init_digraph(4, 0);

	assert_false(scc_is_valid_digraph(NULL));
	assert_true(scc_is_valid_digraph(&dg1));
	assert_false(scc_is_valid_digraph(&dg2));
	assert_true(scc_is_valid_digraph(&dg3));

	dg3.head = dg1.head;
	dg1.head = NULL;

	assert_false(scc_is_valid_digraph(&dg1));
	assert_false(scc_is_valid_digraph(&dg3));

	scc_free_digraph(&dg1);
	scc_free_digraph(&dg2);
	scc_free_digraph(&dg3);
}

void scc_ut_is_sound_digraph(void** state) {
	(void) state;

	scc_Arcref tail_ptr[5] = {0,1,2,3,4};
	scc_Vid head[4] = {0,1,2,3};
	scc_Digraph dg1 = scc_digraph_from_pieces(4, 4, tail_ptr, head);
	assert_true(scc_is_sound_digraph(&dg1));

	dg1.tail_ptr[0] = 10;
	assert_false(scc_is_sound_digraph(&dg1));
	dg1.tail_ptr[0] = 0;
	assert_true(scc_is_sound_digraph(&dg1));

	dg1.tail_ptr[4] = 10;
	assert_false(scc_is_sound_digraph(&dg1));
	dg1.tail_ptr[4] = 4;
	assert_true(scc_is_sound_digraph(&dg1));

	dg1.tail_ptr[3] = 4;
	dg1.tail_ptr[4] = 3;
	assert_false(scc_is_sound_digraph(&dg1));
	dg1.tail_ptr[3] = 3;
	dg1.tail_ptr[4] = 4;
	assert_true(scc_is_sound_digraph(&dg1));

	dg1.head[0] = 10;
	assert_false(scc_is_sound_digraph(&dg1));
	dg1.head[0] = 0;
	assert_true(scc_is_sound_digraph(&dg1));

	scc_Digraph dg2 = scc_empty_digraph(4, 10);
	assert_true(scc_is_sound_digraph(&dg2));

	scc_free_digraph(&dg1);
	scc_free_digraph(&dg2);
}

void scc_ut_is_empty_digraph(void** state) {
	(void) state;

	scc_Digraph dg1 = scc_digraph_from_string("**../.*../.*.*/..**/");
	scc_Digraph dg2 = scc_empty_digraph(4, 10);

	assert_false(scc_is_empty_digraph(&dg1));
	assert_true(scc_is_empty_digraph(&dg2));

	scc_free_digraph(&dg1);
	scc_free_digraph(&dg2);
}

void scc_ut_is_balanced_digraph(void** state) {
	(void) state;

	scc_Digraph dg1 = scc_digraph_from_string("*.../.*../.*../..*./");
	scc_Digraph dg2 = scc_empty_digraph(4, 10);

	assert_true(scc_is_balanced_digraph(&dg1, 1));
	assert_false(scc_is_balanced_digraph(&dg2, 1));

	scc_free_digraph(&dg1);
	scc_free_digraph(&dg2);
}

void scc_ut_debug_rest(void** state) {
	(void) state;

	scc_Vid headA[7] = {0,1,1,1,3,2,3};
	scc_Arcref tail_ptrA[5] = {0,2,3,5,7};

	scc_Digraph ut_dg1 = scc_digraph_from_pieces(4, 7, tail_ptrA, headA);
	scc_Digraph ut_dg2 = scc_digraph_from_string("**../.*../.*.*/..**/");
	scc_Digraph ut_dg3 = scc_digraph_from_string("**../.*../.*.*/..**");

	scc_Arcref tail_ptrB[5] = {0,0,0,0,0};
	scc_Digraph ut_dg4 = scc_digraph_from_pieces(4, 0, tail_ptrB, NULL);
	scc_Digraph ut_dg5 = scc_digraph_from_string("..../..../..../....");
	scc_Digraph empty_dg1 = scc_empty_digraph(4, 0);

	scc_Vid headC[4] = {0,1,2,3};
	scc_Arcref tail_ptrC[5] = {0,1,2,3,4};
	scc_Digraph ut_dg6 = scc_digraph_from_pieces(4, 4, tail_ptrC, headC);
	scc_Digraph ut_dg7 = scc_digraph_from_string("*.../.*../..*./...*");
	scc_Digraph ut_dg8 = scc_digraph_from_string("a*.../e fdgtthhh.*..t/2..*.5/7..   .*");

	scc_Arcref tail_ptrD[1] = {0};
	scc_Digraph ut_dg9 = scc_digraph_from_pieces(0, 0, tail_ptrD, NULL);
	scc_Digraph ut_dg10 = scc_digraph_from_string(" ");
	scc_Digraph empty_dg2 = scc_empty_digraph(0, 0);

	assert_sound_digraph(&ut_dg1, 4);
	assert_sound_digraph(&ut_dg2, 4);
	assert_sound_digraph(&ut_dg3, 4);
	assert_sound_digraph(&ut_dg4, 4);
	assert_sound_digraph(&ut_dg5, 4);
	assert_sound_digraph(&ut_dg6, 4);
	assert_sound_digraph(&ut_dg7, 4);
	assert_sound_digraph(&ut_dg8, 4);
	assert_sound_digraph(&ut_dg9, 0);
	assert_sound_digraph(&ut_dg10, 0);

	assert_int_equal(ut_dg1.max_arcs, 7);
	assert_int_equal(ut_dg2.max_arcs, 7);
	assert_int_equal(ut_dg3.max_arcs, 7);
	assert_int_equal(ut_dg4.max_arcs, 0);
	assert_int_equal(ut_dg5.max_arcs, 0);
	assert_int_equal(ut_dg6.max_arcs, 4);
	assert_int_equal(ut_dg7.max_arcs, 4);
	assert_int_equal(ut_dg8.max_arcs, 4);
	assert_int_equal(ut_dg9.max_arcs, 0);
	assert_int_equal(ut_dg10.max_arcs, 0);

	assert_memory_equal(ut_dg1.tail_ptr, &tail_ptrA, 5 * sizeof(scc_Arcref));
	assert_memory_equal(ut_dg2.tail_ptr, &tail_ptrA, 5 * sizeof(scc_Arcref));
	assert_memory_equal(ut_dg3.tail_ptr, &tail_ptrA, 5 * sizeof(scc_Arcref));
	assert_memory_equal(ut_dg4.tail_ptr, &tail_ptrB, 5 * sizeof(scc_Arcref));
	assert_memory_equal(ut_dg5.tail_ptr, &tail_ptrB, 5 * sizeof(scc_Arcref));
	assert_memory_equal(ut_dg6.tail_ptr, &tail_ptrC, 5 * sizeof(scc_Arcref));
	assert_memory_equal(ut_dg7.tail_ptr, &tail_ptrC, 5 * sizeof(scc_Arcref));
	assert_memory_equal(ut_dg8.tail_ptr, &tail_ptrC, 5 * sizeof(scc_Arcref));
	assert_memory_equal(ut_dg9.tail_ptr, &tail_ptrD, 1 * sizeof(scc_Arcref));
	assert_memory_equal(ut_dg10.tail_ptr, &tail_ptrD, 1 * sizeof(scc_Arcref));

	assert_memory_equal(ut_dg1.head, &headA, ut_dg1.max_arcs * sizeof(scc_Vid));
	assert_memory_equal(ut_dg2.head, &headA, ut_dg2.max_arcs * sizeof(scc_Vid));
	assert_memory_equal(ut_dg3.head, &headA, ut_dg3.max_arcs * sizeof(scc_Vid));
	assert_null(ut_dg4.head);
	assert_null(ut_dg5.head);
	assert_memory_equal(ut_dg6.head, &headC, ut_dg6.max_arcs * sizeof(scc_Vid));
	assert_memory_equal(ut_dg7.head, &headC, ut_dg7.max_arcs * sizeof(scc_Vid));
	assert_memory_equal(ut_dg8.head, &headC, ut_dg8.max_arcs * sizeof(scc_Vid));
	assert_null(ut_dg9.head);
	assert_null(ut_dg10.head);

	assert_true(scc_digraphs_equal(&ut_dg1, &ut_dg2));
	assert_true(scc_digraphs_equal(&ut_dg1, &ut_dg3));
	assert_true(scc_digraphs_equal(&ut_dg4, &ut_dg5));
	assert_true(scc_digraphs_equal(&ut_dg4, &empty_dg1));
	assert_true(scc_digraphs_equal(&ut_dg6, &ut_dg7));
	assert_true(scc_digraphs_equal(&ut_dg6, &ut_dg8));
	assert_true(scc_digraphs_equal(&ut_dg9, &ut_dg10));
	assert_true(scc_digraphs_equal(&ut_dg9, &empty_dg2));

	assert_false(scc_digraphs_equal(&ut_dg1, &ut_dg5));
	assert_false(scc_digraphs_equal(&empty_dg1, &empty_dg2));
	assert_false(scc_digraphs_equal(&ut_dg6, &ut_dg10));

	assert_identical_digraph(&ut_dg1, &ut_dg2);
	assert_identical_digraph(&ut_dg1, &ut_dg3);
	assert_identical_digraph(&ut_dg4, &ut_dg5);
	assert_identical_digraph(&ut_dg4, &empty_dg1);
	assert_identical_digraph(&ut_dg6, &ut_dg7);
	assert_identical_digraph(&ut_dg6, &ut_dg8);
	assert_identical_digraph(&ut_dg9, &ut_dg10);
	assert_identical_digraph(&ut_dg9, &empty_dg2);

	scc_print_digraph(&ut_dg1);
	scc_print_digraph(&ut_dg2);
	scc_print_digraph(&ut_dg3);
	scc_print_digraph(&ut_dg4);
	scc_print_digraph(&ut_dg5);
	scc_print_digraph(&ut_dg6);
	scc_print_digraph(&ut_dg7);
	scc_print_digraph(&ut_dg8);
	scc_print_digraph(&ut_dg9);
	scc_print_digraph(&ut_dg10);

	scc_free_digraph(&ut_dg1);
	scc_free_digraph(&ut_dg2);
	scc_free_digraph(&ut_dg3);
	scc_free_digraph(&ut_dg4);
	scc_free_digraph(&ut_dg5);
	scc_free_digraph(&ut_dg6);
	scc_free_digraph(&ut_dg7);
	scc_free_digraph(&ut_dg8);
	scc_free_digraph(&ut_dg9);
	scc_free_digraph(&ut_dg10);

	scc_free_digraph(&empty_dg1);
	scc_free_digraph(&empty_dg2);
}


int main(void) {
	const struct CMUnitTest test_debug[] = {
		cmocka_unit_test(scc_ut_is_valid_digraph),
		cmocka_unit_test(scc_ut_is_sound_digraph),
		cmocka_unit_test(scc_ut_is_empty_digraph),
		cmocka_unit_test(scc_ut_is_balanced_digraph),
		cmocka_unit_test(scc_ut_debug_rest),
	};

	return cmocka_run_group_tests_name("debug module", test_debug, NULL, NULL);
}
