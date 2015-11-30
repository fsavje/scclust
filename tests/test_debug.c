#include "test_suite.h"
#include "assert_digraph.h"

#include <stddef.h>

#include "../digraph.h"
#include "../core.h"
#include "../debug.h"


void ut_tbg_is_valid_digraph(void** state) {
	(void) state;

	tbg_Digraph dg1 = tbg_init_digraph(4, 10);
	tbg_Digraph dg2 = tbg_null_digraph();
	tbg_Digraph dg3 = tbg_init_digraph(4, 0);

	assert_false(tbg_is_valid_digraph(NULL));
	assert_true(tbg_is_valid_digraph(&dg1));
	assert_false(tbg_is_valid_digraph(&dg2));
	assert_true(tbg_is_valid_digraph(&dg3));

	dg3.head = dg1.head;
	dg1.head = NULL;

	assert_false(tbg_is_valid_digraph(&dg1));
	assert_false(tbg_is_valid_digraph(&dg3));

	tbg_free_digraph(&dg1);
	tbg_free_digraph(&dg2);
	tbg_free_digraph(&dg3);
}

void ut_tbg_is_sound_digraph(void** state) {
	(void) state;

	tbg_Arcref tail_ptr[5] = {0,1,2,3,4};
	tbg_Vid head[4] = {0,1,2,3};
	tbg_Digraph dg1 = tbg_digraph_from_pieces(4, 4, tail_ptr, head);
	assert_true(tbg_is_sound_digraph(&dg1));

	dg1.tail_ptr[0] = 10;
	assert_false(tbg_is_sound_digraph(&dg1));
	dg1.tail_ptr[0] = 0;
	assert_true(tbg_is_sound_digraph(&dg1));

	dg1.tail_ptr[4] = 10;
	assert_false(tbg_is_sound_digraph(&dg1));
	dg1.tail_ptr[4] = 4;
	assert_true(tbg_is_sound_digraph(&dg1));

	dg1.tail_ptr[3] = 4;
	dg1.tail_ptr[4] = 3;
	assert_false(tbg_is_sound_digraph(&dg1));
	dg1.tail_ptr[3] = 3;
	dg1.tail_ptr[4] = 4;
	assert_true(tbg_is_sound_digraph(&dg1));

	dg1.head[0] = 10;
	assert_false(tbg_is_sound_digraph(&dg1));
	dg1.head[0] = 0;
	assert_true(tbg_is_sound_digraph(&dg1));

	tbg_Digraph dg2 = tbg_empty_digraph(4, 10);
	assert_true(tbg_is_sound_digraph(&dg2));

	tbg_free_digraph(&dg1);
	tbg_free_digraph(&dg2);
}

void ut_tbg_is_empty_digraph(void** state) {
	(void) state;

	tbg_Digraph dg1 = tbg_digraph_from_string("**../.*../.*.*/..**/");
	tbg_Digraph dg2 = tbg_empty_digraph(4, 10);

	assert_false(tbg_is_empty_digraph(&dg1));
	assert_true(tbg_is_empty_digraph(&dg2));

	tbg_free_digraph(&dg1);
	tbg_free_digraph(&dg2);
}

void ut_tbg_is_balanced_digraph(void** state) {
	(void) state;

	tbg_Digraph dg1 = tbg_digraph_from_string("*.../.*../.*../..*./");
	tbg_Digraph dg2 = tbg_empty_digraph(4, 10);

	assert_true(tbg_is_balanced_digraph(&dg1, 1));
	assert_false(tbg_is_balanced_digraph(&dg2, 1));

	tbg_free_digraph(&dg1);
	tbg_free_digraph(&dg2);
}

void ut_tbg_debug_rest(void** state) {
	(void) state;

	tbg_Vid headA[7] = {0,1,1,1,3,2,3};
	tbg_Arcref tail_ptrA[5] = {0,2,3,5,7};

	tbg_Digraph ut_dg1 = tbg_digraph_from_pieces(4, 7, tail_ptrA, headA);
	tbg_Digraph ut_dg2 = tbg_digraph_from_string("**../.*../.*.*/..**/");
	tbg_Digraph ut_dg3 = tbg_digraph_from_string("**../.*../.*.*/..**");

	tbg_Arcref tail_ptrB[5] = {0,0,0,0,0};
	tbg_Digraph ut_dg4 = tbg_digraph_from_pieces(4, 0, tail_ptrB, NULL);
	tbg_Digraph ut_dg5 = tbg_digraph_from_string("..../..../..../....");
	tbg_Digraph empty_dg1 = tbg_empty_digraph(4, 0);

	tbg_Vid headC[4] = {0,1,2,3};
	tbg_Arcref tail_ptrC[5] = {0,1,2,3,4};
	tbg_Digraph ut_dg6 = tbg_digraph_from_pieces(4, 4, tail_ptrC, headC);
	tbg_Digraph ut_dg7 = tbg_digraph_from_string("*.../.*../..*./...*");
	tbg_Digraph ut_dg8 = tbg_digraph_from_string("a*.../e fdgtthhh.*..t/2..*.5/7..   .*");

	tbg_Arcref tail_ptrD[1] = {0};
	tbg_Digraph ut_dg9 = tbg_digraph_from_pieces(0, 0, tail_ptrD, NULL);
	tbg_Digraph ut_dg10 = tbg_digraph_from_string(" ");
	tbg_Digraph empty_dg2 = tbg_empty_digraph(0, 0);

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

	assert_memory_equal(ut_dg1.tail_ptr, &tail_ptrA, 5 * sizeof(tbg_Arcref));
	assert_memory_equal(ut_dg2.tail_ptr, &tail_ptrA, 5 * sizeof(tbg_Arcref));
	assert_memory_equal(ut_dg3.tail_ptr, &tail_ptrA, 5 * sizeof(tbg_Arcref));
	assert_memory_equal(ut_dg4.tail_ptr, &tail_ptrB, 5 * sizeof(tbg_Arcref));
	assert_memory_equal(ut_dg5.tail_ptr, &tail_ptrB, 5 * sizeof(tbg_Arcref));
	assert_memory_equal(ut_dg6.tail_ptr, &tail_ptrC, 5 * sizeof(tbg_Arcref));
	assert_memory_equal(ut_dg7.tail_ptr, &tail_ptrC, 5 * sizeof(tbg_Arcref));
	assert_memory_equal(ut_dg8.tail_ptr, &tail_ptrC, 5 * sizeof(tbg_Arcref));
	assert_memory_equal(ut_dg9.tail_ptr, &tail_ptrD, 1 * sizeof(tbg_Arcref));
	assert_memory_equal(ut_dg10.tail_ptr, &tail_ptrD, 1 * sizeof(tbg_Arcref));

	assert_memory_equal(ut_dg1.head, &headA, ut_dg1.max_arcs * sizeof(tbg_Vid));
	assert_memory_equal(ut_dg2.head, &headA, ut_dg2.max_arcs * sizeof(tbg_Vid));
	assert_memory_equal(ut_dg3.head, &headA, ut_dg3.max_arcs * sizeof(tbg_Vid));
	assert_null(ut_dg4.head);
	assert_null(ut_dg5.head);
	assert_memory_equal(ut_dg6.head, &headC, ut_dg6.max_arcs * sizeof(tbg_Vid));
	assert_memory_equal(ut_dg7.head, &headC, ut_dg7.max_arcs * sizeof(tbg_Vid));
	assert_memory_equal(ut_dg8.head, &headC, ut_dg8.max_arcs * sizeof(tbg_Vid));
	assert_null(ut_dg9.head);
	assert_null(ut_dg10.head);

	assert_true(tbg_digraphs_equal(&ut_dg1, &ut_dg2));
	assert_true(tbg_digraphs_equal(&ut_dg1, &ut_dg3));
	assert_true(tbg_digraphs_equal(&ut_dg4, &ut_dg5));
	assert_true(tbg_digraphs_equal(&ut_dg4, &empty_dg1));
	assert_true(tbg_digraphs_equal(&ut_dg6, &ut_dg7));
	assert_true(tbg_digraphs_equal(&ut_dg6, &ut_dg8));
	assert_true(tbg_digraphs_equal(&ut_dg9, &ut_dg10));
	assert_true(tbg_digraphs_equal(&ut_dg9, &empty_dg2));

	assert_false(tbg_digraphs_equal(&ut_dg1, &ut_dg5));
	assert_false(tbg_digraphs_equal(&empty_dg1, &empty_dg2));
	assert_false(tbg_digraphs_equal(&ut_dg6, &ut_dg10));

	assert_identical_digraph(&ut_dg1, &ut_dg2);
	assert_identical_digraph(&ut_dg1, &ut_dg3);
	assert_identical_digraph(&ut_dg4, &ut_dg5);
	assert_identical_digraph(&ut_dg4, &empty_dg1);
	assert_identical_digraph(&ut_dg6, &ut_dg7);
	assert_identical_digraph(&ut_dg6, &ut_dg8);
	assert_identical_digraph(&ut_dg9, &ut_dg10);
	assert_identical_digraph(&ut_dg9, &empty_dg2);

	tbg_print_digraph(&ut_dg1);
	tbg_print_digraph(&ut_dg2);
	tbg_print_digraph(&ut_dg3);
	tbg_print_digraph(&ut_dg4);
	tbg_print_digraph(&ut_dg5);
	tbg_print_digraph(&ut_dg6);
	tbg_print_digraph(&ut_dg7);
	tbg_print_digraph(&ut_dg8);
	tbg_print_digraph(&ut_dg9);
	tbg_print_digraph(&ut_dg10);

	tbg_free_digraph(&ut_dg1);
	tbg_free_digraph(&ut_dg2);
	tbg_free_digraph(&ut_dg3);
	tbg_free_digraph(&ut_dg4);
	tbg_free_digraph(&ut_dg5);
	tbg_free_digraph(&ut_dg6);
	tbg_free_digraph(&ut_dg7);
	tbg_free_digraph(&ut_dg8);
	tbg_free_digraph(&ut_dg9);
	tbg_free_digraph(&ut_dg10);

	tbg_free_digraph(&empty_dg1);
	tbg_free_digraph(&empty_dg2);
}


int main(void) {
    const struct CMUnitTest test_debug[] = {
        cmocka_unit_test(ut_tbg_is_valid_digraph),
        cmocka_unit_test(ut_tbg_is_sound_digraph),
        cmocka_unit_test(ut_tbg_is_empty_digraph),
        cmocka_unit_test(ut_tbg_is_balanced_digraph),
        cmocka_unit_test(ut_tbg_debug_rest),
    };

    return cmocka_run_group_tests_name("debug module", test_debug, NULL, NULL);
}
