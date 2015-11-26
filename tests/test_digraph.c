#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "malloc_override.h"

#include "../digraph.c"
#include "../digraph_debug.h"

#include "assert_digraph.h"


void ut_thm_free_digraph(void** state) {
	thm_Digraph null_graph = (thm_Digraph) { 0 };

	thm_Digraph my_graph1 = thm_empty_digraph(10, 10);
	thm_free_digraph(&my_graph1);
	assert_memory_equal(&my_graph1, &null_graph, sizeof(thm_Digraph));

	thm_Digraph my_graph2 = thm_empty_digraph(10, 10);
	free(my_graph2.head);
	my_graph2.head = NULL;
	thm_free_digraph(&my_graph2);
	assert_memory_equal(&my_graph2, &null_graph, sizeof(thm_Digraph));

	thm_Digraph my_graph3 = (thm_Digraph) { 0 };
	thm_free_digraph(&my_graph3);
	assert_memory_equal(&my_graph3, &null_graph, sizeof(thm_Digraph));

	thm_free_digraph(NULL);
}

void ut_thm_init_digraph(void** state) {
	thm_Digraph my_graph1 = thm_init_digraph(0, 0);
	assert_valid_digraph(&my_graph1, 0);
	assert_int_equal(my_graph1.max_arcs, 0);
	assert_free_digraph(&my_graph1);

	thm_Digraph my_graph2 = thm_init_digraph(10, 100);
	assert_valid_digraph(&my_graph2, 10);
	assert_int_equal(my_graph2.max_arcs, 100);
	assert_free_digraph(&my_graph2);
}

void ut_thm_change_digraph_head_size(void** state) {
	thm_Digraph my_graph1 = thm_empty_digraph(10, 100);
	assert_true(thm_change_digraph_head_size(&my_graph1, 100));
	assert_empty_digraph(&my_graph1, 10);
	assert_int_equal(my_graph1.max_arcs, 100);
	assert_free_digraph(&my_graph1);

	thm_Digraph my_graph2 = digraph_from_string("*.../.*../..*./...*/");
	assert_false(thm_change_digraph_head_size(&my_graph2, 2));
	assert_sound_digraph(&my_graph2, 4);
	assert_int_equal(my_graph2.max_arcs, 4);
	assert_free_digraph(&my_graph2);

	thm_Digraph my_graph3 = thm_empty_digraph(10, 100);
	assert_true(thm_change_digraph_head_size(&my_graph3, 50));
	assert_empty_digraph(&my_graph3, 10);
	assert_int_equal(my_graph3.max_arcs, 50);
	assert_free_digraph(&my_graph3);

	thm_Digraph my_graph4 = thm_empty_digraph(10, 100);
	assert_true(thm_change_digraph_head_size(&my_graph4, 200));
	assert_empty_digraph(&my_graph4, 10);
	assert_int_equal(my_graph4.max_arcs, 200);
	assert_free_digraph(&my_graph4);

	thm_Digraph my_graph5 = thm_empty_digraph(0, 100);
	assert_true(thm_change_digraph_head_size(&my_graph5, 0));
	assert_empty_digraph(&my_graph5, 0);
	assert_int_equal(my_graph5.max_arcs, 0);
	assert_free_digraph(&my_graph5);
}

void ut_thm_empty_digraph(void** state) {
	thm_Digraph my_graph1 = thm_empty_digraph(0, 0);
	assert_empty_digraph(&my_graph1, 0);
	assert_int_equal(my_graph1.max_arcs, 0);
	assert_free_digraph(&my_graph1);

	thm_Digraph my_graph2 = thm_empty_digraph(10, 100);
	assert_empty_digraph(&my_graph2, 10);
	assert_int_equal(my_graph2.max_arcs, 100);
	assert_free_digraph(&my_graph2);
}

void ut_thm_balanced_digraph(void** state) {
	thm_Vid* heads1 = NULL;
	thm_Digraph my_graph1 = thm_balanced_digraph(0, 0, heads1);
	assert_balanced_digraph(&my_graph1, 0, 0);
	assert_int_equal(my_graph1.max_arcs, 0);
	assert_free_digraph(&my_graph1);

	thm_Vid* heads2 = malloc(sizeof(thm_Vid[10 * 4]));
	for (size_t i = 0; i < 40; ++i) heads2[i] = i % 10;
	thm_Digraph my_graph2 = thm_balanced_digraph(10, 4, heads2);
	assert_balanced_digraph(&my_graph2, 10, 4);
	assert_int_equal(my_graph2.max_arcs, 40);
	for (size_t i = 0; i < 40; ++i) assert_int_equal(my_graph2.head[i], i % 10);
	assert_free_digraph(&my_graph2);
}

void ut_thm_add_digraphs(void** state) {
	thm_Digraph ut_dg1 = digraph_from_string("*.../.*../..*./...*/");
	thm_Digraph ut_dg2 = digraph_from_string("...*/..*./.*../*.../");
	thm_Digraph ut_dg3 = digraph_from_string("*.../*.../*.../*.../");


	const thm_Digraph* sum_12[2] = {&ut_dg1, &ut_dg2};
	thm_Digraph ut_make12_ard = thm_add_digraphs(2, sum_12, true);
	thm_Digraph ut_make12_anrd = thm_add_digraphs(2, sum_12, false);
	thm_Digraph ut_make12_trd = thm_add_two_digraphs(&ut_dg1, &ut_dg2, true);
	thm_Digraph ut_make12_tnrd = thm_add_two_digraphs(&ut_dg1, &ut_dg2, false);

	assert_sound_digraph(&ut_make12_ard, 4);
	assert_sound_digraph(&ut_make12_anrd, 4);
	assert_sound_digraph(&ut_make12_trd, 4);
	assert_sound_digraph(&ut_make12_tnrd, 4);
	assert_true(digraphs_equal(&ut_make12_ard, &ut_make12_anrd));
	assert_true(digraphs_equal(&ut_make12_ard, &ut_make12_trd));
	assert_true(digraphs_equal(&ut_make12_ard, &ut_make12_tnrd));

	thm_Arcref tail_ptr12[5] = {0,2,4,6,8};
	thm_Vid head12[8] = {0,3,1,2,2,1,3,0};
	thm_Digraph ut_control12 = digraph_from_pieces(4, 8, tail_ptr12, head12);
	assert_identical_digraph(&ut_control12, &ut_make12_ard);
	assert_identical_digraph(&ut_control12, &ut_make12_anrd);
	assert_identical_digraph(&ut_control12, &ut_make12_trd);
	assert_identical_digraph(&ut_control12, &ut_make12_tnrd);


	const thm_Digraph* sum_13[2] = {&ut_dg1, &ut_dg3};
	thm_Digraph ut_make13_ard = thm_add_digraphs(2, sum_13, true);
	thm_Digraph ut_make13_anrd = thm_add_digraphs(2, sum_13, false);
	thm_Digraph ut_make13_trd = thm_add_two_digraphs(&ut_dg1, &ut_dg3, true);
	thm_Digraph ut_make13_tnrd = thm_add_two_digraphs(&ut_dg1, &ut_dg3, false);
	assert_sound_digraph(&ut_make13_ard, 4);
	assert_sound_digraph(&ut_make13_anrd, 4);
	assert_sound_digraph(&ut_make13_trd, 4);
	assert_sound_digraph(&ut_make13_tnrd, 4);
	assert_true(digraphs_equal(&ut_make13_ard, &ut_make13_anrd));
	assert_true(digraphs_equal(&ut_make13_ard, &ut_make13_trd));
	assert_true(digraphs_equal(&ut_make13_ard, &ut_make13_tnrd));

	thm_Arcref tail_ptr13_rd[5] = {0,1,3,5,7};
	thm_Vid head13_rd[7] = {0,1,0,2,0,3,0};
	thm_Digraph ut_control13_rd = digraph_from_pieces(4, 7, tail_ptr13_rd, head13_rd);
	assert_identical_digraph(&ut_control13_rd, &ut_make13_ard);
	assert_identical_digraph(&ut_control13_rd, &ut_make13_trd);

	thm_Arcref tail_ptr13_nrd[5] = {0,2,4,6,8};
	thm_Vid head13_nrd[8] = {0,0,1,0,2,0,3,0};
	thm_Digraph ut_control13_nrd = digraph_from_pieces(4, 8, tail_ptr13_nrd, head13_nrd);
	assert_identical_digraph(&ut_control13_nrd, &ut_make13_anrd);
	assert_identical_digraph(&ut_control13_nrd, &ut_make13_tnrd);


	const thm_Digraph* sum_31[2] = {&ut_dg3, &ut_dg1};
	thm_Digraph ut_make31_ard = thm_add_digraphs(2, sum_31, true);
	thm_Digraph ut_make31_anrd = thm_add_digraphs(2, sum_31, false);
	thm_Digraph ut_make31_trd = thm_add_two_digraphs(&ut_dg3, &ut_dg1, true);
	thm_Digraph ut_make31_tnrd = thm_add_two_digraphs(&ut_dg3, &ut_dg1, false);
	assert_sound_digraph(&ut_make31_ard, 4);
	assert_sound_digraph(&ut_make31_anrd, 4);
	assert_sound_digraph(&ut_make31_trd, 4);
	assert_sound_digraph(&ut_make31_tnrd, 4);
	assert_true(digraphs_equal(&ut_make31_ard, &ut_make31_anrd));
	assert_true(digraphs_equal(&ut_make31_ard, &ut_make31_trd));
	assert_true(digraphs_equal(&ut_make31_ard, &ut_make31_tnrd));

	assert_true(digraphs_equal(&ut_make31_ard, &ut_make13_ard));

	thm_Arcref tail_ptr31_rd[5] = {0,1,3,5,7};
	thm_Vid head31_rd[7] = {0,0,1,0,2,0,3};
	thm_Digraph ut_control31_rd = digraph_from_pieces(4, 7, tail_ptr31_rd, head31_rd);
	assert_identical_digraph(&ut_control31_rd, &ut_make31_ard);
	assert_identical_digraph(&ut_control31_rd, &ut_make31_trd);

	thm_Arcref tail_ptr31_nrd[5] = {0,2,4,6,8};
	thm_Vid head31_nrd[8] = {0,0,0,1,0,2,0,3};
	thm_Digraph ut_control31_nrd = digraph_from_pieces(4, 8, tail_ptr31_nrd, head31_nrd);
	assert_identical_digraph(&ut_control31_nrd, &ut_make31_anrd);
	assert_identical_digraph(&ut_control31_nrd, &ut_make31_tnrd);


	const thm_Digraph* sum_123[3] = {&ut_dg1, &ut_dg2, &ut_dg3};
	thm_Digraph ut_make123_rd = thm_add_digraphs(3, sum_123, true);
	thm_Digraph ut_make123_nrd = thm_add_digraphs(3, sum_123, false);
	assert_sound_digraph(&ut_make123_rd, 4);
	assert_sound_digraph(&ut_make123_nrd, 4);
	assert_true(digraphs_equal(&ut_make123_rd, &ut_make123_nrd));

	thm_Arcref tail_ptr123_rd[5] = {0,2,5,8,10};
	thm_Vid head123_rd[10] = {0,3,1,2,0,2,1,0,3,0};
	thm_Digraph ut_control123_rd = digraph_from_pieces(4, 10, tail_ptr123_rd, head123_rd);
	assert_identical_digraph(&ut_control123_rd, &ut_make123_rd);

	thm_Arcref tail_ptr123_nrd[5] = {0,3,6,9,12};
	thm_Vid head123_nrd[12] = {0,3,0,1,2,0,2,1,0,3,0,0};
	thm_Digraph ut_control123_nrd = digraph_from_pieces(4, 12, tail_ptr123_nrd, head123_nrd);
	assert_identical_digraph(&ut_control123_nrd, &ut_make123_nrd);


	const thm_Digraph* sum_132[3] = {&ut_dg1, &ut_dg3, &ut_dg2};
	thm_Digraph ut_make132_rd = thm_add_digraphs(3, sum_132, true);
	thm_Digraph ut_make132_nrd = thm_add_digraphs(3, sum_132, false);
	assert_sound_digraph(&ut_make132_rd, 4);
	assert_sound_digraph(&ut_make132_nrd, 4);
	assert_true(digraphs_equal(&ut_make132_rd, &ut_make132_nrd));

	const thm_Digraph* sum_213[3] = {&ut_dg2, &ut_dg1, &ut_dg3};
	thm_Digraph ut_make213_rd = thm_add_digraphs(3, sum_213, true);
	thm_Digraph ut_make213_nrd = thm_add_digraphs(3, sum_213, false);
	assert_sound_digraph(&ut_make213_rd, 4);
	assert_sound_digraph(&ut_make213_nrd, 4);
	assert_true(digraphs_equal(&ut_make213_rd, &ut_make213_nrd));

	const thm_Digraph* sum_321[3] = {&ut_dg3, &ut_dg2, &ut_dg1};
	thm_Digraph ut_make321_rd = thm_add_digraphs(3, sum_321, true);
	thm_Digraph ut_make321_nrd = thm_add_digraphs(3, sum_321, false);
	assert_sound_digraph(&ut_make321_rd, 4);
	assert_sound_digraph(&ut_make321_nrd, 4);
	assert_true(digraphs_equal(&ut_make321_rd, &ut_make321_nrd));

	assert_true(digraphs_equal(&ut_make123_rd, &ut_make132_rd));
	assert_true(digraphs_equal(&ut_make123_rd, &ut_make213_rd));
	assert_true(digraphs_equal(&ut_make123_rd, &ut_make321_rd));

	assert_free_digraph(&ut_dg1);
	assert_free_digraph(&ut_dg2);
	assert_free_digraph(&ut_dg3);
	assert_free_digraph(&ut_make12_ard);
	assert_free_digraph(&ut_make12_anrd);
	assert_free_digraph(&ut_make12_trd);
	assert_free_digraph(&ut_make12_tnrd);
	assert_free_digraph(&ut_make13_ard);
	assert_free_digraph(&ut_make13_anrd);
	assert_free_digraph(&ut_make13_trd);
	assert_free_digraph(&ut_make13_tnrd);
	assert_free_digraph(&ut_make31_ard);
	assert_free_digraph(&ut_make31_anrd);
	assert_free_digraph(&ut_make31_trd);
	assert_free_digraph(&ut_make31_tnrd);
	assert_free_digraph(&ut_make123_rd);
	assert_free_digraph(&ut_make123_nrd);
	assert_free_digraph(&ut_make132_rd);
	assert_free_digraph(&ut_make132_nrd);
	assert_free_digraph(&ut_make213_rd);
	assert_free_digraph(&ut_make213_nrd);
	assert_free_digraph(&ut_make321_rd);
	assert_free_digraph(&ut_make321_nrd);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(ut_thm_free_digraph),
        cmocka_unit_test(ut_thm_init_digraph),
        cmocka_unit_test(ut_thm_change_digraph_head_size),
        cmocka_unit_test(ut_thm_empty_digraph),
        cmocka_unit_test(ut_thm_balanced_digraph),
        cmocka_unit_test(ut_thm_add_digraphs),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
