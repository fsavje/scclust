/* =============================================================================
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
 * ========================================================================== */

#define SCC_STABLE_FINDSEED

#include "init_test.h"
#include <include/scclust.h>
#include <src/digraph_core.h>
#include <src/digraph_debug.h>
#include <src/nng_findseeds.c>
#include <src/scclust_types.h>


void scc_ut_fs_debug_bucket_sort(void** state)
{
	(void) state;


	iscc_Dpid buckets1[7] = {1, 4, 3, 5, 6, 2, 0};
	iscc_Dpid inwards_count1[7] = {3, 1, 2, 2, 1, 2, 2};
	iscc_Dpid* vertex_index1[7];
	for (iscc_Dpid* b = buckets1; b != buckets1 + 7; ++b) vertex_index1[*b] = b;
	iscc_fs_debug_bucket_sort(buckets1 + 2, buckets1 + 5, inwards_count1, vertex_index1);

	iscc_Dpid ref_buckets1[7] = {1, 4, 2, 3, 5, 6, 0};
	iscc_Dpid ref_inwards_count1[7] = {3, 1, 2, 2, 1, 2, 2};
	iscc_Dpid* ref_vertex_index1[7];
	for (iscc_Dpid* b = ref_buckets1; b != ref_buckets1 + 7; ++b) ref_vertex_index1[*b] = b - ref_buckets1 + buckets1;
	assert_memory_equal(buckets1, ref_buckets1, 7 * sizeof(iscc_Dpid));
	assert_memory_equal(inwards_count1, ref_inwards_count1, 7 * sizeof(iscc_Dpid));
	assert_memory_equal(vertex_index1, ref_vertex_index1, 7 * sizeof(iscc_Dpid*));


	iscc_Dpid buckets2[7] = {1, 4, 2, 5, 6, 3, 0};
	iscc_Dpid inwards_count2[7] = {3, 1, 2, 2, 1, 2, 2};
	iscc_Dpid* vertex_index2[7];
	for (iscc_Dpid* b = buckets2; b != buckets2 + 7; ++b) vertex_index2[*b] = b;
	iscc_fs_debug_bucket_sort(buckets2 + 2, buckets2 + 5, inwards_count2, vertex_index2);

	iscc_Dpid ref_buckets2[7] = {1, 4, 2, 3, 5, 6, 0};
	iscc_Dpid ref_inwards_count2[7] = {3, 1, 2, 2, 1, 2, 2};
	iscc_Dpid* ref_vertex_index2[7];
	for (iscc_Dpid* b = ref_buckets2; b != ref_buckets2 + 7; ++b) ref_vertex_index2[*b] = b - ref_buckets2 + buckets2;
	assert_memory_equal(buckets2, ref_buckets2, 7 * sizeof(iscc_Dpid));
	assert_memory_equal(inwards_count2, ref_inwards_count2, 7 * sizeof(iscc_Dpid));
	assert_memory_equal(vertex_index2, ref_vertex_index2, 7 * sizeof(iscc_Dpid*));


	iscc_Dpid buckets3[7] = {1, 4, 2, 3, 5, 6, 0};
	iscc_Dpid inwards_count3[7] = {3, 1, 2, 2, 1, 2, 2};
	iscc_Dpid* vertex_index3[7];
	for (iscc_Dpid* b = buckets3; b != buckets3 + 7; ++b) vertex_index3[*b] = b;
	iscc_fs_debug_bucket_sort(buckets3 + 6, buckets3 + 6, inwards_count3, vertex_index3);

	iscc_Dpid ref_buckets3[7] = {1, 4, 2, 3, 5, 6, 0};
	iscc_Dpid ref_inwards_count3[7] = {3, 1, 2, 2, 1, 2, 2};
	iscc_Dpid* ref_vertex_index3[7];
	for (iscc_Dpid* b = ref_buckets3; b != ref_buckets3 + 7; ++b) ref_vertex_index3[*b] = b - ref_buckets3 + buckets3;
	assert_memory_equal(buckets3, ref_buckets3, 7 * sizeof(iscc_Dpid));
	assert_memory_equal(inwards_count3, ref_inwards_count3, 7 * sizeof(iscc_Dpid));
	assert_memory_equal(vertex_index3, ref_vertex_index3, 7 * sizeof(iscc_Dpid*));
}


void scc_ut_find_seeds_stable(void** state)
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

	iscc_Dpid fp_seeds3alt[5] = {2, 6, 8, 12, 13};
	iscc_SeedResult sr3alt = {
		.capacity = 1,
		.count = 0,
		.seeds = NULL,
	};
	scc_ErrorCode ec3alt = iscc_find_seeds(&nng, SCC_SM_INWARDS_ALT_UPDATING, &sr3alt);
	assert_int_equal(ec3alt, SCC_ER_OK);
	assert_int_equal(sr3alt.count, 5);
	assert_int_equal(sr3alt.capacity, sr3alt.count);
	assert_non_null(sr3alt.seeds);
	assert_memory_equal(sr3alt.seeds, fp_seeds3alt, sr3alt.count * sizeof(iscc_Dpid));

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

	iscc_Dpid fp_seeds5[5] = {8, 15, 17, 10, 0};
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
	free(sr3alt.seeds);
	free(sr4.seeds);
	free(sr5.seeds);
	iscc_free_digraph(&nng);
}


void scc_ut_find_seeds_stable_withdiag(void** state)
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

	iscc_Dpid fp_seeds3alt[5] = {2, 6, 8, 12, 13};
	iscc_SeedResult sr3alt = {
		.capacity = 1,
		.count = 0,
		.seeds = NULL,
	};
	scc_ErrorCode ec3alt = iscc_find_seeds(&nng, SCC_SM_INWARDS_ALT_UPDATING, &sr3alt);
	assert_int_equal(ec3alt, SCC_ER_OK);
	assert_int_equal(sr3alt.count, 5);
	assert_int_equal(sr3alt.capacity, sr3alt.count);
	assert_non_null(sr3alt.seeds);
	assert_memory_equal(sr3alt.seeds, fp_seeds3alt, sr3alt.count * sizeof(iscc_Dpid));

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

	iscc_Dpid fp_seeds5[5] = {8, 15, 17, 10, 0};
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
	free(sr3alt.seeds);
	free(sr4.seeds);
	free(sr5.seeds);
	iscc_free_digraph(&nng);
}


int main(void)
{
	scc_ut_init_tests();

	const struct CMUnitTest test_cases[] = {
		cmocka_unit_test(scc_ut_fs_debug_bucket_sort),
		cmocka_unit_test(scc_ut_find_seeds_stable),
		cmocka_unit_test(scc_ut_find_seeds_stable_withdiag),
	};

	return cmocka_run_group_tests_name("stable flag nng_findseeds.c", test_cases, NULL, NULL);
}
