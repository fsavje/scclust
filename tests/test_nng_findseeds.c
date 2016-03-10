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
#include "../src/nng_findseeds.h"

#include <stddef.h>
#include "../include/scclust.h"
#include "../src/config.h"
#include "../src/digraph_core.h"
#include "../src/digraph_debug.h"

#ifdef SCC_STABLE_CLUSTERING
    #error Please run this test without the SCC_STABLE_CLUSTERING flag
#endif


void scc_ut_find_seeds(void** state)
{
	(void) state;

	iscc_Digraph nng;
	iscc_digraph_from_string(".#..#............./"
	                         "#...#............./"
	                         "....#..#........../"
	                         "#...#............./"
	                         ".#.#............../"
	                         "..#.....#........./"
	                         "...#.....#......../"
	                         "......#.#........./"
	                         ".....#.....#....../"
	                         "..........#.....#./"
	                         ".......#.....#..../"
	                         "........#.#......./"
	                         "...............##./"
	                         "..............#..#/"
	                         ".............#...#/"
	                         ".........#..#...../"
	                         ".............##.../"
	                         "..............#.#./",
	                         &nng);

	iscc_Dpid fp_seeds1[5] = {0, 5, 6, 10, 12};
	iscc_SeedResult sr1 = {
		.capacity = 1,
		.count = 0,
		.seeds = NULL,
	};
	scc_ErrorCode ec1 = iscc_find_seeds(&nng, SCC_SM_LEXICAL, &sr1);
	assert_int_equal(ec1, SCC_ER_OK);
	assert_int_equal(sr1.count, 5);
	assert_int_equal(sr1.capacity, sr1.count);
	assert_non_null(sr1.seeds);
	assert_memory_equal(sr1.seeds, fp_seeds1, sr1.count * sizeof(iscc_Dpid));

	iscc_Dpid fp_seeds2[5] = {2, 6, 11, 12, 13};
	iscc_SeedResult sr2 = {
		.capacity = 1,
		.count = 0,
		.seeds = NULL,
	};
	scc_ErrorCode ec2 = iscc_find_seeds(&nng, SCC_SM_INWARDS_ORDER, &sr2);
	assert_int_equal(ec2, SCC_ER_OK);
	assert_int_equal(sr2.count, 5);
	assert_int_equal(sr2.capacity, sr2.count);
	assert_non_null(sr2.seeds);
	assert_memory_equal(sr2.seeds, fp_seeds2, sr2.count * sizeof(iscc_Dpid));

	iscc_Dpid fp_seeds3[5] = {2, 6, 11, 12, 13};
	iscc_SeedResult sr3 = {
		.capacity = 1,
		.count = 0,
		.seeds = NULL,
	};
	scc_ErrorCode ec3 = iscc_find_seeds(&nng, SCC_SM_INWARDS_UPDATING, &sr3);
	assert_int_equal(ec3, SCC_ER_OK);
	assert_int_equal(sr3.count, 5);
	assert_int_equal(sr3.capacity, sr3.count);
	assert_non_null(sr3.seeds);
	assert_memory_equal(sr3.seeds, fp_seeds3, sr3.count * sizeof(iscc_Dpid));

	iscc_Dpid fp_seeds4[4] = {8, 15, 0, 13};
	iscc_SeedResult sr4 = {
		.capacity = 1,
		.count = 0,
		.seeds = NULL,
	};
	scc_ErrorCode ec4 = iscc_find_seeds(&nng, SCC_SM_EXCLUSION_ORDER, &sr4);
	assert_int_equal(ec4, SCC_ER_OK);
	assert_int_equal(sr4.count, 4);
	assert_int_equal(sr4.capacity, sr4.count);
	assert_non_null(sr4.seeds);
	assert_memory_equal(sr4.seeds, fp_seeds4, sr4.count * sizeof(iscc_Dpid));

	iscc_Dpid fp_seeds5[5] = {8, 15, 17, 10, 4};
	iscc_SeedResult sr5 = {
		.capacity = 1,
		.count = 0,
		.seeds = NULL,
	};
	scc_ErrorCode ec5 = iscc_find_seeds(&nng, SCC_SM_EXCLUSION_UPDATING, &sr5);
	assert_int_equal(ec5, SCC_ER_OK);
	assert_int_equal(sr5.count, 5);
	assert_int_equal(sr5.capacity, sr5.count);
	assert_non_null(sr5.seeds);
	assert_memory_equal(sr5.seeds, fp_seeds5, sr5.count * sizeof(iscc_Dpid));

	free(sr1.seeds);
	free(sr2.seeds);
	free(sr3.seeds);
	free(sr4.seeds);
	free(sr5.seeds);
	iscc_free_digraph(&nng);
}


void scc_ut_find_seeds_withdiag(void** state)
{
	(void) state;

	iscc_Digraph nng;
	iscc_digraph_from_string("##..#............./"
	                         "##..#............./"
	                         "..#.#..#........../"
	                         "#..##............./"
	                         ".#.##............./"
	                         "..#..#..#........./"
	                         "...#..#..#......../"
	                         "......###........./"
	                         ".....#..#..#....../"
	                         ".........##.....#./"
	                         ".......#..#..#..../"
	                         "........#.##....../"
	                         "............#..##./"
	                         ".............##..#/"
	                         ".............##..#/"
	                         ".........#..#..#../"
	                         ".............##.#./"
	                         "..............#.##/",
	                         &nng);

	iscc_Dpid fp_seeds1[5] = {0, 5, 6, 10, 12};
	iscc_SeedResult sr1 = {
		.capacity = 1,
		.count = 0,
		.seeds = NULL,
	};
	scc_ErrorCode ec1 = iscc_find_seeds(&nng, SCC_SM_LEXICAL, &sr1);
	assert_int_equal(ec1, SCC_ER_OK);
	assert_int_equal(sr1.count, 5);
	assert_int_equal(sr1.capacity, sr1.count);
	assert_non_null(sr1.seeds);
	assert_memory_equal(sr1.seeds, fp_seeds1, sr1.count * sizeof(iscc_Dpid));

	iscc_Dpid fp_seeds2[5] = {2, 6, 11, 12, 13};
	iscc_SeedResult sr2 = {
		.capacity = 1,
		.count = 0,
		.seeds = NULL,
	};
	scc_ErrorCode ec2 = iscc_find_seeds(&nng, SCC_SM_INWARDS_ORDER, &sr2);
	assert_int_equal(ec2, SCC_ER_OK);
	assert_int_equal(sr2.count, 5);
	assert_int_equal(sr2.capacity, sr2.count);
	assert_non_null(sr2.seeds);
	assert_memory_equal(sr2.seeds, fp_seeds2, sr2.count * sizeof(iscc_Dpid));

	iscc_Dpid fp_seeds3[5] = {2, 6, 11, 12, 13};
	iscc_SeedResult sr3 = {
		.capacity = 1,
		.count = 0,
		.seeds = NULL,
	};
	scc_ErrorCode ec3 = iscc_find_seeds(&nng, SCC_SM_INWARDS_UPDATING, &sr3);
	assert_int_equal(ec3, SCC_ER_OK);
	assert_int_equal(sr3.count, 5);
	assert_int_equal(sr3.capacity, sr3.count);
	assert_non_null(sr3.seeds);
	assert_memory_equal(sr3.seeds, fp_seeds3, sr3.count * sizeof(iscc_Dpid));

	iscc_Dpid fp_seeds4[4] = {8, 15, 0, 13};
	iscc_SeedResult sr4 = {
		.capacity = 1,
		.count = 0,
		.seeds = NULL,
	};
	scc_ErrorCode ec4 = iscc_find_seeds(&nng, SCC_SM_EXCLUSION_ORDER, &sr4);
	assert_int_equal(ec4, SCC_ER_OK);
	assert_int_equal(sr4.count, 4);
	assert_int_equal(sr4.capacity, sr4.count);
	assert_non_null(sr4.seeds);
	assert_memory_equal(sr4.seeds, fp_seeds4, sr4.count * sizeof(iscc_Dpid));

	iscc_Dpid fp_seeds5[5] = {8, 15, 17, 10, 4};
	iscc_SeedResult sr5 = {
		.capacity = 1,
		.count = 0,
		.seeds = NULL,
	};
	scc_ErrorCode ec5 = iscc_find_seeds(&nng, SCC_SM_EXCLUSION_UPDATING, &sr5);
	assert_int_equal(ec5, SCC_ER_OK);
	assert_int_equal(sr5.count, 5);
	assert_int_equal(sr5.capacity, sr5.count);
	assert_non_null(sr5.seeds);
	assert_memory_equal(sr5.seeds, fp_seeds5, sr5.count * sizeof(iscc_Dpid));

	free(sr1.seeds);
	free(sr2.seeds);
	free(sr3.seeds);
	free(sr4.seeds);
	free(sr5.seeds);
	iscc_free_digraph(&nng);
}


int main(void)
{
	const struct CMUnitTest test_cases[] = {
		cmocka_unit_test(scc_ut_find_seeds),
		cmocka_unit_test(scc_ut_find_seeds_withdiag),
	};
	
	return cmocka_run_group_tests_name("nng_findseeds.c", test_cases, NULL, NULL);
}
