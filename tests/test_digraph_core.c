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
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * ============================================================================== */


#include "test_suite.h"
#include "assert_digraph.h"

#include <stddef.h>
#include "../src/digraph_core.h"
#include "../src/digraph_debug.h"


void scc_ut_init_digraph(void** state)
{
	(void) state;

	iscc_Digraph my_graph1 = iscc_init_digraph(0, 0);
	assert_initialized_digraph(&my_graph1, 0);
	assert_int_equal(my_graph1.max_arcs, 0);
	assert_free_digraph(&my_graph1);

	iscc_Digraph my_graph2 = iscc_init_digraph(10, 100);
	assert_initialized_digraph(&my_graph2, 10);
	assert_int_equal(my_graph2.max_arcs, 100);
	assert_free_digraph(&my_graph2);
}


void scc_ut_digraph_is_initialized(void** state)
{
	(void) state;

	iscc_Digraph dg1 = iscc_init_digraph(4, 10);
	iscc_Digraph dg2 = ISCC_NULL_DIGRAPH;
	iscc_Digraph dg3 = iscc_init_digraph(4, 0);

	assert_false(iscc_digraph_is_initialized(NULL));
	assert_true(iscc_digraph_is_initialized(&dg1));
	assert_false(iscc_digraph_is_initialized(&dg2));
	assert_true(iscc_digraph_is_initialized(&dg3));

	dg3.head = dg1.head;
	dg1.head = NULL;

	assert_false(iscc_digraph_is_initialized(&dg1));
	assert_false(iscc_digraph_is_initialized(&dg3));

	iscc_free_digraph(&dg1);
	iscc_free_digraph(&dg2);
	iscc_free_digraph(&dg3);
}


void scc_ut_free_digraph(void** state)
{
	(void) state;

	iscc_Digraph null_graph = ISCC_NULL_DIGRAPH;

	iscc_Digraph my_graph1 = iscc_empty_digraph(10, 10);
	iscc_free_digraph(&my_graph1);
	assert_memory_equal(&my_graph1, &null_graph, sizeof(iscc_Digraph));

	iscc_Digraph my_graph2 = iscc_empty_digraph(10, 10);
	free(my_graph2.head);
	my_graph2.head = NULL;
	iscc_free_digraph(&my_graph2);
	assert_memory_equal(&my_graph2, &null_graph, sizeof(iscc_Digraph));

	iscc_Digraph my_graph3 = ISCC_NULL_DIGRAPH;
	iscc_free_digraph(&my_graph3);
	assert_memory_equal(&my_graph3, &null_graph, sizeof(iscc_Digraph));

	iscc_free_digraph(NULL);
}


void scc_ut_change_arc_storage(void** state)
{
	(void) state;

	iscc_Digraph my_graph1 = iscc_empty_digraph(10, 100);
	assert_true(iscc_change_arc_storage(&my_graph1, 100));
	assert_empty_digraph(&my_graph1, 10);
	assert_int_equal(my_graph1.max_arcs, 100);
	assert_free_digraph(&my_graph1);

	iscc_Digraph my_graph2 = iscc_digraph_from_string("#.../.#../..#./...#/");
	assert_false(iscc_change_arc_storage(&my_graph2, 2));
	assert_valid_digraph(&my_graph2, 4);
	assert_int_equal(my_graph2.max_arcs, 4);
	assert_free_digraph(&my_graph2);

	iscc_Digraph my_graph3 = iscc_empty_digraph(10, 100);
	assert_true(iscc_change_arc_storage(&my_graph3, 50));
	assert_empty_digraph(&my_graph3, 10);
	assert_int_equal(my_graph3.max_arcs, 50);
	assert_free_digraph(&my_graph3);

	iscc_Digraph my_graph4 = iscc_empty_digraph(10, 100);
	assert_true(iscc_change_arc_storage(&my_graph4, 200));
	assert_empty_digraph(&my_graph4, 10);
	assert_int_equal(my_graph4.max_arcs, 200);
	assert_free_digraph(&my_graph4);

	iscc_Digraph my_graph5 = iscc_empty_digraph(0, 100);
	assert_true(iscc_change_arc_storage(&my_graph5, 0));
	assert_empty_digraph(&my_graph5, 0);
	assert_int_equal(my_graph5.max_arcs, 0);
	assert_free_digraph(&my_graph5);
}


void scc_ut_empty_digraph(void** state)
{
	(void) state;

	iscc_Digraph my_graph1 = iscc_empty_digraph(0, 0);
	assert_empty_digraph(&my_graph1, 0);
	assert_int_equal(my_graph1.max_arcs, 0);
	assert_free_digraph(&my_graph1);

	iscc_Digraph my_graph2 = iscc_empty_digraph(10, 100);
	assert_empty_digraph(&my_graph2, 10);
	assert_int_equal(my_graph2.max_arcs, 100);
	assert_free_digraph(&my_graph2);
}


void scc_ut_copy_digraph(void** state)
{
	(void) state;

	iscc_Digraph dg1 = iscc_digraph_from_string("####/..#./####/#.../");
	iscc_Digraph dg2 = iscc_empty_digraph(0, 0);

	iscc_Digraph res1 = iscc_copy_digraph(&dg1);
	iscc_Digraph res2 = iscc_copy_digraph(&dg2);

	assert_valid_digraph(&res1, 4);
	assert_valid_digraph(&res2, 0);

	assert_equal_digraph(&res1, &dg1);
	assert_equal_digraph(&res2, &dg2);

	assert_free_digraph(&dg1);
	assert_free_digraph(&dg2);
	assert_free_digraph(&res1);
	assert_free_digraph(&res2);
}


void scc_ut_delete_arcs_by_tails(void** state)
{
	(void) state;

	iscc_Digraph dg1 = iscc_digraph_from_string("####/..../####/..../");

	bool to_delete1[4] = { false, false, false, false };
	bool to_delete2[4] = { true, true, true, true };
	bool to_delete3[4] = { true, false, true, false };
	bool to_delete4[4] = { false, false, true, true };

	iscc_Digraph temp_dg;
	iscc_Digraph ref;

	temp_dg = iscc_copy_digraph(&dg1);
	iscc_delete_arcs_by_tails(&temp_dg, to_delete1);
	assert_equal_digraph(&temp_dg, &dg1);
	assert_free_digraph(&temp_dg);

	ref = iscc_empty_digraph(4, 0);
	temp_dg = iscc_copy_digraph(&dg1);
	iscc_delete_arcs_by_tails(&temp_dg, to_delete2);
	assert_equal_digraph(&temp_dg, &ref);
	assert_free_digraph(&temp_dg);
	assert_free_digraph(&ref);
	
	ref = iscc_empty_digraph(4, 0);
	temp_dg = iscc_copy_digraph(&dg1);
	iscc_delete_arcs_by_tails(&temp_dg, to_delete3);
	assert_equal_digraph(&temp_dg, &ref);
	assert_free_digraph(&temp_dg);
	assert_free_digraph(&ref);
	
	ref = iscc_digraph_from_string("####/..../..../..../");
	temp_dg = iscc_copy_digraph(&dg1);
	iscc_delete_arcs_by_tails(&temp_dg, to_delete4);
	assert_equal_digraph(&temp_dg, &ref);
	assert_free_digraph(&temp_dg);
	assert_free_digraph(&ref);


	iscc_Digraph dg2 = iscc_digraph_from_string("##../.#.#/..##/#.#./");

	temp_dg = iscc_copy_digraph(&dg2);
	iscc_delete_arcs_by_tails(&temp_dg, to_delete1);
	assert_equal_digraph(&temp_dg, &dg2);
	assert_free_digraph(&temp_dg);

	ref = iscc_empty_digraph(4, 0);
	temp_dg = iscc_copy_digraph(&dg2);
	iscc_delete_arcs_by_tails(&temp_dg, to_delete2);
	assert_equal_digraph(&temp_dg, &ref);
	assert_free_digraph(&temp_dg);
	assert_free_digraph(&ref);
	
	ref = iscc_digraph_from_string("..../.#.#/..../#.#./");
	temp_dg = iscc_copy_digraph(&dg2);
	iscc_delete_arcs_by_tails(&temp_dg, to_delete3);
	assert_equal_digraph(&temp_dg, &ref);
	assert_free_digraph(&temp_dg);
	assert_free_digraph(&ref);
	
	ref = iscc_digraph_from_string("##../.#.#/..../..../");
	temp_dg = iscc_copy_digraph(&dg2);
	iscc_delete_arcs_by_tails(&temp_dg, to_delete4);
	assert_equal_digraph(&temp_dg, &ref);
	assert_free_digraph(&temp_dg);
	assert_free_digraph(&ref);

	assert_free_digraph(&dg1);
	assert_free_digraph(&dg2);
}


void scc_ut_delete_loops(void** state)
{
	(void) state;

	iscc_Digraph dg1 = iscc_digraph_from_string("#####/#####/#####/#####/#####/");
	iscc_Digraph dg2 = iscc_digraph_from_string("...../...../...../...../...../");
	iscc_Digraph dg3 = iscc_digraph_from_string(".####/#.###/##.##/###.#/####./");
	iscc_Digraph dg4 = iscc_digraph_from_string("#..../.#.../..#../...#./....#/");
	iscc_Digraph dg5 = iscc_digraph_from_string("#.#../#..../#.#../##.../##..#/");

	iscc_delete_loops(&dg1);
	iscc_delete_loops(&dg2);
	iscc_delete_loops(&dg3);
	iscc_delete_loops(&dg4);
	iscc_delete_loops(&dg5);

	iscc_Digraph ref1 = iscc_digraph_from_string(".####/#.###/##.##/###.#/####./");
	iscc_Digraph ref2 = iscc_digraph_from_string("...../...../...../...../...../");
	iscc_Digraph ref3 = iscc_digraph_from_string(".####/#.###/##.##/###.#/####./");
	iscc_Digraph ref4 = iscc_digraph_from_string("...../...../...../...../...../");
	iscc_Digraph ref5 = iscc_digraph_from_string("..#../#..../#..../##.../##.../");

	assert_free_digraph(&dg1);
	assert_free_digraph(&dg2);
	assert_free_digraph(&dg3);
	assert_free_digraph(&dg4);
	assert_free_digraph(&dg5);
	assert_free_digraph(&ref1);
	assert_free_digraph(&ref2);
	assert_free_digraph(&ref3);
	assert_free_digraph(&ref4);
	assert_free_digraph(&ref5);
}


int main(void)
{
	const struct CMUnitTest test_core[] = {
		cmocka_unit_test(scc_ut_init_digraph),
		cmocka_unit_test(scc_ut_digraph_is_initialized),
		cmocka_unit_test(scc_ut_free_digraph),
		cmocka_unit_test(scc_ut_change_arc_storage),
		cmocka_unit_test(scc_ut_empty_digraph),
		cmocka_unit_test(scc_ut_copy_digraph),
		cmocka_unit_test(scc_ut_delete_arcs_by_tails),
		cmocka_unit_test(scc_ut_delete_loops),
	};
	
	return cmocka_run_group_tests_name("core module", test_core, NULL, NULL);
}
