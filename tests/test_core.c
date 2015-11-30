#include "test_suite.h"
#include "assert_digraph.h"

#include <stddef.h>

#include "../digraph.h"
#include "../core.h"
#include "../debug.h"


void ut_tbg_init_digraph(void** state) {
	(void) state;

	tbg_Digraph my_graph1 = tbg_init_digraph(0, 0);
	assert_valid_digraph(&my_graph1, 0);
	assert_int_equal(my_graph1.max_arcs, 0);
	assert_free_digraph(&my_graph1);

	tbg_Digraph my_graph2 = tbg_init_digraph(10, 100);
	assert_valid_digraph(&my_graph2, 10);
	assert_int_equal(my_graph2.max_arcs, 100);
	assert_free_digraph(&my_graph2);
}

void ut_tbg_free_digraph(void** state) {
	(void) state;

	tbg_Digraph null_graph = tbg_null_digraph();

	tbg_Digraph my_graph1 = tbg_empty_digraph(10, 10);
	tbg_free_digraph(&my_graph1);
	assert_memory_equal(&my_graph1, &null_graph, sizeof(tbg_Digraph));

	tbg_Digraph my_graph2 = tbg_empty_digraph(10, 10);
	free(my_graph2.head);
	my_graph2.head = NULL;
	tbg_free_digraph(&my_graph2);
	assert_memory_equal(&my_graph2, &null_graph, sizeof(tbg_Digraph));

	tbg_Digraph my_graph3 = tbg_null_digraph();
	tbg_free_digraph(&my_graph3);
	assert_memory_equal(&my_graph3, &null_graph, sizeof(tbg_Digraph));

	tbg_free_digraph(NULL);
}

void ut_tbg_change_arc_storage(void** state) {
	(void) state;

	tbg_Digraph my_graph1 = tbg_empty_digraph(10, 100);
	assert_true(tbg_change_arc_storage(&my_graph1, 100));
	assert_empty_digraph(&my_graph1, 10);
	assert_int_equal(my_graph1.max_arcs, 100);
	assert_free_digraph(&my_graph1);

	tbg_Digraph my_graph2 = tbg_digraph_from_string("*.../.*../..*./...*/");
	assert_false(tbg_change_arc_storage(&my_graph2, 2));
	assert_sound_digraph(&my_graph2, 4);
	assert_int_equal(my_graph2.max_arcs, 4);
	assert_free_digraph(&my_graph2);

	tbg_Digraph my_graph3 = tbg_empty_digraph(10, 100);
	assert_true(tbg_change_arc_storage(&my_graph3, 50));
	assert_empty_digraph(&my_graph3, 10);
	assert_int_equal(my_graph3.max_arcs, 50);
	assert_free_digraph(&my_graph3);

	tbg_Digraph my_graph4 = tbg_empty_digraph(10, 100);
	assert_true(tbg_change_arc_storage(&my_graph4, 200));
	assert_empty_digraph(&my_graph4, 10);
	assert_int_equal(my_graph4.max_arcs, 200);
	assert_free_digraph(&my_graph4);

	tbg_Digraph my_graph5 = tbg_empty_digraph(0, 100);
	assert_true(tbg_change_arc_storage(&my_graph5, 0));
	assert_empty_digraph(&my_graph5, 0);
	assert_int_equal(my_graph5.max_arcs, 0);
	assert_free_digraph(&my_graph5);
}

void ut_tbg_empty_digraph(void** state) {
	(void) state;

	tbg_Digraph my_graph1 = tbg_empty_digraph(0, 0);
	assert_empty_digraph(&my_graph1, 0);
	assert_int_equal(my_graph1.max_arcs, 0);
	assert_free_digraph(&my_graph1);

	tbg_Digraph my_graph2 = tbg_empty_digraph(10, 100);
	assert_empty_digraph(&my_graph2, 10);
	assert_int_equal(my_graph2.max_arcs, 100);
	assert_free_digraph(&my_graph2);
}

void ut_tbg_identity_digraph(void** state) {
	(void) state;

	tbg_Digraph my_graph1 = tbg_identity_digraph(0);
	assert_empty_digraph(&my_graph1, 0);
	assert_int_equal(my_graph1.max_arcs, 0);

	tbg_Digraph my_graph2 = tbg_identity_digraph(4);
	assert_sound_digraph(&my_graph2, 4);
	tbg_Digraph control2 = tbg_digraph_from_string("*.../.*../..*./...*/");
	assert_equal_digraph(&my_graph2, &control2);
	assert_identical_digraph(&my_graph2, &control2);

	tbg_Digraph my_graph3 = tbg_identity_digraph(100);
	assert_sound_digraph(&my_graph3, 100);

	assert_free_digraph(&my_graph1);
	assert_free_digraph(&my_graph2);
	assert_free_digraph(&my_graph3);
	assert_free_digraph(&control2);
}

void ut_tbg_balanced_digraph(void** state) {
	(void) state;

	tbg_Vid* heads1 = NULL;
	tbg_Digraph my_graph1 = tbg_balanced_digraph(0, 0, heads1);
	assert_balanced_digraph(&my_graph1, 0, 0);
	assert_int_equal(my_graph1.max_arcs, 0);
	assert_free_digraph(&my_graph1);

	tbg_Vid* heads2 = malloc(sizeof(tbg_Vid[10 * 4]));
	for (size_t i = 0; i < 40; ++i) heads2[i] = i % 10;
	tbg_Digraph my_graph2 = tbg_balanced_digraph(10, 4, heads2);
	assert_balanced_digraph(&my_graph2, 10, 4);
	assert_int_equal(my_graph2.max_arcs, 40);
	for (size_t i = 0; i < 40; ++i) assert_int_equal(my_graph2.head[i], i % 10);
	assert_free_digraph(&my_graph2);
}

void ut_tbg_copy_digraph(void** state) {
	(void) state;

	tbg_Digraph dg1 = tbg_digraph_from_string("****/..*./****/*.../");
	tbg_Digraph dg2 = tbg_empty_digraph(0, 0);
	tbg_Digraph dg3 = tbg_null_digraph();

	tbg_Digraph res1 = tbg_copy_digraph(&dg1);
	tbg_Digraph res2 = tbg_copy_digraph(&dg2);
	tbg_Digraph res3 = tbg_copy_digraph(&dg3);
	tbg_Digraph res4 = tbg_copy_digraph(NULL);

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


int main(void) {
	const struct CMUnitTest test_core[] = {
	    cmocka_unit_test(ut_tbg_init_digraph),
	    cmocka_unit_test(ut_tbg_free_digraph),
	    cmocka_unit_test(ut_tbg_change_arc_storage),
	    cmocka_unit_test(ut_tbg_empty_digraph),
	    cmocka_unit_test(ut_tbg_identity_digraph),
	    cmocka_unit_test(ut_tbg_balanced_digraph),
	    cmocka_unit_test(ut_tbg_copy_digraph),
	};
	
    return cmocka_run_group_tests_name("core module", test_core, NULL, NULL);
}