#include "test_suite.h"
#include "assert_digraph.h"

#include "../config.h"
#include "../digraph_debug.h"

#include "../digraph.c"


void ut_thm_free_digraph(void** state) {
	(void) state;

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
	(void) state;

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
	(void) state;

	thm_Digraph my_graph1 = thm_empty_digraph(10, 100);
	assert_true(thm_change_digraph_head_size(&my_graph1, 100));
	assert_empty_digraph(&my_graph1, 10);
	assert_int_equal(my_graph1.max_arcs, 100);
	assert_free_digraph(&my_graph1);

	thm_Digraph my_graph2 = thm_digraph_from_string("*.../.*../..*./...*/");
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
	(void) state;

	thm_Digraph my_graph1 = thm_empty_digraph(0, 0);
	assert_empty_digraph(&my_graph1, 0);
	assert_int_equal(my_graph1.max_arcs, 0);
	assert_free_digraph(&my_graph1);

	thm_Digraph my_graph2 = thm_empty_digraph(10, 100);
	assert_empty_digraph(&my_graph2, 10);
	assert_int_equal(my_graph2.max_arcs, 100);
	assert_free_digraph(&my_graph2);
}

void ut_thm_identity_digraph(void** state) {
	(void) state;

	thm_Digraph my_graph1 = thm_identity_digraph(0);
	assert_empty_digraph(&my_graph1, 0);
	assert_int_equal(my_graph1.max_arcs, 0);

	thm_Digraph my_graph2 = thm_identity_digraph(4);
	assert_sound_digraph(&my_graph2, 4);
	thm_Digraph control2 = thm_digraph_from_string("*.../.*../..*./...*/");
	assert_equal_digraph(&my_graph2, &control2);
	assert_identical_digraph(&my_graph2, &control2);

	thm_Digraph my_graph3 = thm_identity_digraph(100);
	assert_sound_digraph(&my_graph3, 100);

	assert_free_digraph(&my_graph1);
	assert_free_digraph(&my_graph2);
	assert_free_digraph(&my_graph3);
	assert_free_digraph(&control2);
}

void ut_thm_balanced_digraph(void** state) {
	(void) state;

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

void ut_thm_copy_digraph(void** state) {
	(void) state;

	thm_Digraph dg1 = thm_digraph_from_string("****/..*./****/*.../");
	thm_Digraph dg2 = thm_empty_digraph(0, 0);
	thm_Digraph dg3 = (thm_Digraph) { 0 };

	thm_Digraph res1 = thm_copy_digraph(&dg1);
	thm_Digraph res2 = thm_copy_digraph(&dg2);
	thm_Digraph res3 = thm_copy_digraph(&dg3);
	thm_Digraph res4 = thm_copy_digraph(NULL);

	assert_sound_digraph(&res1, 4);
	assert_sound_digraph(&res2, 0);

	assert_equal_digraph(&res1, &dg1);
	assert_equal_digraph(&res2, &dg2);
	assert_equal_digraph(&res3, &dg3);
	assert_equal_digraph(&res4, &dg3);

	assert_free_digraph(&dg1);
	assert_free_digraph(&dg2);
	assert_free_digraph(&res1);
	assert_free_digraph(&res2);
}

void ut_thm_union_digraphs(void** state) {
	(void) state;

	thm_Digraph ut_dg1 = thm_digraph_from_string("*.../.*../..*./...*/");
	thm_Digraph ut_dg2 = thm_digraph_from_string("...*/..*./.*../*.../");
	thm_Digraph ut_dg3 = thm_digraph_from_string("*.../*.../*.../*.../");


	const thm_Digraph* sum_12[2] = {&ut_dg1, &ut_dg2};
	thm_Digraph ut_make12_a = thm_digraph_union(2, sum_12);
	thm_Digraph ut_make12_t = thm_digraph_union_two(&ut_dg1, &ut_dg2);
	assert_sound_digraph(&ut_make12_a, 4);
	assert_sound_digraph(&ut_make12_t, 4);
	assert_equal_digraph(&ut_make12_a, &ut_make12_t);
	thm_Arcref tail_ptr12[5] = {0,2,4,6,8};
	thm_Vid head12[8] = {0,3,1,2,2,1,3,0};
	thm_Digraph ut_control12 = thm_digraph_from_pieces(4, 8, tail_ptr12, head12);
	assert_identical_digraph(&ut_control12, &ut_make12_a);
	assert_identical_digraph(&ut_control12, &ut_make12_t);


	const thm_Digraph* sum_13[2] = {&ut_dg1, &ut_dg3};
	thm_Digraph ut_make13_a = thm_digraph_union(2, sum_13);
	thm_Digraph ut_make13_t = thm_digraph_union_two(&ut_dg1, &ut_dg3);
	assert_sound_digraph(&ut_make13_a, 4);
	assert_sound_digraph(&ut_make13_t, 4);
	assert_equal_digraph(&ut_make13_a, &ut_make13_t);
	thm_Arcref tail_ptr13[5] = {0,1,3,5,7};
	thm_Vid head13[7] = {0,1,0,2,0,3,0};
	thm_Digraph ut_control13 = thm_digraph_from_pieces(4, 7, tail_ptr13, head13);
	assert_identical_digraph(&ut_control13, &ut_make13_a);
	assert_identical_digraph(&ut_control13, &ut_make13_t);


	const thm_Digraph* sum_31[2] = {&ut_dg3, &ut_dg1};
	thm_Digraph ut_make31_a = thm_digraph_union(2, sum_31);
	thm_Digraph ut_make31_t = thm_digraph_union_two(&ut_dg3, &ut_dg1);
	assert_sound_digraph(&ut_make31_a, 4);
	assert_sound_digraph(&ut_make31_t, 4);
	assert_equal_digraph(&ut_make31_a, &ut_make31_t);
	thm_Arcref tail_ptr31[5] = {0,1,3,5,7};
	thm_Vid head31[7] = {0,0,1,0,2,0,3};
	thm_Digraph ut_control31 = thm_digraph_from_pieces(4, 7, tail_ptr31, head31);
	assert_identical_digraph(&ut_control31, &ut_make31_a);
	assert_identical_digraph(&ut_control31, &ut_make31_t);

	assert_equal_digraph(&ut_make31_a, &ut_make13_a);


	const thm_Digraph* sum_123[3] = {&ut_dg1, &ut_dg2, &ut_dg3};
	thm_Digraph ut_make123 = thm_digraph_union(3, sum_123);
	assert_sound_digraph(&ut_make123, 4);
	thm_Arcref tail_ptr123[5] = {0,2,5,8,10};
	thm_Vid head123[10] = {0,3,1,2,0,2,1,0,3,0};
	thm_Digraph ut_control123 = thm_digraph_from_pieces(4, 10, tail_ptr123, head123);
	assert_identical_digraph(&ut_control123, &ut_make123);

	const thm_Digraph* sum_132[3] = {&ut_dg1, &ut_dg3, &ut_dg2};
	thm_Digraph ut_make132 = thm_digraph_union(3, sum_132);
	assert_sound_digraph(&ut_make132, 4);
	thm_Arcref tail_ptr132[5] = {0,2,5,8,10};
	thm_Vid head132[10] = {0,3,1,0,2,2,0,1,3,0};
	thm_Digraph ut_control132 = thm_digraph_from_pieces(4, 10, tail_ptr132, head132);
	assert_identical_digraph(&ut_control132, &ut_make132);

	const thm_Digraph* sum_213[3] = {&ut_dg2, &ut_dg1, &ut_dg3};
	thm_Digraph ut_make213 = thm_digraph_union(3, sum_213);
	assert_sound_digraph(&ut_make213, 4);
	thm_Arcref tail_ptr213[5] = {0,2,5,8,10};
	thm_Vid head213[10] = {3,0,2,1,0,1,2,0,0,3};
	thm_Digraph ut_control213 = thm_digraph_from_pieces(4, 10, tail_ptr213, head213);
	assert_identical_digraph(&ut_control213, &ut_make213);

	const thm_Digraph* sum_321[3] = {&ut_dg3, &ut_dg2, &ut_dg1};
	thm_Digraph ut_make321 = thm_digraph_union(3, sum_321);
	assert_sound_digraph(&ut_make321, 4);
	thm_Arcref tail_ptr321[5] = {0,2,5,8,10};
	thm_Vid head321[10] = {0,3,0,2,1,0,1,2,0,3};
	thm_Digraph ut_control321 = thm_digraph_from_pieces(4, 10, tail_ptr321, head321);
	assert_identical_digraph(&ut_control321, &ut_make321);

	assert_equal_digraph(&ut_make123, &ut_make132);
	assert_equal_digraph(&ut_make123, &ut_make213);
	assert_equal_digraph(&ut_make123, &ut_make321);


	assert_free_digraph(&ut_dg1);
	assert_free_digraph(&ut_dg2);
	assert_free_digraph(&ut_dg3);
	assert_free_digraph(&ut_make12_a);
	assert_free_digraph(&ut_make12_t);
	assert_free_digraph(&ut_make13_a);
	assert_free_digraph(&ut_make13_t);
	assert_free_digraph(&ut_make31_a);
	assert_free_digraph(&ut_make31_t);
	assert_free_digraph(&ut_make123);
	assert_free_digraph(&ut_make132);
	assert_free_digraph(&ut_make213);
	assert_free_digraph(&ut_make321);
	assert_free_digraph(&ut_control12);
	assert_free_digraph(&ut_control13);
	assert_free_digraph(&ut_control31);
	assert_free_digraph(&ut_control123);
	assert_free_digraph(&ut_control132);
	assert_free_digraph(&ut_control213);
	assert_free_digraph(&ut_control321);
}

void ut_thm_digraph_transpose(void** state) {
	(void) state;

	thm_Digraph ut_dg1 = thm_digraph_from_string("*.../.*../..*./...*/");
	thm_Digraph ut_dg2 = thm_digraph_from_string("****/..*./****/*.../");
	thm_Digraph ut_dg3 = thm_digraph_from_string("*.../*.../*.../*.../");
	thm_Digraph ut_dg4 = thm_digraph_from_string("..../..../..../..../");
	thm_Digraph ut_dg5 = thm_empty_digraph(0, 0);

	thm_Digraph control1 = thm_digraph_from_string("*.../.*../..*./...*/");
	thm_Digraph control2 = thm_digraph_from_string("*.**/*.*./***./*.*./");
	thm_Digraph control3 = thm_digraph_from_string("****/..../..../..../");
	thm_Digraph control4 = thm_empty_digraph(4, 0);
	thm_Digraph control5 = thm_empty_digraph(0, 0);
	thm_Digraph control6 = (thm_Digraph) { 0 };
	thm_Digraph control7 = (thm_Digraph) { 0 };

	thm_Digraph res1 = thm_digraph_transpose(&ut_dg1);
	thm_Digraph res2 = thm_digraph_transpose(&ut_dg2);
	thm_Digraph res3 = thm_digraph_transpose(&ut_dg3);
	thm_Digraph res4 = thm_digraph_transpose(&ut_dg4);
	thm_Digraph res5 = thm_digraph_transpose(&ut_dg5);
	thm_Digraph res6 = thm_digraph_transpose(NULL);
	thm_Digraph res7 = thm_digraph_transpose(&control7);

	assert_sound_digraph(&res1, 4);
	assert_sound_digraph(&res2, 4);
	assert_sound_digraph(&res3, 4);
	assert_sound_digraph(&res4, 4);
	assert_sound_digraph(&res5, 0);

	assert_equal_digraph(&res1, &control1);
	assert_equal_digraph(&res2, &control2);
	assert_equal_digraph(&res3, &control3);
	assert_equal_digraph(&res4, &control4);
	assert_equal_digraph(&res5, &control5);
	assert_equal_digraph(&res6, &control6);
	assert_equal_digraph(&res7, &control7);

	assert_free_digraph(&ut_dg1);
	assert_free_digraph(&ut_dg2);
	assert_free_digraph(&ut_dg3);
	assert_free_digraph(&ut_dg4);
	assert_free_digraph(&ut_dg5);
	assert_free_digraph(&control1);
	assert_free_digraph(&control2);
	assert_free_digraph(&control3);
	assert_free_digraph(&control4);
	assert_free_digraph(&control5);
	assert_free_digraph(&res1);
	assert_free_digraph(&res2);
	assert_free_digraph(&res3);
	assert_free_digraph(&res4);
	assert_free_digraph(&res5);
}


void ut_thm_digraph_adjacency_product(void** state) {
	(void) state;

	thm_Digraph dg1 = thm_digraph_from_string("**.../...*./.*.../..*../...*./");
	thm_Digraph dg1_i = thm_digraph_from_string(".*.../...*./.*.../..*../...*./");
	thm_Digraph dg1_f = thm_digraph_from_string("**.../.*.*./.**../..**./...**/");
	thm_Digraph dg2 = thm_digraph_from_string("..*../..**./...../..*../*.*../");
	thm_Digraph dg2_i = thm_digraph_from_string("..*../..**./...../..*../*.*../");
	thm_Digraph dg2_f = thm_digraph_from_string("*.*../.***./..*../..**./*.*.*/");

	thm_Digraph prod0 = thm_digraph_adjacency_product(&dg1, &dg1, true, true);
	thm_Digraph none = (thm_Digraph) { 0 };
	assert_identical_digraph(&prod0, &none);


	thm_Digraph prod1 = thm_digraph_adjacency_product(&dg1, &dg1, false, false);
	assert_sound_digraph(&prod1, 5);
	thm_Digraph control1 = thm_digraph_from_string("**.*./..*../...*./.*.../..*../");
	assert_equal_digraph(&prod1, &control1);

	thm_Digraph prod2 = thm_digraph_adjacency_product(&dg1, &dg1, false, true);
	assert_sound_digraph(&prod2, 5);
	thm_Digraph control2 = thm_digraph_from_string("...*./..*../...*./.*.../..*../");
	thm_Digraph control2alt = thm_digraph_adjacency_product(&dg1_i, &dg1, false, false);
	assert_equal_digraph(&prod2, &control2);
	assert_equal_digraph(&prod2, &control2alt);

	thm_Digraph prod3 = thm_digraph_adjacency_product(&dg1, &dg1, true, false);
	assert_sound_digraph(&prod3, 5);
	thm_Digraph control3 = thm_digraph_from_string("**.*./..**./.*.*./.**../..**./");
	thm_Digraph control3alt = thm_digraph_adjacency_product(&dg1_f, &dg1, false, false);
	assert_equal_digraph(&prod3, &control3);
	assert_equal_digraph(&prod3, &control3alt);


	thm_Digraph prod4 = thm_digraph_adjacency_product(&dg1, &prod2, false, false);
	assert_sound_digraph(&prod4, 5);
	thm_Digraph control4 = thm_digraph_from_string("..**./.*.../..*../...*./.*.../");
	assert_equal_digraph(&prod4, &control4);

	thm_Digraph prod5 = thm_digraph_adjacency_product(&dg1, &prod2, false, true);
	assert_sound_digraph(&prod5, 5);
	thm_Digraph control5 = thm_digraph_from_string("..*../.*.../..*../...*./.*.../");
	thm_Digraph control5alt = thm_digraph_adjacency_product(&dg1_i, &prod2, false, false);
	assert_equal_digraph(&prod5, &control5);
	assert_equal_digraph(&prod5, &control5alt);

	thm_Digraph prod6 = thm_digraph_adjacency_product(&dg1, &prod2, true, false);
	assert_sound_digraph(&prod6, 5);
	thm_Digraph control6 = thm_digraph_from_string("..**./.**../..**./.*.*./.**../");
	thm_Digraph control6alt = thm_digraph_adjacency_product(&dg1_f, &prod2, false, false);
	assert_equal_digraph(&prod6, &control6);
	assert_equal_digraph(&prod6, &control6alt);


	thm_Digraph prod7 = thm_digraph_adjacency_product(&dg1, &dg2, false, false);
	assert_sound_digraph(&prod7, 5);
	thm_Digraph control7 = thm_digraph_from_string("..**./..*../..**./...../..*../");
	assert_equal_digraph(&prod7, &control7);

	thm_Digraph prod8 = thm_digraph_adjacency_product(&dg1, &dg2, false, true);
	assert_sound_digraph(&prod8, 5);
	thm_Digraph control8 = thm_digraph_adjacency_product(&dg1_i, &dg2, false, false);
	assert_equal_digraph(&prod8, &control8);

	thm_Digraph prod9 = thm_digraph_adjacency_product(&dg1, &dg2, true, false);
	assert_sound_digraph(&prod9, 5);
	thm_Digraph control9 = thm_digraph_adjacency_product(&dg1_f, &dg2, false, false);
	assert_equal_digraph(&prod9, &control9);


	thm_Digraph prod10 = thm_digraph_adjacency_product(&dg2, &dg1, false, false);
	assert_sound_digraph(&prod10, 5);
	thm_Digraph control10 = thm_digraph_from_string(".*.../.**../...../.*.../**.../");
	assert_equal_digraph(&prod10, &control10);

	thm_Digraph prod11 = thm_digraph_adjacency_product(&dg2, &dg1, false, true);
	assert_sound_digraph(&prod11, 5);
	thm_Digraph control11 = thm_digraph_adjacency_product(&dg2_i, &dg1, false, false);
	assert_equal_digraph(&prod11, &control11);

	thm_Digraph prod12 = thm_digraph_adjacency_product(&dg2, &dg1, true, false);
	assert_sound_digraph(&prod12, 5);
	thm_Digraph control12 = thm_digraph_adjacency_product(&dg2_f, &dg1, false, false);
	assert_equal_digraph(&prod12, &control12);


	assert_free_digraph(&dg1);
	assert_free_digraph(&dg2);
	assert_free_digraph(&dg1_i);
	assert_free_digraph(&dg2_i);
	assert_free_digraph(&dg1_f);
	assert_free_digraph(&dg2_f);
	assert_free_digraph(&prod1);
	assert_free_digraph(&prod2);
	assert_free_digraph(&prod3);
	assert_free_digraph(&prod4);
	assert_free_digraph(&prod5);
	assert_free_digraph(&prod6);
	assert_free_digraph(&prod7);
	assert_free_digraph(&prod8);
	assert_free_digraph(&prod9);
	assert_free_digraph(&prod10);
	assert_free_digraph(&prod11);
	assert_free_digraph(&prod12);
	assert_free_digraph(&control1);
	assert_free_digraph(&control2);
	assert_free_digraph(&control3);
	assert_free_digraph(&control4);
	assert_free_digraph(&control5);
	assert_free_digraph(&control6);
	assert_free_digraph(&control2alt);
	assert_free_digraph(&control3alt);
	assert_free_digraph(&control5alt);
	assert_free_digraph(&control6alt);
	assert_free_digraph(&control7);
	assert_free_digraph(&control8);
	assert_free_digraph(&control9);
	assert_free_digraph(&control10);
	assert_free_digraph(&control11);
	assert_free_digraph(&control12);
}

int main(void) {
	const struct CMUnitTest test_digraph[] = {
	    cmocka_unit_test(ut_thm_free_digraph),
	    cmocka_unit_test(ut_thm_init_digraph),
	    cmocka_unit_test(ut_thm_change_digraph_head_size),
	    cmocka_unit_test(ut_thm_empty_digraph),
	    cmocka_unit_test(ut_thm_identity_digraph),
	    cmocka_unit_test(ut_thm_balanced_digraph),
	    cmocka_unit_test(ut_thm_copy_digraph),
	    cmocka_unit_test(ut_thm_union_digraphs),
	    cmocka_unit_test(ut_thm_digraph_transpose),
	    cmocka_unit_test(ut_thm_digraph_adjacency_product),
	};
    return cmocka_run_group_tests_name("digraph.c", test_digraph, NULL, NULL);
}
