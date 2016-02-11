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

#include "../include/config.h"
#include "../src/digraph_core.h"
#include "../src/digraph_debug.h"


void scc_ut_init_digraph(void** state)
{
	(void) state;

	scc_Digraph my_graph1 = scc_init_digraph(0, 0);
	assert_initialized_digraph(&my_graph1, 0);
	assert_int_equal(my_graph1.max_arcs, 0);
	assert_free_digraph(&my_graph1);

	scc_Digraph my_graph2 = scc_init_digraph(10, 100);
	assert_initialized_digraph(&my_graph2, 10);
	assert_int_equal(my_graph2.max_arcs, 100);
	assert_free_digraph(&my_graph2);
}


void scc_ut_digraph_is_initialized(void** state)
{
	(void) state;

	scc_Digraph dg1 = scc_init_digraph(4, 10);
	scc_Digraph dg2 = SCC_NULL_DIGRAPH;
	scc_Digraph dg3 = scc_init_digraph(4, 0);

	assert_false(scc_digraph_is_initialized(NULL));
	assert_true(scc_digraph_is_initialized(&dg1));
	assert_false(scc_digraph_is_initialized(&dg2));
	assert_true(scc_digraph_is_initialized(&dg3));

	dg3.head = dg1.head;
	dg1.head = NULL;

	assert_false(scc_digraph_is_initialized(&dg1));
	assert_false(scc_digraph_is_initialized(&dg3));

	scc_free_digraph(&dg1);
	scc_free_digraph(&dg2);
	scc_free_digraph(&dg3);
}


void scc_ut_free_digraph(void** state)
{
	(void) state;

	scc_Digraph null_graph = SCC_NULL_DIGRAPH;

	scc_Digraph my_graph1 = scc_empty_digraph(10, 10);
	scc_free_digraph(&my_graph1);
	assert_memory_equal(&my_graph1, &null_graph, sizeof(scc_Digraph));

	scc_Digraph my_graph2 = scc_empty_digraph(10, 10);
	free(my_graph2.head);
	my_graph2.head = NULL;
	scc_free_digraph(&my_graph2);
	assert_memory_equal(&my_graph2, &null_graph, sizeof(scc_Digraph));

	scc_Digraph my_graph3 = SCC_NULL_DIGRAPH;
	scc_free_digraph(&my_graph3);
	assert_memory_equal(&my_graph3, &null_graph, sizeof(scc_Digraph));

	scc_free_digraph(NULL);
}


void scc_ut_change_arc_storage(void** state)
{
	(void) state;

	scc_Digraph my_graph1 = scc_empty_digraph(10, 100);
	assert_true(scc_change_arc_storage(&my_graph1, 100));
	assert_empty_digraph(&my_graph1, 10);
	assert_int_equal(my_graph1.max_arcs, 100);
	assert_free_digraph(&my_graph1);

	scc_Digraph my_graph2 = scc_digraph_from_string("#.../.#../..#./...#/");
	assert_false(scc_change_arc_storage(&my_graph2, 2));
	assert_valid_digraph(&my_graph2, 4);
	assert_int_equal(my_graph2.max_arcs, 4);
	assert_free_digraph(&my_graph2);

	scc_Digraph my_graph3 = scc_empty_digraph(10, 100);
	assert_true(scc_change_arc_storage(&my_graph3, 50));
	assert_empty_digraph(&my_graph3, 10);
	assert_int_equal(my_graph3.max_arcs, 50);
	assert_free_digraph(&my_graph3);

	scc_Digraph my_graph4 = scc_empty_digraph(10, 100);
	assert_true(scc_change_arc_storage(&my_graph4, 200));
	assert_empty_digraph(&my_graph4, 10);
	assert_int_equal(my_graph4.max_arcs, 200);
	assert_free_digraph(&my_graph4);

	scc_Digraph my_graph5 = scc_empty_digraph(0, 100);
	assert_true(scc_change_arc_storage(&my_graph5, 0));
	assert_empty_digraph(&my_graph5, 0);
	assert_int_equal(my_graph5.max_arcs, 0);
	assert_free_digraph(&my_graph5);
}


void scc_ut_empty_digraph(void** state)
{
	(void) state;

	scc_Digraph my_graph1 = scc_empty_digraph(0, 0);
	assert_empty_digraph(&my_graph1, 0);
	assert_int_equal(my_graph1.max_arcs, 0);
	assert_free_digraph(&my_graph1);

	scc_Digraph my_graph2 = scc_empty_digraph(10, 100);
	assert_empty_digraph(&my_graph2, 10);
	assert_int_equal(my_graph2.max_arcs, 100);
	assert_free_digraph(&my_graph2);
}


void scc_ut_copy_digraph(void** state)
{
	(void) state;

	scc_Digraph dg1 = scc_digraph_from_string("####/..#./####/#.../");
	scc_Digraph dg2 = scc_empty_digraph(0, 0);
	scc_Digraph dg3 = SCC_NULL_DIGRAPH;

	scc_Digraph res1 = scc_copy_digraph(&dg1);
	scc_Digraph res2 = scc_copy_digraph(&dg2);
	scc_Digraph res3 = scc_copy_digraph(&dg3);
	scc_Digraph res4 = scc_copy_digraph(NULL);

	assert_valid_digraph(&res1, 4);
	assert_valid_digraph(&res2, 0);

	assert_equal_digraph(&res1, &dg1);
	assert_equal_digraph(&res2, &dg2);
	assert_equal_digraph(&res3, &dg3);
	assert_equal_digraph(&res4, &dg3);

	assert_free_digraph(&dg1);
	assert_free_digraph(&dg2);
	assert_free_digraph(&res1);
	assert_free_digraph(&res2);
}


void scc_ut_delete_arcs_by_tails(void** state)
{
	(void) state;

	scc_Digraph dg1 = scc_digraph_from_string("####/..../####/..../");

	bool to_delete1[4] = { false, false, false, false };
	bool to_delete2[4] = { true, true, true, true };
	bool to_delete3[4] = { true, false, true, false };
	bool to_delete4[4] = { false, false, true, true };

	scc_Digraph temp_dg;
	scc_Digraph ref;

	temp_dg = scc_copy_digraph(&dg1);
	assert_true(scc_delete_arcs_by_tails(&temp_dg, to_delete1));
	assert_equal_digraph(&temp_dg, &dg1);
	assert_free_digraph(&temp_dg);

	ref = scc_empty_digraph(4, 0);
	temp_dg = scc_copy_digraph(&dg1);
	assert_true(scc_delete_arcs_by_tails(&temp_dg, to_delete2));
	assert_equal_digraph(&temp_dg, &ref);
	assert_free_digraph(&temp_dg);
	assert_free_digraph(&ref);
	
	ref = scc_empty_digraph(4, 0);
	temp_dg = scc_copy_digraph(&dg1);
	assert_true(scc_delete_arcs_by_tails(&temp_dg, to_delete3));
	assert_equal_digraph(&temp_dg, &ref);
	assert_free_digraph(&temp_dg);
	assert_free_digraph(&ref);
	
	ref = scc_digraph_from_string("####/..../..../..../");
	temp_dg = scc_copy_digraph(&dg1);
	assert_true(scc_delete_arcs_by_tails(&temp_dg, to_delete4));
	assert_equal_digraph(&temp_dg, &ref);
	assert_free_digraph(&temp_dg);
	assert_free_digraph(&ref);


	scc_Digraph dg2 = scc_digraph_from_string("##../.#.#/..##/#.#./");

	temp_dg = scc_copy_digraph(&dg2);
	assert_true(scc_delete_arcs_by_tails(&temp_dg, to_delete1));
	assert_equal_digraph(&temp_dg, &dg2);
	assert_free_digraph(&temp_dg);

	ref = scc_empty_digraph(4, 0);
	temp_dg = scc_copy_digraph(&dg2);
	assert_true(scc_delete_arcs_by_tails(&temp_dg, to_delete2));
	assert_equal_digraph(&temp_dg, &ref);
	assert_free_digraph(&temp_dg);
	assert_free_digraph(&ref);
	
	ref = scc_digraph_from_string("..../.#.#/..../#.#./");
	temp_dg = scc_copy_digraph(&dg2);
	assert_true(scc_delete_arcs_by_tails(&temp_dg, to_delete3));
	assert_equal_digraph(&temp_dg, &ref);
	assert_free_digraph(&temp_dg);
	assert_free_digraph(&ref);
	
	ref = scc_digraph_from_string("##../.#.#/..../..../");
	temp_dg = scc_copy_digraph(&dg2);
	assert_true(scc_delete_arcs_by_tails(&temp_dg, to_delete4));
	assert_equal_digraph(&temp_dg, &ref);
	assert_free_digraph(&temp_dg);
	assert_free_digraph(&ref);

	assert_free_digraph(&dg1);
	assert_free_digraph(&dg2);
}


void scc_ut_delete_loops(void** state)
{
	(void) state;

	scc_Digraph dg1 = scc_digraph_from_string("#####/#####/#####/#####/#####/");
	scc_Digraph dg2 = scc_digraph_from_string("...../...../...../...../...../");
	scc_Digraph dg3 = scc_digraph_from_string(".####/#.###/##.##/###.#/####./");
	scc_Digraph dg4 = scc_digraph_from_string("#..../.#.../..#../...#./....#/");
	scc_Digraph dg5 = scc_digraph_from_string("#.#../#..../#.#../##.../##..#/");

	assert_true(scc_delete_loops(&dg1));
	assert_true(scc_delete_loops(&dg2));
	assert_true(scc_delete_loops(&dg3));
	assert_true(scc_delete_loops(&dg4));
	assert_true(scc_delete_loops(&dg5));

	scc_Digraph ref1 = scc_digraph_from_string(".####/#.###/##.##/###.#/####./");
	scc_Digraph ref2 = scc_digraph_from_string("...../...../...../...../...../");
	scc_Digraph ref3 = scc_digraph_from_string(".####/#.###/##.##/###.#/####./");
	scc_Digraph ref4 = scc_digraph_from_string("...../...../...../...../...../");
	scc_Digraph ref5 = scc_digraph_from_string("..#../#..../#..../##.../##.../");

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
