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

#include "init_test.h"
#include <include/scclust.h>
#include <src/digraph_core.h>
#include <src/digraph_operations.h>
#include <src/digraph_debug.h>
#include <src/nng_findseeds.c>
#include <src/scclust_types.h>
#include "assert_digraph.h"

#ifdef SCC_STABLE_FINDSEED
    #error Please run this test without the SCC_STABLE_FINDSEED flag
#endif


void scc_ut_findseeds_lexical(void** state)
{
	(void) state;

	iscc_Digraph nng;
	iscc_digraph_from_string(".#......../"
	                         "...#....../"
	                         "#........./"
	                         ".#......../"
	                         ".....#..../"
	                         "...#....../"
	                         "....#...../"
	                         "......#.../"
	                         "......#.../"
	                         "......#.../",
	                         &nng);
	iscc_SeedResult sr = {
		.capacity = 10,
		.count = 0,
		.seeds = NULL,
	};
	scc_PointIndex fp_seeds[3] = {0, 4, 7};

	scc_ErrorCode ec = iscc_findseeds_lexical(&nng, &sr);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(sr.capacity, 10);
	assert_int_equal(sr.count, 3);
	assert_non_null(sr.seeds);
	assert_memory_equal(sr.seeds, fp_seeds, 3 * sizeof(scc_PointIndex));

	free(sr.seeds);
	iscc_free_digraph(&nng);
}


void scc_ut_findseeds_inwards(void** state)
{
	(void) state;

	iscc_Digraph nng1;
	iscc_digraph_from_string("...#....../"
	                         "...#....../"
	                         "#........./"
	                         ".#......../"
	                         ".....#..../"
	                         ".#......../"
	                         "....#...../"
	                         "......#.../"
	                         "......#.../"
	                         "......#.../",
	                         &nng1);
	iscc_SeedResult sr1 = {
		.capacity = 10,
		.count = 0,
		.seeds = NULL,
	};
	scc_PointIndex fp_seeds1[4] = {2, 7, 4, 1};

	scc_ErrorCode ec1 = iscc_findseeds_inwards(&nng1, false, &sr1);
	assert_int_equal(ec1, SCC_ER_OK);
	assert_int_equal(sr1.capacity, 10);
	assert_int_equal(sr1.count, 4);
	assert_non_null(sr1.seeds);
	assert_memory_equal(sr1.seeds, fp_seeds1, 4 * sizeof(scc_PointIndex));
	free(sr1.seeds);
	iscc_free_digraph(&nng1);

	iscc_Digraph nng2;
	iscc_digraph_from_string("...#....../"
	                         "...#....../"
	                         "#........./"
	                         ".#......../"
	                         ".....#..../"
	                         ".#......../"
	                         "....#...../"
	                         "......#.../"
	                         "......#.../"
	                         "......#.../",
	                         &nng2);
	iscc_SeedResult sr2 = {
		.capacity = 10,
		.count = 0,
		.seeds = NULL,
	};
	scc_PointIndex fp_seeds2[4] = {2, 7, 4, 3};

	scc_ErrorCode ec2 = iscc_findseeds_inwards(&nng2, true, &sr2);
	assert_int_equal(ec2, SCC_ER_OK);
	assert_int_equal(sr2.capacity, 10);
	assert_int_equal(sr2.count, 4);
	assert_non_null(sr2.seeds);
	assert_memory_equal(sr2.seeds, fp_seeds2, 4 * sizeof(scc_PointIndex));

	free(sr2.seeds);
	iscc_free_digraph(&nng2);
}


void scc_ut_findseeds_inwards_alt(void** state)
{
	(void) state;

	iscc_Digraph nng1;
	iscc_digraph_from_string("...#....../"
	                         "...#....../"
	                         "#........./"
	                         ".#......../"
	                         ".....#..../"
	                         ".#......../"
	                         "....#...../"
	                         "......#.../"
	                         "......#.../"
	                         "......#.../",
	                         &nng1);
	iscc_SeedResult sr1 = {
		.capacity = 10,
		.count = 0,
		.seeds = NULL,
	};
	scc_PointIndex fp_seeds1[4] = {2, 7, 4, 3};

	scc_ErrorCode ec1 = iscc_findseeds_inwards_alt(&nng1, &sr1);
	assert_int_equal(ec1, SCC_ER_OK);
	assert_int_equal(sr1.capacity, 10);
	assert_int_equal(sr1.count, 4);
	assert_non_null(sr1.seeds);
	assert_memory_equal(sr1.seeds, fp_seeds1, 4 * sizeof(scc_PointIndex));
	free(sr1.seeds);
	iscc_free_digraph(&nng1);
}


void scc_ut_findseeds_exclusion(void** state)
{
	(void) state;

	iscc_Digraph nng1;
	iscc_digraph_from_string("...#....../"
	                         "...#....../"
	                         ".#......../"
	                         ".#......../"
	                         ".....#..../"
	                         ".#......../"
	                         "....#...../"
	                         "......#.../"
	                         "......#.../"
	                         "......#.../",
	                         &nng1);
	iscc_SeedResult sr1 = {
		.capacity = 10,
		.count = 0,
		.seeds = NULL,
	};
	scc_PointIndex fp_seeds1[4] = {0, 4, 2, 7};

	scc_ErrorCode ec1 = iscc_findseeds_exclusion(&nng1, false, &sr1);
	assert_int_equal(ec1, SCC_ER_OK);
	assert_int_equal(sr1.capacity, 10);
	assert_int_equal(sr1.count, 4);
	assert_non_null(sr1.seeds);
	assert_memory_equal(sr1.seeds, fp_seeds1, 4 * sizeof(scc_PointIndex));
	free(sr1.seeds);
	iscc_free_digraph(&nng1);

	iscc_Digraph nng2;
	iscc_digraph_from_string("...#. ...../"
	                         "...#. ...../"
	                         ".#... ...../"
	                         ".#... ...../"
	                         "..... #..../"
	                         ".#... ...../"
	                         "....# ...../"
	                         "..... .#.../"
	                         "..... .#.../"
	                         "..... .#.../",
	                         &nng2);
	iscc_SeedResult sr2 = {
		.capacity = 10,
		.count = 0,
		.seeds = NULL,
	};
	scc_PointIndex fp_seeds2[4] = {0, 2, 4, 9};

	scc_ErrorCode ec2 = iscc_findseeds_exclusion(&nng2, true, &sr2);
	assert_int_equal(ec2, SCC_ER_OK);
	assert_int_equal(sr2.capacity, 10);
	assert_int_equal(sr2.count, 4);
	assert_non_null(sr2.seeds);
	assert_memory_equal(sr2.seeds, fp_seeds2, 4 * sizeof(scc_PointIndex));

	free(sr2.seeds);
	iscc_free_digraph(&nng2);
}


void scc_ut_findseeds_lexical_withdiag(void** state)
{
	(void) state;

	iscc_Digraph nng;
	iscc_digraph_from_string("##......../"
	                         ".#.#....../"
	                         "#.#......./"
	                         ".#.#....../"
	                         "....##..../"
	                         "...#.#..../"
	                         "....#.#.../"
	                         "......##../"
	                         "......#.#./"
	                         "......#..#/",
	                         &nng);
	iscc_SeedResult sr = {
		.capacity = 10,
		.count = 0,
		.seeds = NULL,
	};
	scc_PointIndex fp_seeds[3] = {0, 4, 7};

	scc_ErrorCode ec = iscc_findseeds_lexical(&nng, &sr);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(sr.capacity, 10);
	assert_int_equal(sr.count, 3);
	assert_non_null(sr.seeds);
	assert_memory_equal(sr.seeds, fp_seeds, 3 * sizeof(scc_PointIndex));

	free(sr.seeds);
	iscc_free_digraph(&nng);
}


void scc_ut_findseeds_inwards_withdiag(void** state)
{
	(void) state;

	iscc_Digraph nng1;
	iscc_digraph_from_string("#..#....../"
	                         ".#.#....../"
	                         "#.#......./"
	                         ".#.#....../"
	                         "....##..../"
	                         ".#...#..../"
	                         "....#.#.../"
	                         "......##../"
	                         "......#.#./"
	                         "......#..#/",
	                         &nng1);
	iscc_SeedResult sr1 = {
		.capacity = 10,
		.count = 0,
		.seeds = NULL,
	};
	scc_PointIndex fp_seeds1[4] = {2, 7, 4, 1};

	scc_ErrorCode ec1 = iscc_findseeds_inwards(&nng1, false, &sr1);
	assert_int_equal(ec1, SCC_ER_OK);
	assert_int_equal(sr1.capacity, 10);
	assert_int_equal(sr1.count, 4);
	assert_non_null(sr1.seeds);
	assert_memory_equal(sr1.seeds, fp_seeds1, 4 * sizeof(scc_PointIndex));
	free(sr1.seeds);
	iscc_free_digraph(&nng1);

	iscc_Digraph nng2;
	iscc_digraph_from_string("#..#....../"
	                         ".#.#....../"
	                         "#.#......./"
	                         ".#.#....../"
	                         "....##..../"
	                         ".#...#..../"
	                         "....#.#.../"
	                         "......##../"
	                         "......#.#./"
	                         "......#..#/",
	                         &nng2);
	iscc_SeedResult sr2 = {
		.capacity = 10,
		.count = 0,
		.seeds = NULL,
	};
	scc_PointIndex fp_seeds2[4] = {2, 7, 4, 3};

	scc_ErrorCode ec2 = iscc_findseeds_inwards(&nng2, true, &sr2);
	assert_int_equal(ec2, SCC_ER_OK);
	assert_int_equal(sr2.capacity, 10);
	assert_int_equal(sr2.count, 4);
	assert_non_null(sr2.seeds);
	assert_memory_equal(sr2.seeds, fp_seeds2, 4 * sizeof(scc_PointIndex));

	free(sr2.seeds);
	iscc_free_digraph(&nng2);
}


void scc_ut_findseeds_inwards_alt_withdiag(void** state)
{
	(void) state;

	iscc_Digraph nng1;
	iscc_digraph_from_string("#..#....../"
	                         ".#.#....../"
	                         "#.#......./"
	                         ".#.#....../"
	                         "....##..../"
	                         ".#...#..../"
	                         "....#.#.../"
	                         "......##../"
	                         "......#.#./"
	                         "......#..#/",
	                         &nng1);
	iscc_SeedResult sr1 = {
		.capacity = 10,
		.count = 0,
		.seeds = NULL,
	};
	scc_PointIndex fp_seeds1[4] = {2, 7, 4, 3};

	scc_ErrorCode ec1 = iscc_findseeds_inwards_alt(&nng1, &sr1);
	assert_int_equal(ec1, SCC_ER_OK);
	assert_int_equal(sr1.capacity, 10);
	assert_int_equal(sr1.count, 4);
	assert_non_null(sr1.seeds);
	assert_memory_equal(sr1.seeds, fp_seeds1, 4 * sizeof(scc_PointIndex));
	free(sr1.seeds);
	iscc_free_digraph(&nng1);
}


void scc_ut_findseeds_exclusion_withdiag(void** state)
{
	(void) state;

	iscc_Digraph nng1;
	iscc_digraph_from_string("#..#....../"
	                         ".#.#....../"
	                         ".##......./"
	                         ".#.#....../"
	                         "....##..../"
	                         ".#...#..../"
	                         "....#.#.../"
	                         "......##../"
	                         "......#.#./"
	                         "......#..#/",
	                         &nng1);
	iscc_SeedResult sr1 = {
		.capacity = 10,
		.count = 0,
		.seeds = NULL,
	};
	scc_PointIndex fp_seeds1[4] = {0, 4, 2, 7};

	scc_ErrorCode ec1 = iscc_findseeds_exclusion(&nng1, false, &sr1);
	assert_int_equal(ec1, SCC_ER_OK);
	assert_int_equal(sr1.capacity, 10);
	assert_int_equal(sr1.count, 4);
	assert_non_null(sr1.seeds);
	assert_memory_equal(sr1.seeds, fp_seeds1, 4 * sizeof(scc_PointIndex));
	free(sr1.seeds);
	iscc_free_digraph(&nng1);

	iscc_Digraph nng2;
	iscc_digraph_from_string("#..#. ...../"
	                         ".#.#. ...../"
	                         ".##.. ...../"
	                         ".#.#. ...../"
	                         "....# #..../"
	                         ".#... #..../"
	                         "....# .#.../"
	                         "..... .##../"
	                         "..... .#.#./"
	                         "..... .#..#/",
	                         &nng2);
	iscc_SeedResult sr2 = {
		.capacity = 10,
		.count = 0,
		.seeds = NULL,
	};
	scc_PointIndex fp_seeds2[4] = {0, 2, 4, 9};

	scc_ErrorCode ec2 = iscc_findseeds_exclusion(&nng2, true, &sr2);
	assert_int_equal(ec2, SCC_ER_OK);
	assert_int_equal(sr2.capacity, 10);
	assert_int_equal(sr2.count, 4);
	assert_non_null(sr2.seeds);
	assert_memory_equal(sr2.seeds, fp_seeds2, 4 * sizeof(scc_PointIndex));

	free(sr2.seeds);
	iscc_free_digraph(&nng2);
}


void scc_ut_findseeds_lexical_caliper(void** state)
{
	(void) state;

	iscc_Digraph nng;
	iscc_digraph_from_string("..... ...../"
	                         "..... #.#../"
	                         "..... #..../"
	                         "..... .##../"
	                         "..... ...../"
	                         "..... ...../"
	                         ".#... ..#../"
	                         "..#.# ...../"
	                         "...## ....#/"
	                         "...#. .#.../",
	                         &nng);
	iscc_SeedResult sr = {
		.capacity = 10,
		.count = 0,
		.seeds = NULL,
	};
	scc_PointIndex fp_seeds[2] = {1, 8};

	scc_ErrorCode ec = iscc_findseeds_lexical(&nng, &sr);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(sr.capacity, 10);
	assert_int_equal(sr.count, 2);
	assert_non_null(sr.seeds);
	assert_memory_equal(sr.seeds, fp_seeds, 2 * sizeof(scc_PointIndex));

	free(sr.seeds);
	iscc_free_digraph(&nng);
}


void scc_ut_findseeds_inwards_caliper(void** state)
{
	(void) state;

	iscc_Digraph nng1;
	iscc_digraph_from_string("..... ...../"
	                         "..... #.#../"
	                         "..... #..../"
	                         "..... .##../"
	                         "..... ...../"
	                         "..... ...../"
	                         ".#... ..#../"
	                         "..#.# ...../"
	                         "...## ....#/"
	                         "...#. .#.../",
	                         &nng1);
	iscc_SeedResult sr1 = {
		.capacity = 10,
		.count = 0,
		.seeds = NULL,
	};
	scc_PointIndex fp_seeds1[2] = {8, 1};

	scc_ErrorCode ec1 = iscc_findseeds_inwards(&nng1, false, &sr1);
	assert_int_equal(ec1, SCC_ER_OK);
	assert_int_equal(sr1.capacity, 10);
	assert_int_equal(sr1.count, 2);
	assert_non_null(sr1.seeds);
	assert_memory_equal(sr1.seeds, fp_seeds1, 2 * sizeof(scc_PointIndex));
	free(sr1.seeds);
	iscc_free_digraph(&nng1);

	iscc_Digraph nng2;
	iscc_digraph_from_string("..... ...../"
	                         "..... #.#../"
	                         "..... #..../"
	                         "..... .##../"
	                         "..... ...../"
	                         "..... ...../"
	                         ".#... ..#../"
	                         "..#.# ...../"
	                         "...## ....#/"
	                         "...#. .#.../",
	                         &nng2);
	iscc_SeedResult sr2 = {
		.capacity = 10,
		.count = 0,
		.seeds = NULL,
	};
	scc_PointIndex fp_seeds2[3] = {8, 6, 2};

	scc_ErrorCode ec2 = iscc_findseeds_inwards(&nng2, true, &sr2);
	assert_int_equal(ec2, SCC_ER_OK);
	assert_int_equal(sr2.capacity, 10);
	assert_int_equal(sr2.count, 3);
	assert_non_null(sr2.seeds);
	assert_memory_equal(sr2.seeds, fp_seeds2, 3 * sizeof(scc_PointIndex));

	free(sr2.seeds);
	iscc_free_digraph(&nng2);
}


void scc_ut_findseeds_inwards_alt_caliper(void** state)
{
	(void) state;

	iscc_Digraph nng1;
	iscc_digraph_from_string("..... ...../"
	                         "..... #.#../"
	                         "..... #..../"
	                         "..... .##../"
	                         "..... ...../"
	                         "..... ...../"
	                         ".#... ..#../"
	                         "..#.# ...../"
	                         "...## ....#/"
	                         "...#. .#.../",
	                         &nng1);
	iscc_SeedResult sr1 = {
		.capacity = 10,
		.count = 0,
		.seeds = NULL,
	};
	scc_PointIndex fp_seeds1[3] = {8, 6, 2};

	scc_ErrorCode ec1 = iscc_findseeds_inwards_alt(&nng1, &sr1);
	assert_int_equal(ec1, SCC_ER_OK);
	assert_int_equal(sr1.capacity, 10);
	assert_int_equal(sr1.count, 3);
	assert_non_null(sr1.seeds);
	assert_memory_equal(sr1.seeds, fp_seeds1, 3 * sizeof(scc_PointIndex));
	free(sr1.seeds);
	iscc_free_digraph(&nng1);
}


void scc_ut_findseeds_exclusion_caliper(void** state)
{
	(void) state;

	iscc_Digraph nng1;
	iscc_digraph_from_string("..... ...../"
	                         "..... #.#../"
	                         "..... #..../"
	                         "..... .##../"
	                         "..... ...../"
	                         "..... ...../"
	                         ".#... ..#../"
	                         "..#.# ...../"
	                         "...## ....#/"
	                         "...#. .#.../",
	                         &nng1);
	iscc_SeedResult sr1 = {
		.capacity = 10,
		.count = 0,
		.seeds = NULL,
	};
	scc_PointIndex fp_seeds1[3] = {2, 8, 6};

	scc_ErrorCode ec1 = iscc_findseeds_exclusion(&nng1, false, &sr1);
	assert_int_equal(ec1, SCC_ER_OK);
	assert_int_equal(sr1.capacity, 10);
	assert_int_equal(sr1.count, 3);
	assert_non_null(sr1.seeds);
	assert_memory_equal(sr1.seeds, fp_seeds1, 3 * sizeof(scc_PointIndex));
	free(sr1.seeds);
	iscc_free_digraph(&nng1);

	iscc_Digraph nng2;
	iscc_digraph_from_string("..... ...../"
	                         "..... #.#../"
	                         "..... #..../"
	                         "..... .##../"
	                         "..... ...../"
	                         "..... ...../"
	                         ".#... ..#../"
	                         "..#.# ...../"
	                         "...## ....#/"
	                         "...#. .#.../",
	                         &nng2);
	iscc_SeedResult sr2 = {
		.capacity = 10,
		.count = 0,
		.seeds = NULL,
	};
	scc_PointIndex fp_seeds2[3] = {2, 8, 6};

	scc_ErrorCode ec2 = iscc_findseeds_exclusion(&nng2, true, &sr2);
	assert_int_equal(ec2, SCC_ER_OK);
	assert_int_equal(sr2.capacity, 10);
	assert_int_equal(sr2.count, 3);
	assert_non_null(sr2.seeds);
	assert_memory_equal(sr2.seeds, fp_seeds2, 3 * sizeof(scc_PointIndex));

	free(sr2.seeds);
	iscc_free_digraph(&nng2);
}


void scc_ut_findseeds_lexical_caliper_withdiag(void** state)
{
	(void) state;

	iscc_Digraph nng;
	iscc_digraph_from_string("..... ...../"
	                         ".#... #.#../"
	                         "..#.. #..../"
	                         "...#. .##../"
	                         "..... ...../"
	                         "..... ...../"
	                         ".#... .##../"
	                         "..#.# ..#../"
	                         "...## ...##/"
	                         "...#. .#..#/",
	                         &nng);
	iscc_SeedResult sr = {
		.capacity = 10,
		.count = 0,
		.seeds = NULL,
	};
	scc_PointIndex fp_seeds[2] = {1, 8};

	scc_ErrorCode ec = iscc_findseeds_lexical(&nng, &sr);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(sr.capacity, 10);
	assert_int_equal(sr.count, 2);
	assert_non_null(sr.seeds);
	assert_memory_equal(sr.seeds, fp_seeds, 2 * sizeof(scc_PointIndex));

	free(sr.seeds);
	iscc_free_digraph(&nng);
}


void scc_ut_findseeds_inwards_caliper_withdiag(void** state)
{
	(void) state;

	iscc_Digraph nng1;
	iscc_digraph_from_string("..... ...../"
	                         ".#... #.#../"
	                         "..#.. #..../"
	                         "...#. .##../"
	                         "..... ...../"
	                         "..... ...../"
	                         ".#... .##../"
	                         "..#.# ..#../"
	                         "...## ...##/"
	                         "...#. .#..#/",
	                         &nng1);
	iscc_SeedResult sr1 = {
		.capacity = 10,
		.count = 0,
		.seeds = NULL,
	};
	scc_PointIndex fp_seeds1[2] = {8, 1};

	scc_ErrorCode ec1 = iscc_findseeds_inwards(&nng1, false, &sr1);
	assert_int_equal(ec1, SCC_ER_OK);
	assert_int_equal(sr1.capacity, 10);
	assert_int_equal(sr1.count, 2);
	assert_non_null(sr1.seeds);
	assert_memory_equal(sr1.seeds, fp_seeds1, 2 * sizeof(scc_PointIndex));
	free(sr1.seeds);
	iscc_free_digraph(&nng1);

	iscc_Digraph nng2;
	iscc_digraph_from_string("..... ...../"
	                         ".#... #.#../"
	                         "..#.. #..../"
	                         "...#. .##../"
	                         "..... ...../"
	                         "..... ...../"
	                         ".#... .##../"
	                         "..#.# ..#../"
	                         "...## ...##/"
	                         "...#. .#..#/",
	                         &nng2);
	iscc_SeedResult sr2 = {
		.capacity = 10,
		.count = 0,
		.seeds = NULL,
	};
	scc_PointIndex fp_seeds2[3] = {8, 6, 2};

	scc_ErrorCode ec2 = iscc_findseeds_inwards(&nng2, true, &sr2);
	assert_int_equal(ec2, SCC_ER_OK);
	assert_int_equal(sr2.capacity, 10);
	assert_int_equal(sr2.count, 3);
	assert_non_null(sr2.seeds);
	assert_memory_equal(sr2.seeds, fp_seeds2, 3 * sizeof(scc_PointIndex));

	free(sr2.seeds);
	iscc_free_digraph(&nng2);
}


void scc_ut_findseeds_inwards_alt_caliper_withdiag(void** state)
{
	(void) state;

	iscc_Digraph nng1;
	iscc_digraph_from_string("..... ...../"
	                         ".#... #.#../"
	                         "..#.. #..../"
	                         "...#. .##../"
	                         "..... ...../"
	                         "..... ...../"
	                         ".#... .##../"
	                         "..#.# ..#../"
	                         "...## ...##/"
	                         "...#. .#..#/",
	                         &nng1);
	iscc_SeedResult sr1 = {
		.capacity = 10,
		.count = 0,
		.seeds = NULL,
	};
	scc_PointIndex fp_seeds1[3] = {8, 6, 2};

	scc_ErrorCode ec1 = iscc_findseeds_inwards_alt(&nng1, &sr1);
	assert_int_equal(ec1, SCC_ER_OK);
	assert_int_equal(sr1.capacity, 10);
	assert_int_equal(sr1.count, 3);
	assert_non_null(sr1.seeds);
	assert_memory_equal(sr1.seeds, fp_seeds1, 3 * sizeof(scc_PointIndex));
	free(sr1.seeds);
	iscc_free_digraph(&nng1);
}


void scc_ut_findseeds_exclusion_caliper_withdiag(void** state)
{
	(void) state;

	iscc_Digraph nng1;
	iscc_digraph_from_string("..... ...../"
	                         ".#... #.#../"
	                         "..#.. #..../"
	                         "...#. .##../"
	                         "..... ...../"
	                         "..... ...../"
	                         ".#... .##../"
	                         "..#.# ..#../"
	                         "...## ...##/"
	                         "...#. .#..#/",
	                         &nng1);
	iscc_SeedResult sr1 = {
		.capacity = 10,
		.count = 0,
		.seeds = NULL,
	};
	scc_PointIndex fp_seeds1[3] = {2, 8, 6};

	scc_ErrorCode ec1 = iscc_findseeds_exclusion(&nng1, false, &sr1);
	assert_int_equal(ec1, SCC_ER_OK);
	assert_int_equal(sr1.capacity, 10);
	assert_int_equal(sr1.count, 3);
	assert_non_null(sr1.seeds);
	assert_memory_equal(sr1.seeds, fp_seeds1, 3 * sizeof(scc_PointIndex));
	free(sr1.seeds);
	iscc_free_digraph(&nng1);

	iscc_Digraph nng2;
	iscc_digraph_from_string("..... ...../"
	                         ".#... #.#../"
	                         "..#.. #..../"
	                         "...#. .##../"
	                         "..... ...../"
	                         "..... ...../"
	                         ".#... .##../"
	                         "..#.# ..#../"
	                         "...## ...##/"
	                         "...#. .#..#/",
	                         &nng2);
	iscc_SeedResult sr2 = {
		.capacity = 10,
		.count = 0,
		.seeds = NULL,
	};
	scc_PointIndex fp_seeds2[3] = {2, 8, 6};

	scc_ErrorCode ec2 = iscc_findseeds_exclusion(&nng2, true, &sr2);
	assert_int_equal(ec2, SCC_ER_OK);
	assert_int_equal(sr2.capacity, 10);
	assert_int_equal(sr2.count, 3);
	assert_non_null(sr2.seeds);
	assert_memory_equal(sr2.seeds, fp_seeds2, 3 * sizeof(scc_PointIndex));

	free(sr2.seeds);
	iscc_free_digraph(&nng2);
}


void scc_ut_findseeds_lexical_caliper_selfpointing_withdiag(void** state)
{
	(void) state;

	iscc_Digraph nng;
	iscc_digraph_from_string("#.... ...../"
	                         ".#... #.#../"
	                         "..#.. #..../"
	                         "...#. .##../"
	                         "..... ...../"
	                         "..... #..../"
	                         ".#... .##../"
	                         "..#.# ..#../"
	                         "...## ...##/"
	                         "...#. .#..#/",
	                         &nng);
	iscc_SeedResult sr = {
		.capacity = 10,
		.count = 0,
		.seeds = NULL,
	};
	scc_PointIndex fp_seeds[3] = {0, 1, 8};

	scc_ErrorCode ec = iscc_findseeds_lexical(&nng, &sr);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(sr.capacity, 10);
	assert_int_equal(sr.count, 3);
	assert_non_null(sr.seeds);
	assert_memory_equal(sr.seeds, fp_seeds, 3 * sizeof(scc_PointIndex));

	free(sr.seeds);
	iscc_free_digraph(&nng);
}


void scc_ut_findseeds_inwards_caliper_selfpointing_withdiag(void** state)
{
	(void) state;

	iscc_Digraph nng1;
	iscc_digraph_from_string("#.... ...../"
	                         ".#... #.#../"
	                         "..#.. #..../"
	                         "...#. .##../"
	                         "..... ...../"
	                         "..... #..../"
	                         ".#... .##../"
	                         "..#.# ..#../"
	                         "...## ...##/"
	                         "...#. .#..#/",
	                         &nng1);
	iscc_SeedResult sr1 = {
		.capacity = 10,
		.count = 0,
		.seeds = NULL,
	};
	scc_PointIndex fp_seeds1[3] = {0, 8, 1};

	scc_ErrorCode ec1 = iscc_findseeds_inwards(&nng1, false, &sr1);
	assert_int_equal(ec1, SCC_ER_OK);
	assert_int_equal(sr1.capacity, 10);
	assert_int_equal(sr1.count, 3);
	assert_non_null(sr1.seeds);
	assert_memory_equal(sr1.seeds, fp_seeds1, 3 * sizeof(scc_PointIndex));
	free(sr1.seeds);
	iscc_free_digraph(&nng1);

	iscc_Digraph nng2;
	iscc_digraph_from_string("#.... ...../"
	                         ".#... #.#../"
	                         "..#.. #..../"
	                         "...#. .##../"
	                         "..... ...../"
	                         "..... #..../"
	                         ".#... .##../"
	                         "..#.# ..#../"
	                         "...## ...##/"
	                         "...#. .#..#/",
	                         &nng2);
	iscc_SeedResult sr2 = {
		.capacity = 10,
		.count = 0,
		.seeds = NULL,
	};
	scc_PointIndex fp_seeds2[4] = {0, 8, 6, 2};

	scc_ErrorCode ec2 = iscc_findseeds_inwards(&nng2, true, &sr2);
	assert_int_equal(ec2, SCC_ER_OK);
	assert_int_equal(sr2.capacity, 10);
	assert_int_equal(sr2.count, 4);
	assert_non_null(sr2.seeds);
	assert_memory_equal(sr2.seeds, fp_seeds2, 4 * sizeof(scc_PointIndex));

	free(sr2.seeds);
	iscc_free_digraph(&nng2);
}


void scc_ut_findseeds_inwards_alt_caliper_selfpointing_withdiag(void** state)
{
	(void) state;

	iscc_Digraph nng1;
	iscc_digraph_from_string("#.... ...../"
	                         ".#... #.#../"
	                         "..#.. #..../"
	                         "...#. .##../"
	                         "..... ...../"
	                         "..... #..../"
	                         ".#... .##../"
	                         "..#.# ..#../"
	                         "...## ...##/"
	                         "...#. .#..#/",
	                         &nng1);
	iscc_SeedResult sr1 = {
		.capacity = 10,
		.count = 0,
		.seeds = NULL,
	};
	scc_PointIndex fp_seeds1[4] = {0, 8, 6, 2};

	scc_ErrorCode ec1 = iscc_findseeds_inwards_alt(&nng1, &sr1);
	assert_int_equal(ec1, SCC_ER_OK);
	assert_int_equal(sr1.capacity, 10);
	assert_int_equal(sr1.count, 4);
	assert_non_null(sr1.seeds);
	assert_memory_equal(sr1.seeds, fp_seeds1, 4 * sizeof(scc_PointIndex));
	free(sr1.seeds);
	iscc_free_digraph(&nng1);
}


void scc_ut_findseeds_exclusion_caliper_selfpointing_withdiag(void** state)
{
	(void) state;

	iscc_Digraph nng1;
	iscc_digraph_from_string("#.... ...../"
	                         ".#... #.#../"
	                         "..#.. #..../"
	                         "...#. .##../"
	                         "..... ...../"
	                         "..... #..../"
	                         ".#... .##../"
	                         "..#.# ..#../"
	                         "...## ...##/"
	                         "...#. .#..#/",
	                         &nng1);
	iscc_SeedResult sr1 = {
		.capacity = 10,
		.count = 0,
		.seeds = NULL,
	};
	scc_PointIndex fp_seeds1[4] = {0, 5, 8, 6};

	scc_ErrorCode ec1 = iscc_findseeds_exclusion(&nng1, false, &sr1);
	assert_int_equal(ec1, SCC_ER_OK);
	assert_int_equal(sr1.capacity, 10);
	assert_int_equal(sr1.count, 4);
	assert_non_null(sr1.seeds);
	assert_memory_equal(sr1.seeds, fp_seeds1, 4 * sizeof(scc_PointIndex));
	free(sr1.seeds);
	iscc_free_digraph(&nng1);

	iscc_Digraph nng2;
	iscc_digraph_from_string("#.... ...../"
	                         ".#... #.#../"
	                         "..#.. #..../"
	                         "...#. .##../"
	                         "..... ...../"
	                         "..... #..../"
	                         ".#... .##../"
	                         "..#.# ..#../"
	                         "...## ...##/"
	                         "...#. .#..#/",
	                         &nng2);
	iscc_SeedResult sr2 = {
		.capacity = 10,
		.count = 0,
		.seeds = NULL,
	};
	scc_PointIndex fp_seeds2[4] = {0, 5, 8, 6};

	scc_ErrorCode ec2 = iscc_findseeds_exclusion(&nng2, true, &sr2);
	assert_int_equal(ec2, SCC_ER_OK);
	assert_int_equal(sr2.capacity, 10);
	assert_int_equal(sr2.count, 4);
	assert_non_null(sr2.seeds);
	assert_memory_equal(sr2.seeds, fp_seeds2, 4 * sizeof(scc_PointIndex));

	free(sr2.seeds);
	iscc_free_digraph(&nng2);
}


void scc_ut_fs_exclusion_graph(void** state)
{
	(void) state;

	bool keep_vertex[18] = { true, true, true, true, true, true,
	                         true, true, true, true, true, true,
	                         true, true, true, true, true, true };
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
	iscc_Digraph exg;
	iscc_digraph_from_string(".####............./"
	                         "#.###............./"
	                         "##.###.#..#......./"
	                         "###.#.#.........../"
	                         "####..#.........../"
	                         "..#....##..#....../"
	                         "...##..#.#.....#../"
	                         "..#..##.#.##....../"
	                         ".....#.#...#....../"
	                         "......#...###..###/"
	                         "..#....#.#.#.##.#./"
	                         ".....#.####......./"
	                         ".........#.....###/"
	                         "..........#...#.##/"
	                         "..........#..#..##/"
	                         "......#..#..#...../"
	                         ".........##.###..#/"
	                         ".........#..###.#./",
	                         &exg);
	iscc_Digraph exclusion_graph;
	scc_ErrorCode ec1 = iscc_fs_exclusion_graph(&nng, keep_vertex, &exclusion_graph);
	assert_int_equal(ec1, SCC_ER_OK);
	assert_equal_digraph(&exg, &exclusion_graph);
	iscc_free_digraph(&nng);
	iscc_free_digraph(&exg);
	iscc_free_digraph(&exclusion_graph);

	bool keep_vertex2[10] = { true, true, true, true, true,
	                          true, true, true, true, true };
	iscc_Digraph nng2;
	iscc_digraph_from_string(".#..# ...../"
	                         "..... ...../"
	                         ".#... ..#../"
	                         "..#.. #..../"
	                         "..... ...../"
	                         "#.... ...../"
	                         "..#.. ...../"
	                         "....# ...../"
	                         "..... ..#../"
	                         "..... ..##./",
	                         &nng2);

	iscc_Digraph exg2;
	iscc_digraph_from_string(".##.# #.#../"
	                         "#.#.. ...../"
	                         "##.#. .####/"
	                         "..#.. ##.../"
	                         "#.... ..#../"
	                         "#..#. ...../"
	                         "..##. ...../"
	                         "#.#.# ...##/"
	                         "..#.. ..#.#/"
	                         "..#.. ..##./",
	                         &exg2);
	iscc_Digraph exclusion_graph2;
	scc_ErrorCode ec2 = iscc_fs_exclusion_graph(&nng2, keep_vertex2, &exclusion_graph2);
	assert_int_equal(ec2, SCC_ER_OK);
	assert_equal_digraph(&exg2, &exclusion_graph2);
	iscc_free_digraph(&nng2);
	iscc_free_digraph(&exg2);
	iscc_free_digraph(&exclusion_graph2);

	iscc_Digraph nng3;
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
	                         &nng3);
	iscc_Digraph exg3;
	iscc_digraph_from_string(".####............./"
	                         "#.###............./"
	                         "##.###.#..#......./"
	                         "###.#.#.........../"
	                         "####..#.........../"
	                         "..#....##..#....../"
	                         "...##..#.#.....#../"
	                         "..#..##.#.##....../"
	                         ".....#.#...#....../"
	                         "......#...###..###/"
	                         "..#....#.#.#.##.#./"
	                         ".....#.####......./"
	                         ".........#.....###/"
	                         "..........#...#.##/"
	                         "..........#..#..##/"
	                         "......#..#..#...../"
	                         ".........##.###..#/"
	                         ".........#..###.#./",
	                         &exg3);
	iscc_Digraph exclusion_graph3;
	scc_ErrorCode ec3 = iscc_fs_exclusion_graph(&nng3, keep_vertex, &exclusion_graph3);
	assert_int_equal(ec3, SCC_ER_OK);
	assert_equal_digraph(&exg3, &exclusion_graph3);
	iscc_free_digraph(&nng3);
	iscc_free_digraph(&exg3);
	iscc_free_digraph(&exclusion_graph3);

	iscc_Digraph nng4;
	iscc_digraph_from_string("##..# ...../"
	                         ".#... ...../"
	                         ".##.. ..#../"
	                         "..##. #..../"
	                         "....# ...../"
	                         "#.... #..../"
	                         "..#.. .#.../"
	                         "....# ..#../"
	                         "..... ..##./"
	                         "..... ..###/",
	                         &nng4);

	iscc_Digraph exg4;
	iscc_digraph_from_string(".##.# #.#../"
	                         "#.#.. ...../"
	                         "##.#. .####/"
	                         "..#.. ##.../"
	                         "#.... ..#../"
	                         "#..#. ...../"
	                         "..##. ...../"
	                         "#.#.# ...##/"
	                         "..#.. ..#.#/"
	                         "..#.. ..##./",
	                         &exg4);
	iscc_Digraph exclusion_graph4;
	scc_ErrorCode ec4 = iscc_fs_exclusion_graph(&nng4, keep_vertex2, &exclusion_graph4);
	assert_int_equal(ec4, SCC_ER_OK);
	assert_equal_digraph(&exg4, &exclusion_graph4);
	iscc_free_digraph(&nng4);
	iscc_free_digraph(&exg4);
	iscc_free_digraph(&exclusion_graph4);


	bool keep_vertex5[18] = { true, false, true, false, true, false,
	                          true, false, true, false, true, false,
	                          true, false, true, false, true, false };
	iscc_Digraph nng5;
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
	                         &nng5);
	iscc_Digraph exg5;
	iscc_digraph_from_string(".####............./"
	                         "................../"
	                         "##.###.#..#......./"
	                         "................../"
	                         "####..#.........../"
	                         "................../"
	                         "...##..#.#.....#../"
	                         "................../"
	                         ".....#.#...#....../"
	                         "................../"
	                         "..#....#.#.#.##.#./"
	                         "................../"
	                         ".........#.....###/"
	                         "................../"
	                         "..........#..#..##/"
	                         "................../"
	                         ".........##.###..#/"
	                         "................../",
	                         &exg5);
	iscc_Digraph exclusion_graph5;
	scc_ErrorCode ec5 = iscc_fs_exclusion_graph(&nng5, keep_vertex5, &exclusion_graph5);
	assert_int_equal(ec5, SCC_ER_OK);
	assert_equal_digraph(&exg5, &exclusion_graph5);
	iscc_free_digraph(&nng5);
	iscc_free_digraph(&exg5);
	iscc_free_digraph(&exclusion_graph5);

	bool keep_vertex6[10] = { true, false, true, false, true,
	                          true, true, true, false, false };
	iscc_Digraph nng6;
	iscc_digraph_from_string(".#..# ...../"
	                         "..... ...../"
	                         ".#... ..#../"
	                         "..#.. #..../"
	                         "..... ...../"
	                         "#.... ...../"
	                         "..#.. ...../"
	                         "....# ...../"
	                         "..... ..#../"
	                         "..... ..##./",
	                         &nng6);

	iscc_Digraph exg6;
	iscc_digraph_from_string(".##.# #.#../"
	                         "..... ...../"
	                         "##.#. .####/"
	                         "..... ...../"
	                         "#.... ..#../"
	                         "#..#. ...../"
	                         "..##. ...../"
	                         "#.#.# ...##/"
	                         "..... ...../"
	                         "..... ...../",
	                         &exg6);
	iscc_Digraph exclusion_graph6;
	scc_ErrorCode ec6 = iscc_fs_exclusion_graph(&nng6, keep_vertex6, &exclusion_graph6);
	assert_int_equal(ec6, SCC_ER_OK);
	assert_equal_digraph(&exg6, &exclusion_graph6);
	iscc_free_digraph(&nng6);
	iscc_free_digraph(&exg6);
	iscc_free_digraph(&exclusion_graph6);
}


void scc_ut_fs_add_seed(void** state)
{
	(void) state;

	const scc_PointIndex ref_seeds[8] = { 2, 7, 4, 3, 9, 5, 1, 0 };

	iscc_SeedResult sr1 = {
		.capacity = 1,
		.count = 0,
		.seeds = malloc(sizeof(scc_PointIndex[1])),
	};

	scc_ErrorCode ec;

	ec = iscc_fs_add_seed(2, &sr1);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(sr1.capacity, 1);
	assert_int_equal(sr1.count, 1);
	assert_memory_equal(sr1.seeds, ref_seeds, 1 * sizeof(scc_PointIndex));

	ec = iscc_fs_add_seed(7, &sr1);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(sr1.capacity, 1025);
	assert_int_equal(sr1.count, 2);
	assert_memory_equal(sr1.seeds, ref_seeds, 2 * sizeof(scc_PointIndex));

	ec = iscc_fs_add_seed(4, &sr1);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(sr1.capacity, 1025);
	assert_int_equal(sr1.count, 3);
	assert_memory_equal(sr1.seeds, ref_seeds, 3 * sizeof(scc_PointIndex));

	ec = iscc_fs_add_seed(3, &sr1);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(sr1.capacity, 1025);
	assert_int_equal(sr1.count, 4);
	assert_memory_equal(sr1.seeds, ref_seeds, 4 * sizeof(scc_PointIndex));

	ec = iscc_fs_add_seed(9, &sr1);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(sr1.capacity, 1025);
	assert_int_equal(sr1.count, 5);
	assert_memory_equal(sr1.seeds, ref_seeds, 5 * sizeof(scc_PointIndex));

	ec = iscc_fs_add_seed(5, &sr1);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(sr1.capacity, 1025);
	assert_int_equal(sr1.count, 6);
	assert_memory_equal(sr1.seeds, ref_seeds, 6 * sizeof(scc_PointIndex));

	ec = iscc_fs_add_seed(1, &sr1);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(sr1.capacity, 1025);
	assert_int_equal(sr1.count, 7);
	assert_memory_equal(sr1.seeds, ref_seeds, 7 * sizeof(scc_PointIndex));

	ec = iscc_fs_add_seed(0, &sr1);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(sr1.capacity, 1025);
	assert_int_equal(sr1.count, 8);
	assert_memory_equal(sr1.seeds, ref_seeds, 8 * sizeof(scc_PointIndex));

	free(sr1.seeds);

	iscc_SeedResult sr2 = {
		.capacity = 10,
		.count = 3,
		.seeds = malloc(sizeof(scc_PointIndex[10])),
	};

	sr2.seeds[0] = 2;
	sr2.seeds[1] = 7;
	sr2.seeds[2] = 4;

	assert_int_equal(sr2.capacity, 10);
	assert_int_equal(sr2.count, 3);
	assert_memory_equal(sr2.seeds, ref_seeds, 3 * sizeof(scc_PointIndex));

	ec = iscc_fs_add_seed(3, &sr2);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(sr2.capacity, 10);
	assert_int_equal(sr2.count, 4);
	assert_memory_equal(sr2.seeds, ref_seeds, 4 * sizeof(scc_PointIndex));

	ec = iscc_fs_add_seed(9, &sr2);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(sr2.capacity, 10);
	assert_int_equal(sr2.count, 5);
	assert_memory_equal(sr2.seeds, ref_seeds, 5 * sizeof(scc_PointIndex));

	ec = iscc_fs_add_seed(5, &sr2);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(sr2.capacity, 10);
	assert_int_equal(sr2.count, 6);
	assert_memory_equal(sr2.seeds, ref_seeds, 6 * sizeof(scc_PointIndex));

	ec = iscc_fs_add_seed(1, &sr2);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(sr2.capacity, 10);
	assert_int_equal(sr2.count, 7);
	assert_memory_equal(sr2.seeds, ref_seeds, 7 * sizeof(scc_PointIndex));

	ec = iscc_fs_add_seed(0, &sr2);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(sr2.capacity, 10);
	assert_int_equal(sr2.count, 8);
	assert_memory_equal(sr2.seeds, ref_seeds, 8 * sizeof(scc_PointIndex));

	free(sr2.seeds);

	iscc_SeedResult sr3 = {
		.capacity = SCC_CLABEL_MAX,
		.count = SCC_CLABEL_MAX,
		.seeds = malloc(sizeof(scc_PointIndex[1])),
	};

	ec = iscc_fs_add_seed(0, &sr3);
	assert_int_equal(ec, SCC_ER_TOO_LARGE_PROBLEM);

	free(sr3.seeds);
}


void scc_ut_fs_check_neighbors_marks(void** state)
{
	(void) state;

	iscc_Digraph nng;
	iscc_digraph_from_string("..##..#/"
	                         "..#.#.#/"
	                         "#...#.#/"
	                         "..#..##/"
	                         "......./"
	                         "..#..../"
	                         "......./",
	                         &nng);

	bool marks[7] = {true, false, false, false, true, false, false};

	assert_false(iscc_fs_check_neighbors_marks(0, &nng, marks));
	assert_false(iscc_fs_check_neighbors_marks(1, &nng, marks));
	assert_false(iscc_fs_check_neighbors_marks(2, &nng, marks));
	assert_true(iscc_fs_check_neighbors_marks(3, &nng, marks));
	assert_false(iscc_fs_check_neighbors_marks(4, &nng, marks));
	assert_true(iscc_fs_check_neighbors_marks(5, &nng, marks));
	assert_false(iscc_fs_check_neighbors_marks(6, &nng, marks));

	iscc_free_digraph(&nng);
}


void scc_ut_fs_check_neighbors_marks_withdiag(void** state)
{
	(void) state;

	iscc_Digraph nng;
	iscc_digraph_from_string("#.##..#/"
	                         ".##.#.#/"
	                         "#.#.#.#/"
	                         "..##.##/"
	                         "....#../"
	                         "..#..#./"
	                         "......./",
	                         &nng);

	bool marks[7] = {true, false, false, false, true, false, false};

	assert_false(iscc_fs_check_neighbors_marks(0, &nng, marks));
	assert_false(iscc_fs_check_neighbors_marks(1, &nng, marks));
	assert_false(iscc_fs_check_neighbors_marks(2, &nng, marks));
	assert_true(iscc_fs_check_neighbors_marks(3, &nng, marks));
	assert_false(iscc_fs_check_neighbors_marks(4, &nng, marks));
	assert_true(iscc_fs_check_neighbors_marks(5, &nng, marks));
	assert_false(iscc_fs_check_neighbors_marks(6, &nng, marks));

	iscc_free_digraph(&nng);
}


void scc_ut_fs_mark_seed_neighbors(void** state)
{
	(void) state;

	iscc_Digraph nng;
	iscc_digraph_from_string("..##..#/"
	                         "..#.#.#/"
	                         "#...#.#/"
	                         "..#..##/"
	                         "......./"
	                         "..#..../"
	                         "......./",
	                         &nng);

	bool stc_marks[7] = {false, false, false, false, false, false, false};

	iscc_fs_mark_seed_neighbors(0, &nng, stc_marks);
	bool ref_marks0[7] = {true, false, true, true, false, false, true};
	assert_memory_equal(stc_marks, ref_marks0, 7 * sizeof(bool));

	stc_marks[0] = stc_marks[2] = stc_marks[3] = stc_marks[6] = false;

	iscc_fs_mark_seed_neighbors(1, &nng, stc_marks);
	bool ref_marks1[7] = {false, true, true, false, true, false, true};
	assert_memory_equal(stc_marks, ref_marks1, 7 * sizeof(bool));

	stc_marks[2] = stc_marks[4] = stc_marks[6] = false;

	iscc_fs_mark_seed_neighbors(2, &nng, stc_marks);
	bool ref_marks2[7] = {true, true, true, false, true, false, true};
	assert_memory_equal(stc_marks, ref_marks2, 7 * sizeof(bool));

	stc_marks[0] = stc_marks[1] = stc_marks[2] = stc_marks[4] = stc_marks[6] = false;

	iscc_fs_mark_seed_neighbors(5, &nng, stc_marks);
	bool ref_marks5[7] = {false, false, true, false, false, true, false};
	assert_memory_equal(stc_marks, ref_marks5, 7 * sizeof(bool));

	stc_marks[2] = stc_marks[5] = false;

	iscc_fs_mark_seed_neighbors(3, &nng, stc_marks);
	bool ref_marks3[7] = {false, false, true, true, false, true, true};
	assert_memory_equal(stc_marks, ref_marks3, 7 * sizeof(bool));

	stc_marks[6] = false;

	iscc_fs_mark_seed_neighbors(6, &nng, stc_marks);
	bool ref_marks6[7] = {false, false, true, true, false, true, true};
	assert_memory_equal(stc_marks, ref_marks6, 7 * sizeof(bool));

	iscc_free_digraph(&nng);
}


void scc_ut_fs_mark_seed_neighbors_withdiag(void** state)
{
	(void) state;

	iscc_Digraph nng;
	iscc_digraph_from_string("#.##..#/"
	                         ".##.#.#/"
	                         "#.#.#.#/"
	                         "..##.##/"
	                         "....#../"
	                         "..#..#./"
	                         "......#/",
	                         &nng);

	bool stc_marks[7] = {false, false, false, false, false, false, false};

	iscc_fs_mark_seed_neighbors(0, &nng, stc_marks);
	bool ref_marks0[7] = {true, false, true, true, false, false, true};
	assert_memory_equal(stc_marks, ref_marks0, 7 * sizeof(bool));

	stc_marks[0] = stc_marks[2] = stc_marks[3] = stc_marks[6] = false;

	iscc_fs_mark_seed_neighbors(1, &nng, stc_marks);
	bool ref_marks1[7] = {false, true, true, false, true, false, true};
	assert_memory_equal(stc_marks, ref_marks1, 7 * sizeof(bool));

	stc_marks[2] = stc_marks[4] = stc_marks[6] = false;

	iscc_fs_mark_seed_neighbors(2, &nng, stc_marks);
	bool ref_marks2[7] = {true, true, true, false, true, false, true};
	assert_memory_equal(stc_marks, ref_marks2, 7 * sizeof(bool));

	stc_marks[0] = stc_marks[1] = stc_marks[2] = stc_marks[4] = stc_marks[6] = false;

	iscc_fs_mark_seed_neighbors(5, &nng, stc_marks);
	bool ref_marks5[7] = {false, false, true, false, false, true, false};
	assert_memory_equal(stc_marks, ref_marks5, 7 * sizeof(bool));

	stc_marks[2] = stc_marks[5] = false;

	iscc_fs_mark_seed_neighbors(3, &nng, stc_marks);
	bool ref_marks3[7] = {false, false, true, true, false, true, true};
	assert_memory_equal(stc_marks, ref_marks3, 7 * sizeof(bool));

	stc_marks[6] = false;

	iscc_fs_mark_seed_neighbors(6, &nng, stc_marks);
	bool ref_marks6[7] = {false, false, true, true, false, true, true};
	assert_memory_equal(stc_marks, ref_marks6, 7 * sizeof(bool));

	iscc_free_digraph(&nng);
}


void scc_ut_fs_free_sort_result(void** state)
{
	(void) state;

	iscc_fs_free_sort_result(NULL);

	iscc_Digraph nng;
	iscc_digraph_from_string("##...#/"
	                         "###.../"
	                         "##.##./"
	                         "#.###./"
	                         "####../"
	                         "###.../",
	                         &nng);

	iscc_fs_SortResult sort;
	iscc_fs_sort_by_inwards(&nng, false, &sort);

	iscc_fs_free_sort_result(&sort);
	iscc_free_digraph(&nng);
}


void scc_ut_fs_sort_by_inwards(void** state)
{
	(void) state;

	iscc_Digraph nng1;
	iscc_digraph_from_string("##...#/"
	                         "###.../"
	                         "##.##./"
	                         "#.###./"
	                         "####../"
	                         "###.../",
	                         &nng1);
	scc_PointIndex ref_sorted_vertices[6] = {5, 4, 3, 2, 1, 0};
	scc_PointIndex ref_inwards_count[6] = {6, 5, 4, 3, 2, 1};
	ptrdiff_t ref_vertex_index[6] = {5, 4, 3, 2, 1, 0};
	ptrdiff_t ref_bucket_index[7] = {0, 0, 1, 2, 3, 4, 5};

	iscc_fs_SortResult sort_n;
	scc_ErrorCode ec1 = iscc_fs_sort_by_inwards(&nng1, false, &sort_n);
	assert_int_equal(ec1, SCC_ER_OK);
	assert_memory_equal(sort_n.sorted_vertices, ref_sorted_vertices, 6 * sizeof(scc_PointIndex));
	assert_null(sort_n.inwards_count);
	assert_null(sort_n.vertex_index);
	assert_null(sort_n.bucket_index);

	iscc_fs_SortResult sort_i;
	scc_ErrorCode ec2 = iscc_fs_sort_by_inwards(&nng1, true, &sort_i);
	assert_int_equal(ec2, SCC_ER_OK);
	ptrdiff_t check_vertex_index[6];
	ptrdiff_t check_bucket_index[7];
	for (size_t i = 0; i < 6; ++i) check_vertex_index[i] = sort_i.vertex_index[i] - sort_i.sorted_vertices;
	for (size_t i = 0; i < 7; ++i) check_bucket_index[i] = sort_i.bucket_index[i] - sort_i.sorted_vertices;

	assert_memory_equal(sort_i.sorted_vertices, ref_sorted_vertices, 6 * sizeof(scc_PointIndex));
	assert_memory_equal(sort_i.inwards_count, ref_inwards_count, 6 * sizeof(scc_PointIndex));
	assert_memory_equal(check_vertex_index, ref_vertex_index, 6 * sizeof(ptrdiff_t));
	assert_memory_equal(check_bucket_index, ref_bucket_index, 7 * sizeof(ptrdiff_t));

	assert_memory_equal(sort_i.sorted_vertices, sort_n.sorted_vertices, 6 * sizeof(scc_PointIndex));


	iscc_Digraph nng2;
	iscc_digraph_from_string(".###.#./"
	                         ".#....#/"
	                         "##.#.#./"
	                         "#..#..#/"
	                         ".###.#./"
	                         ".....##/"
	                         "###..../",
	                         &nng2);
	scc_PointIndex ref_sorted_vertices2[7] = {4, 0, 2, 6, 3, 5, 1};
	scc_PointIndex ref_inwards_count2[7] = {3, 5, 3, 4, 0, 4, 3};
	ptrdiff_t ref_vertex_index2[7] = {1, 6, 2, 4, 0, 5, 3};
	ptrdiff_t ref_bucket_index2[6] = {0, 1, 1, 1, 4, 6};

	iscc_fs_SortResult sort_n2;
	scc_ErrorCode ec3 = iscc_fs_sort_by_inwards(&nng2, false, &sort_n2);
	assert_int_equal(ec3, SCC_ER_OK);
	assert_memory_equal(sort_n2.sorted_vertices, ref_sorted_vertices2, 7 * sizeof(scc_PointIndex));
	assert_null(sort_n2.inwards_count);
	assert_null(sort_n2.vertex_index);
	assert_null(sort_n2.bucket_index);

	iscc_fs_SortResult sort_i2;
	scc_ErrorCode ec3b = iscc_fs_sort_by_inwards(&nng2, true, &sort_i2);
	assert_int_equal(ec3b, SCC_ER_OK);
	ptrdiff_t check_vertex_index2[7];
	ptrdiff_t check_bucket_index2[6];
	for (size_t i = 0; i < 7; ++i) check_vertex_index2[i] = sort_i2.vertex_index[i] - sort_i2.sorted_vertices;
	for (size_t i = 0; i < 6; ++i) check_bucket_index2[i] = sort_i2.bucket_index[i] - sort_i2.sorted_vertices;

	assert_memory_equal(sort_i2.sorted_vertices, ref_sorted_vertices2, 7 * sizeof(scc_PointIndex));
	assert_memory_equal(sort_i2.inwards_count, ref_inwards_count2, 7 * sizeof(scc_PointIndex));
	assert_memory_equal(check_vertex_index2, ref_vertex_index2, 7 * sizeof(ptrdiff_t));
	assert_memory_equal(check_bucket_index2, ref_bucket_index2, 6 * sizeof(ptrdiff_t));

	assert_memory_equal(sort_i2.sorted_vertices, sort_n2.sorted_vertices, 7 * sizeof(scc_PointIndex));


	iscc_Digraph nng3;
	iscc_digraph_from_string("#..#...##./"
	                         ".#.##..###/"
	                         "#.##.#.##./"
	                         ".####.####/"
	                         ".###.#####/"
	                         ".###.#####/"
	                         "..#..#####/"
	                         "..#...####/"
	                         "..#...####/"
	                         "..#....###/",
	                         &nng3);
	scc_PointIndex ref_sorted_vertices3[10] = {0, 4, 1, 5, 3, 6, 2, 9, 7, 8};
	scc_PointIndex ref_inwards_count3[10] = {2, 4, 8, 6, 2, 4, 6, 10, 10, 8};
	ptrdiff_t ref_vertex_index3[10] = {0, 2, 6, 4, 1, 3, 5, 8, 9, 7};
	ptrdiff_t ref_bucket_index3[11] = {0, 0, 0, 2, 2, 4, 4, 6, 6, 8, 8};

	iscc_fs_SortResult sort_n3;
	scc_ErrorCode ec4 = iscc_fs_sort_by_inwards(&nng3, false, &sort_n3);
	assert_int_equal(ec4, SCC_ER_OK);
	assert_memory_equal(sort_n3.sorted_vertices, ref_sorted_vertices3, 10 * sizeof(scc_PointIndex));
	assert_null(sort_n3.inwards_count);
	assert_null(sort_n3.vertex_index);
	assert_null(sort_n3.bucket_index);

	iscc_fs_SortResult sort_i3;
	scc_ErrorCode ec4b = iscc_fs_sort_by_inwards(&nng3, true, &sort_i3);
	assert_int_equal(ec4b, SCC_ER_OK);
	ptrdiff_t check_vertex_index3[10];
	ptrdiff_t check_bucket_index3[11];
	for (size_t i = 0; i < 10; ++i) check_vertex_index3[i] = sort_i3.vertex_index[i] - sort_i3.sorted_vertices;
	for (size_t i = 0; i < 11; ++i) check_bucket_index3[i] = sort_i3.bucket_index[i] - sort_i3.sorted_vertices;

	assert_memory_equal(sort_i3.sorted_vertices, ref_sorted_vertices3, 10 * sizeof(scc_PointIndex));
	assert_memory_equal(sort_i3.inwards_count, ref_inwards_count3, 10 * sizeof(scc_PointIndex));
	assert_memory_equal(check_vertex_index3, ref_vertex_index3, 10 * sizeof(ptrdiff_t));
	assert_memory_equal(check_bucket_index3, ref_bucket_index3, 11 * sizeof(ptrdiff_t));

	assert_memory_equal(sort_i3.sorted_vertices, sort_n3.sorted_vertices, 10 * sizeof(scc_PointIndex));

	iscc_fs_free_sort_result(&sort_n);
	iscc_fs_free_sort_result(&sort_i);
	iscc_fs_free_sort_result(&sort_n2);
	iscc_fs_free_sort_result(&sort_i2);
	iscc_fs_free_sort_result(&sort_n3);
	iscc_fs_free_sort_result(&sort_i3);
	iscc_free_digraph(&nng1);
	iscc_free_digraph(&nng2);
	iscc_free_digraph(&nng3);
}


void scc_ut_fs_decrease_v_in_sort(void** state)
{
	(void) state;

	iscc_Digraph nng;
	iscc_digraph_from_string("...#...###/"
	                         "..###..###/"
	                         "##.#.####./"
	                         ".##.#.####/"
	                         "####.#####/"
	                         ".###..####/"
	                         "..##.#.###/"
	                         "..#..#####/"
	                         "..#...####/"
	                         "..#....##./",
	                         &nng);
	scc_PointIndex ref_sorted_vertices[10] = {0, 4, 1, 5, 3, 6, 2, 9, 7, 8};
	scc_PointIndex ref_inwards_count[10] = {2, 4, 8, 6, 2, 4, 6, 10, 10, 8};
	ptrdiff_t ref_vertex_index[10] = {0, 2, 6, 4, 1, 3, 5, 8, 9, 7};
	ptrdiff_t ref_bucket_index[11] = {0, 0, 0, 2, 2, 4, 4, 6, 6, 8, 8};

	iscc_fs_SortResult sort;
	scc_ErrorCode ec1 = iscc_fs_sort_by_inwards(&nng, true, &sort);
	assert_int_equal(ec1, SCC_ER_OK);
	ptrdiff_t check_vertex_index[10];
	ptrdiff_t check_bucket_index[11];
	for (size_t i = 0; i < 10; ++i) check_vertex_index[i] = sort.vertex_index[i] - sort.sorted_vertices;
	for (size_t i = 0; i < 11; ++i) check_bucket_index[i] = sort.bucket_index[i] - sort.sorted_vertices;
	assert_memory_equal(sort.sorted_vertices, ref_sorted_vertices, 10 * sizeof(scc_PointIndex));
	assert_memory_equal(sort.inwards_count, ref_inwards_count, 10 * sizeof(scc_PointIndex));
	assert_memory_equal(check_vertex_index, ref_vertex_index, 10 * sizeof(ptrdiff_t));
	assert_memory_equal(check_bucket_index, ref_bucket_index, 11 * sizeof(ptrdiff_t));

	iscc_fs_decrease_v_in_sort(8, sort.inwards_count, sort.vertex_index, sort.bucket_index, sort.sorted_vertices);
	scc_PointIndex ref_sorted_vertices2[10] = {0, 4, 1, 5, 3, 6, 2, 9, 8, 7};
	scc_PointIndex ref_inwards_count2[10] = {2, 4, 8, 6, 2, 4, 6, 10, 9, 8};
	ptrdiff_t ref_vertex_index2[10] = {0, 2, 6, 4, 1, 3, 5, 9, 8, 7};
	ptrdiff_t ref_bucket_index2[11] = {0, 0, 0, 2, 2, 4, 4, 6, 6, 8, 9};
	for (size_t i = 0; i < 10; ++i) check_vertex_index[i] = sort.vertex_index[i] - sort.sorted_vertices;
	for (size_t i = 0; i < 11; ++i) check_bucket_index[i] = sort.bucket_index[i] - sort.sorted_vertices;
	assert_memory_equal(sort.sorted_vertices, ref_sorted_vertices2, 10 * sizeof(scc_PointIndex));
	assert_memory_equal(sort.inwards_count, ref_inwards_count2, 10 * sizeof(scc_PointIndex));
	assert_memory_equal(check_vertex_index, ref_vertex_index2, 10 * sizeof(ptrdiff_t));
	assert_memory_equal(check_bucket_index, ref_bucket_index2, 11 * sizeof(ptrdiff_t));

	iscc_fs_decrease_v_in_sort(8, sort.inwards_count, sort.vertex_index, sort.bucket_index, sort.sorted_vertices);
	scc_PointIndex ref_sorted_vertices3[10] = {0, 4, 1, 5, 3, 6, 2, 9, 8, 7};
	scc_PointIndex ref_inwards_count3[10] = {2, 4, 8, 6, 2, 4, 6, 10, 8, 8};
	ptrdiff_t ref_vertex_index3[10] = {0, 2, 6, 4, 1, 3, 5, 9, 8, 7};
	ptrdiff_t ref_bucket_index3[11] = {0, 0, 0, 2, 2, 4, 4, 6, 6, 9, 9};
	for (size_t i = 0; i < 10; ++i) check_vertex_index[i] = sort.vertex_index[i] - sort.sorted_vertices;
	for (size_t i = 0; i < 11; ++i) check_bucket_index[i] = sort.bucket_index[i] - sort.sorted_vertices;
	assert_memory_equal(sort.sorted_vertices, ref_sorted_vertices3, 10 * sizeof(scc_PointIndex));
	assert_memory_equal(sort.inwards_count, ref_inwards_count3, 10 * sizeof(scc_PointIndex));
	assert_memory_equal(check_vertex_index, ref_vertex_index3, 10 * sizeof(ptrdiff_t));
	assert_memory_equal(check_bucket_index, ref_bucket_index3, 11 * sizeof(ptrdiff_t));

	iscc_fs_decrease_v_in_sort(8, sort.inwards_count, sort.vertex_index, sort.bucket_index, sort.sorted_vertices + 6);
	scc_PointIndex ref_sorted_vertices4[10] = {0, 4, 1, 5, 3, 6, 2, 8, 9, 7};
	scc_PointIndex ref_inwards_count4[10] = {2, 4, 8, 6, 2, 4, 6, 10, 7, 8};
	ptrdiff_t ref_vertex_index4[10] = {0, 2, 6, 4, 1, 3, 5, 9, 7, 8};
	ptrdiff_t ref_bucket_index4[11] = {0, 0, 0, 2, 2, 4, 4, 7, 8, 9, 9};
	for (size_t i = 0; i < 10; ++i) check_vertex_index[i] = sort.vertex_index[i] - sort.sorted_vertices;
	for (size_t i = 0; i < 11; ++i) check_bucket_index[i] = sort.bucket_index[i] - sort.sorted_vertices;
	assert_memory_equal(sort.sorted_vertices, ref_sorted_vertices4, 10 * sizeof(scc_PointIndex));
	assert_memory_equal(sort.inwards_count, ref_inwards_count4, 10 * sizeof(scc_PointIndex));
	assert_memory_equal(check_vertex_index, ref_vertex_index4, 10 * sizeof(ptrdiff_t));
	assert_memory_equal(check_bucket_index, ref_bucket_index4, 11 * sizeof(ptrdiff_t));


	iscc_Digraph nng2;
	iscc_digraph_from_string(".......##./"
	                         "..###..###/"
	                         "##.#.#####/"
	                         ".##.#.####/"
	                         "####.#####/"
	                         ".###..####/"
	                         "..##.#.###/"
	                         "..##.##.##/"
	                         "..#...##.#/"
	                         "..#....##./",
	                         &nng2);
	scc_PointIndex ref2_sorted_vertices[10] = {0, 4, 1, 5, 3, 6, 2, 9, 7, 8};
	scc_PointIndex ref2_inwards_count[10] = {2, 4, 8, 6, 2, 4, 6, 9, 9, 8};
	ptrdiff_t ref2_vertex_index[10] = {0, 2, 6, 4, 1, 3, 5, 8, 9, 7};
	ptrdiff_t ref2_bucket_index[10] = {0, 0, 0, 2, 2, 4, 4, 6, 6, 8};

	iscc_fs_SortResult sort2;
	scc_ErrorCode ec2 = iscc_fs_sort_by_inwards(&nng2, true, &sort2);
	assert_int_equal(ec2, SCC_ER_OK);
	ptrdiff_t check2_vertex_index[10];
	ptrdiff_t check2_bucket_index[10];
	for (size_t i = 0; i < 10; ++i) check2_vertex_index[i] = sort2.vertex_index[i] - sort2.sorted_vertices;
	for (size_t i = 0; i < 10; ++i) check2_bucket_index[i] = sort2.bucket_index[i] - sort2.sorted_vertices;
	assert_memory_equal(sort2.sorted_vertices, ref2_sorted_vertices, 10 * sizeof(scc_PointIndex));
	assert_memory_equal(sort2.inwards_count, ref2_inwards_count, 10 * sizeof(scc_PointIndex));
	assert_memory_equal(check2_vertex_index, ref2_vertex_index, 10 * sizeof(ptrdiff_t));
	assert_memory_equal(check2_bucket_index, ref2_bucket_index, 10 * sizeof(ptrdiff_t));

	iscc_fs_decrease_v_in_sort(2, sort2.inwards_count, sort2.vertex_index, sort2.bucket_index, sort2.sorted_vertices + 0);
	iscc_fs_decrease_v_in_sort(3, sort2.inwards_count, sort2.vertex_index, sort2.bucket_index, sort2.sorted_vertices + 0);
	iscc_fs_decrease_v_in_sort(5, sort2.inwards_count, sort2.vertex_index, sort2.bucket_index, sort2.sorted_vertices + 0);
	iscc_fs_decrease_v_in_sort(6, sort2.inwards_count, sort2.vertex_index, sort2.bucket_index, sort2.sorted_vertices + 0);
	iscc_fs_decrease_v_in_sort(9, sort2.inwards_count, sort2.vertex_index, sort2.bucket_index, sort2.sorted_vertices + 0);
	iscc_fs_decrease_v_in_sort(2, sort2.inwards_count, sort2.vertex_index, sort2.bucket_index, sort2.sorted_vertices + 0);
	iscc_fs_decrease_v_in_sort(6, sort2.inwards_count, sort2.vertex_index, sort2.bucket_index, sort2.sorted_vertices + 0);
	iscc_fs_decrease_v_in_sort(9, sort2.inwards_count, sort2.vertex_index, sort2.bucket_index, sort2.sorted_vertices + 0);

	scc_PointIndex ref2_sorted_vertices2[10] = {0, 4, 5, 1, 6, 3, 2, 9, 7, 8};
	scc_PointIndex ref2_inwards_count2[10] = {2, 4, 6, 5, 2, 3, 4, 9, 9, 6};
	ptrdiff_t ref2_vertex_index2[10] = {0, 3, 6, 5, 1, 2, 4, 8, 9, 7};
	ptrdiff_t ref2_bucket_index2[10] = {0, 0, 0, 2, 3, 5, 6, 8, 8, 8};
	for (size_t i = 0; i < 10; ++i) check2_vertex_index[i] = sort2.vertex_index[i] - sort2.sorted_vertices;
	for (size_t i = 0; i < 10; ++i) check2_bucket_index[i] = sort2.bucket_index[i] - sort2.sorted_vertices;
	assert_memory_equal(sort2.sorted_vertices, ref2_sorted_vertices2, 10 * sizeof(scc_PointIndex));
	assert_memory_equal(sort2.inwards_count, ref2_inwards_count2, 10 * sizeof(scc_PointIndex));
	assert_memory_equal(check2_vertex_index, ref2_vertex_index2, 10 * sizeof(ptrdiff_t));
	assert_memory_equal(check2_bucket_index, ref2_bucket_index2, 10 * sizeof(ptrdiff_t));

	iscc_fs_free_sort_result(&sort);
	iscc_fs_free_sort_result(&sort2);
	iscc_free_digraph(&nng);
	iscc_free_digraph(&nng2);
}


int main(void)
{
	if(!scc_ut_init_tests()) return 1;

	const struct CMUnitTest test_cases[] = {
		cmocka_unit_test(scc_ut_findseeds_lexical),
		cmocka_unit_test(scc_ut_findseeds_inwards),
		cmocka_unit_test(scc_ut_findseeds_inwards_alt),
		cmocka_unit_test(scc_ut_findseeds_exclusion),
		cmocka_unit_test(scc_ut_findseeds_lexical_withdiag),
		cmocka_unit_test(scc_ut_findseeds_inwards_withdiag),
		cmocka_unit_test(scc_ut_findseeds_inwards_alt_withdiag),
		cmocka_unit_test(scc_ut_findseeds_exclusion_withdiag),
		cmocka_unit_test(scc_ut_findseeds_lexical_caliper),
		cmocka_unit_test(scc_ut_findseeds_inwards_caliper),
		cmocka_unit_test(scc_ut_findseeds_inwards_alt_caliper),
		cmocka_unit_test(scc_ut_findseeds_exclusion_caliper),
		cmocka_unit_test(scc_ut_findseeds_lexical_caliper_withdiag),
		cmocka_unit_test(scc_ut_findseeds_inwards_caliper_withdiag),
		cmocka_unit_test(scc_ut_findseeds_inwards_alt_caliper_withdiag),
		cmocka_unit_test(scc_ut_findseeds_exclusion_caliper_withdiag),
		cmocka_unit_test(scc_ut_findseeds_lexical_caliper_selfpointing_withdiag),
		cmocka_unit_test(scc_ut_findseeds_inwards_caliper_selfpointing_withdiag),
		cmocka_unit_test(scc_ut_findseeds_inwards_alt_caliper_selfpointing_withdiag),
		cmocka_unit_test(scc_ut_findseeds_exclusion_caliper_selfpointing_withdiag),
		cmocka_unit_test(scc_ut_fs_exclusion_graph),
		cmocka_unit_test(scc_ut_fs_add_seed),
		cmocka_unit_test(scc_ut_fs_check_neighbors_marks),
		cmocka_unit_test(scc_ut_fs_check_neighbors_marks_withdiag),
		cmocka_unit_test(scc_ut_fs_mark_seed_neighbors),
		cmocka_unit_test(scc_ut_fs_mark_seed_neighbors_withdiag),
		cmocka_unit_test(scc_ut_fs_free_sort_result),
		cmocka_unit_test(scc_ut_fs_sort_by_inwards),
		cmocka_unit_test(scc_ut_fs_decrease_v_in_sort),
	};

	return cmocka_run_group_tests_name("internal nng_findseeds.c", test_cases, NULL, NULL);
}
