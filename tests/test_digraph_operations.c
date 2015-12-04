/*  scclust -- A C library for size constrained clustering
    https://github.com/fsavje/scclust

    Copyright (C) 2015  Fredrik Savje -- http://fredriksavje.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
==============================================================================*/


#include "test_suite.h"
#include "assert_digraph.h"

#include <stddef.h>

#include "../include/config.h"
#include "../include/digraph.h"
#include "../include/digraph_debug.h"


void scc_ut_union_digraphs(void** state) {
	(void) state;

	scc_Digraph ut_dg1 = scc_digraph_from_string("*.../.*../..*./...*/");
	scc_Digraph ut_dg2 = scc_digraph_from_string("...*/..*./.*../*.../");
	scc_Digraph ut_dg3 = scc_digraph_from_string("*.../*.../*.../*.../");


	const scc_Digraph* sum_12[2] = {&ut_dg1, &ut_dg2};
	scc_Digraph ut_make12_a = scc_digraph_union(2, sum_12);
	assert_sound_digraph(&ut_make12_a, 4);
	scc_Arcref tail_ptr12[5] = {0,2,4,6,8};
	scc_Vid head12[8] = {0,3,1,2,2,1,3,0};
	scc_Digraph ut_control12 = scc_digraph_from_pieces(4, 8, tail_ptr12, head12);
	assert_identical_digraph(&ut_control12, &ut_make12_a);


	const scc_Digraph* sum_13[2] = {&ut_dg1, &ut_dg3};
	scc_Digraph ut_make13_a = scc_digraph_union(2, sum_13);
	assert_sound_digraph(&ut_make13_a, 4);
	scc_Arcref tail_ptr13[5] = {0,1,3,5,7};
	scc_Vid head13[7] = {0,1,0,2,0,3,0};
	scc_Digraph ut_control13 = scc_digraph_from_pieces(4, 7, tail_ptr13, head13);
	assert_identical_digraph(&ut_control13, &ut_make13_a);


	const scc_Digraph* sum_31[2] = {&ut_dg3, &ut_dg1};
	scc_Digraph ut_make31_a = scc_digraph_union(2, sum_31);
	assert_sound_digraph(&ut_make31_a, 4);
	scc_Arcref tail_ptr31[5] = {0,1,3,5,7};
	scc_Vid head31[7] = {0,0,1,0,2,0,3};
	scc_Digraph ut_control31 = scc_digraph_from_pieces(4, 7, tail_ptr31, head31);
	assert_identical_digraph(&ut_control31, &ut_make31_a);

	assert_equal_digraph(&ut_make31_a, &ut_make13_a);


	const scc_Digraph* sum_123[3] = {&ut_dg1, &ut_dg2, &ut_dg3};
	scc_Digraph ut_make123 = scc_digraph_union(3, sum_123);
	assert_sound_digraph(&ut_make123, 4);
	scc_Arcref tail_ptr123[5] = {0,2,5,8,10};
	scc_Vid head123[10] = {0,3,1,2,0,2,1,0,3,0};
	scc_Digraph ut_control123 = scc_digraph_from_pieces(4, 10, tail_ptr123, head123);
	assert_identical_digraph(&ut_control123, &ut_make123);

	const scc_Digraph* sum_132[3] = {&ut_dg1, &ut_dg3, &ut_dg2};
	scc_Digraph ut_make132 = scc_digraph_union(3, sum_132);
	assert_sound_digraph(&ut_make132, 4);
	scc_Arcref tail_ptr132[5] = {0,2,5,8,10};
	scc_Vid head132[10] = {0,3,1,0,2,2,0,1,3,0};
	scc_Digraph ut_control132 = scc_digraph_from_pieces(4, 10, tail_ptr132, head132);
	assert_identical_digraph(&ut_control132, &ut_make132);

	const scc_Digraph* sum_213[3] = {&ut_dg2, &ut_dg1, &ut_dg3};
	scc_Digraph ut_make213 = scc_digraph_union(3, sum_213);
	assert_sound_digraph(&ut_make213, 4);
	scc_Arcref tail_ptr213[5] = {0,2,5,8,10};
	scc_Vid head213[10] = {3,0,2,1,0,1,2,0,0,3};
	scc_Digraph ut_control213 = scc_digraph_from_pieces(4, 10, tail_ptr213, head213);
	assert_identical_digraph(&ut_control213, &ut_make213);

	const scc_Digraph* sum_321[3] = {&ut_dg3, &ut_dg2, &ut_dg1};
	scc_Digraph ut_make321 = scc_digraph_union(3, sum_321);
	assert_sound_digraph(&ut_make321, 4);
	scc_Arcref tail_ptr321[5] = {0,2,5,8,10};
	scc_Vid head321[10] = {0,3,0,2,1,0,1,2,0,3};
	scc_Digraph ut_control321 = scc_digraph_from_pieces(4, 10, tail_ptr321, head321);
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

void scc_ut_digraph_transpose(void** state) {
	(void) state;

	scc_Digraph ut_dg1 = scc_digraph_from_string("*.../.*../..*./...*/");
	scc_Digraph ut_dg2 = scc_digraph_from_string("****/..*./****/*.../");
	scc_Digraph ut_dg3 = scc_digraph_from_string("*.../*.../*.../*.../");
	scc_Digraph ut_dg4 = scc_digraph_from_string("..../..../..../..../");
	scc_Digraph ut_dg5 = scc_empty_digraph(0, 0);

	scc_Digraph control1 = scc_digraph_from_string("*.../.*../..*./...*/");
	scc_Digraph control2 = scc_digraph_from_string("*.**/*.*./***./*.*./");
	scc_Digraph control3 = scc_digraph_from_string("****/..../..../..../");
	scc_Digraph control4 = scc_empty_digraph(4, 0);
	scc_Digraph control5 = scc_empty_digraph(0, 0);
	scc_Digraph control6 = scc_null_digraph();
	scc_Digraph control7 = scc_null_digraph();

	scc_Digraph res1 = scc_digraph_transpose(&ut_dg1);
	scc_Digraph res2 = scc_digraph_transpose(&ut_dg2);
	scc_Digraph res3 = scc_digraph_transpose(&ut_dg3);
	scc_Digraph res4 = scc_digraph_transpose(&ut_dg4);
	scc_Digraph res5 = scc_digraph_transpose(&ut_dg5);
	scc_Digraph res6 = scc_digraph_transpose(NULL);
	scc_Digraph res7 = scc_digraph_transpose(&control7);

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


void scc_ut_adjacency_product(void** state) {
	(void) state;

	scc_Digraph dg1 = scc_digraph_from_string("**.../...*./.*.../..*../...*./");
	scc_Digraph dg1_i = scc_digraph_from_string(".*.../...*./.*.../..*../...*./");
	scc_Digraph dg1_f = scc_digraph_from_string("**.../.*.*./.**../..**./...**/");
	scc_Digraph dg2 = scc_digraph_from_string("..*../..**./...../..*../*.*../");
	scc_Digraph dg2_i = scc_digraph_from_string("..*../..**./...../..*../*.*../");
	scc_Digraph dg2_f = scc_digraph_from_string("*.*../.***./..*../..**./*.*.*/");

	scc_Digraph prod0 = scc_adjacency_product(&dg1, &dg1, true, true);
	scc_Digraph none = scc_null_digraph();
	assert_identical_digraph(&prod0, &none);


	scc_Digraph prod1 = scc_adjacency_product(&dg1, &dg1, false, false);
	assert_sound_digraph(&prod1, 5);
	scc_Digraph control1 = scc_digraph_from_string("**.*./..*../...*./.*.../..*../");
	assert_equal_digraph(&prod1, &control1);

	scc_Digraph prod2 = scc_adjacency_product(&dg1, &dg1, false, true);
	assert_sound_digraph(&prod2, 5);
	scc_Digraph control2 = scc_digraph_from_string("...*./..*../...*./.*.../..*../");
	scc_Digraph control2alt = scc_adjacency_product(&dg1_i, &dg1, false, false);
	assert_equal_digraph(&prod2, &control2);
	assert_equal_digraph(&prod2, &control2alt);

	scc_Digraph prod3 = scc_adjacency_product(&dg1, &dg1, true, false);
	assert_sound_digraph(&prod3, 5);
	scc_Digraph control3 = scc_digraph_from_string("**.*./..**./.*.*./.**../..**./");
	scc_Digraph control3alt = scc_adjacency_product(&dg1_f, &dg1, false, false);
	assert_equal_digraph(&prod3, &control3);
	assert_equal_digraph(&prod3, &control3alt);


	scc_Digraph prod4 = scc_adjacency_product(&dg1, &prod2, false, false);
	assert_sound_digraph(&prod4, 5);
	scc_Digraph control4 = scc_digraph_from_string("..**./.*.../..*../...*./.*.../");
	assert_equal_digraph(&prod4, &control4);

	scc_Digraph prod5 = scc_adjacency_product(&dg1, &prod2, false, true);
	assert_sound_digraph(&prod5, 5);
	scc_Digraph control5 = scc_digraph_from_string("..*../.*.../..*../...*./.*.../");
	scc_Digraph control5alt = scc_adjacency_product(&dg1_i, &prod2, false, false);
	assert_equal_digraph(&prod5, &control5);
	assert_equal_digraph(&prod5, &control5alt);

	scc_Digraph prod6 = scc_adjacency_product(&dg1, &prod2, true, false);
	assert_sound_digraph(&prod6, 5);
	scc_Digraph control6 = scc_digraph_from_string("..**./.**../..**./.*.*./.**../");
	scc_Digraph control6alt = scc_adjacency_product(&dg1_f, &prod2, false, false);
	assert_equal_digraph(&prod6, &control6);
	assert_equal_digraph(&prod6, &control6alt);


	scc_Digraph prod7 = scc_adjacency_product(&dg1, &dg2, false, false);
	assert_sound_digraph(&prod7, 5);
	scc_Digraph control7 = scc_digraph_from_string("..**./..*../..**./...../..*../");
	assert_equal_digraph(&prod7, &control7);

	scc_Digraph prod8 = scc_adjacency_product(&dg1, &dg2, false, true);
	assert_sound_digraph(&prod8, 5);
	scc_Digraph control8 = scc_adjacency_product(&dg1_i, &dg2, false, false);
	assert_equal_digraph(&prod8, &control8);

	scc_Digraph prod9 = scc_adjacency_product(&dg1, &dg2, true, false);
	assert_sound_digraph(&prod9, 5);
	scc_Digraph control9 = scc_adjacency_product(&dg1_f, &dg2, false, false);
	assert_equal_digraph(&prod9, &control9);


	scc_Digraph prod10 = scc_adjacency_product(&dg2, &dg1, false, false);
	assert_sound_digraph(&prod10, 5);
	scc_Digraph control10 = scc_digraph_from_string(".*.../.**../...../.*.../**.../");
	assert_equal_digraph(&prod10, &control10);

	scc_Digraph prod11 = scc_adjacency_product(&dg2, &dg1, false, true);
	assert_sound_digraph(&prod11, 5);
	scc_Digraph control11 = scc_adjacency_product(&dg2_i, &dg1, false, false);
	assert_equal_digraph(&prod11, &control11);

	scc_Digraph prod12 = scc_adjacency_product(&dg2, &dg1, true, false);
	assert_sound_digraph(&prod12, 5);
	scc_Digraph control12 = scc_adjacency_product(&dg2_f, &dg1, false, false);
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
	const struct CMUnitTest test_operations[] = {
	    cmocka_unit_test(scc_ut_union_digraphs),
	    cmocka_unit_test(scc_ut_digraph_transpose),
	    cmocka_unit_test(scc_ut_adjacency_product),
	};

    return cmocka_run_group_tests_name("operations module", test_operations, NULL, NULL);
}
