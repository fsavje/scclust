#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "malloc_override.h"

#include "../digraph_debug.h"
#include "../digraph.c"

#include "assert_digraph.h"

void ut_check_debug(void** state) {
	thm_Vid headA[7] = {0,1,1,1,3,2,3};
	thm_Arcref tail_ptrA[5] = {0,2,3,5,7};

	thm_Digraph ut_dg1 = digraph_from_pieces(4, 7, tail_ptrA, headA);
	thm_Digraph ut_dg2 = digraph_from_string("**../.*../.*.*/..**/");
	thm_Digraph ut_dg3 = digraph_from_string("**../.*../.*.*/..**");

	thm_Arcref tail_ptrB[5] = {0,0,0,0,0};
	thm_Digraph ut_dg4 = digraph_from_pieces(4, 0, tail_ptrB, NULL);
	thm_Digraph ut_dg5 = digraph_from_string("..../..../..../....");
	thm_Digraph empty_dg1 = thm_empty_digraph(4, 0);

	thm_Vid headC[4] = {0,1,2,3};
	thm_Arcref tail_ptrC[5] = {0,1,2,3,4};
	thm_Digraph ut_dg6 = digraph_from_pieces(4, 4, tail_ptrC, headC);
	thm_Digraph ut_dg7 = digraph_from_string("*.../.*../..*./...*");
	thm_Digraph ut_dg8 = digraph_from_string("a*.../e fdgtthhh.*..t/2..*.5/7..   .*");

	thm_Arcref tail_ptrD[1] = {0};
	thm_Digraph ut_dg9 = digraph_from_pieces(0, 0, tail_ptrD, NULL);
	thm_Digraph ut_dg10 = digraph_from_string(" ");
	thm_Digraph empty_dg2 = thm_empty_digraph(0, 0);

	assert_identical_digraph(&ut_dg1, &ut_dg2);
	assert_identical_digraph(&ut_dg1, &ut_dg3);
	assert_identical_digraph(&ut_dg4, &ut_dg5);
	assert_identical_digraph(&ut_dg4, &empty_dg1);
	assert_identical_digraph(&ut_dg6, &ut_dg7);
	assert_identical_digraph(&ut_dg6, &ut_dg8);
	assert_identical_digraph(&ut_dg9, &ut_dg10);
	assert_identical_digraph(&ut_dg9, &empty_dg2);

	assert_true(digraphs_equal(&ut_dg1, &ut_dg2));
	assert_true(digraphs_equal(&ut_dg1, &ut_dg3));
	assert_true(digraphs_equal(&ut_dg4, &ut_dg5));
	assert_true(digraphs_equal(&ut_dg4, &empty_dg1));
	assert_true(digraphs_equal(&ut_dg6, &ut_dg7));
	assert_true(digraphs_equal(&ut_dg6, &ut_dg8));
	assert_true(digraphs_equal(&ut_dg9, &ut_dg10));
	assert_true(digraphs_equal(&ut_dg9, &empty_dg2));

	assert_false(digraphs_equal(&ut_dg1, &ut_dg5));
	assert_false(digraphs_equal(&empty_dg1, &empty_dg2));
	assert_false(digraphs_equal(&ut_dg6, &ut_dg10));

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

	assert_memory_equal(ut_dg1.tail_ptr, &tail_ptrA, 5 * sizeof(thm_Arcref));
	assert_memory_equal(ut_dg2.tail_ptr, &tail_ptrA, 5 * sizeof(thm_Arcref));
	assert_memory_equal(ut_dg3.tail_ptr, &tail_ptrA, 5 * sizeof(thm_Arcref));
	assert_memory_equal(ut_dg4.tail_ptr, &tail_ptrB, 5 * sizeof(thm_Arcref));
	assert_memory_equal(ut_dg5.tail_ptr, &tail_ptrB, 5 * sizeof(thm_Arcref));
	assert_memory_equal(ut_dg6.tail_ptr, &tail_ptrC, 5 * sizeof(thm_Arcref));
	assert_memory_equal(ut_dg7.tail_ptr, &tail_ptrC, 5 * sizeof(thm_Arcref));
	assert_memory_equal(ut_dg8.tail_ptr, &tail_ptrC, 5 * sizeof(thm_Arcref));
	assert_memory_equal(ut_dg9.tail_ptr, &tail_ptrD, 1 * sizeof(thm_Arcref));
	assert_memory_equal(ut_dg10.tail_ptr, &tail_ptrD, 1 * sizeof(thm_Arcref));

	assert_memory_equal(ut_dg1.head, &headA, ut_dg1.max_arcs * sizeof(thm_Vid));
	assert_memory_equal(ut_dg2.head, &headA, ut_dg2.max_arcs * sizeof(thm_Vid));
	assert_memory_equal(ut_dg3.head, &headA, ut_dg3.max_arcs * sizeof(thm_Vid));
	assert_null(ut_dg4.head);
	assert_null(ut_dg5.head);
	assert_memory_equal(ut_dg6.head, &headC, ut_dg6.max_arcs * sizeof(thm_Vid));
	assert_memory_equal(ut_dg7.head, &headC, ut_dg7.max_arcs * sizeof(thm_Vid));
	assert_memory_equal(ut_dg8.head, &headC, ut_dg8.max_arcs * sizeof(thm_Vid));
	assert_null(ut_dg9.head);
	assert_null(ut_dg10.head);

	print_digraph(&ut_dg1);
	print_digraph(&ut_dg2);
	print_digraph(&ut_dg3);
	print_digraph(&ut_dg4);
	print_digraph(&ut_dg5);
	print_digraph(&ut_dg6);
	print_digraph(&ut_dg7);
	print_digraph(&ut_dg8);
	print_digraph(&ut_dg9);
	print_digraph(&ut_dg10);

	thm_free_digraph(&ut_dg2);
	thm_free_digraph(&ut_dg3);
	thm_free_digraph(&ut_dg5);
	thm_free_digraph(&ut_dg7);
	thm_free_digraph(&ut_dg8);
	thm_free_digraph(&ut_dg10);

	thm_free_digraph(&empty_dg1);
	thm_free_digraph(&empty_dg2);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(ut_check_debug),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
