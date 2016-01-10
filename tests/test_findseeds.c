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
#include "test_tools.h"

#include <stdbool.h>

#include "../src/findseeds.h"
#include "../include/config.h"
#include "../include/digraph.h"
#include "../include/digraph_debug.h"


#ifdef SCC_STABLE_CLUSTERING
    #error Please run this test without the SCC_STABLE_CLUSTERING flag
#endif


void scc_ut_findseeds_lexical(void** state)
{
	(void) state;

	scc_Digraph nng = scc_digraph_from_string(".#......../"
	                                          "...#....../"
	                                          "#........./"
	                                          ".#......../"
	                                          ".....#..../"
	                                          "...#....../"
	                                          "....#...../"
	                                          "......#.../"
	                                          "......#.../"
	                                          "......#.../");

	scc_Vid fp_seeds[3] = {0, 4, 7};

	iscc_SeedArray seed_array = iscc_findseeds_lexical(&nng, 1);
	assert_int_equal(seed_array.seed_capacity, 3);
	assert_int_equal(seed_array.num_seeds, 3);
	assert_memory_equal(seed_array.seeds, fp_seeds, 3 * sizeof(scc_Vid));

	iscc_free_SeedArray(&seed_array);
	scc_free_digraph(&nng);
}


void scc_ut_findseeds_inwards(void** state)
{
	(void) state;

	scc_Digraph nng = scc_digraph_from_string("...#....../"
	                                          "...#....../"
	                                          "#........./"
	                                          ".#......../"
	                                          ".....#..../"
	                                          ".#......../"
	                                          "....#...../"
	                                          "......#.../"
	                                          "......#.../"
	                                          "......#.../");

	scc_Vid fp_seeds[4] = {2, 7, 4, 1};

	iscc_SeedArray seed_array = iscc_findseeds_inwards(&nng, 1, false);
	assert_int_equal(seed_array.seed_capacity, 4);
	assert_int_equal(seed_array.num_seeds, 4);
	assert_memory_equal(seed_array.seeds, fp_seeds, 4 * sizeof(scc_Vid));

	iscc_free_SeedArray(&seed_array);
	scc_free_digraph(&nng);
}


void scc_ut_findseeds_inwards_updating(void** state)
{
	(void) state;

	scc_Digraph nng = scc_digraph_from_string("...#....../"
	                                          "...#....../"
	                                          "#........./"
	                                          ".#......../"
	                                          ".....#..../"
	                                          ".#......../"
	                                          "....#...../"
	                                          "......#.../"
	                                          "......#.../"
	                                          "......#.../");

	scc_Vid fp_seeds[4] = {2, 7, 4, 3};

	iscc_SeedArray seed_array = iscc_findseeds_inwards(&nng, 1, true);
	assert_int_equal(seed_array.seed_capacity, 4);
	assert_int_equal(seed_array.num_seeds, 4);
	assert_memory_equal(seed_array.seeds, fp_seeds, 4 * sizeof(scc_Vid));

	iscc_free_SeedArray(&seed_array);
	scc_free_digraph(&nng);
}


void scc_ut_findseeds_exclusion(void** state)
{
	(void) state;

	scc_Digraph nng = scc_digraph_from_string("...#....../"
	                                          "...#....../"
	                                          ".#......../"
	                                          ".#......../"
	                                          ".....#..../"
	                                          ".#......../"
	                                          "....#...../"
	                                          "......#.../"
	                                          "......#.../"
	                                          "......#.../");

	scc_Vid fp_seeds[4] = {0, 4, 2, 7};

	iscc_SeedArray seed_array = iscc_findseeds_exclusion(&nng, 1, false);
	assert_int_equal(seed_array.seed_capacity, 4);
	assert_int_equal(seed_array.num_seeds, 4);
	assert_memory_equal(seed_array.seeds, fp_seeds, 4 * sizeof(scc_Vid));

	iscc_free_SeedArray(&seed_array);
	scc_free_digraph(&nng);
}


void scc_ut_findseeds_exclusion_updating(void** state)
{
	(void) state;

	scc_Digraph nng = scc_digraph_from_string("...#....../"
	                                          "...#....../"
	                                          ".#......../"
	                                          ".#......../"
	                                          ".....#..../"
	                                          ".#......../"
	                                          "....#...../"
	                                          "......#.../"
	                                          "......#.../"
	                                          "......#.../");

	scc_Vid fp_seeds[4] = {0, 2, 4, 7};

	iscc_SeedArray seed_array = iscc_findseeds_exclusion(&nng, 1, true);
	assert_int_equal(seed_array.seed_capacity, 4);
	assert_int_equal(seed_array.num_seeds, 4);
	assert_memory_equal(seed_array.seeds, fp_seeds, 4 * sizeof(scc_Vid));

	iscc_free_SeedArray(&seed_array);
	scc_free_digraph(&nng);
}


void scc_ut_findseeds_checkseeds18(void** state)
{
	(void) state;

	scc_Digraph nng = scc_digraph_from_string(".#..#............./"
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
	                                          "..............#.#./");

	iscc_SeedArray cl_seed_lexical = iscc_findseeds_lexical(&nng, 1);
	scc_Vid ref_seeds_lexical[5] = {0, 5, 6, 10, 12};
	assert_memory_equal(cl_seed_lexical.seeds, ref_seeds_lexical, 5 * sizeof(scc_Vid));
	assert_int_equal(cl_seed_lexical.num_seeds, 5);
	assert_int_equal(cl_seed_lexical.seed_capacity, 5);

	iscc_SeedArray cl_seed_inwards = iscc_findseeds_inwards(&nng, 1, false);
	scc_Vid ref_seeds_inwards[5] = {2, 6, 11, 12, 13};
	assert_memory_equal(cl_seed_inwards.seeds, ref_seeds_inwards, 5 * sizeof(scc_Vid));
	assert_int_equal(cl_seed_inwards.num_seeds, 5);
	assert_int_equal(cl_seed_inwards.seed_capacity, 5);

	iscc_SeedArray cl_seed_inupdat = iscc_findseeds_inwards(&nng, 1, true);
	scc_Vid ref_seeds_inupdat[5] = {2, 6, 11, 12, 13};
	assert_memory_equal(cl_seed_inupdat.seeds, ref_seeds_inupdat, 5 * sizeof(scc_Vid));
	assert_int_equal(cl_seed_inupdat.num_seeds, 5);
	assert_int_equal(cl_seed_inupdat.seed_capacity, 5);

	iscc_SeedArray cl_seed_exclusi = iscc_findseeds_exclusion(&nng, 1, false);
	scc_Vid ref_seeds_exclusi[4] = {8, 15, 0, 13};
	assert_memory_equal(cl_seed_exclusi.seeds, ref_seeds_exclusi, 4 * sizeof(scc_Vid));
	assert_int_equal(cl_seed_exclusi.num_seeds, 4);
	assert_int_equal(cl_seed_exclusi.seed_capacity, 4);

	iscc_SeedArray cl_seed_exupdat = iscc_findseeds_exclusion(&nng, 1, true);
	scc_Vid ref_seeds_exupdat[5] = {8, 15, 17, 10, 4};
	assert_memory_equal(cl_seed_exupdat.seeds, ref_seeds_exupdat, 5 * sizeof(scc_Vid));
	assert_int_equal(cl_seed_exupdat.num_seeds, 5);
	assert_int_equal(cl_seed_exupdat.seed_capacity, 5);

	scc_free_digraph(&nng);
	iscc_free_SeedArray(&cl_seed_lexical);
	iscc_free_SeedArray(&cl_seed_inwards);
	iscc_free_SeedArray(&cl_seed_inupdat);
	iscc_free_SeedArray(&cl_seed_exclusi);
	iscc_free_SeedArray(&cl_seed_exupdat);
}


int main(void)
{
	const struct CMUnitTest test_findseeds[] = {
		cmocka_unit_test(scc_ut_findseeds_lexical),
		cmocka_unit_test(scc_ut_findseeds_inwards),
		cmocka_unit_test(scc_ut_findseeds_inwards_updating),
		cmocka_unit_test(scc_ut_findseeds_exclusion),
		cmocka_unit_test(scc_ut_findseeds_exclusion_updating),
		cmocka_unit_test(scc_ut_findseeds_checkseeds18),
	};

	return cmocka_run_group_tests_name("find seeds module", test_findseeds, NULL, NULL);
}
