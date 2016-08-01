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
 * License along with this library. If not, see http://www.gnu.org/licenses/
 * ============================================================================== */

#include "test_suite.h"
#include "assert_digraph.h"
#include "../src/digraph_operations.c"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "../src/digraph_core.h"
#include "../src/digraph_debug.h"
#include "../src/scclust_int.h"


void scc_ut_do_union_and_delete(void** state)
{
	(void) state;

	iscc_Dpid row_markers[4];

	iscc_Digraph ut_dg1;
	iscc_digraph_from_string("#..#/.#.#/..##/...#/", &ut_dg1);
	iscc_Digraph ut_dg2;
	iscc_digraph_from_string("...#/..#./.#../#.../", &ut_dg2);
	iscc_Digraph ut_dg3;
	iscc_digraph_from_string("#.../#.../#.../#.../", &ut_dg3);

	const bool tails_to_keep1[4] = { true, true, false, false };
	const bool tails_to_keep2[4] = { true, false, true, false };
	const bool tails_to_keep3[4] = { false, false, true, true };


	const iscc_Digraph sum_12[2] = {ut_dg1, ut_dg2};
	
	const uint64_t ut_count_12 = iscc_do_union_and_delete(2, sum_12, row_markers, NULL, false, NULL, NULL);
	assert_int_equal(ut_count_12, 6);
	iscc_Arci out_tail_ptr_12[5];
	iscc_Dpid out_head_12[6];
	const uint64_t ut_count_do_12 = iscc_do_union_and_delete(2, sum_12, row_markers, NULL, true, out_tail_ptr_12, out_head_12);
	assert_int_equal(ut_count_do_12, ut_count_12);
	iscc_Arci ref_tail_ptr_12[5] = { 0, 1, 3, 5, 6 };
	iscc_Dpid ref_head_12[6] = { 3, 3, 2, 3, 1, 0 };
	assert_memory_equal(out_tail_ptr_12, ref_tail_ptr_12, 5 * sizeof(iscc_Arci));
	assert_memory_equal(out_head_12, ref_head_12, ut_count_12 * sizeof(iscc_Dpid));

	const uint64_t ut_count_12_ttk = iscc_do_union_and_delete(2, sum_12, row_markers, tails_to_keep1, false, NULL, NULL);
	assert_int_equal(ut_count_12_ttk, 3);
	iscc_Arci out_tail_ptr_12_ttk[5];
	iscc_Dpid out_head_12_ttk[3];
	const uint64_t ut_count_do_12_ttk = iscc_do_union_and_delete(2, sum_12, row_markers, tails_to_keep1, true, out_tail_ptr_12_ttk, out_head_12_ttk);
	assert_int_equal(ut_count_do_12_ttk, ut_count_12_ttk);
	iscc_Arci ref_tail_ptr_12_ttk[5] = { 0, 1, 3, 3, 3 };
	iscc_Dpid ref_head_12_ttk[3] = { 3, 3, 2 };
	assert_memory_equal(out_tail_ptr_12_ttk, ref_tail_ptr_12_ttk, 5 * sizeof(iscc_Arci));
	assert_memory_equal(out_head_12_ttk, ref_head_12_ttk, ut_count_12_ttk * sizeof(iscc_Dpid));


	const iscc_Digraph sum_13[2] = {ut_dg1, ut_dg3};
	
	const uint64_t ut_count_13 = iscc_do_union_and_delete(2, sum_13, row_markers, NULL, false, NULL, NULL);
	assert_int_equal(ut_count_13, 6);
	iscc_Arci out_tail_ptr_13[5];
	iscc_Dpid out_head_13[6];
	const uint64_t ut_count_do_13 = iscc_do_union_and_delete(2, sum_13, row_markers, NULL, true, out_tail_ptr_13, out_head_13);
	assert_int_equal(ut_count_do_13, ut_count_13);
	iscc_Arci ref_tail_ptr_13[5] = { 0, 1, 3, 5, 6 };
	iscc_Dpid ref_head_13[6] = { 3, 3, 0, 3, 0, 0 };
	assert_memory_equal(out_tail_ptr_13, ref_tail_ptr_13, 5 * sizeof(iscc_Arci));
	assert_memory_equal(out_head_13, ref_head_13, ut_count_13 * sizeof(iscc_Dpid));

	const uint64_t ut_count_13_ttk = iscc_do_union_and_delete(2, sum_13, row_markers, tails_to_keep2, false, NULL, NULL);
	assert_int_equal(ut_count_13_ttk, 3);
	iscc_Arci out_tail_ptr_13_ttk[5];
	iscc_Dpid out_head_13_ttk[3];
	const uint64_t ut_count_do_13_ttk = iscc_do_union_and_delete(2, sum_13, row_markers, tails_to_keep2, true, out_tail_ptr_13_ttk, out_head_13_ttk);
	assert_int_equal(ut_count_do_13_ttk, ut_count_13_ttk);
	iscc_Arci ref_tail_ptr_13_ttk[5] = { 0, 1, 1, 3, 3 };
	iscc_Dpid ref_head_13_ttk[3] = { 3, 3, 0 };
	assert_memory_equal(out_tail_ptr_13_ttk, ref_tail_ptr_13_ttk, 5 * sizeof(iscc_Arci));
	assert_memory_equal(out_head_13_ttk, ref_head_13_ttk, ut_count_13_ttk * sizeof(iscc_Dpid));


	const iscc_Digraph sum_31[2] = {ut_dg3, ut_dg1};
	
	const uint64_t ut_count_31 = iscc_do_union_and_delete(2, sum_31, row_markers, NULL, false, NULL, NULL);
	assert_int_equal(ut_count_31, 6);
	iscc_Arci out_tail_ptr_31[5];
	iscc_Dpid out_head_31[6];
	const uint64_t ut_count_do_31 = iscc_do_union_and_delete(2, sum_31, row_markers, NULL, true, out_tail_ptr_31, out_head_31);
	assert_int_equal(ut_count_do_31, ut_count_31);
	iscc_Arci ref_tail_ptr_31[5] = { 0, 1, 3, 5, 6 };
	iscc_Dpid ref_head_31[6] = { 3, 0, 3, 0, 3, 0 };
	assert_memory_equal(out_tail_ptr_31, ref_tail_ptr_31, 5 * sizeof(iscc_Arci));
	assert_memory_equal(out_head_31, ref_head_31, ut_count_31 * sizeof(iscc_Dpid));

	const uint64_t ut_count_31_ttk = iscc_do_union_and_delete(2, sum_31, row_markers, tails_to_keep3, false, NULL, NULL);
	assert_int_equal(ut_count_31_ttk, 3);
	iscc_Arci out_tail_ptr_31_ttk[5];
	iscc_Dpid out_head_31_ttk[3];
	const uint64_t ut_count_do_31_ttk = iscc_do_union_and_delete(2, sum_31, row_markers, tails_to_keep3, true, out_tail_ptr_31_ttk, out_head_31_ttk);
	assert_int_equal(ut_count_do_31_ttk, ut_count_31_ttk);
	iscc_Arci ref_tail_ptr_31_ttk[5] = { 0, 0, 0, 2, 3 };
	iscc_Dpid ref_head_31_ttk[3] = { 0, 3, 0 };
	assert_memory_equal(out_tail_ptr_31_ttk, ref_tail_ptr_31_ttk, 5 * sizeof(iscc_Arci));
	assert_memory_equal(out_head_31_ttk, ref_head_31_ttk, ut_count_31_ttk * sizeof(iscc_Dpid));


	const iscc_Digraph sum_123[3] = {ut_dg1, ut_dg2, ut_dg3};
	
	const uint64_t ut_count_123 = iscc_do_union_and_delete(3, sum_123, row_markers, NULL, false, NULL, NULL);
	assert_int_equal(ut_count_123, 8);
	iscc_Arci out_tail_ptr_123[5];
	iscc_Dpid out_head_123[8];
	const uint64_t ut_count_do_123 = iscc_do_union_and_delete(3, sum_123, row_markers, NULL, true, out_tail_ptr_123, out_head_123);
	assert_int_equal(ut_count_do_123, ut_count_123);
	iscc_Arci ref_tail_ptr_123[5] = { 0, 1, 4, 7, 8 };
	iscc_Dpid ref_head_123[8] = { 3, 3, 2, 0, 3, 1, 0, 0 };
	assert_memory_equal(out_tail_ptr_123, ref_tail_ptr_123, 5 * sizeof(iscc_Arci));
	assert_memory_equal(out_head_123, ref_head_123, ut_count_123 * sizeof(iscc_Dpid));

	const uint64_t ut_count_123_ttk = iscc_do_union_and_delete(3, sum_123, row_markers, tails_to_keep1, false, NULL, NULL);
	assert_int_equal(ut_count_123_ttk, 4);
	iscc_Arci out_tail_ptr_123_ttk[5];
	iscc_Dpid out_head_123_ttk[4];
	const uint64_t ut_count_do_123_ttk = iscc_do_union_and_delete(3, sum_123, row_markers, tails_to_keep1, true, out_tail_ptr_123_ttk, out_head_123_ttk);
	assert_int_equal(ut_count_do_123_ttk, ut_count_123_ttk);
	iscc_Arci ref_tail_ptr_123_ttk[5] = { 0, 1, 4, 4, 4 };
	iscc_Dpid ref_head_123_ttk[4] = { 3, 3, 2, 0 };
	assert_memory_equal(out_tail_ptr_123_ttk, ref_tail_ptr_123_ttk, 5 * sizeof(iscc_Arci));
	assert_memory_equal(out_head_123_ttk, ref_head_123_ttk, ut_count_123_ttk * sizeof(iscc_Dpid));


	const iscc_Digraph sum_132[3] = {ut_dg1, ut_dg3, ut_dg2};
	
	const uint64_t ut_count_132 = iscc_do_union_and_delete(3, sum_132, row_markers, NULL, false, NULL, NULL);
	assert_int_equal(ut_count_132, 8);
	iscc_Arci out_tail_ptr_132[5];
	iscc_Dpid out_head_132[8];
	const uint64_t ut_count_do_132 = iscc_do_union_and_delete(3, sum_132, row_markers, NULL, true, out_tail_ptr_132, out_head_132);
	assert_int_equal(ut_count_do_132, ut_count_132);
	iscc_Arci ref_tail_ptr_132[5] = { 0, 1, 4, 7, 8 };
	iscc_Dpid ref_head_132[8] = { 3, 3, 0, 2, 3, 0, 1, 0 };
	assert_memory_equal(out_tail_ptr_132, ref_tail_ptr_132, 5 * sizeof(iscc_Arci));
	assert_memory_equal(out_head_132, ref_head_132, ut_count_132 * sizeof(iscc_Dpid));

	const uint64_t ut_count_132_ttk = iscc_do_union_and_delete(3, sum_132, row_markers, tails_to_keep2, false, NULL, NULL);
	assert_int_equal(ut_count_132_ttk, 4);
	iscc_Arci out_tail_ptr_132_ttk[5];
	iscc_Dpid out_head_132_ttk[4];
	const uint64_t ut_count_do_132_ttk = iscc_do_union_and_delete(3, sum_132, row_markers, tails_to_keep2, true, out_tail_ptr_132_ttk, out_head_132_ttk);
	assert_int_equal(ut_count_do_132_ttk, ut_count_132_ttk);
	iscc_Arci ref_tail_ptr_132_ttk[5] = { 0, 1, 1, 4, 4 };
	iscc_Dpid ref_head_132_ttk[4] = { 3, 3, 0, 1 };
	assert_memory_equal(out_tail_ptr_132_ttk, ref_tail_ptr_132_ttk, 5 * sizeof(iscc_Arci));
	assert_memory_equal(out_head_132_ttk, ref_head_132_ttk, ut_count_132_ttk * sizeof(iscc_Dpid));


	const iscc_Digraph sum_213[3] = {ut_dg2, ut_dg1, ut_dg3};
	
	const uint64_t ut_count_213 = iscc_do_union_and_delete(3, sum_213, row_markers, NULL, false, NULL, NULL);
	assert_int_equal(ut_count_213, 8);
	iscc_Arci out_tail_ptr_213[5];
	iscc_Dpid out_head_213[8];
	const uint64_t ut_count_do_213 = iscc_do_union_and_delete(3, sum_213, row_markers, NULL, true, out_tail_ptr_213, out_head_213);
	assert_int_equal(ut_count_do_213, ut_count_213);
	iscc_Arci ref_tail_ptr_213[5] = { 0, 1, 4, 7, 8 };
	iscc_Dpid ref_head_213[8] = { 3, 2, 3, 0, 1, 3, 0, 0 };
	assert_memory_equal(out_tail_ptr_213, ref_tail_ptr_213, 5 * sizeof(iscc_Arci));
	assert_memory_equal(out_head_213, ref_head_213, ut_count_213 * sizeof(iscc_Dpid));

	const uint64_t ut_count_213_ttk = iscc_do_union_and_delete(3, sum_213, row_markers, tails_to_keep3, false, NULL, NULL);
	assert_int_equal(ut_count_213_ttk, 4);
	iscc_Arci out_tail_ptr_213_ttk[5];
	iscc_Dpid out_head_213_ttk[4];
	const uint64_t ut_count_do_213_ttk = iscc_do_union_and_delete(3, sum_213, row_markers, tails_to_keep3, true, out_tail_ptr_213_ttk, out_head_213_ttk);
	assert_int_equal(ut_count_do_213_ttk, ut_count_213_ttk);
	iscc_Arci ref_tail_ptr_213_ttk[5] = { 0, 0, 0, 3, 4 };
	iscc_Dpid ref_head_213_ttk[4] = { 1, 3, 0, 0 };
	assert_memory_equal(out_tail_ptr_213_ttk, ref_tail_ptr_213_ttk, 5 * sizeof(iscc_Arci));
	assert_memory_equal(out_head_213_ttk, ref_head_213_ttk, ut_count_213_ttk * sizeof(iscc_Dpid));


	const iscc_Digraph sum_321[3] = {ut_dg3, ut_dg2, ut_dg1};
	
	const uint64_t ut_count_321 = iscc_do_union_and_delete(3, sum_321, row_markers, NULL, false, NULL, NULL);
	assert_int_equal(ut_count_321, 8);
	iscc_Arci out_tail_ptr_321[5];
	iscc_Dpid out_head_321[8];
	const uint64_t ut_count_do_321 = iscc_do_union_and_delete(3, sum_321, row_markers, NULL, true, out_tail_ptr_321, out_head_321);
	assert_int_equal(ut_count_do_321, ut_count_321);
	iscc_Arci ref_tail_ptr_321[5] = { 0, 1, 4, 7, 8 };
	iscc_Dpid ref_head_321[8] = { 3, 0, 2, 3, 0, 1, 3, 0 };
	assert_memory_equal(out_tail_ptr_321, ref_tail_ptr_321, 5 * sizeof(iscc_Arci));
	assert_memory_equal(out_head_321, ref_head_321, ut_count_321 * sizeof(iscc_Dpid));

	const uint64_t ut_count_321_ttk = iscc_do_union_and_delete(3, sum_321, row_markers, tails_to_keep1, false, NULL, NULL);
	assert_int_equal(ut_count_321_ttk, 4);
	iscc_Arci out_tail_ptr_321_ttk[5];
	iscc_Dpid out_head_321_ttk[4];
	const uint64_t ut_count_do_321_ttk = iscc_do_union_and_delete(3, sum_321, row_markers, tails_to_keep1, true, out_tail_ptr_321_ttk, out_head_321_ttk);
	assert_int_equal(ut_count_do_321_ttk, ut_count_321_ttk);
	iscc_Arci ref_tail_ptr_321_ttk[5] = { 0, 1, 4, 4, 4 };
	iscc_Dpid ref_head_321_ttk[4] = { 3, 0, 2, 3 };
	assert_memory_equal(out_tail_ptr_321_ttk, ref_tail_ptr_321_ttk, 5 * sizeof(iscc_Arci));
	assert_memory_equal(out_head_321_ttk, ref_head_321_ttk, ut_count_321_ttk * sizeof(iscc_Dpid));
	

	iscc_free_digraph(&ut_dg1);
	iscc_free_digraph(&ut_dg2);
	iscc_free_digraph(&ut_dg3);
}


void scc_ut_do_adjacency_product(void** state)
{
	(void) state;

	iscc_Dpid row_markers[5];

	iscc_Digraph dg1;
	iscc_digraph_from_string("##.../...#./.#.../..#../...#./", &dg1);
	iscc_Digraph dg1_f;
	iscc_digraph_from_string("##.../.#.#./.##../..##./...##/", &dg1_f);
	iscc_Digraph dg2;
	iscc_digraph_from_string("..#../..##./...../..#../#.#../", &dg2);
	iscc_Digraph dg2_f;
	iscc_digraph_from_string("#.#../.###./..#../..##./#.#.#/", &dg2_f);

	const uint64_t count_ref1 = 6;
	iscc_Digraph prod1;
	iscc_adjacency_product(&dg1, &dg1, false, &prod1);
	const uint64_t count1 = iscc_do_adjacency_product(&dg1, &dg1, row_markers, false, false, NULL, NULL);
	assert_valid_digraph(&prod1, 5);
	assert_int_equal(count1, count_ref1);
	assert_int_equal(prod1.tail_ptr[prod1.vertices], count_ref1);

	const uint64_t count_ref2 = 10;
	iscc_Digraph prod2;
	iscc_adjacency_product(&dg1, &dg1, true, &prod2);
	const uint64_t count2 = iscc_do_adjacency_product(&dg1, &dg1, row_markers, true, false, NULL, NULL);
	iscc_Digraph prod2alt;
	iscc_adjacency_product(&dg1_f, &dg1, false, &prod2alt);
	const uint64_t count2alt = iscc_do_adjacency_product(&dg1_f, &dg1, row_markers, false, false, NULL, NULL);
	assert_valid_digraph(&prod2, 5);
	assert_valid_digraph(&prod2alt, 5);
	assert_int_equal(count2, count_ref2);
	assert_int_equal(count2alt, count_ref2);
	assert_int_equal(prod2.tail_ptr[prod2.vertices], count_ref2);
	assert_int_equal(prod2alt.tail_ptr[prod2alt.vertices], count_ref2);

	const uint64_t count_ref3 = 8;
	iscc_Digraph prod3;
	iscc_adjacency_product(&dg1, &prod2, false, &prod3);
	const uint64_t count3 = iscc_do_adjacency_product(&dg1, &prod2, row_markers, false, false, NULL, NULL);
	assert_valid_digraph(&prod3, 5);
	assert_int_equal(count3, count_ref3);
	assert_int_equal(prod3.tail_ptr[prod3.vertices], count_ref3);

	const uint64_t count_ref4 = 12;
	iscc_Digraph prod4;
	iscc_adjacency_product(&dg1, &prod2, true, &prod4);
	const uint64_t count4 = iscc_do_adjacency_product(&dg1, &prod2, row_markers, true, false, NULL, NULL);
	iscc_Digraph prod4alt;
	iscc_adjacency_product(&dg1_f, &prod2, false, &prod4alt);
	const uint64_t count4alt = iscc_do_adjacency_product(&dg1_f, &prod2, row_markers, false, false, NULL, NULL);
	assert_valid_digraph(&prod4, 5);
	assert_valid_digraph(&prod4alt, 5);
	assert_int_equal(count4, count_ref4);
	assert_int_equal(count4alt, count_ref4);
	assert_int_equal(prod4.tail_ptr[prod4.vertices], count_ref4);
	assert_int_equal(prod4alt.tail_ptr[prod4alt.vertices], count_ref4);

	const uint64_t count_ref5 = 5;
	iscc_Digraph prod5;
	iscc_adjacency_product(&dg1, &dg2, false, &prod5);
	const uint64_t count5 = iscc_do_adjacency_product(&dg1, &dg2, row_markers, false, false, NULL, NULL);
	assert_valid_digraph(&prod5, 5);
	assert_int_equal(count5, count_ref5);
	assert_int_equal(prod5.tail_ptr[prod5.vertices], count_ref5);

	const uint64_t count_ref6 = 8;
	iscc_Digraph prod6;
	iscc_adjacency_product(&dg1, &dg2, true, &prod6);
	const uint64_t count6 = iscc_do_adjacency_product(&dg1, &dg2, row_markers, true, false, NULL, NULL);
	iscc_Digraph prod6alt;
	iscc_adjacency_product(&dg1_f, &dg2, false, &prod6alt);
	const uint64_t count6alt = iscc_do_adjacency_product(&dg1_f, &dg2, row_markers, false, false, NULL, NULL);
	assert_valid_digraph(&prod6, 5);
	assert_valid_digraph(&prod6alt, 5);
	assert_int_equal(count6, count_ref6);
	assert_int_equal(count6alt, count_ref6);
	assert_int_equal(prod6.tail_ptr[prod6.vertices], count_ref6);
	assert_int_equal(prod6alt.tail_ptr[prod6alt.vertices], count_ref6);

	const uint64_t count_ref7 = 5;
	iscc_Digraph prod7;
	iscc_adjacency_product(&dg2, &dg1, false, &prod7);
	const uint64_t count7 = iscc_do_adjacency_product(&dg2, &dg1, row_markers, false, false, NULL, NULL);
	assert_valid_digraph(&prod7, 5);
	assert_int_equal(count7, count_ref7);
	assert_int_equal(prod7.tail_ptr[prod7.vertices], count_ref7);

	const uint64_t count_ref8 = 9;
	iscc_Digraph prod8;
	iscc_adjacency_product(&dg2, &dg1, true, &prod8);
	const uint64_t count8 = iscc_do_adjacency_product(&dg2, &dg1, row_markers, true, false, NULL, NULL);
	iscc_Digraph prod8alt;
	iscc_adjacency_product(&dg2_f, &dg1, false, &prod8alt);
	const uint64_t count8alt = iscc_do_adjacency_product(&dg2_f, &dg1, row_markers, false, false, NULL, NULL);
	assert_valid_digraph(&prod8, 5);
	assert_valid_digraph(&prod8alt, 5);
	assert_int_equal(count8, count_ref8);
	assert_int_equal(count8alt, count_ref8);
	assert_int_equal(prod8.tail_ptr[prod8.vertices], count_ref8);
	assert_int_equal(prod8alt.tail_ptr[prod8alt.vertices], count_ref8);

	assert_free_digraph(&dg1);
	assert_free_digraph(&dg1_f);
	assert_free_digraph(&dg2);
	assert_free_digraph(&dg2_f);
	assert_free_digraph(&prod1);
	assert_free_digraph(&prod2);
	assert_free_digraph(&prod2alt);
	assert_free_digraph(&prod3);
	assert_free_digraph(&prod4);
	assert_free_digraph(&prod4alt);
	assert_free_digraph(&prod5);
	assert_free_digraph(&prod6);
	assert_free_digraph(&prod6alt);
	assert_free_digraph(&prod7);
	assert_free_digraph(&prod8);
	assert_free_digraph(&prod8alt);
}


int main(void)
{
	const struct CMUnitTest test_cases[] = {
		cmocka_unit_test(scc_ut_do_union_and_delete),
		cmocka_unit_test(scc_ut_do_adjacency_product),
	};
	
	return cmocka_run_group_tests_name("internal digraph_operations.c", test_cases, NULL, NULL);
}
