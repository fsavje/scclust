#ifndef UT_THM_ASSERT_DIGRAPH_HG
#define UT_THM_ASSERT_DIGRAPH_HG

#include "../digraph.h"

void assert_valid_digraph(thm_Digraph* test_dg, thm_Vid vertices) {
	assert_true(test_dg->valid);
	assert_int_equal(test_dg->vertices, vertices);
	assert_non_null(test_dg->tail_ptr);
	if (test_dg->max_arcs == 0) assert_null(test_dg->head);
	if (test_dg->max_arcs > 0) assert_non_null(test_dg->head);
}

void assert_free_digraph(thm_Digraph* test_dg) {
	for (size_t i = 0; i <= test_dg->vertices; ++i) test_dg->tail_ptr[i] = 1;
	for (size_t i = 0; i < test_dg->max_arcs; ++i) test_dg->head[i] = 1;
	thm_free_digraph(test_dg);
}

void assert_sound_digraph(thm_Digraph* test_dg, thm_Vid vertices) {
	assert_valid_digraph(test_dg, vertices);
	assert_int_equal(test_dg->tail_ptr[0], 0);
	assert_true(test_dg->tail_ptr[vertices] <= test_dg->max_arcs);
	for (size_t i = 0; i < vertices; ++i) assert_true(test_dg->tail_ptr[i] <= test_dg->tail_ptr[i + 1]);
	for (size_t i = 0; i < test_dg->tail_ptr[vertices]; ++i) assert_true(test_dg->head[i] < vertices);
}

void assert_identical_digraph(thm_Digraph* test_dg1, thm_Digraph* test_dg2) {
	assert_true(test_dg1->valid == test_dg2->valid);
	assert_true(test_dg1->max_arcs == test_dg2->max_arcs);
	assert_true(test_dg1->vertices == test_dg2->vertices);

	if (!test_dg1->tail_ptr) {
		assert_null(test_dg2->tail_ptr);
	} else {
		assert_non_null(test_dg2->tail_ptr);
		assert_memory_equal(test_dg1->tail_ptr, test_dg2->tail_ptr, (test_dg1->vertices + 1) * sizeof(thm_Arcref));
	}

	if (!test_dg1->head) {
		assert_null(test_dg2->head);
	} else {
		assert_non_null(test_dg2->head);
		assert_memory_equal(test_dg1->head, test_dg2->head, test_dg1->max_arcs * sizeof(thm_Arcref));
	}
}

void assert_empty_digraph(thm_Digraph* test_dg, thm_Vid vertices) {
	assert_sound_digraph(test_dg, vertices);
	assert_int_equal(test_dg->tail_ptr[vertices], 0);
}

void assert_balanced_digraph(thm_Digraph* test_dg, thm_Vid vertices, thm_Vid arcs_per_vertex) {
	assert_sound_digraph(test_dg, vertices);
	assert_true(vertices * arcs_per_vertex <= test_dg->max_arcs);
	for (size_t i = 0; i <= vertices; ++i) {
		assert_int_equal(test_dg->tail_ptr[i], i * arcs_per_vertex);
	}
}

#endif
