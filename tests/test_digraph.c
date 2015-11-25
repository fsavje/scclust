#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "malloc_override.h"

#include "../digraph.c"

#include "assert_digraph.h"


void ut_thm_Digraph(void** state) {
	size_t digraph_size = sizeof(thm_Vid) + sizeof(thm_Arcref*) + sizeof(thm_Arcref) + sizeof(thm_Vid*);
	assert_int_equal(digraph_size, sizeof(thm_Digraph));
	thm_Digraph my_graph = {
		.vertices = 0,
		.tail_ptr = NULL,
		.max_arcs = 0,
		.head = NULL,
	};

	(void) my_graph;
}

void ut_thm_init_digraph(void** state) {
	thm_Digraph my_graph1 = thm_init_digraph(0);
	assert_object_digraph(&my_graph1, 0);
	assert_int_equal(my_graph1.max_arcs, 0);
	assert_mem_digraph(&my_graph1);
	thm_free_digraph(&my_graph1);

	thm_Digraph my_graph2 = thm_init_digraph(10);
	assert_object_digraph(&my_graph2, 10);
	assert_int_equal(my_graph2.max_arcs, 0);
	assert_mem_digraph(&my_graph2);
	thm_free_digraph(&my_graph2);
}

void ut_thm_empty_digraph(void** state) {
	thm_Digraph my_graph1 = thm_empty_digraph(0, 0);
	assert_empty_digraph(&my_graph1, 0);
	assert_int_equal(my_graph1.max_arcs, 0);
	assert_mem_digraph(&my_graph1);
	thm_free_digraph(&my_graph1);

	thm_Digraph my_graph2 = thm_empty_digraph(10, 100);
	assert_empty_digraph(&my_graph2, 10);
	assert_int_equal(my_graph2.max_arcs, 100);
	assert_mem_digraph(&my_graph2);
	thm_free_digraph(&my_graph2);
}

void ut_thm_balanced_digraph(void** state) {
	thm_Vid* heads1 = NULL;
	thm_Digraph my_graph1 = thm_balanced_digraph(0, 0, heads1);
	assert_balanced_digraph(&my_graph1, 0, 0);
	assert_int_equal(my_graph1.max_arcs, 0);
	assert_mem_digraph(&my_graph1);
	thm_free_digraph(&my_graph1);

	thm_Vid* heads2 = malloc(sizeof(thm_Vid[10 * 4]));
	for (size_t i = 0; i < 40; ++i) heads2[i] = i % 10;
	thm_Digraph my_graph2 = thm_balanced_digraph(10, 4, heads2);
	assert_balanced_digraph(&my_graph2, 10, 4);
	assert_int_equal(my_graph2.max_arcs, 40);
	for (size_t i = 0; i < 40; ++i) assert_int_equal(my_graph2.head[i], i % 10);
	assert_mem_digraph(&my_graph2);
	thm_free_digraph(&my_graph2);
}

void ut_thm_free_digraph(void** state) {
	thm_Digraph null_graph = (thm_Digraph) { 0 };

	thm_free_digraph(NULL);

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
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(ut_thm_Digraph),
        cmocka_unit_test(ut_thm_init_digraph),
        cmocka_unit_test(ut_thm_empty_digraph),
        cmocka_unit_test(ut_thm_balanced_digraph),
        cmocka_unit_test(ut_thm_free_digraph),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
