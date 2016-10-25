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
#include "../src/digraph_operations.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "../src/digraph_core.h"
#include "../src/digraph_debug.h"


void scc_ut_delete_loops(void** state)
{
	(void) state;

	iscc_Digraph dg1;
	iscc_digraph_from_string("#####/#####/#####/#####/#####/", &dg1);
	iscc_Digraph dg2;
	iscc_digraph_from_string("...../...../...../...../...../", &dg2);
	iscc_Digraph dg3;
	iscc_digraph_from_string(".####/#.###/##.##/###.#/####./", &dg3);
	iscc_Digraph dg4;
	iscc_digraph_from_string("#..../.#.../..#../...#./....#/", &dg4);
	iscc_Digraph dg5;
	iscc_digraph_from_string("#.#../#..../#.#../##.../##..#/", &dg5);
	iscc_Digraph dg6;
	iscc_empty_digraph(5, 0, &dg6);

	scc_ErrorCode ec1 = iscc_delete_loops(&dg1);
	scc_ErrorCode ec2 = iscc_delete_loops(&dg2);
	scc_ErrorCode ec3 = iscc_delete_loops(&dg3);
	scc_ErrorCode ec4 = iscc_delete_loops(&dg4);
	scc_ErrorCode ec5 = iscc_delete_loops(&dg5);
	scc_ErrorCode ec6 = iscc_delete_loops(&dg6);

	assert_int_equal(ec1, SCC_ER_OK);
	assert_int_equal(ec2, SCC_ER_OK);
	assert_int_equal(ec3, SCC_ER_OK);
	assert_int_equal(ec4, SCC_ER_OK);
	assert_int_equal(ec5, SCC_ER_OK);
	assert_int_equal(ec6, SCC_ER_OK);

	iscc_Digraph ref1;
	iscc_digraph_from_string(".####/#.###/##.##/###.#/####./", &ref1);
	iscc_Digraph ref2;
	iscc_digraph_from_string("...../...../...../...../...../", &ref2);
	iscc_Digraph ref3;
	iscc_digraph_from_string(".####/#.###/##.##/###.#/####./", &ref3);
	iscc_Digraph ref4;
	iscc_digraph_from_string("...../...../...../...../...../", &ref4);
	iscc_Digraph ref5;
	iscc_digraph_from_string("..#../#..../#..../##.../##.../", &ref5);

	assert_identical_digraph(&dg1, &ref1);
	assert_identical_digraph(&dg2, &ref2);
	assert_identical_digraph(&dg3, &ref3);
	assert_identical_digraph(&dg4, &ref4);
	assert_identical_digraph(&dg5, &ref5);

	assert_int_equal(dg6.vertices, 5);
	assert_int_equal(dg6.max_arcs, 0);
	assert_null(dg6.head);
	assert_non_null(dg6.tail_ptr);
	const iscc_Arci ref_tail_ptr[6] = { 0, 0, 0, 0, 0, 0 };
	assert_memory_equal(dg6.tail_ptr, ref_tail_ptr, 6 * sizeof(iscc_Arci));

	assert_free_digraph(&dg1);
	assert_free_digraph(&dg2);
	assert_free_digraph(&dg3);
	assert_free_digraph(&dg4);
	assert_free_digraph(&dg5);
	assert_free_digraph(&dg6);
	assert_free_digraph(&ref1);
	assert_free_digraph(&ref2);
	assert_free_digraph(&ref3);
	assert_free_digraph(&ref4);
	assert_free_digraph(&ref5);
}


void scc_ut_digraph_union_and_delete(void** state)
{
	(void) state;

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
	iscc_Digraph ut_make12_a;
	iscc_Digraph ut_make12_a_ttk;
	scc_ErrorCode ec1 = iscc_digraph_union_and_delete(2, sum_12, NULL, false, &ut_make12_a);
	scc_ErrorCode ec2 = iscc_digraph_union_and_delete(2, sum_12, tails_to_keep1, false, &ut_make12_a_ttk);
	assert_int_equal(ec1, SCC_ER_OK);
	assert_int_equal(ec2, SCC_ER_OK);
	assert_valid_digraph(&ut_make12_a, 4);
	assert_valid_digraph(&ut_make12_a_ttk, 4);
	iscc_Digraph ut_control12;
	iscc_Digraph ut_control12_ttk;
	iscc_digraph_from_string("...#/..##/.#.#/#.../", &ut_control12);
	iscc_digraph_from_string("...#/..##/..../..../", &ut_control12_ttk);
	assert_equal_digraph(&ut_make12_a, &ut_control12);
	assert_equal_digraph(&ut_make12_a_ttk, &ut_control12_ttk);

	iscc_Digraph ut_control13;
	iscc_digraph_from_string("...#/#..#/#..#/#.../", &ut_control13);

	const iscc_Digraph sum_13[2] = {ut_dg1, ut_dg3};
	iscc_Digraph ut_make13_a;
	iscc_Digraph ut_make13_a_ttk;
	scc_ErrorCode ec3 = iscc_digraph_union_and_delete(2, sum_13, NULL, false, &ut_make13_a);
	scc_ErrorCode ec4 = iscc_digraph_union_and_delete(2, sum_13, tails_to_keep2, false, &ut_make13_a_ttk);
	assert_int_equal(ec3, SCC_ER_OK);
	assert_int_equal(ec4, SCC_ER_OK);
	assert_valid_digraph(&ut_make13_a, 4);
	assert_valid_digraph(&ut_make13_a_ttk, 4);
	iscc_Digraph ut_control13_ttk;
	iscc_digraph_from_string("...#/..../#..#/..../", &ut_control13_ttk);
	assert_equal_digraph(&ut_make13_a, &ut_control13);
	assert_equal_digraph(&ut_make13_a_ttk, &ut_control13_ttk);

	const iscc_Digraph sum_31[2] = {ut_dg3, ut_dg1};
	iscc_Digraph ut_make31_a;
	iscc_Digraph ut_make31_a_ttk;
	scc_ErrorCode ec5 = iscc_digraph_union_and_delete(2, sum_31, NULL, false, &ut_make31_a);
	scc_ErrorCode ec6 = iscc_digraph_union_and_delete(2, sum_31, tails_to_keep3, false, &ut_make31_a_ttk);
	assert_int_equal(ec5, SCC_ER_OK);
	assert_int_equal(ec6, SCC_ER_OK);
	assert_valid_digraph(&ut_make31_a, 4);
	assert_valid_digraph(&ut_make31_a_ttk, 4);
	iscc_Digraph ut_control31_ttk;
	iscc_digraph_from_string("..../..../#..#/#.../", &ut_control31_ttk);
	assert_equal_digraph(&ut_make31_a, &ut_control13);
	assert_equal_digraph(&ut_make31_a_ttk, &ut_control31_ttk);

	iscc_Digraph ut_control123;
	iscc_digraph_from_string("...#/#.##/##.#/#.../", &ut_control123);

	const iscc_Digraph sum_123[3] = {ut_dg1, ut_dg2, ut_dg3};
	iscc_Digraph ut_make123;
	iscc_Digraph ut_make123_ttk;
	scc_ErrorCode ec7 = iscc_digraph_union_and_delete(3, sum_123, NULL, false, &ut_make123);
	scc_ErrorCode ec8 = iscc_digraph_union_and_delete(3, sum_123, tails_to_keep1, false, &ut_make123_ttk);
	assert_int_equal(ec7, SCC_ER_OK);
	assert_int_equal(ec8, SCC_ER_OK);
	assert_valid_digraph(&ut_make123, 4);
	assert_valid_digraph(&ut_make123_ttk, 4);
	iscc_Digraph ut_control123_ttk;
	iscc_digraph_from_string("...#/#.##/..../..../", &ut_control123_ttk);
	assert_equal_digraph(&ut_make123, &ut_control123);
	assert_equal_digraph(&ut_make123_ttk, &ut_control123_ttk);

	const iscc_Digraph sum_132[3] = {ut_dg1, ut_dg3, ut_dg2};
	iscc_Digraph ut_make132;
	iscc_Digraph ut_make132_ttk;
	scc_ErrorCode ec9 = iscc_digraph_union_and_delete(3, sum_132, NULL, false, &ut_make132);
	scc_ErrorCode ec10 = iscc_digraph_union_and_delete(3, sum_132, tails_to_keep2, false, &ut_make132_ttk);
	assert_int_equal(ec9, SCC_ER_OK);
	assert_int_equal(ec10, SCC_ER_OK);
	assert_valid_digraph(&ut_make132, 4);
	assert_valid_digraph(&ut_make132_ttk, 4);
	iscc_Digraph ut_control132_ttk;
	iscc_digraph_from_string("...#/..../##.#/..../", &ut_control132_ttk);
	assert_equal_digraph(&ut_make132, &ut_control123);
	assert_equal_digraph(&ut_make132_ttk, &ut_control132_ttk);

	const iscc_Digraph sum_213[3] = {ut_dg2, ut_dg1, ut_dg3};
	iscc_Digraph ut_make213;
	iscc_Digraph ut_make213_ttk;
	scc_ErrorCode ec11 = iscc_digraph_union_and_delete(3, sum_213, NULL, false, &ut_make213);
	scc_ErrorCode ec12 = iscc_digraph_union_and_delete(3, sum_213, tails_to_keep3, false, &ut_make213_ttk);
	assert_int_equal(ec11, SCC_ER_OK);
	assert_int_equal(ec12, SCC_ER_OK);
	assert_valid_digraph(&ut_make213, 4);
	assert_valid_digraph(&ut_make213_ttk, 4);
	iscc_Digraph ut_control213_ttk;
	iscc_digraph_from_string("..../..../##.#/#.../", &ut_control213_ttk);
	assert_equal_digraph(&ut_make213, &ut_control123);
	assert_equal_digraph(&ut_make213_ttk, &ut_control213_ttk);

	const iscc_Digraph sum_321[3] = {ut_dg3, ut_dg2, ut_dg1};
	iscc_Digraph ut_make321;
	iscc_Digraph ut_make321_ttk;
	scc_ErrorCode ec13 = iscc_digraph_union_and_delete(3, sum_321, NULL, false, &ut_make321);
	scc_ErrorCode ec14 = iscc_digraph_union_and_delete(3, sum_321, tails_to_keep1, false, &ut_make321_ttk);
	assert_int_equal(ec13, SCC_ER_OK);
	assert_int_equal(ec14, SCC_ER_OK);
	assert_valid_digraph(&ut_make321, 4);
	assert_valid_digraph(&ut_make321_ttk, 4);
	iscc_Digraph ut_control321_ttk;
	iscc_digraph_from_string("...#/#.##/..../..../", &ut_control321_ttk);
	assert_equal_digraph(&ut_make321, &ut_control123);
	assert_equal_digraph(&ut_make321_ttk, &ut_control321_ttk);

	assert_free_digraph(&ut_dg1);
	assert_free_digraph(&ut_dg2);
	assert_free_digraph(&ut_dg3);
	assert_free_digraph(&ut_make12_a);
	assert_free_digraph(&ut_make12_a_ttk);
	assert_free_digraph(&ut_make13_a);
	assert_free_digraph(&ut_make13_a_ttk);
	assert_free_digraph(&ut_make31_a);
	assert_free_digraph(&ut_make31_a_ttk);
	assert_free_digraph(&ut_make123);
	assert_free_digraph(&ut_make123_ttk);
	assert_free_digraph(&ut_make132);
	assert_free_digraph(&ut_make132_ttk);
	assert_free_digraph(&ut_make213);
	assert_free_digraph(&ut_make213_ttk);
	assert_free_digraph(&ut_make321);
	assert_free_digraph(&ut_make321_ttk);
	assert_free_digraph(&ut_control12);
	assert_free_digraph(&ut_control12_ttk);
	assert_free_digraph(&ut_control13);
	assert_free_digraph(&ut_control13_ttk);
	assert_free_digraph(&ut_control31_ttk);
	assert_free_digraph(&ut_control123);
	assert_free_digraph(&ut_control123_ttk);
	assert_free_digraph(&ut_control132_ttk);
	assert_free_digraph(&ut_control213_ttk);
	assert_free_digraph(&ut_control321_ttk);
}


void scc_ut_digraph_union_and_delete_empty(void** state)
{
	(void) state;

	iscc_Digraph ut_dg4;
	iscc_digraph_from_string("#..#/.#.#/..##/...#/", &ut_dg4);
	iscc_Digraph ut_dg5;
	iscc_digraph_from_string("..../..../..../..../", &ut_dg5);
	iscc_Digraph ut_dg6;
	iscc_empty_digraph(4, 0, &ut_dg6);
	const bool tails_to_keep4[4] = { true, true, false, false };

	const iscc_Digraph sum_45[2] = {ut_dg4, ut_dg5};
	iscc_Digraph ut_make45;
	iscc_Digraph ut_make45_ttk;
	scc_ErrorCode ec15 = iscc_digraph_union_and_delete(2, sum_45, NULL, false, &ut_make45);
	scc_ErrorCode ec16 = iscc_digraph_union_and_delete(2, sum_45, tails_to_keep4, false, &ut_make45_ttk);
	assert_int_equal(ec15, SCC_ER_OK);
	assert_int_equal(ec16, SCC_ER_OK);
	assert_valid_digraph(&ut_make45, 4);
	assert_valid_digraph(&ut_make45_ttk, 4);
	iscc_Digraph ut_control45;
	iscc_Digraph ut_control45_ttk;
	iscc_digraph_from_string("...#/...#/...#/..../", &ut_control45);
	iscc_digraph_from_string("...#/...#/..../..../", &ut_control45_ttk);
	assert_equal_digraph(&ut_make45, &ut_control45);
	assert_equal_digraph(&ut_make45_ttk, &ut_control45_ttk);

	const iscc_Digraph sum_46[2] = {ut_dg4, ut_dg6};
	iscc_Digraph ut_make46;
	iscc_Digraph ut_make46_ttk;
	scc_ErrorCode ec17 = iscc_digraph_union_and_delete(2, sum_46, NULL, false, &ut_make46);
	scc_ErrorCode ec18 = iscc_digraph_union_and_delete(2, sum_46, tails_to_keep4, false, &ut_make46_ttk);
	assert_int_equal(ec17, SCC_ER_OK);
	assert_int_equal(ec18, SCC_ER_OK);
	assert_valid_digraph(&ut_make46, 4);
	assert_valid_digraph(&ut_make46_ttk, 4);
	iscc_Digraph ut_control46;
	iscc_Digraph ut_control46_ttk;
	iscc_digraph_from_string("...#/...#/...#/..../", &ut_control46);
	iscc_digraph_from_string("...#/...#/..../..../", &ut_control46_ttk);
	assert_equal_digraph(&ut_make46, &ut_control46);
	assert_equal_digraph(&ut_make46_ttk, &ut_control46_ttk);

	const iscc_Digraph sum_54[2] = {ut_dg5, ut_dg4};
	iscc_Digraph ut_make54;
	iscc_Digraph ut_make54_ttk;
	scc_ErrorCode ec19 = iscc_digraph_union_and_delete(2, sum_54, NULL, false, &ut_make54);
	scc_ErrorCode ec20 = iscc_digraph_union_and_delete(2, sum_54, tails_to_keep4, false, &ut_make54_ttk);
	assert_int_equal(ec19, SCC_ER_OK);
	assert_int_equal(ec20, SCC_ER_OK);
	assert_valid_digraph(&ut_make54, 4);
	assert_valid_digraph(&ut_make54_ttk, 4);
	iscc_Digraph ut_control54;
	iscc_Digraph ut_control54_ttk;
	iscc_digraph_from_string("...#/...#/...#/..../", &ut_control54);
	iscc_digraph_from_string("...#/...#/..../..../", &ut_control54_ttk);
	assert_equal_digraph(&ut_make54, &ut_control54);
	assert_equal_digraph(&ut_make54_ttk, &ut_control54_ttk);

	const iscc_Digraph sum_64[2] = {ut_dg6, ut_dg4};
	iscc_Digraph ut_make64;
	iscc_Digraph ut_make64_ttk;
	scc_ErrorCode ec21 = iscc_digraph_union_and_delete(2, sum_64, NULL, false, &ut_make64);
	scc_ErrorCode ec22 = iscc_digraph_union_and_delete(2, sum_64, tails_to_keep4, false, &ut_make64_ttk);
	assert_int_equal(ec21, SCC_ER_OK);
	assert_int_equal(ec22, SCC_ER_OK);
	assert_valid_digraph(&ut_make64, 4);
	assert_valid_digraph(&ut_make64_ttk, 4);
	iscc_Digraph ut_control64;
	iscc_Digraph ut_control64_ttk;
	iscc_digraph_from_string("...#/...#/...#/..../", &ut_control64);
	iscc_digraph_from_string("...#/...#/..../..../", &ut_control64_ttk);
	assert_equal_digraph(&ut_make64, &ut_control64);
	assert_equal_digraph(&ut_make64_ttk, &ut_control64_ttk);

	const iscc_Digraph sum_56[2] = {ut_dg5, ut_dg6};
	iscc_Digraph ut_make56;
	iscc_Digraph ut_make56_ttk;
	scc_ErrorCode ec23 = iscc_digraph_union_and_delete(2, sum_56, NULL, false, &ut_make56);
	scc_ErrorCode ec24 = iscc_digraph_union_and_delete(2, sum_56, tails_to_keep4, false, &ut_make56_ttk);
	assert_int_equal(ec23, SCC_ER_OK);
	assert_int_equal(ec24, SCC_ER_OK);
	assert_valid_digraph(&ut_make56, 4);
	assert_valid_digraph(&ut_make56_ttk, 4);
	iscc_Digraph ut_control56;
	iscc_Digraph ut_control56_ttk;
	iscc_digraph_from_string("..../..../..../..../", &ut_control56);
	iscc_digraph_from_string("..../..../..../..../", &ut_control56_ttk);
	assert_equal_digraph(&ut_make56, &ut_control56);
	assert_equal_digraph(&ut_make56_ttk, &ut_control56_ttk);

	const iscc_Digraph sum_65[2] = {ut_dg6, ut_dg5};
	iscc_Digraph ut_make65;
	iscc_Digraph ut_make65_ttk;
	scc_ErrorCode ec25 = iscc_digraph_union_and_delete(2, sum_65, NULL, false, &ut_make65);
	scc_ErrorCode ec26 = iscc_digraph_union_and_delete(2, sum_65, tails_to_keep4, false, &ut_make65_ttk);
	assert_int_equal(ec25, SCC_ER_OK);
	assert_int_equal(ec26, SCC_ER_OK);
	assert_valid_digraph(&ut_make65, 4);
	assert_valid_digraph(&ut_make65_ttk, 4);
	iscc_Digraph ut_control65;
	iscc_Digraph ut_control65_ttk;
	iscc_digraph_from_string("..../..../..../..../", &ut_control65);
	iscc_digraph_from_string("..../..../..../..../", &ut_control65_ttk);
	assert_equal_digraph(&ut_make65, &ut_control65);
	assert_equal_digraph(&ut_make65_ttk, &ut_control65_ttk);

	const iscc_Digraph sum_55[2] = {ut_dg5, ut_dg5};
	iscc_Digraph ut_make55;
	iscc_Digraph ut_make55_ttk;
	scc_ErrorCode ec27 = iscc_digraph_union_and_delete(2, sum_55, NULL, false, &ut_make55);
	scc_ErrorCode ec28 = iscc_digraph_union_and_delete(2, sum_55, tails_to_keep4, false, &ut_make55_ttk);
	assert_int_equal(ec27, SCC_ER_OK);
	assert_int_equal(ec28, SCC_ER_OK);
	assert_valid_digraph(&ut_make55, 4);
	assert_valid_digraph(&ut_make55_ttk, 4);
	iscc_Digraph ut_control55;
	iscc_Digraph ut_control55_ttk;
	iscc_digraph_from_string("..../..../..../..../", &ut_control55);
	iscc_digraph_from_string("..../..../..../..../", &ut_control55_ttk);
	assert_equal_digraph(&ut_make55, &ut_control55);
	assert_equal_digraph(&ut_make55_ttk, &ut_control55_ttk);

	const iscc_Digraph sum_66[2] = {ut_dg6, ut_dg6};
	iscc_Digraph ut_make66;
	iscc_Digraph ut_make66_ttk;
	scc_ErrorCode ec29 = iscc_digraph_union_and_delete(2, sum_66, NULL, false, &ut_make66);
	scc_ErrorCode ec30 = iscc_digraph_union_and_delete(2, sum_66, tails_to_keep4, false, &ut_make66_ttk);
	assert_int_equal(ec29, SCC_ER_OK);
	assert_int_equal(ec30, SCC_ER_OK);
	assert_valid_digraph(&ut_make66, 4);
	assert_valid_digraph(&ut_make66_ttk, 4);
	iscc_Digraph ut_control66;
	iscc_Digraph ut_control66_ttk;
	iscc_digraph_from_string("..../..../..../..../", &ut_control66);
	iscc_digraph_from_string("..../..../..../..../", &ut_control66_ttk);
	assert_equal_digraph(&ut_make66, &ut_control66);
	assert_equal_digraph(&ut_make66_ttk, &ut_control66_ttk);

	assert_free_digraph(&ut_dg4);
	assert_free_digraph(&ut_dg5);
	assert_free_digraph(&ut_dg6);
	assert_free_digraph(&ut_make45);
	assert_free_digraph(&ut_make45_ttk);
	assert_free_digraph(&ut_make46);
	assert_free_digraph(&ut_make46_ttk);
	assert_free_digraph(&ut_make54);
	assert_free_digraph(&ut_make54_ttk);
	assert_free_digraph(&ut_make64);
	assert_free_digraph(&ut_make64_ttk);
	assert_free_digraph(&ut_make56);
	assert_free_digraph(&ut_make56_ttk);
	assert_free_digraph(&ut_make65);
	assert_free_digraph(&ut_make65_ttk);
	assert_free_digraph(&ut_make55);
	assert_free_digraph(&ut_make55_ttk);
	assert_free_digraph(&ut_make66);
	assert_free_digraph(&ut_make66_ttk);
	assert_free_digraph(&ut_control45);
	assert_free_digraph(&ut_control45_ttk);
	assert_free_digraph(&ut_control46);
	assert_free_digraph(&ut_control46_ttk);
	assert_free_digraph(&ut_control54);
	assert_free_digraph(&ut_control54_ttk);
	assert_free_digraph(&ut_control64);
	assert_free_digraph(&ut_control64_ttk);
	assert_free_digraph(&ut_control56);
	assert_free_digraph(&ut_control56_ttk);
	assert_free_digraph(&ut_control65);
	assert_free_digraph(&ut_control65_ttk);
	assert_free_digraph(&ut_control55);
	assert_free_digraph(&ut_control55_ttk);
	assert_free_digraph(&ut_control66);
	assert_free_digraph(&ut_control66_ttk);
}


void scc_ut_digraph_union_and_delete_single(void** state)
{
	(void) state;

	iscc_Digraph ut_dg7;
	iscc_digraph_from_string("#..#/.#.#/..##/...#/", &ut_dg7);
	const bool tails_to_keep5[4] = { true, true, false, false };

	const iscc_Digraph sum_7[1] = {ut_dg7};
	iscc_Digraph ut_make7;
	iscc_Digraph ut_make7_ttk;
	scc_ErrorCode ec31 = iscc_digraph_union_and_delete(1, sum_7, NULL, false, &ut_make7);
	scc_ErrorCode ec32 = iscc_digraph_union_and_delete(1, sum_7, tails_to_keep5, false, &ut_make7_ttk);
	assert_int_equal(ec31, SCC_ER_OK);
	assert_int_equal(ec32, SCC_ER_OK);
	assert_valid_digraph(&ut_make7, 4);
	assert_valid_digraph(&ut_make7_ttk, 4);
	iscc_Digraph ut_control7;
	iscc_Digraph ut_control7_ttk;
	iscc_digraph_from_string("...#/...#/...#/..../", &ut_control7);
	iscc_digraph_from_string("...#/...#/..../..../", &ut_control7_ttk);
	assert_equal_digraph(&ut_make7, &ut_control7);
	assert_equal_digraph(&ut_make7_ttk, &ut_control7_ttk);

	assert_free_digraph(&ut_dg7);
	assert_free_digraph(&ut_make7);
	assert_free_digraph(&ut_make7_ttk);
	assert_free_digraph(&ut_control7);
	assert_free_digraph(&ut_control7_ttk);
}


void scc_ut_digraph_union_and_delete_keep_loops(void** state)
{
	(void) state;

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
	iscc_Digraph ut_make12_a;
	iscc_Digraph ut_make12_a_ttk;
	scc_ErrorCode ec1 = iscc_digraph_union_and_delete(2, sum_12, NULL, true, &ut_make12_a);
	scc_ErrorCode ec2 = iscc_digraph_union_and_delete(2, sum_12, tails_to_keep1, true, &ut_make12_a_ttk);
	assert_int_equal(ec1, SCC_ER_OK);
	assert_int_equal(ec2, SCC_ER_OK);
	assert_valid_digraph(&ut_make12_a, 4);
	assert_valid_digraph(&ut_make12_a_ttk, 4);
	iscc_Digraph ut_control12;
	iscc_Digraph ut_control12_ttk;
	iscc_digraph_from_string("#..#/.###/.###/#..#/", &ut_control12);
	iscc_digraph_from_string("#..#/.###/..../..../", &ut_control12_ttk);
	assert_equal_digraph(&ut_make12_a, &ut_control12);
	assert_equal_digraph(&ut_make12_a_ttk, &ut_control12_ttk);

	iscc_Digraph ut_control13;
	iscc_digraph_from_string("#..#/##.#/#.##/#..#/", &ut_control13);

	const iscc_Digraph sum_13[2] = {ut_dg1, ut_dg3};
	iscc_Digraph ut_make13_a;
	iscc_Digraph ut_make13_a_ttk;
	scc_ErrorCode ec3 = iscc_digraph_union_and_delete(2, sum_13, NULL, true, &ut_make13_a);
	scc_ErrorCode ec4 = iscc_digraph_union_and_delete(2, sum_13, tails_to_keep2, true, &ut_make13_a_ttk);
	assert_int_equal(ec3, SCC_ER_OK);
	assert_int_equal(ec4, SCC_ER_OK);
	assert_valid_digraph(&ut_make13_a, 4);
	assert_valid_digraph(&ut_make13_a_ttk, 4);
	iscc_Digraph ut_control13_ttk;
	iscc_digraph_from_string("#..#/..../#.##/..../", &ut_control13_ttk);
	assert_equal_digraph(&ut_make13_a, &ut_control13);
	assert_equal_digraph(&ut_make13_a_ttk, &ut_control13_ttk);

	const iscc_Digraph sum_31[2] = {ut_dg3, ut_dg1};
	iscc_Digraph ut_make31_a;
	iscc_Digraph ut_make31_a_ttk;
	scc_ErrorCode ec5 = iscc_digraph_union_and_delete(2, sum_31, NULL, true, &ut_make31_a);
	scc_ErrorCode ec6 = iscc_digraph_union_and_delete(2, sum_31, tails_to_keep3, true, &ut_make31_a_ttk);
	assert_int_equal(ec5, SCC_ER_OK);
	assert_int_equal(ec6, SCC_ER_OK);
	assert_valid_digraph(&ut_make31_a, 4);
	assert_valid_digraph(&ut_make31_a_ttk, 4);
	iscc_Digraph ut_control31_ttk;
	iscc_digraph_from_string("..../..../#.##/#..#/", &ut_control31_ttk);
	assert_equal_digraph(&ut_make31_a, &ut_control13);
	assert_equal_digraph(&ut_make31_a_ttk, &ut_control31_ttk);

	iscc_Digraph ut_control123;
	iscc_digraph_from_string("#..#/####/####/#..#/", &ut_control123);

	const iscc_Digraph sum_123[3] = {ut_dg1, ut_dg2, ut_dg3};
	iscc_Digraph ut_make123;
	iscc_Digraph ut_make123_ttk;
	scc_ErrorCode ec7 = iscc_digraph_union_and_delete(3, sum_123, NULL, true, &ut_make123);
	scc_ErrorCode ec8 = iscc_digraph_union_and_delete(3, sum_123, tails_to_keep1, true, &ut_make123_ttk);
	assert_int_equal(ec7, SCC_ER_OK);
	assert_int_equal(ec8, SCC_ER_OK);
	assert_valid_digraph(&ut_make123, 4);
	assert_valid_digraph(&ut_make123_ttk, 4);
	iscc_Digraph ut_control123_ttk;
	iscc_digraph_from_string("#..#/####/..../..../", &ut_control123_ttk);
	assert_equal_digraph(&ut_make123, &ut_control123);
	assert_equal_digraph(&ut_make123_ttk, &ut_control123_ttk);

	const iscc_Digraph sum_132[3] = {ut_dg1, ut_dg3, ut_dg2};
	iscc_Digraph ut_make132;
	iscc_Digraph ut_make132_ttk;
	scc_ErrorCode ec9 = iscc_digraph_union_and_delete(3, sum_132, NULL, true, &ut_make132);
	scc_ErrorCode ec10 = iscc_digraph_union_and_delete(3, sum_132, tails_to_keep2, true, &ut_make132_ttk);
	assert_int_equal(ec9, SCC_ER_OK);
	assert_int_equal(ec10, SCC_ER_OK);
	assert_valid_digraph(&ut_make132, 4);
	assert_valid_digraph(&ut_make132_ttk, 4);
	iscc_Digraph ut_control132_ttk;
	iscc_digraph_from_string("#..#/..../####/..../", &ut_control132_ttk);
	assert_equal_digraph(&ut_make132, &ut_control123);
	assert_equal_digraph(&ut_make132_ttk, &ut_control132_ttk);

	const iscc_Digraph sum_213[3] = {ut_dg2, ut_dg1, ut_dg3};
	iscc_Digraph ut_make213;
	iscc_Digraph ut_make213_ttk;
	scc_ErrorCode ec11 = iscc_digraph_union_and_delete(3, sum_213, NULL, true, &ut_make213);
	scc_ErrorCode ec12 = iscc_digraph_union_and_delete(3, sum_213, tails_to_keep3, true, &ut_make213_ttk);
	assert_int_equal(ec11, SCC_ER_OK);
	assert_int_equal(ec12, SCC_ER_OK);
	assert_valid_digraph(&ut_make213, 4);
	assert_valid_digraph(&ut_make213_ttk, 4);
	iscc_Digraph ut_control213_ttk;
	iscc_digraph_from_string("..../..../####/#..#/", &ut_control213_ttk);
	assert_equal_digraph(&ut_make213, &ut_control123);
	assert_equal_digraph(&ut_make213_ttk, &ut_control213_ttk);

	const iscc_Digraph sum_321[3] = {ut_dg3, ut_dg2, ut_dg1};
	iscc_Digraph ut_make321;
	iscc_Digraph ut_make321_ttk;
	scc_ErrorCode ec13 = iscc_digraph_union_and_delete(3, sum_321, NULL, true, &ut_make321);
	scc_ErrorCode ec14 = iscc_digraph_union_and_delete(3, sum_321, tails_to_keep1, true, &ut_make321_ttk);
	assert_int_equal(ec13, SCC_ER_OK);
	assert_int_equal(ec14, SCC_ER_OK);
	assert_valid_digraph(&ut_make321, 4);
	assert_valid_digraph(&ut_make321_ttk, 4);
	iscc_Digraph ut_control321_ttk;
	iscc_digraph_from_string("#..#/####/..../..../", &ut_control321_ttk);
	assert_equal_digraph(&ut_make321, &ut_control123);
	assert_equal_digraph(&ut_make321_ttk, &ut_control321_ttk);

	assert_free_digraph(&ut_dg1);
	assert_free_digraph(&ut_dg2);
	assert_free_digraph(&ut_dg3);
	assert_free_digraph(&ut_make12_a);
	assert_free_digraph(&ut_make12_a_ttk);
	assert_free_digraph(&ut_make13_a);
	assert_free_digraph(&ut_make13_a_ttk);
	assert_free_digraph(&ut_make31_a);
	assert_free_digraph(&ut_make31_a_ttk);
	assert_free_digraph(&ut_make123);
	assert_free_digraph(&ut_make123_ttk);
	assert_free_digraph(&ut_make132);
	assert_free_digraph(&ut_make132_ttk);
	assert_free_digraph(&ut_make213);
	assert_free_digraph(&ut_make213_ttk);
	assert_free_digraph(&ut_make321);
	assert_free_digraph(&ut_make321_ttk);
	assert_free_digraph(&ut_control12);
	assert_free_digraph(&ut_control12_ttk);
	assert_free_digraph(&ut_control13);
	assert_free_digraph(&ut_control13_ttk);
	assert_free_digraph(&ut_control31_ttk);
	assert_free_digraph(&ut_control123);
	assert_free_digraph(&ut_control123_ttk);
	assert_free_digraph(&ut_control132_ttk);
	assert_free_digraph(&ut_control213_ttk);
	assert_free_digraph(&ut_control321_ttk);
}


void scc_ut_digraph_union_and_delete_keep_loops_empty(void** state)
{
	(void) state;

	iscc_Digraph ut_dg4;
	iscc_digraph_from_string("#..#/.#.#/..##/...#/", &ut_dg4);
	iscc_Digraph ut_dg5;
	iscc_digraph_from_string("..../..../..../..../", &ut_dg5);
	iscc_Digraph ut_dg6;
	iscc_empty_digraph(4, 0, &ut_dg6);
	const bool tails_to_keep4[4] = { true, true, false, false };

	const iscc_Digraph sum_45[2] = {ut_dg4, ut_dg5};
	iscc_Digraph ut_make45;
	iscc_Digraph ut_make45_ttk;
	scc_ErrorCode ec15 = iscc_digraph_union_and_delete(2, sum_45, NULL, true, &ut_make45);
	scc_ErrorCode ec16 = iscc_digraph_union_and_delete(2, sum_45, tails_to_keep4, true, &ut_make45_ttk);
	assert_int_equal(ec15, SCC_ER_OK);
	assert_int_equal(ec16, SCC_ER_OK);
	assert_valid_digraph(&ut_make45, 4);
	assert_valid_digraph(&ut_make45_ttk, 4);
	iscc_Digraph ut_control45;
	iscc_Digraph ut_control45_ttk;
	iscc_digraph_from_string("#..#/.#.#/..##/...#/", &ut_control45);
	iscc_digraph_from_string("#..#/.#.#/..../..../", &ut_control45_ttk);
	assert_equal_digraph(&ut_make45, &ut_control45);
	assert_equal_digraph(&ut_make45_ttk, &ut_control45_ttk);

	const iscc_Digraph sum_46[2] = {ut_dg4, ut_dg6};
	iscc_Digraph ut_make46;
	iscc_Digraph ut_make46_ttk;
	scc_ErrorCode ec17 = iscc_digraph_union_and_delete(2, sum_46, NULL, true, &ut_make46);
	scc_ErrorCode ec18 = iscc_digraph_union_and_delete(2, sum_46, tails_to_keep4, true, &ut_make46_ttk);
	assert_int_equal(ec17, SCC_ER_OK);
	assert_int_equal(ec18, SCC_ER_OK);
	assert_valid_digraph(&ut_make46, 4);
	assert_valid_digraph(&ut_make46_ttk, 4);
	iscc_Digraph ut_control46;
	iscc_Digraph ut_control46_ttk;
	iscc_digraph_from_string("#..#/.#.#/..##/...#/", &ut_control46);
	iscc_digraph_from_string("#..#/.#.#/..../..../", &ut_control46_ttk);
	assert_equal_digraph(&ut_make46, &ut_control46);
	assert_equal_digraph(&ut_make46_ttk, &ut_control46_ttk);

	const iscc_Digraph sum_54[2] = {ut_dg5, ut_dg4};
	iscc_Digraph ut_make54;
	iscc_Digraph ut_make54_ttk;
	scc_ErrorCode ec19 = iscc_digraph_union_and_delete(2, sum_54, NULL, true, &ut_make54);
	scc_ErrorCode ec20 = iscc_digraph_union_and_delete(2, sum_54, tails_to_keep4, true, &ut_make54_ttk);
	assert_int_equal(ec19, SCC_ER_OK);
	assert_int_equal(ec20, SCC_ER_OK);
	assert_valid_digraph(&ut_make54, 4);
	assert_valid_digraph(&ut_make54_ttk, 4);
	iscc_Digraph ut_control54;
	iscc_Digraph ut_control54_ttk;
	iscc_digraph_from_string("#..#/.#.#/..##/...#/", &ut_control54);
	iscc_digraph_from_string("#..#/.#.#/..../..../", &ut_control54_ttk);
	assert_equal_digraph(&ut_make54, &ut_control54);
	assert_equal_digraph(&ut_make54_ttk, &ut_control54_ttk);

	const iscc_Digraph sum_64[2] = {ut_dg6, ut_dg4};
	iscc_Digraph ut_make64;
	iscc_Digraph ut_make64_ttk;
	scc_ErrorCode ec21 = iscc_digraph_union_and_delete(2, sum_64, NULL, true, &ut_make64);
	scc_ErrorCode ec22 = iscc_digraph_union_and_delete(2, sum_64, tails_to_keep4, true, &ut_make64_ttk);
	assert_int_equal(ec21, SCC_ER_OK);
	assert_int_equal(ec22, SCC_ER_OK);
	assert_valid_digraph(&ut_make64, 4);
	assert_valid_digraph(&ut_make64_ttk, 4);
	iscc_Digraph ut_control64;
	iscc_Digraph ut_control64_ttk;
	iscc_digraph_from_string("#..#/.#.#/..##/...#/", &ut_control64);
	iscc_digraph_from_string("#..#/.#.#/..../..../", &ut_control64_ttk);
	assert_equal_digraph(&ut_make64, &ut_control64);
	assert_equal_digraph(&ut_make64_ttk, &ut_control64_ttk);

	const iscc_Digraph sum_56[2] = {ut_dg5, ut_dg6};
	iscc_Digraph ut_make56;
	iscc_Digraph ut_make56_ttk;
	scc_ErrorCode ec23 = iscc_digraph_union_and_delete(2, sum_56, NULL, true, &ut_make56);
	scc_ErrorCode ec24 = iscc_digraph_union_and_delete(2, sum_56, tails_to_keep4, true, &ut_make56_ttk);
	assert_int_equal(ec23, SCC_ER_OK);
	assert_int_equal(ec24, SCC_ER_OK);
	assert_valid_digraph(&ut_make56, 4);
	assert_valid_digraph(&ut_make56_ttk, 4);
	iscc_Digraph ut_control56;
	iscc_Digraph ut_control56_ttk;
	iscc_digraph_from_string("..../..../..../..../", &ut_control56);
	iscc_digraph_from_string("..../..../..../..../", &ut_control56_ttk);
	assert_equal_digraph(&ut_make56, &ut_control56);
	assert_equal_digraph(&ut_make56_ttk, &ut_control56_ttk);

	const iscc_Digraph sum_65[2] = {ut_dg6, ut_dg5};
	iscc_Digraph ut_make65;
	iscc_Digraph ut_make65_ttk;
	scc_ErrorCode ec25 = iscc_digraph_union_and_delete(2, sum_65, NULL, true, &ut_make65);
	scc_ErrorCode ec26 = iscc_digraph_union_and_delete(2, sum_65, tails_to_keep4, true, &ut_make65_ttk);
	assert_int_equal(ec25, SCC_ER_OK);
	assert_int_equal(ec26, SCC_ER_OK);
	assert_valid_digraph(&ut_make65, 4);
	assert_valid_digraph(&ut_make65_ttk, 4);
	iscc_Digraph ut_control65;
	iscc_Digraph ut_control65_ttk;
	iscc_digraph_from_string("..../..../..../..../", &ut_control65);
	iscc_digraph_from_string("..../..../..../..../", &ut_control65_ttk);
	assert_equal_digraph(&ut_make65, &ut_control65);
	assert_equal_digraph(&ut_make65_ttk, &ut_control65_ttk);

	const iscc_Digraph sum_55[2] = {ut_dg5, ut_dg5};
	iscc_Digraph ut_make55;
	iscc_Digraph ut_make55_ttk;
	scc_ErrorCode ec27 = iscc_digraph_union_and_delete(2, sum_55, NULL, true, &ut_make55);
	scc_ErrorCode ec28 = iscc_digraph_union_and_delete(2, sum_55, tails_to_keep4, true, &ut_make55_ttk);
	assert_int_equal(ec27, SCC_ER_OK);
	assert_int_equal(ec28, SCC_ER_OK);
	assert_valid_digraph(&ut_make55, 4);
	assert_valid_digraph(&ut_make55_ttk, 4);
	iscc_Digraph ut_control55;
	iscc_Digraph ut_control55_ttk;
	iscc_digraph_from_string("..../..../..../..../", &ut_control55);
	iscc_digraph_from_string("..../..../..../..../", &ut_control55_ttk);
	assert_equal_digraph(&ut_make55, &ut_control55);
	assert_equal_digraph(&ut_make55_ttk, &ut_control55_ttk);

	const iscc_Digraph sum_66[2] = {ut_dg6, ut_dg6};
	iscc_Digraph ut_make66;
	iscc_Digraph ut_make66_ttk;
	scc_ErrorCode ec29 = iscc_digraph_union_and_delete(2, sum_66, NULL, true, &ut_make66);
	scc_ErrorCode ec30 = iscc_digraph_union_and_delete(2, sum_66, tails_to_keep4, true, &ut_make66_ttk);
	assert_int_equal(ec29, SCC_ER_OK);
	assert_int_equal(ec30, SCC_ER_OK);
	assert_valid_digraph(&ut_make66, 4);
	assert_valid_digraph(&ut_make66_ttk, 4);
	iscc_Digraph ut_control66;
	iscc_Digraph ut_control66_ttk;
	iscc_digraph_from_string("..../..../..../..../", &ut_control66);
	iscc_digraph_from_string("..../..../..../..../", &ut_control66_ttk);
	assert_equal_digraph(&ut_make66, &ut_control66);
	assert_equal_digraph(&ut_make66_ttk, &ut_control66_ttk);

	assert_free_digraph(&ut_dg4);
	assert_free_digraph(&ut_dg5);
	assert_free_digraph(&ut_dg6);
	assert_free_digraph(&ut_make45);
	assert_free_digraph(&ut_make45_ttk);
	assert_free_digraph(&ut_make46);
	assert_free_digraph(&ut_make46_ttk);
	assert_free_digraph(&ut_make54);
	assert_free_digraph(&ut_make54_ttk);
	assert_free_digraph(&ut_make64);
	assert_free_digraph(&ut_make64_ttk);
	assert_free_digraph(&ut_make56);
	assert_free_digraph(&ut_make56_ttk);
	assert_free_digraph(&ut_make65);
	assert_free_digraph(&ut_make65_ttk);
	assert_free_digraph(&ut_make55);
	assert_free_digraph(&ut_make55_ttk);
	assert_free_digraph(&ut_make66);
	assert_free_digraph(&ut_make66_ttk);
	assert_free_digraph(&ut_control45);
	assert_free_digraph(&ut_control45_ttk);
	assert_free_digraph(&ut_control46);
	assert_free_digraph(&ut_control46_ttk);
	assert_free_digraph(&ut_control54);
	assert_free_digraph(&ut_control54_ttk);
	assert_free_digraph(&ut_control64);
	assert_free_digraph(&ut_control64_ttk);
	assert_free_digraph(&ut_control56);
	assert_free_digraph(&ut_control56_ttk);
	assert_free_digraph(&ut_control65);
	assert_free_digraph(&ut_control65_ttk);
	assert_free_digraph(&ut_control55);
	assert_free_digraph(&ut_control55_ttk);
	assert_free_digraph(&ut_control66);
	assert_free_digraph(&ut_control66_ttk);
}


void scc_ut_digraph_union_and_delete_keep_loops_single(void** state)
{
	(void) state;

	iscc_Digraph ut_dg7;
	iscc_digraph_from_string("#..#/.#.#/..##/...#/", &ut_dg7);
	const bool tails_to_keep5[4] = { true, true, false, false };

	const iscc_Digraph sum_7[1] = {ut_dg7};
	iscc_Digraph ut_make7;
	iscc_Digraph ut_make7_ttk;
	scc_ErrorCode ec31 = iscc_digraph_union_and_delete(1, sum_7, NULL, true, &ut_make7);
	scc_ErrorCode ec32 = iscc_digraph_union_and_delete(1, sum_7, tails_to_keep5, true, &ut_make7_ttk);
	assert_int_equal(ec31, SCC_ER_OK);
	assert_int_equal(ec32, SCC_ER_OK);
	assert_valid_digraph(&ut_make7, 4);
	assert_valid_digraph(&ut_make7_ttk, 4);
	iscc_Digraph ut_control7;
	iscc_Digraph ut_control7_ttk;
	iscc_digraph_from_string("#..#/.#.#/..##/...#/", &ut_control7);
	iscc_digraph_from_string("#..#/.#.#/..../..../", &ut_control7_ttk);
	assert_equal_digraph(&ut_make7, &ut_control7);
	assert_equal_digraph(&ut_make7_ttk, &ut_control7_ttk);

	assert_free_digraph(&ut_dg7);
	assert_free_digraph(&ut_make7);
	assert_free_digraph(&ut_make7_ttk);
	assert_free_digraph(&ut_control7);
	assert_free_digraph(&ut_control7_ttk);
}


void scc_ut_digraph_difference(void** state)
{
	(void) state;

	iscc_Digraph minuend_dg1;
	iscc_digraph_from_string("#####/#####/#####/#####/#####/", &minuend_dg1);
	iscc_Digraph subtrahend_dg1;
	iscc_digraph_from_string("#####/#####/#####/#####/#####/", &subtrahend_dg1);
	iscc_Digraph ref_dg1;
	iscc_digraph_from_string("...../...../...../...../...../", &ref_dg1);
	scc_ErrorCode ec1 = iscc_digraph_difference(&minuend_dg1, &subtrahend_dg1, 5);
	assert_equal_digraph(&minuend_dg1, &ref_dg1);
	assert_int_equal(ec1, SCC_ER_OK);

	iscc_Digraph minuend_dg2;
	iscc_digraph_from_string("#####/#####/#####/#####/#####/", &minuend_dg2);
	iscc_Digraph subtrahend_dg2;
	iscc_digraph_from_string("...##/##..#/.#.#./#.#.#/..#../", &subtrahend_dg2);
	iscc_Digraph ref_dg2;
	iscc_digraph_from_string("###../..##./#.#.#/.#.#./##.##/", &ref_dg2);
	scc_ErrorCode ec2 = iscc_digraph_difference(&minuend_dg2, &subtrahend_dg2, 5);
	assert_equal_digraph(&minuend_dg2, &ref_dg2);
	assert_int_equal(ec2, SCC_ER_OK);

	iscc_Digraph minuend_dg2b;
	iscc_digraph_from_string("#####/#####/#####/#####/#####/", &minuend_dg2b);
	iscc_Digraph subtrahend_dg2b;
	iscc_digraph_from_string("...##/##..#/.#.#./#.#.#/..#../", &subtrahend_dg2b);
	iscc_Digraph ref_dg2b;
	iscc_digraph_from_string("##.../..##./#.#../.#.#./##.../", &ref_dg2b);
	scc_ErrorCode ec2b = iscc_digraph_difference(&minuend_dg2b, &subtrahend_dg2b, 2);
	assert_equal_digraph(&minuend_dg2b, &ref_dg2b);
	assert_int_equal(ec2b, SCC_ER_OK);

	iscc_Digraph minuend_dg3;
	iscc_digraph_from_string("#.#../#.##./#.#../##.../##..#/", &minuend_dg3);
	iscc_Digraph subtrahend_dg3;
	iscc_digraph_from_string("#####/#####/#####/#####/#####/", &subtrahend_dg3);
	iscc_Digraph ref_dg3;
	iscc_digraph_from_string("...../...../...../...../...../", &ref_dg3);
	scc_ErrorCode ec3 = iscc_digraph_difference(&minuend_dg3, &subtrahend_dg3, 5);
	assert_equal_digraph(&minuend_dg3, &ref_dg3);
	assert_int_equal(ec3, SCC_ER_OK);

	iscc_Digraph minuend_dg4;
	iscc_digraph_from_string("#.#../#.##./#.#../##.../##..#/", &minuend_dg4);
	iscc_Digraph subtrahend_dg4;
	iscc_digraph_from_string("...../...../...../...../...../", &subtrahend_dg4);
	iscc_Digraph ref_dg4;
	iscc_digraph_from_string("#.#../#.##./#.#../##.../##..#/", &ref_dg4);
	scc_ErrorCode ec4 = iscc_digraph_difference(&minuend_dg4, &subtrahend_dg4, 5);
	assert_equal_digraph(&minuend_dg4, &ref_dg4);
	assert_int_equal(ec4, SCC_ER_OK);

	iscc_Digraph minuend_dg5;
	iscc_digraph_from_string("#.#../#.##./#.#../##.../##..#/", &minuend_dg5);
	iscc_Digraph subtrahend_dg5;
	iscc_digraph_from_string("...##/##..#/.#.#./#.#.#/..#../", &subtrahend_dg5);
	iscc_Digraph ref_dg5;
	iscc_digraph_from_string("#.#../..##./#.#../.#.../##..#/", &ref_dg5);
	scc_ErrorCode ec5 = iscc_digraph_difference(&minuend_dg5, &subtrahend_dg5, 5);
	assert_equal_digraph(&minuend_dg5, &ref_dg5);
	assert_int_equal(ec5, SCC_ER_OK);

	iscc_Digraph minuend_dg5b;
	iscc_digraph_from_string("#.#../#.##./#.#../##.../##..#/", &minuend_dg5b);
	iscc_Digraph subtrahend_dg5b;
	iscc_digraph_from_string("...##/##..#/.#.#./#.#.#/..#../", &subtrahend_dg5b);
	iscc_Digraph ref_dg5b;
	iscc_digraph_from_string("#..../..#../#..../.#.../#..../", &ref_dg5b);
	scc_ErrorCode ec5b = iscc_digraph_difference(&minuend_dg5b, &subtrahend_dg5b, 1);
	assert_equal_digraph(&minuend_dg5b, &ref_dg5b);
	assert_int_equal(ec5b, SCC_ER_OK);

	assert_free_digraph(&minuend_dg1);
	assert_free_digraph(&subtrahend_dg1);
	assert_free_digraph(&ref_dg1);
	assert_free_digraph(&minuend_dg2);
	assert_free_digraph(&subtrahend_dg2);
	assert_free_digraph(&ref_dg2);
	assert_free_digraph(&minuend_dg2b);
	assert_free_digraph(&subtrahend_dg2b);
	assert_free_digraph(&ref_dg2b);
	assert_free_digraph(&minuend_dg3);
	assert_free_digraph(&subtrahend_dg3);
	assert_free_digraph(&ref_dg3);
	assert_free_digraph(&minuend_dg4);
	assert_free_digraph(&subtrahend_dg4);
	assert_free_digraph(&ref_dg4);
	assert_free_digraph(&minuend_dg5);
	assert_free_digraph(&subtrahend_dg5);
	assert_free_digraph(&ref_dg5);
	assert_free_digraph(&minuend_dg5b);
	assert_free_digraph(&subtrahend_dg5b);
	assert_free_digraph(&ref_dg5b);

	iscc_Digraph minuend_dg6;
	iscc_digraph_from_string("...../...../...../...../...../", &minuend_dg6);
	iscc_Digraph subtrahend_dg6;
	iscc_digraph_from_string("...##/##..#/.#.#./#.#.#/..#../", &subtrahend_dg6);
	iscc_Digraph ref_dg6;
	iscc_digraph_from_string("...../...../...../...../...../", &ref_dg6);
	scc_ErrorCode ec6 = iscc_digraph_difference(&minuend_dg6, &subtrahend_dg6, 5);
	assert_equal_digraph(&minuend_dg6, &ref_dg6);
	assert_int_equal(ec6, SCC_ER_OK);
	assert_free_digraph(&minuend_dg6);
	assert_free_digraph(&subtrahend_dg6);
	assert_free_digraph(&ref_dg6);

	iscc_Digraph minuend_dg7;
	iscc_digraph_from_string("...##/##..#/.#.#./#.#.#/..#../", &minuend_dg7);
	iscc_Digraph subtrahend_dg7;
	iscc_digraph_from_string("...../...../...../...../...../", &subtrahend_dg7);
	iscc_Digraph ref_dg7;
	iscc_digraph_from_string("...##/##..#/.#.#./#.#.#/..#../", &ref_dg7);
	scc_ErrorCode ec7 = iscc_digraph_difference(&minuend_dg7, &subtrahend_dg7, 5);
	assert_equal_digraph(&minuend_dg7, &ref_dg7);
	assert_int_equal(ec7, SCC_ER_OK);
	assert_free_digraph(&minuend_dg7);
	assert_free_digraph(&subtrahend_dg7);
	assert_free_digraph(&ref_dg7);

	iscc_Digraph minuend_dg8;
	iscc_empty_digraph(5, 0, &minuend_dg8);
	iscc_Digraph subtrahend_dg8;
	iscc_digraph_from_string("...##/##..#/.#.#./#.#.#/..#../", &subtrahend_dg8);
	scc_ErrorCode ec8 = iscc_digraph_difference(&minuend_dg8, &subtrahend_dg8, 5);
	assert_int_equal(ec8, SCC_ER_OK);
	assert_int_equal(minuend_dg8.vertices, 5);
	assert_int_equal(minuend_dg8.max_arcs, 0);
	assert_null(minuend_dg8.head);
	assert_non_null(minuend_dg8.tail_ptr);
	const iscc_Arci ref_tail_ptr[6] = { 0, 0, 0, 0, 0, 0 };
	assert_memory_equal(minuend_dg8.tail_ptr, ref_tail_ptr, 6 * sizeof(iscc_Arci));
	assert_free_digraph(&minuend_dg8);
	assert_free_digraph(&subtrahend_dg8);

	iscc_Digraph minuend_dg9;
	iscc_digraph_from_string("...##/##..#/.#.#./#.#.#/..#../", &minuend_dg9);
	iscc_Digraph subtrahend_dg9;
	iscc_empty_digraph(5, 0, &subtrahend_dg9);
	iscc_Digraph ref_dg9;
	iscc_digraph_from_string("...##/##..#/.#.#./#.#.#/..#../", &ref_dg9);
	scc_ErrorCode ec9 = iscc_digraph_difference(&minuend_dg9, &subtrahend_dg9, 5);
	assert_equal_digraph(&minuend_dg9, &ref_dg9);
	assert_int_equal(ec9, SCC_ER_OK);
	assert_free_digraph(&minuend_dg9);
	assert_free_digraph(&subtrahend_dg9);
	assert_free_digraph(&ref_dg9);

	iscc_Digraph minuend_dg10;
	iscc_digraph_from_string("...##/##..#/.#.#./#.#.#/..#../", &minuend_dg10);
	iscc_Digraph subtrahend_dg10;
	iscc_empty_digraph(5, 0, &subtrahend_dg10);
	iscc_Digraph ref_dg10;
	iscc_digraph_from_string("...#./#..../.#.../#..../..#../", &ref_dg10);
	scc_ErrorCode ec10 = iscc_digraph_difference(&minuend_dg10, &subtrahend_dg10, 1);
	assert_equal_digraph(&minuend_dg10, &ref_dg10);
	assert_int_equal(ec10, SCC_ER_OK);
	assert_free_digraph(&minuend_dg10);
	assert_free_digraph(&subtrahend_dg10);
	assert_free_digraph(&ref_dg10);

	iscc_Digraph minuend_dg10a;
	iscc_digraph_from_string("...##/##..#/.#.#./#.#.#/..#../", &minuend_dg10a);
	iscc_Digraph subtrahend_dg10a;
	iscc_digraph_from_string("...../...../...../...../...../", &subtrahend_dg10a);
	iscc_Digraph ref_dg10a;
	iscc_digraph_from_string("...#./#..../.#.../#..../..#../", &ref_dg10a);
	scc_ErrorCode ec10a = iscc_digraph_difference(&minuend_dg10a, &subtrahend_dg10a, 1);
	assert_equal_digraph(&minuend_dg10a, &ref_dg10a);
	assert_int_equal(ec10a, SCC_ER_OK);
	assert_free_digraph(&minuend_dg10a);
	assert_free_digraph(&subtrahend_dg10a);
	assert_free_digraph(&ref_dg10a);

	iscc_Digraph minuend_dg11;
	iscc_digraph_from_string("...../...../...../...../...../", &minuend_dg11);
	iscc_Digraph subtrahend_dg11;
	iscc_digraph_from_string("...../...../...../...../...../", &subtrahend_dg11);
	iscc_Digraph ref_dg11;
	iscc_digraph_from_string("...../...../...../...../...../", &ref_dg11);
	scc_ErrorCode ec11 = iscc_digraph_difference(&minuend_dg11, &subtrahend_dg11, 5);
	assert_equal_digraph(&minuend_dg11, &ref_dg11);
	assert_int_equal(ec11, SCC_ER_OK);
	assert_free_digraph(&minuend_dg11);
	assert_free_digraph(&subtrahend_dg11);
	assert_free_digraph(&ref_dg11);

	iscc_Digraph minuend_dg12;
	iscc_digraph_from_string("...../...../...../...../...../", &minuend_dg12);
	iscc_Digraph subtrahend_dg12;
	iscc_empty_digraph(5, 0, &subtrahend_dg12);
	iscc_Digraph ref_dg12;
	iscc_digraph_from_string("...../...../...../...../...../", &ref_dg12);
	scc_ErrorCode ec12 = iscc_digraph_difference(&minuend_dg12, &subtrahend_dg12, 5);
	assert_equal_digraph(&minuend_dg12, &ref_dg12);
	assert_int_equal(ec12, SCC_ER_OK);
	assert_free_digraph(&minuend_dg12);
	assert_free_digraph(&subtrahend_dg12);
	assert_free_digraph(&ref_dg12);

	iscc_Digraph minuend_dg13;
	iscc_empty_digraph(5, 0, &minuend_dg13);
	iscc_Digraph subtrahend_dg13;
	iscc_digraph_from_string("...../...../...../...../...../", &subtrahend_dg13);
	scc_ErrorCode ec13 = iscc_digraph_difference(&minuend_dg13, &subtrahend_dg13, 5);
	assert_int_equal(ec13, SCC_ER_OK);
	assert_int_equal(minuend_dg13.vertices, 5);
	assert_int_equal(minuend_dg13.max_arcs, 0);
	assert_null(minuend_dg13.head);
	assert_non_null(minuend_dg13.tail_ptr);
	const iscc_Arci ref_tail_ptr13[6] = { 0, 0, 0, 0, 0, 0 };
	assert_memory_equal(minuend_dg13.tail_ptr, ref_tail_ptr13, 6 * sizeof(iscc_Arci));
	assert_free_digraph(&minuend_dg13);
	assert_free_digraph(&subtrahend_dg13);

	iscc_Digraph minuend_dg14;
	iscc_empty_digraph(5, 0, &minuend_dg14);
	iscc_Digraph subtrahend_dg14;
	iscc_empty_digraph(5, 0, &subtrahend_dg14);
	scc_ErrorCode ec14 = iscc_digraph_difference(&minuend_dg14, &subtrahend_dg14, 5);
	assert_int_equal(ec14, SCC_ER_OK);
	assert_int_equal(minuend_dg14.vertices, 5);
	assert_int_equal(minuend_dg14.max_arcs, 0);
	assert_null(minuend_dg14.head);
	assert_non_null(minuend_dg14.tail_ptr);
	const iscc_Arci ref_tail_ptr14[6] = { 0, 0, 0, 0, 0, 0 };
	assert_memory_equal(minuend_dg14.tail_ptr, ref_tail_ptr14, 6 * sizeof(iscc_Arci));
	assert_free_digraph(&minuend_dg14);
	assert_free_digraph(&subtrahend_dg14);
}


void scc_ut_digraph_transpose(void** state)
{
	(void) state;

	iscc_Digraph ut_dg1;
	iscc_digraph_from_string("#.../.#../..#./...#/", &ut_dg1);
	iscc_Digraph ut_dg2;
	iscc_digraph_from_string("####/..#./####/#.../", &ut_dg2);
	iscc_Digraph ut_dg3;
	iscc_digraph_from_string("#.../#.../#.../#.../", &ut_dg3);
	iscc_Digraph ut_dg4;
	iscc_digraph_from_string("..../..../..../..../", &ut_dg4);
	iscc_Digraph ut_dg5;
	iscc_digraph_from_string("##.#/..#./.###/#.../", &ut_dg5);
	iscc_Digraph ut_dg6;
	iscc_empty_digraph(4, 0, &ut_dg6);

	iscc_Digraph control1;
	iscc_digraph_from_string("#.../.#../..#./...#/", &control1);
	iscc_Digraph control2;
	iscc_digraph_from_string("#.##/#.#./###./#.#./", &control2);
	iscc_Digraph control3;
	iscc_digraph_from_string("####/..../..../..../", &control3);
	iscc_Digraph control4;
	iscc_empty_digraph(4, 0, &control4);
	iscc_Digraph control5;
	iscc_digraph_from_string("#..#/#.#./.##./#.#./", &control5);
	iscc_Digraph control6;
	iscc_empty_digraph(4, 0, &control6);

	iscc_Digraph res1;
	scc_ErrorCode ec1 = iscc_digraph_transpose(&ut_dg1, &res1);
	iscc_Digraph res2;
	scc_ErrorCode ec2 = iscc_digraph_transpose(&ut_dg2, &res2);
	iscc_Digraph res3;
	scc_ErrorCode ec3 = iscc_digraph_transpose(&ut_dg3, &res3);
	iscc_Digraph res4;
	scc_ErrorCode ec4 = iscc_digraph_transpose(&ut_dg4, &res4);
	iscc_Digraph res5;
	scc_ErrorCode ec5 = iscc_digraph_transpose(&ut_dg5, &res5);
	iscc_Digraph res6;
	scc_ErrorCode ec6 = iscc_digraph_transpose(&ut_dg6, &res6);

	assert_int_equal(ec1, SCC_ER_OK);
	assert_int_equal(ec2, SCC_ER_OK);
	assert_int_equal(ec3, SCC_ER_OK);
	assert_int_equal(ec4, SCC_ER_OK);
	assert_int_equal(ec5, SCC_ER_OK);
	assert_int_equal(ec6, SCC_ER_OK);

	assert_valid_digraph(&res1, 4);
	assert_valid_digraph(&res2, 4);
	assert_valid_digraph(&res3, 4);
	assert_valid_digraph(&res4, 4);
	assert_valid_digraph(&res5, 4);
	assert_valid_digraph(&res6, 4);

	assert_equal_digraph(&res1, &control1);
	assert_equal_digraph(&res2, &control2);
	assert_equal_digraph(&res3, &control3);
	assert_equal_digraph(&res4, &control4);
	assert_equal_digraph(&res5, &control5);
	assert_equal_digraph(&res6, &control6);

	assert_free_digraph(&ut_dg1);
	assert_free_digraph(&ut_dg2);
	assert_free_digraph(&ut_dg3);
	assert_free_digraph(&ut_dg4);
	assert_free_digraph(&ut_dg5);
	assert_free_digraph(&ut_dg6);
	assert_free_digraph(&control1);
	assert_free_digraph(&control2);
	assert_free_digraph(&control3);
	assert_free_digraph(&control4);
	assert_free_digraph(&control5);
	assert_free_digraph(&control6);
	assert_free_digraph(&res1);
	assert_free_digraph(&res2);
	assert_free_digraph(&res3);
	assert_free_digraph(&res4);
	assert_free_digraph(&res5);
	assert_free_digraph(&res6);
}


void scc_ut_adjacency_product(void** state)
{
	(void) state;

	iscc_Digraph dg1;
	iscc_digraph_from_string("##.../...#./.#.../..#../...#./", &dg1);
	iscc_Digraph dg1_f;
	iscc_digraph_from_string("##.../.#.#./.##../..##./...##/", &dg1_f);
	iscc_Digraph dg2;
	iscc_digraph_from_string("..#../..##./...../..#../#.#../", &dg2);
	iscc_Digraph dg2_f;
	iscc_digraph_from_string("#.#../.###./..#../..##./#.#.#/", &dg2_f);

	iscc_Digraph control1;
	iscc_digraph_from_string(".#.#./..#../...#./.#.../..#../", &control1);
	iscc_Digraph prod1;
	scc_ErrorCode ec1 = iscc_adjacency_product(&dg1, &dg1, false, &prod1);
	assert_int_equal(ec1, SCC_ER_OK);
	assert_valid_digraph(&prod1, 5);
	assert_equal_digraph(&prod1, &control1);

	iscc_Digraph control2;
	iscc_digraph_from_string(".#.#./..##./.#.#./.##../..##./", &control2);
	iscc_Digraph prod2;
	scc_ErrorCode ec2 = iscc_adjacency_product(&dg1, &dg1, true, &prod2);
	iscc_Digraph prod2alt;
	scc_ErrorCode ec2alt = iscc_adjacency_product(&dg1_f, &dg1, false, &prod2alt);
	assert_int_equal(ec2, SCC_ER_OK);
	assert_int_equal(ec2alt, SCC_ER_OK);
	assert_valid_digraph(&prod2, 5);
	assert_valid_digraph(&prod2alt, 5);
	assert_equal_digraph(&prod2, &control2);
	assert_equal_digraph(&prod2alt, &control2);

	iscc_Digraph control3;
	iscc_digraph_from_string(".###./..#../...#./.#.../.##../", &control3);
	iscc_Digraph prod3;
	scc_ErrorCode ec3 = iscc_adjacency_product(&dg1, &prod2, false, &prod3);
	assert_int_equal(ec3, SCC_ER_OK);
	assert_valid_digraph(&prod3, 5);
	assert_equal_digraph(&prod3, &control3);

	iscc_Digraph control4;
	iscc_digraph_from_string(".###./..##./.#.#./.##../.###./", &control4);
	iscc_Digraph prod4;
	scc_ErrorCode ec4 = iscc_adjacency_product(&dg1, &prod2, true, &prod4);
	iscc_Digraph prod4alt;
	scc_ErrorCode ec4alt = iscc_adjacency_product(&dg1_f, &prod2, false, &prod4alt);
	assert_int_equal(ec4, SCC_ER_OK);
	assert_int_equal(ec4alt, SCC_ER_OK);
	assert_valid_digraph(&prod4, 5);
	assert_valid_digraph(&prod4alt, 5);
	assert_equal_digraph(&prod4, &control4);
	assert_equal_digraph(&prod4alt, &control4);

	iscc_Digraph control5;
	iscc_digraph_from_string("..##./..#../...#./...../..#../", &control5);
	iscc_Digraph prod5;
	scc_ErrorCode ec5 = iscc_adjacency_product(&dg1, &dg2, false, &prod5);
	assert_int_equal(ec5, SCC_ER_OK);
	assert_valid_digraph(&prod5, 5);
	assert_equal_digraph(&prod5, &control5);

	iscc_Digraph control6;
	iscc_digraph_from_string("..##./..##./...#./..#../#.#../", &control6);
	iscc_Digraph prod6;
	scc_ErrorCode ec6 = iscc_adjacency_product(&dg1, &dg2, true, &prod6);
	iscc_Digraph prod6alt;
	scc_ErrorCode ec6alt = iscc_adjacency_product(&dg1_f, &dg2, false, &prod6alt);
	assert_int_equal(ec6, SCC_ER_OK);
	assert_int_equal(ec6alt, SCC_ER_OK);
	assert_valid_digraph(&prod6, 5);
	assert_valid_digraph(&prod6alt, 5);
	assert_equal_digraph(&prod6, &control6);
	assert_equal_digraph(&prod6alt, &control6);

	iscc_Digraph control7;
	iscc_digraph_from_string(".#.../..#../...../.#.../##.../", &control7);
	iscc_Digraph prod7;
	scc_ErrorCode ec7 = iscc_adjacency_product(&dg2, &dg1, false, &prod7);
	assert_int_equal(ec7, SCC_ER_OK);
	assert_valid_digraph(&prod7, 5);
	assert_equal_digraph(&prod7, &control7);

	iscc_Digraph control8;
	iscc_digraph_from_string(".#.../..##./.#.../.##../##.#./", &control8);
	iscc_Digraph prod8;
	scc_ErrorCode ec8 = iscc_adjacency_product(&dg2, &dg1, true, &prod8);
	iscc_Digraph prod8alt;
	scc_ErrorCode ec8alt = iscc_adjacency_product(&dg2_f, &dg1, false, &prod8alt);
	assert_int_equal(ec8, SCC_ER_OK);
	assert_int_equal(ec8alt, SCC_ER_OK);
	assert_valid_digraph(&prod8, 5);
	assert_valid_digraph(&prod8alt, 5);
	assert_equal_digraph(&prod8, &control8);
	assert_equal_digraph(&prod8alt, &control8);

	assert_free_digraph(&dg1);
	assert_free_digraph(&dg1_f);
	assert_free_digraph(&dg2);
	assert_free_digraph(&dg2_f);
	assert_free_digraph(&control1);
	assert_free_digraph(&prod1);
	assert_free_digraph(&control2);
	assert_free_digraph(&prod2);
	assert_free_digraph(&prod2alt);
	assert_free_digraph(&control3);
	assert_free_digraph(&prod3);
	assert_free_digraph(&control4);
	assert_free_digraph(&prod4);
	assert_free_digraph(&prod4alt);
	assert_free_digraph(&control5);
	assert_free_digraph(&prod5);
	assert_free_digraph(&control6);
	assert_free_digraph(&prod6);
	assert_free_digraph(&prod6alt);
	assert_free_digraph(&control7);
	assert_free_digraph(&prod7);
	assert_free_digraph(&control8);
	assert_free_digraph(&prod8);
	assert_free_digraph(&prod8alt);
}


int main(void)
{
	const struct CMUnitTest test_cases[] = {
		cmocka_unit_test(scc_ut_delete_loops),
		cmocka_unit_test(scc_ut_digraph_union_and_delete),
		cmocka_unit_test(scc_ut_digraph_union_and_delete_empty),
		cmocka_unit_test(scc_ut_digraph_union_and_delete_single),
		cmocka_unit_test(scc_ut_digraph_union_and_delete_keep_loops),
		cmocka_unit_test(scc_ut_digraph_union_and_delete_keep_loops_empty),
		cmocka_unit_test(scc_ut_digraph_union_and_delete_keep_loops_single),
		cmocka_unit_test(scc_ut_digraph_difference),
		cmocka_unit_test(scc_ut_digraph_transpose),
		cmocka_unit_test(scc_ut_adjacency_product),
	};

	return cmocka_run_group_tests_name("digraph_operations.c", test_cases, NULL, NULL);
}
