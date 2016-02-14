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
#include "../include/scclust.h"
#include "../src/digraph_core.h"
#include "../src/digraph_operations.h"
#include "../src/digraph_debug.h"


void scc_ut_union_digraphs(void** state)
{
	(void) state;

	iscc_Digraph ut_dg1 = iscc_digraph_from_string("#.../.#../..#./...#/");
	iscc_Digraph ut_dg2 = iscc_digraph_from_string("...#/..#./.#../#.../");
	iscc_Digraph ut_dg3 = iscc_digraph_from_string("#.../#.../#.../#.../");


	const iscc_Digraph sum_12[2] = {ut_dg1, ut_dg2};
	iscc_Digraph ut_make12_a = iscc_digraph_union(2, sum_12, false);
	assert_valid_digraph(&ut_make12_a, 4);
	scc_Arci tail_ptr12[5] = {0,2,4,6,8};
	scc_Dpid head12[8] = {0,3,1,2,2,1,3,0};
	iscc_Digraph ut_control12 = iscc_digraph_from_pieces(4, 8, tail_ptr12, head12);
	assert_identical_digraph(&ut_control12, &ut_make12_a);


	const iscc_Digraph sum_13[2] = {ut_dg1, ut_dg3};
	iscc_Digraph ut_make13_a = iscc_digraph_union(2, sum_13, false);
	assert_valid_digraph(&ut_make13_a, 4);
	scc_Arci tail_ptr13[5] = {0,1,3,5,7};
	scc_Dpid head13[7] = {0,1,0,2,0,3,0};
	iscc_Digraph ut_control13 = iscc_digraph_from_pieces(4, 7, tail_ptr13, head13);
	assert_identical_digraph(&ut_control13, &ut_make13_a);


	const iscc_Digraph sum_31[2] = {ut_dg3, ut_dg1};
	iscc_Digraph ut_make31_a = iscc_digraph_union(2, sum_31, false);
	assert_valid_digraph(&ut_make31_a, 4);
	scc_Arci tail_ptr31[5] = {0,1,3,5,7};
	scc_Dpid head31[7] = {0,0,1,0,2,0,3};
	iscc_Digraph ut_control31 = iscc_digraph_from_pieces(4, 7, tail_ptr31, head31);
	assert_identical_digraph(&ut_control31, &ut_make31_a);

	assert_equal_digraph(&ut_make31_a, &ut_make13_a);


	const iscc_Digraph sum_123[3] = {ut_dg1, ut_dg2, ut_dg3};
	iscc_Digraph ut_make123 = iscc_digraph_union(3, sum_123, false);
	assert_valid_digraph(&ut_make123, 4);
	scc_Arci tail_ptr123[5] = {0,2,5,8,10};
	scc_Dpid head123[10] = {0,3,1,2,0,2,1,0,3,0};
	iscc_Digraph ut_control123 = iscc_digraph_from_pieces(4, 10, tail_ptr123, head123);
	assert_identical_digraph(&ut_control123, &ut_make123);

	const iscc_Digraph sum_132[3] = {ut_dg1, ut_dg3, ut_dg2};
	iscc_Digraph ut_make132 = iscc_digraph_union(3, sum_132, false);
	assert_valid_digraph(&ut_make132, 4);
	scc_Arci tail_ptr132[5] = {0,2,5,8,10};
	scc_Dpid head132[10] = {0,3,1,0,2,2,0,1,3,0};
	iscc_Digraph ut_control132 = iscc_digraph_from_pieces(4, 10, tail_ptr132, head132);
	assert_identical_digraph(&ut_control132, &ut_make132);

	const iscc_Digraph sum_213[3] = {ut_dg2, ut_dg1, ut_dg3};
	iscc_Digraph ut_make213 = iscc_digraph_union(3, sum_213, false);
	assert_valid_digraph(&ut_make213, 4);
	scc_Arci tail_ptr213[5] = {0,2,5,8,10};
	scc_Dpid head213[10] = {3,0,2,1,0,1,2,0,0,3};
	iscc_Digraph ut_control213 = iscc_digraph_from_pieces(4, 10, tail_ptr213, head213);
	assert_identical_digraph(&ut_control213, &ut_make213);

	const iscc_Digraph sum_321[3] = {ut_dg3, ut_dg2, ut_dg1};
	iscc_Digraph ut_make321 = iscc_digraph_union(3, sum_321, false);
	assert_valid_digraph(&ut_make321, 4);
	scc_Arci tail_ptr321[5] = {0,2,5,8,10};
	scc_Dpid head321[10] = {0,3,0,2,1,0,1,2,0,3};
	iscc_Digraph ut_control321 = iscc_digraph_from_pieces(4, 10, tail_ptr321, head321);
	assert_identical_digraph(&ut_control321, &ut_make321);

	assert_equal_digraph(&ut_make123, &ut_make132);
	assert_equal_digraph(&ut_make123, &ut_make213);
	assert_equal_digraph(&ut_make123, &ut_make321);

	assert_free_digraph(&ut_dg1);
	assert_free_digraph(&ut_dg2);
	assert_free_digraph(&ut_dg3);
	assert_free_digraph(&ut_make12_a);
	assert_free_digraph(&ut_make13_a);
	assert_free_digraph(&ut_make31_a);
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


void scc_ut_union_digraphs_ignore_diag(void** state)
{
	(void) state;

	iscc_Digraph ut_dg1 = iscc_digraph_from_string("#.../.#../..#./...#/");
	iscc_Digraph ut_dg2 = iscc_digraph_from_string("...#/..#./.#../#.../");
	iscc_Digraph ut_dg3 = iscc_digraph_from_string("#.../#.../#.../#.../");


	const iscc_Digraph sum_12[2] = {ut_dg1, ut_dg2};
	iscc_Digraph ut_make12_a = iscc_digraph_union(2, sum_12, true);
	assert_valid_digraph(&ut_make12_a, 4);
	scc_Arci tail_ptr12[5] = {0,1,2,3,4};
	scc_Dpid head12[4] = {3,2,1,0};
	iscc_Digraph ut_control12 = iscc_digraph_from_pieces(4, 4, tail_ptr12, head12);
	assert_identical_digraph(&ut_control12, &ut_make12_a);


	const iscc_Digraph sum_13[2] = {ut_dg1, ut_dg3};
	iscc_Digraph ut_make13_a = iscc_digraph_union(2, sum_13, true);
	assert_valid_digraph(&ut_make13_a, 4);
	scc_Arci tail_ptr13[5] = {0,0,1,2,3};
	scc_Dpid head13[3] = {0,0,0};
	iscc_Digraph ut_control13 = iscc_digraph_from_pieces(4, 3, tail_ptr13, head13);
	assert_identical_digraph(&ut_control13, &ut_make13_a);


	const iscc_Digraph sum_31[2] = {ut_dg3, ut_dg1};
	iscc_Digraph ut_make31_a = iscc_digraph_union(2, sum_31, true);
	assert_valid_digraph(&ut_make31_a, 4);
	scc_Arci tail_ptr31[5] = {0,0,1,2,3};
	scc_Dpid head31[3] = {0,0,0};
	iscc_Digraph ut_control31 = iscc_digraph_from_pieces(4, 3, tail_ptr31, head31);
	assert_identical_digraph(&ut_control31, &ut_make31_a);

	assert_equal_digraph(&ut_make31_a, &ut_make13_a);


	const iscc_Digraph sum_123[3] = {ut_dg1, ut_dg2, ut_dg3};
	iscc_Digraph ut_make123 = iscc_digraph_union(3, sum_123, true);
	assert_valid_digraph(&ut_make123, 4);
	scc_Arci tail_ptr123[5] = {0,1,3,5,6};
	scc_Dpid head123[6] = {3,2,0,1,0,0};
	iscc_Digraph ut_control123 = iscc_digraph_from_pieces(4, 6, tail_ptr123, head123);
	assert_identical_digraph(&ut_control123, &ut_make123);

	const iscc_Digraph sum_132[3] = {ut_dg1, ut_dg3, ut_dg2};
	iscc_Digraph ut_make132 = iscc_digraph_union(3, sum_132, true);
	assert_valid_digraph(&ut_make132, 4);
	scc_Arci tail_ptr132[5] = {0,1,3,5,6};
	scc_Dpid head132[6] = {3,0,2,0,1,0};
	iscc_Digraph ut_control132 = iscc_digraph_from_pieces(4, 6, tail_ptr132, head132);
	assert_identical_digraph(&ut_control132, &ut_make132);

	const iscc_Digraph sum_213[3] = {ut_dg2, ut_dg1, ut_dg3};
	iscc_Digraph ut_make213 = iscc_digraph_union(3, sum_213, true);
	assert_valid_digraph(&ut_make213, 4);
	scc_Arci tail_ptr213[5] = {0,1,3,5,6};
	scc_Dpid head213[6] = {3,2,0,1,0,0};
	iscc_Digraph ut_control213 = iscc_digraph_from_pieces(4, 6, tail_ptr213, head213);
	assert_identical_digraph(&ut_control213, &ut_make213);

	const iscc_Digraph sum_321[3] = {ut_dg3, ut_dg2, ut_dg1};
	iscc_Digraph ut_make321 = iscc_digraph_union(3, sum_321, true);
	assert_valid_digraph(&ut_make321, 4);
	scc_Arci tail_ptr321[5] = {0,1,3,5,6};
	scc_Dpid head321[6] = {3,0,2,0,1,0};
	iscc_Digraph ut_control321 = iscc_digraph_from_pieces(4, 6, tail_ptr321, head321);
	assert_identical_digraph(&ut_control321, &ut_make321);

	assert_equal_digraph(&ut_make123, &ut_make132);
	assert_equal_digraph(&ut_make123, &ut_make213);
	assert_equal_digraph(&ut_make123, &ut_make321);


	assert_free_digraph(&ut_dg1);
	assert_free_digraph(&ut_dg2);
	assert_free_digraph(&ut_dg3);
	assert_free_digraph(&ut_make12_a);
	assert_free_digraph(&ut_make13_a);
	assert_free_digraph(&ut_make31_a);
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


void scc_ut_digraph_transpose(void** state)
{
	(void) state;

	iscc_Digraph ut_dg1 = iscc_digraph_from_string("#.../.#../..#./...#/");
	iscc_Digraph ut_dg2 = iscc_digraph_from_string("####/..#./####/#.../");
	iscc_Digraph ut_dg3 = iscc_digraph_from_string("#.../#.../#.../#.../");
	iscc_Digraph ut_dg4 = iscc_digraph_from_string("..../..../..../..../");
	iscc_Digraph ut_dg5 = iscc_empty_digraph(0, 0);

	iscc_Digraph control1 = iscc_digraph_from_string("#.../.#../..#./...#/");
	iscc_Digraph control2 = iscc_digraph_from_string("#.##/#.#./###./#.#./");
	iscc_Digraph control3 = iscc_digraph_from_string("####/..../..../..../");
	iscc_Digraph control4 = iscc_empty_digraph(4, 0);
	iscc_Digraph control5 = iscc_empty_digraph(0, 0);

	iscc_Digraph res1 = iscc_digraph_transpose(&ut_dg1);
	iscc_Digraph res2 = iscc_digraph_transpose(&ut_dg2);
	iscc_Digraph res3 = iscc_digraph_transpose(&ut_dg3);
	iscc_Digraph res4 = iscc_digraph_transpose(&ut_dg4);
	iscc_Digraph res5 = iscc_digraph_transpose(&ut_dg5);

	assert_valid_digraph(&res1, 4);
	assert_valid_digraph(&res2, 4);
	assert_valid_digraph(&res3, 4);
	assert_valid_digraph(&res4, 4);
	assert_valid_digraph(&res5, 0);

	assert_equal_digraph(&res1, &control1);
	assert_equal_digraph(&res2, &control2);
	assert_equal_digraph(&res3, &control3);
	assert_equal_digraph(&res4, &control4);
	assert_equal_digraph(&res5, &control5);

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


void scc_ut_adjacency_product(void** state)
{
	(void) state;

	iscc_Digraph dg1 = iscc_digraph_from_string("##.../...#./.#.../..#../...#./");
	iscc_Digraph dg1_i = iscc_digraph_from_string(".#.../...#./.#.../..#../...#./");
	iscc_Digraph dg1_f = iscc_digraph_from_string("##.../.#.#./.##../..##./...##/");
	iscc_Digraph dg2 = iscc_digraph_from_string("..#../..##./...../..#../#.#../");
	iscc_Digraph dg2_i = iscc_digraph_from_string("..#../..##./...../..#../#.#../");
	iscc_Digraph dg2_f = iscc_digraph_from_string("#.#../.###./..#../..##./#.#.#/");


	iscc_Digraph prod1 = iscc_adjacency_product(&dg1, &dg1, false, false);
	assert_valid_digraph(&prod1, 5);
	iscc_Digraph control1 = iscc_digraph_from_string("##.#./..#../...#./.#.../..#../");
	assert_equal_digraph(&prod1, &control1);

	iscc_Digraph prod2 = iscc_adjacency_product(&dg1, &dg1, false, true);
	assert_valid_digraph(&prod2, 5);
	iscc_Digraph control2 = iscc_digraph_from_string("...#./..#../...#./.#.../..#../");
	iscc_Digraph control2alt = iscc_adjacency_product(&dg1_i, &dg1, false, false);
	assert_equal_digraph(&prod2, &control2);
	assert_equal_digraph(&prod2, &control2alt);

	iscc_Digraph prod3 = iscc_adjacency_product(&dg1, &dg1, true, false);
	assert_valid_digraph(&prod3, 5);
	iscc_Digraph control3 = iscc_digraph_from_string("##.#./..##./.#.#./.##../..##./");
	iscc_Digraph control3alt = iscc_adjacency_product(&dg1_f, &dg1, false, false);
	assert_equal_digraph(&prod3, &control3);
	assert_equal_digraph(&prod3, &control3alt);


	iscc_Digraph prod4 = iscc_adjacency_product(&dg1, &prod2, false, false);
	assert_valid_digraph(&prod4, 5);
	iscc_Digraph control4 = iscc_digraph_from_string("..##./.#.../..#../...#./.#.../");
	assert_equal_digraph(&prod4, &control4);

	iscc_Digraph prod5 = iscc_adjacency_product(&dg1, &prod2, false, true);
	assert_valid_digraph(&prod5, 5);
	iscc_Digraph control5 = iscc_digraph_from_string("..#../.#.../..#../...#./.#.../");
	iscc_Digraph control5alt = iscc_adjacency_product(&dg1_i, &prod2, false, false);
	assert_equal_digraph(&prod5, &control5);
	assert_equal_digraph(&prod5, &control5alt);

	iscc_Digraph prod6 = iscc_adjacency_product(&dg1, &prod2, true, false);
	assert_valid_digraph(&prod6, 5);
	iscc_Digraph control6 = iscc_digraph_from_string("..##./.##../..##./.#.#./.##../");
	iscc_Digraph control6alt = iscc_adjacency_product(&dg1_f, &prod2, false, false);
	assert_equal_digraph(&prod6, &control6);
	assert_equal_digraph(&prod6, &control6alt);


	iscc_Digraph prod7 = iscc_adjacency_product(&dg1, &dg2, false, false);
	assert_valid_digraph(&prod7, 5);
	iscc_Digraph control7 = iscc_digraph_from_string("..##./..#../..##./...../..#../");
	assert_equal_digraph(&prod7, &control7);

	iscc_Digraph prod8 = iscc_adjacency_product(&dg1, &dg2, false, true);
	assert_valid_digraph(&prod8, 5);
	iscc_Digraph control8 = iscc_adjacency_product(&dg1_i, &dg2, false, false);
	assert_equal_digraph(&prod8, &control8);

	iscc_Digraph prod9 = iscc_adjacency_product(&dg1, &dg2, true, false);
	assert_valid_digraph(&prod9, 5);
	iscc_Digraph control9 = iscc_adjacency_product(&dg1_f, &dg2, false, false);
	assert_equal_digraph(&prod9, &control9);


	iscc_Digraph prod10 = iscc_adjacency_product(&dg2, &dg1, false, false);
	assert_valid_digraph(&prod10, 5);
	iscc_Digraph control10 = iscc_digraph_from_string(".#.../.##../...../.#.../##.../");
	assert_equal_digraph(&prod10, &control10);

	iscc_Digraph prod11 = iscc_adjacency_product(&dg2, &dg1, false, true);
	assert_valid_digraph(&prod11, 5);
	iscc_Digraph control11 = iscc_adjacency_product(&dg2_i, &dg1, false, false);
	assert_equal_digraph(&prod11, &control11);

	iscc_Digraph prod12 = iscc_adjacency_product(&dg2, &dg1, true, false);
	assert_valid_digraph(&prod12, 5);
	iscc_Digraph control12 = iscc_adjacency_product(&dg2_f, &dg1, false, false);
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


int main(void)
{
	const struct CMUnitTest test_operations[] = {
		cmocka_unit_test(scc_ut_union_digraphs),
		cmocka_unit_test(scc_ut_union_digraphs_ignore_diag),
		cmocka_unit_test(scc_ut_digraph_transpose),
		cmocka_unit_test(scc_ut_adjacency_product),
	};

	return cmocka_run_group_tests_name("operations module", test_operations, NULL, NULL);
}
