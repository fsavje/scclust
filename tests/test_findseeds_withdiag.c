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

#include <stddef.h>
#include <stdbool.h>

#include "../src/findseeds.h"
#include "../include/config.h"
#include "../include/digraph.h"
#include "../include/digraph_debug.h"


#ifdef SCC_STABLE_CLUSTERING
    #error Please run this test without the SCC_STABLE_CLUSTERING flag
#endif


void scc_ut_findseeds_lexical_withdiag(void** state)
{
	(void) state;

	scc_Digraph nng = scc_digraph_from_string("##......../"
	                                          ".#.#....../"
	                                          "#.#......./"
	                                          ".#.#....../"
	                                          "....##..../"
	                                          "...#.#..../"
	                                          "....#.#.../"
	                                          "......##../"
	                                          "......#.#./"
	                                          "......#..#/");

	scc_Vid fp_seeds[3] = {0, 4, 7};
	scc_SeedClustering clustering = iscc_findseeds_lexical(&nng, 1);
	assert_int_equal(clustering.vertices, 10);
	assert_int_equal(clustering.num_clusters, 3);
	assert_int_equal(clustering.seed_capacity, 3);
	assert_null(clustering.assigned);
	assert_non_null(clustering.seeds);
	assert_memory_equal(clustering.seeds, fp_seeds, 3 * sizeof(scc_Vid));
	assert_null(clustering.cluster_label);

	scc_free_SeedClustering(&clustering);
	scc_free_digraph(&nng);
}


void scc_ut_findseeds_inwards_withdiag(void** state)
{
	(void) state;

	scc_Digraph nng = scc_digraph_from_string("#..#....../"
	                                          ".#.#....../"
	                                          "#.#......./"
	                                          ".#.#....../"
	                                          "....##..../"
	                                          ".#...#..../"
	                                          "....#.#.../"
	                                          "......##../"
	                                          "......#.#./"
	                                          "......#..#/");

	scc_Vid fp_seeds[4] = {2, 7, 4, 1};
	scc_SeedClustering clustering = iscc_findseeds_inwards(&nng, 1, false);
	assert_int_equal(clustering.vertices, 10);
	assert_int_equal(clustering.num_clusters, 4);
	assert_int_equal(clustering.seed_capacity, 4);
	assert_null(clustering.assigned);
	assert_non_null(clustering.seeds);
	assert_memory_equal(clustering.seeds, fp_seeds, 4 * sizeof(scc_Vid));
	assert_null(clustering.cluster_label);

	scc_free_SeedClustering(&clustering);
	scc_free_digraph(&nng);
}


void scc_ut_findseeds_inwards_updating_withdiag(void** state)
{
	(void) state;

	scc_Digraph nng = scc_digraph_from_string("#..#....../"
	                                          ".#.#....../"
	                                          "#.#......./"
	                                          ".#.#....../"
	                                          "....##..../"
	                                          ".#...#..../"
	                                          "....#.#.../"
	                                          "......##../"
	                                          "......#.#./"
	                                          "......#..#/");

	scc_Vid fp_seeds[4] = {2, 7, 4, 3};
	scc_SeedClustering clustering = iscc_findseeds_inwards(&nng, 1, true);
	assert_int_equal(clustering.vertices, 10);
	assert_int_equal(clustering.num_clusters, 4);
	assert_int_equal(clustering.seed_capacity, 4);
	assert_null(clustering.assigned);
	assert_non_null(clustering.seeds);
	assert_memory_equal(clustering.seeds, fp_seeds, 4 * sizeof(scc_Vid));
	assert_null(clustering.cluster_label);

	scc_free_SeedClustering(&clustering);
	scc_free_digraph(&nng);
}


void scc_ut_findseeds_exclusion_withdiag(void** state)
{
	(void) state;

	scc_Digraph nng = scc_digraph_from_string("#..#....../"
	                                          ".#.#....../"
	                                          ".##......./"
	                                          ".#.#....../"
	                                          "....##..../"
	                                          ".#...#..../"
	                                          "....#.#.../"
	                                          "......##../"
	                                          "......#.#./"
	                                          "......#..#/");

	scc_Vid fp_seeds[4] = {0, 4, 2, 7};
	scc_SeedClustering clustering = iscc_findseeds_exclusion(&nng, 1, false);
	assert_int_equal(clustering.vertices, 10);
	assert_int_equal(clustering.num_clusters, 4);
	assert_int_equal(clustering.seed_capacity, 4);
	assert_null(clustering.assigned);
	assert_non_null(clustering.seeds);
	assert_memory_equal(clustering.seeds, fp_seeds, 4 * sizeof(scc_Vid));
	assert_null(clustering.cluster_label);

	scc_free_SeedClustering(&clustering);
	scc_free_digraph(&nng);
}


void scc_ut_findseeds_exclusion_updating_withdiag(void** state)
{
	(void) state;

	scc_Digraph nng = scc_digraph_from_string("#..#....../"
	                                          ".#.#....../"
	                                          ".##......./"
	                                          ".#.#....../"
	                                          "....##..../"
	                                          ".#...#..../"
	                                          "....#.#.../"
	                                          "......##../"
	                                          "......#.#./"
	                                          "......#..#/");

	scc_Vid fp_seeds[4] = {0, 2, 4, 7};
	scc_SeedClustering clustering = iscc_findseeds_exclusion(&nng, 1, true);
	assert_int_equal(clustering.vertices, 10);
	assert_int_equal(clustering.num_clusters, 4);
	assert_int_equal(clustering.seed_capacity, 4);
	assert_null(clustering.assigned);
	assert_non_null(clustering.seeds);
	assert_memory_equal(clustering.seeds, fp_seeds, 4 * sizeof(scc_Vid));
	assert_null(clustering.cluster_label);

	scc_free_SeedClustering(&clustering);
	scc_free_digraph(&nng);
}


void scc_ut_findseeds_checkseeds18_withdiag(void** state)
{
	(void) state;

	scc_Digraph nng = scc_digraph_from_string("##..#............./"
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
	                                          "..............#.##/");

	scc_Vid ref_seeds_lexical[5] = {0, 5, 6, 10, 12};
	scc_SeedClustering clustering_lexical = iscc_findseeds_lexical(&nng, 1);
	assert_int_equal(clustering_lexical.vertices, 18);
	assert_int_equal(clustering_lexical.num_clusters, 5);
	assert_int_equal(clustering_lexical.seed_capacity, 5);
	assert_null(clustering_lexical.assigned);
	assert_non_null(clustering_lexical.seeds);
	assert_memory_equal(clustering_lexical.seeds, ref_seeds_lexical, 5 * sizeof(scc_Vid));
	assert_null(clustering_lexical.cluster_label);

	scc_Vid ref_seeds_inwards[5] = {2, 6, 11, 12, 13};
	scc_SeedClustering clustering_inwards = iscc_findseeds_inwards(&nng, 1, false);
	assert_int_equal(clustering_inwards.vertices, 18);
	assert_int_equal(clustering_inwards.num_clusters, 5);
	assert_int_equal(clustering_inwards.seed_capacity, 5);
	assert_null(clustering_inwards.assigned);
	assert_non_null(clustering_inwards.seeds);
	assert_memory_equal(clustering_inwards.seeds, ref_seeds_inwards, 5 * sizeof(scc_Vid));
	assert_null(clustering_inwards.cluster_label);

	scc_Vid ref_seeds_inupdat[5] = {2, 6, 11, 12, 13};
	scc_SeedClustering clustering_inupdat = iscc_findseeds_inwards(&nng, 1, true);
	assert_int_equal(clustering_inupdat.vertices, 18);
	assert_int_equal(clustering_inupdat.num_clusters, 5);
	assert_int_equal(clustering_inupdat.seed_capacity, 5);
	assert_null(clustering_inupdat.assigned);
	assert_non_null(clustering_inupdat.seeds);
	assert_memory_equal(clustering_inupdat.seeds, ref_seeds_inupdat, 5 * sizeof(scc_Vid));
	assert_null(clustering_inupdat.cluster_label);

	scc_Vid ref_seeds_exclusi[4] = {8, 15, 0, 13};
	scc_SeedClustering clustering_exclusi = iscc_findseeds_exclusion(&nng, 1, false);
	assert_int_equal(clustering_exclusi.vertices, 18);
	assert_int_equal(clustering_exclusi.num_clusters, 4);
	assert_int_equal(clustering_exclusi.seed_capacity, 4);
	assert_null(clustering_exclusi.assigned);
	assert_non_null(clustering_exclusi.seeds);
	assert_memory_equal(clustering_exclusi.seeds, ref_seeds_exclusi, 4 * sizeof(scc_Vid));
	assert_null(clustering_exclusi.cluster_label);

	scc_Vid ref_seeds_exupdat[5] = {8, 15, 17, 10, 4};
	scc_SeedClustering clustering_exupdat = iscc_findseeds_exclusion(&nng, 1, true);
	assert_int_equal(clustering_exupdat.vertices, 18);
	assert_int_equal(clustering_exupdat.num_clusters, 5);
	assert_int_equal(clustering_exupdat.seed_capacity, 5);
	assert_null(clustering_exupdat.assigned);
	assert_non_null(clustering_exupdat.seeds);
	assert_memory_equal(clustering_exupdat.seeds, ref_seeds_exupdat, 5 * sizeof(scc_Vid));
	assert_null(clustering_exupdat.cluster_label);

	scc_free_digraph(&nng);
	scc_free_SeedClustering(&clustering_lexical);
	scc_free_SeedClustering(&clustering_inwards);
	scc_free_SeedClustering(&clustering_inupdat);
	scc_free_SeedClustering(&clustering_exclusi);
	scc_free_SeedClustering(&clustering_exupdat);
}


int main(void)
{
	const struct CMUnitTest test_findseeds_withdiag[] = {
		cmocka_unit_test(scc_ut_findseeds_lexical_withdiag),
		cmocka_unit_test(scc_ut_findseeds_inwards_withdiag),
		cmocka_unit_test(scc_ut_findseeds_inwards_updating_withdiag),
		cmocka_unit_test(scc_ut_findseeds_exclusion_withdiag),
		cmocka_unit_test(scc_ut_findseeds_exclusion_updating_withdiag),
		cmocka_unit_test(scc_ut_findseeds_checkseeds18_withdiag),
	};

	return cmocka_run_group_tests_name("find seeds module with diagonal", test_findseeds_withdiag, NULL, NULL);
}
