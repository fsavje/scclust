#ifndef UT_THM_ASSERT_DIGRAPH_HG
#define UT_THM_ASSERT_DIGRAPH_HG

void assert_object_digraph(thm_Digraph* test_dg, thm_Vid vertices) {
	assert_int_equal(test_dg->vertices, vertices);
	assert_non_null(test_dg->tail_ptr);
	if (test_dg->max_arcs > 0) assert_non_null(test_dg->head);
}

void assert_mem_digraph(thm_Digraph* test_dg) {
	for (size_t i = 0; i <= test_dg->vertices; ++i) test_dg->tail_ptr[i] = 1;
	for (size_t i = 0; i < test_dg->max_arcs; ++i) test_dg->head[i] = 1;
}

void assert_valid_digraph(thm_Digraph* test_dg, thm_Vid vertices) {
	assert_object_digraph(test_dg, vertices);
	assert_int_equal(test_dg->tail_ptr[0], 0);
	assert_true(test_dg->tail_ptr[vertices] <= test_dg->max_arcs);
	for (size_t i = 0; i < vertices; ++i) assert_true(test_dg->tail_ptr[i] <= test_dg->tail_ptr[i + 1]);
	for (size_t i = 0; i < test_dg->tail_ptr[vertices]; ++i) assert_true(test_dg->head[i] < vertices);
}

void assert_empty_digraph(thm_Digraph* test_dg, thm_Vid vertices) {
	assert_valid_digraph(test_dg, vertices);
	assert_int_equal(test_dg->tail_ptr[vertices], 0);
}

void assert_balanced_digraph(thm_Digraph* test_dg, thm_Vid vertices, thm_Vid arcs_per_vertex) {
	assert_valid_digraph(test_dg, vertices);
	assert_true(vertices * arcs_per_vertex <= test_dg->max_arcs);
	for (size_t i = 0; i <= vertices; ++i) {
		assert_int_equal(test_dg->tail_ptr[i], i * arcs_per_vertex);
	}
}

#endif
