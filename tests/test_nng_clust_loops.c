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

#include <stdbool.h>
#include <stddef.h>

#include "../include/config.h"
#include "../include/digraph.h"
#include "../include/digraph_debug.h"
#include "../src/findseeds.h"
#include "../include/clustering.h"
#include "../include/nng_clustering.h"


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


void scc_ut_findseeds_caliper_withdiag(void** state)
{
	(void) state;

	scc_Digraph nng = scc_digraph_from_string("..... ...../"
	                                          ".#... #.#../"
	                                          "..#.. #..../"
	                                          "...#. .##../"
	                                          "..... ...../"

	                                          "..... ...../"
	                                          ".#... .##../"
	                                          "..#.# ..#../"
	                                          "...## ...##/"
	                                          "...#. .#..#/");

	scc_Vid ref_seeds_lexical[2] = {1, 8};
	scc_SeedClustering clustering_lexical = iscc_findseeds_lexical(&nng, 1);
	assert_int_equal(clustering_lexical.vertices, 10);
	assert_int_equal(clustering_lexical.num_clusters, 2);
	assert_int_equal(clustering_lexical.seed_capacity, 2);
	assert_null(clustering_lexical.assigned);
	assert_non_null(clustering_lexical.seeds);
	assert_memory_equal(clustering_lexical.seeds, ref_seeds_lexical, 2 * sizeof(scc_Vid));
	assert_null(clustering_lexical.cluster_label);

	scc_Vid ref_seeds_inwards[2] = {8, 1};
	scc_SeedClustering clustering_inwards = iscc_findseeds_inwards(&nng, 1, false);
	assert_int_equal(clustering_inwards.vertices, 10);
	assert_int_equal(clustering_inwards.num_clusters, 2);
	assert_int_equal(clustering_inwards.seed_capacity, 2);
	assert_null(clustering_inwards.assigned);
	assert_non_null(clustering_inwards.seeds);
	assert_memory_equal(clustering_inwards.seeds, ref_seeds_inwards, 2 * sizeof(scc_Vid));
	assert_null(clustering_inwards.cluster_label);

	scc_Vid ref_seeds_inupdat[3] = {8, 6, 2};
	scc_SeedClustering clustering_inupdat = iscc_findseeds_inwards(&nng, 1, true);
	assert_int_equal(clustering_inupdat.vertices, 10);
	assert_int_equal(clustering_inupdat.num_clusters, 3);
	assert_int_equal(clustering_inupdat.seed_capacity, 3);
	assert_null(clustering_inupdat.assigned);
	assert_non_null(clustering_inupdat.seeds);
	assert_memory_equal(clustering_inupdat.seeds, ref_seeds_inupdat, 3 * sizeof(scc_Vid));
	assert_null(clustering_inupdat.cluster_label);

	scc_Vid ref_seeds_exclusi[3] = {2, 8, 6};
	scc_SeedClustering clustering_exclusi = iscc_findseeds_exclusion(&nng, 1, false);
	assert_int_equal(clustering_exclusi.vertices, 10);
	assert_int_equal(clustering_exclusi.num_clusters, 3);
	assert_int_equal(clustering_exclusi.seed_capacity, 3);
	assert_null(clustering_exclusi.assigned);
	assert_non_null(clustering_exclusi.seeds);
	assert_memory_equal(clustering_exclusi.seeds, ref_seeds_exclusi, 3 * sizeof(scc_Vid));
	assert_null(clustering_exclusi.cluster_label);

	scc_Vid ref_seeds_exupdat[3] = {2, 8, 6};
	scc_SeedClustering clustering_exupdat = iscc_findseeds_exclusion(&nng, 1, true);
	assert_int_equal(clustering_exupdat.vertices, 10);
	assert_int_equal(clustering_exupdat.num_clusters, 3);
	assert_int_equal(clustering_exupdat.seed_capacity, 3);
	assert_null(clustering_exupdat.assigned);
	assert_non_null(clustering_exupdat.seeds);
	assert_memory_equal(clustering_exupdat.seeds, ref_seeds_exupdat, 3 * sizeof(scc_Vid));
	assert_null(clustering_exupdat.cluster_label);

	scc_free_digraph(&nng);
	scc_free_SeedClustering(&clustering_lexical);
	scc_free_SeedClustering(&clustering_inwards);
	scc_free_SeedClustering(&clustering_inupdat);
	scc_free_SeedClustering(&clustering_exclusi);
	scc_free_SeedClustering(&clustering_exupdat);
}


void scc_ut_nng_caliper_ignore_withdiag(void** state)
{
	(void) state;

	scc_Digraph nng = scc_digraph_from_string("..... ...../"
	                                          ".#... #.#../"
	                                          "..#.. #..../"
	                                          "...#. .##../"
	                                          "..... ...../"

	                                          "..... ...../"
	                                          ".#... .##../"
	                                          "..#.# ..#../"
	                                          "...## ...##/"
	                                          "...#. .#..#/");

	scc_Clabel ref_lexical_cluster_label[10] = {SCC_CLABEL_NA, 0, SCC_CLABEL_NA, 1, 1, 0, SCC_CLABEL_NA, 0, 1, 1};
	scc_SeedClustering lexical_tmp_clustering = scc_get_seed_clustering(&nng, SCC_LEXICAL, 1, NULL);
	scc_Clustering lexical_clustering = scc_ignore_remaining(&lexical_tmp_clustering);
	assert_int_equal(lexical_clustering.vertices, 10);
	assert_int_equal(lexical_clustering.num_clusters, 2);
	assert_false(lexical_clustering.external_labels);
	assert_memory_equal(lexical_clustering.cluster_label, ref_lexical_cluster_label, 10 * sizeof(scc_Clabel));

	scc_Clabel ref_inwards_cluster_label[10] = {SCC_CLABEL_NA, 1, SCC_CLABEL_NA, 0, 0, 1, SCC_CLABEL_NA, 1, 0, 0};
	scc_SeedClustering inwards_tmp_clustering = scc_get_seed_clustering(&nng, SCC_INWARDS_ORDER, 1, NULL);
	scc_Clustering inwards_clustering = scc_ignore_remaining(&inwards_tmp_clustering);
	assert_int_equal(inwards_clustering.vertices, 10);
	assert_int_equal(inwards_clustering.num_clusters, 2);
	assert_false(inwards_clustering.external_labels);
	assert_memory_equal(inwards_clustering.cluster_label, ref_inwards_cluster_label, 10 * sizeof(scc_Clabel));

	scc_Clabel ref_inupdat_cluster_label[10] = {SCC_CLABEL_NA, 1, 2, 0, 0, 2, 1, 1, 0, 0};
	scc_SeedClustering inupdat_tmp_clustering = scc_get_seed_clustering(&nng, SCC_INWARDS_UPDATING, 1, NULL);
	scc_Clustering inupdat_clustering = scc_ignore_remaining(&inupdat_tmp_clustering);
	assert_int_equal(inupdat_clustering.vertices, 10);
	assert_int_equal(inupdat_clustering.num_clusters, 3);
	assert_false(inupdat_clustering.external_labels);
	assert_memory_equal(inupdat_clustering.cluster_label, ref_inupdat_cluster_label, 10 * sizeof(scc_Clabel));

	scc_Clabel ref_exclusi_cluster_label[10] = {SCC_CLABEL_NA, 2, 0, 1, 1, 0, 2, 2, 1, 1};
	scc_SeedClustering exclusi_tmp_clustering = scc_get_seed_clustering(&nng, SCC_EXCLUSION_ORDER, 1, NULL);
	scc_Clustering exclusi_clustering = scc_ignore_remaining(&exclusi_tmp_clustering);
	assert_int_equal(exclusi_clustering.vertices, 10);
	assert_int_equal(exclusi_clustering.num_clusters, 3);
	assert_false(exclusi_clustering.external_labels);
	assert_memory_equal(exclusi_clustering.cluster_label, ref_exclusi_cluster_label, 10 * sizeof(scc_Clabel));

	scc_Clabel ref_exupdat_cluster_label[10] = {SCC_CLABEL_NA, 2, 0, 1, 1, 0, 2, 2, 1, 1};
	scc_SeedClustering exupdat_tmp_clustering = scc_get_seed_clustering(&nng, SCC_EXCLUSION_UPDATING, 1, NULL);
	scc_Clustering exupdat_clustering = scc_ignore_remaining(&exupdat_tmp_clustering);
	assert_int_equal(exupdat_clustering.vertices, 10);
	assert_int_equal(exupdat_clustering.num_clusters, 3);
	assert_false(exupdat_clustering.external_labels);
	assert_memory_equal(exupdat_clustering.cluster_label, ref_exupdat_cluster_label, 10 * sizeof(scc_Clabel));

	scc_free_digraph(&nng);
	scc_free_SeedClustering(&lexical_tmp_clustering);
	scc_free_Clustering(&lexical_clustering);
	scc_free_SeedClustering(&inwards_tmp_clustering);
	scc_free_Clustering(&inwards_clustering);
	scc_free_SeedClustering(&inupdat_tmp_clustering);
	scc_free_Clustering(&inupdat_clustering);
	scc_free_SeedClustering(&exclusi_tmp_clustering);
	scc_free_Clustering(&exclusi_clustering);
	scc_free_SeedClustering(&exupdat_tmp_clustering);
	scc_free_Clustering(&exupdat_clustering);
}


void scc_ut_nng_caliper_assign_withdiag(void** state)
{
	(void) state;

	scc_Digraph nng = scc_digraph_from_string("..... ...../"
	                                          ".#... #.#../"
	                                          "..#.. #..../"
	                                          "...#. .##../"
	                                          "..... ...../"

	                                          "..... ...../"
	                                          ".#... .##../"
	                                          "..#.# ..#../"
	                                          "...## ...##/"
	                                          "...#. .#..#/");

	scc_Clabel ref_lexical_cluster_label[10] = {SCC_CLABEL_NA, 0, 0, 1, 1, 0, 0, 0, 1, 1};
	scc_SeedClustering lexical_tmp_clustering = scc_get_seed_clustering(&nng, SCC_LEXICAL, 1, NULL);
	scc_Clustering lexical_clustering = scc_assign_remaining_lexical(&lexical_tmp_clustering, &nng);
	assert_int_equal(lexical_clustering.vertices, 10);
	assert_int_equal(lexical_clustering.num_clusters, 2);
	assert_false(lexical_clustering.external_labels);
	assert_memory_equal(lexical_clustering.cluster_label, ref_lexical_cluster_label, 10 * sizeof(scc_Clabel));

	scc_Clabel ref_inwards_cluster_label[10] = {SCC_CLABEL_NA, 1, 1, 0, 0, 1, 1, 1, 0, 0};
	scc_SeedClustering inwards_tmp_clustering = scc_get_seed_clustering(&nng, SCC_INWARDS_ORDER, 1, NULL);
	scc_Clustering inwards_clustering = scc_assign_remaining_lexical(&inwards_tmp_clustering, &nng);
	assert_int_equal(inwards_clustering.vertices, 10);
	assert_int_equal(inwards_clustering.num_clusters, 2);
	assert_false(inwards_clustering.external_labels);
	assert_memory_equal(inwards_clustering.cluster_label, ref_inwards_cluster_label, 10 * sizeof(scc_Clabel));

	scc_Clabel ref_inupdat_cluster_label[10] = {SCC_CLABEL_NA, 1, 2, 0, 0, 2, 1, 1, 0, 0};
	scc_SeedClustering inupdat_tmp_clustering = scc_get_seed_clustering(&nng, SCC_INWARDS_UPDATING, 1, NULL);
	scc_Clustering inupdat_clustering = scc_assign_remaining_lexical(&inupdat_tmp_clustering, &nng);
	assert_int_equal(inupdat_clustering.vertices, 10);
	assert_int_equal(inupdat_clustering.num_clusters, 3);
	assert_false(inupdat_clustering.external_labels);
	assert_memory_equal(inupdat_clustering.cluster_label, ref_inupdat_cluster_label, 10 * sizeof(scc_Clabel));

	scc_Clabel ref_exclusi_cluster_label[10] = {SCC_CLABEL_NA, 2, 0, 1, 1, 0, 2, 2, 1, 1};
	scc_SeedClustering exclusi_tmp_clustering = scc_get_seed_clustering(&nng, SCC_EXCLUSION_ORDER, 1, NULL);
	scc_Clustering exclusi_clustering = scc_assign_remaining_lexical(&exclusi_tmp_clustering, &nng);
	assert_int_equal(exclusi_clustering.vertices, 10);
	assert_int_equal(exclusi_clustering.num_clusters, 3);
	assert_false(exclusi_clustering.external_labels);
	assert_memory_equal(exclusi_clustering.cluster_label, ref_exclusi_cluster_label, 10 * sizeof(scc_Clabel));

	scc_Clabel ref_exupdat_cluster_label[10] = {SCC_CLABEL_NA, 2, 0, 1, 1, 0, 2, 2, 1, 1};
	scc_SeedClustering exupdat_tmp_clustering = scc_get_seed_clustering(&nng, SCC_EXCLUSION_UPDATING, 1, NULL);
	scc_Clustering exupdat_clustering = scc_assign_remaining_lexical(&exupdat_tmp_clustering, &nng);
	assert_int_equal(exupdat_clustering.vertices, 10);
	assert_int_equal(exupdat_clustering.num_clusters, 3);
	assert_false(exupdat_clustering.external_labels);
	assert_memory_equal(exupdat_clustering.cluster_label, ref_exupdat_cluster_label, 10 * sizeof(scc_Clabel));

	scc_free_digraph(&nng);
	scc_free_SeedClustering(&lexical_tmp_clustering);
	scc_free_Clustering(&lexical_clustering);
	scc_free_SeedClustering(&inwards_tmp_clustering);
	scc_free_Clustering(&inwards_clustering);
	scc_free_SeedClustering(&inupdat_tmp_clustering);
	scc_free_Clustering(&inupdat_clustering);
	scc_free_SeedClustering(&exclusi_tmp_clustering);
	scc_free_Clustering(&exclusi_clustering);
	scc_free_SeedClustering(&exupdat_tmp_clustering);
	scc_free_Clustering(&exupdat_clustering);
}


void scc_ut_findseeds_caliper_selfpointing_withdiag(void** state)
{
	(void) state;

	scc_Digraph nng = scc_digraph_from_string("#.... ...../"
	                                          ".#... #.#../"
	                                          "..#.. #..../"
	                                          "...#. .##../"
	                                          "..... ...../"

	                                          "..... #..../"
	                                          ".#... .##../"
	                                          "..#.# ..#../"
	                                          "...## ...##/"
	                                          "...#. .#..#/");

	scc_Vid ref_seeds_lexical[3] = {0, 1, 8};
	scc_SeedClustering clustering_lexical = iscc_findseeds_lexical(&nng, 1);
	assert_int_equal(clustering_lexical.vertices, 10);
	assert_int_equal(clustering_lexical.num_clusters, 3);
	assert_int_equal(clustering_lexical.seed_capacity, 3);
	assert_null(clustering_lexical.assigned);
	assert_non_null(clustering_lexical.seeds);
	assert_memory_equal(clustering_lexical.seeds, ref_seeds_lexical, 3 * sizeof(scc_Vid));
	assert_null(clustering_lexical.cluster_label);

	scc_Vid ref_seeds_inwards[3] = {0, 8, 1};
	scc_SeedClustering clustering_inwards = iscc_findseeds_inwards(&nng, 1, false);
	assert_int_equal(clustering_inwards.vertices, 10);
	assert_int_equal(clustering_inwards.num_clusters, 3);
	assert_int_equal(clustering_inwards.seed_capacity, 3);
	assert_null(clustering_inwards.assigned);
	assert_non_null(clustering_inwards.seeds);
	assert_memory_equal(clustering_inwards.seeds, ref_seeds_inwards, 3 * sizeof(scc_Vid));
	assert_null(clustering_inwards.cluster_label);

	scc_Vid ref_seeds_inupdat[4] = {0, 8, 6, 2};
	scc_SeedClustering clustering_inupdat = iscc_findseeds_inwards(&nng, 1, true);
	assert_int_equal(clustering_inupdat.vertices, 10);
	assert_int_equal(clustering_inupdat.num_clusters, 4);
	assert_int_equal(clustering_inupdat.seed_capacity, 4);
	assert_null(clustering_inupdat.assigned);
	assert_non_null(clustering_inupdat.seeds);
	assert_memory_equal(clustering_inupdat.seeds, ref_seeds_inupdat, 4 * sizeof(scc_Vid));
	assert_null(clustering_inupdat.cluster_label);

	scc_Vid ref_seeds_exclusi[4] = {0, 5, 8, 6};
	scc_SeedClustering clustering_exclusi = iscc_findseeds_exclusion(&nng, 1, false);
	assert_int_equal(clustering_exclusi.vertices, 10);
	assert_int_equal(clustering_exclusi.num_clusters, 4);
	assert_int_equal(clustering_exclusi.seed_capacity, 4);
	assert_null(clustering_exclusi.assigned);
	assert_non_null(clustering_exclusi.seeds);
	assert_memory_equal(clustering_exclusi.seeds, ref_seeds_exclusi, 4 * sizeof(scc_Vid));
	assert_null(clustering_exclusi.cluster_label);

	scc_Vid ref_seeds_exupdat[4] = {0, 5, 8, 6};
	scc_SeedClustering clustering_exupdat = iscc_findseeds_exclusion(&nng, 1, true);
	assert_int_equal(clustering_exupdat.vertices, 10);
	assert_int_equal(clustering_exupdat.num_clusters, 4);
	assert_int_equal(clustering_exupdat.seed_capacity, 4);
	assert_null(clustering_exupdat.assigned);
	assert_non_null(clustering_exupdat.seeds);
	assert_memory_equal(clustering_exupdat.seeds, ref_seeds_exupdat, 4 * sizeof(scc_Vid));
	assert_null(clustering_exupdat.cluster_label);

	scc_free_digraph(&nng);
	scc_free_SeedClustering(&clustering_lexical);
	scc_free_SeedClustering(&clustering_inwards);
	scc_free_SeedClustering(&clustering_inupdat);
	scc_free_SeedClustering(&clustering_exclusi);
	scc_free_SeedClustering(&clustering_exupdat);
}


void scc_ut_nng_caliper_ignore_selfpointing_withdiag(void** state)
{
	(void) state;

	scc_Digraph nng = scc_digraph_from_string("#.... ...../"
	                                          ".#... #.#../"
	                                          "..#.. #..../"
	                                          "...#. .##../"
	                                          "..... ...../"

	                                          "..... #..../"
	                                          ".#... .##../"
	                                          "..#.# ..#../"
	                                          "...## ...##/"
	                                          "...#. .#..#/");

	scc_Clabel ref_lexical_cluster_label[10] = {0, 1, SCC_CLABEL_NA, 2, 2, 1, SCC_CLABEL_NA, 1, 2, 2};
	scc_SeedClustering lexical_tmp_clustering = scc_get_seed_clustering(&nng, SCC_LEXICAL, 1, NULL);
	scc_Clustering lexical_clustering = scc_ignore_remaining(&lexical_tmp_clustering);
	assert_int_equal(lexical_clustering.vertices, 10);
	assert_int_equal(lexical_clustering.num_clusters, 3);
	assert_false(lexical_clustering.external_labels);
	assert_memory_equal(lexical_clustering.cluster_label, ref_lexical_cluster_label, 10 * sizeof(scc_Clabel));

	scc_Clabel ref_inwards_cluster_label[10] = {0, 2, SCC_CLABEL_NA, 1, 1, 2, SCC_CLABEL_NA, 2, 1, 1};
	scc_SeedClustering inwards_tmp_clustering = scc_get_seed_clustering(&nng, SCC_INWARDS_ORDER, 1, NULL);
	scc_Clustering inwards_clustering = scc_ignore_remaining(&inwards_tmp_clustering);
	assert_int_equal(inwards_clustering.vertices, 10);
	assert_int_equal(inwards_clustering.num_clusters, 3);
	assert_false(inwards_clustering.external_labels);
	assert_memory_equal(inwards_clustering.cluster_label, ref_inwards_cluster_label, 10 * sizeof(scc_Clabel));

	scc_Clabel ref_inupdat_cluster_label[10] = {0, 2, 3, 1, 1, 3, 2, 2, 1, 1};
	scc_SeedClustering inupdat_tmp_clustering = scc_get_seed_clustering(&nng, SCC_INWARDS_UPDATING, 1, NULL);
	scc_Clustering inupdat_clustering = scc_ignore_remaining(&inupdat_tmp_clustering);
	assert_int_equal(inupdat_clustering.vertices, 10);
	assert_int_equal(inupdat_clustering.num_clusters, 4);
	assert_false(inupdat_clustering.external_labels);
	assert_memory_equal(inupdat_clustering.cluster_label, ref_inupdat_cluster_label, 10 * sizeof(scc_Clabel));

	scc_Clabel ref_exclusi_cluster_label[10] = {0, 3, SCC_CLABEL_NA, 2, 2, 1, 3, 3, 2, 2};
	scc_SeedClustering exclusi_tmp_clustering = scc_get_seed_clustering(&nng, SCC_EXCLUSION_ORDER, 1, NULL);
	scc_Clustering exclusi_clustering = scc_ignore_remaining(&exclusi_tmp_clustering);
	assert_int_equal(exclusi_clustering.vertices, 10);
	assert_int_equal(exclusi_clustering.num_clusters, 4);
	assert_false(exclusi_clustering.external_labels);
	assert_memory_equal(exclusi_clustering.cluster_label, ref_exclusi_cluster_label, 10 * sizeof(scc_Clabel));

	scc_Clabel ref_exupdat_cluster_label[10] = {0, 3, SCC_CLABEL_NA, 2, 2, 1, 3, 3, 2, 2};
	scc_SeedClustering exupdat_tmp_clustering = scc_get_seed_clustering(&nng, SCC_EXCLUSION_UPDATING, 1, NULL);
	scc_Clustering exupdat_clustering = scc_ignore_remaining(&exupdat_tmp_clustering);
	assert_int_equal(exupdat_clustering.vertices, 10);
	assert_int_equal(exupdat_clustering.num_clusters, 4);
	assert_false(exupdat_clustering.external_labels);
	assert_memory_equal(exupdat_clustering.cluster_label, ref_exupdat_cluster_label, 10 * sizeof(scc_Clabel));

	scc_free_digraph(&nng);
	scc_free_SeedClustering(&lexical_tmp_clustering);
	scc_free_Clustering(&lexical_clustering);
	scc_free_SeedClustering(&inwards_tmp_clustering);
	scc_free_Clustering(&inwards_clustering);
	scc_free_SeedClustering(&inupdat_tmp_clustering);
	scc_free_Clustering(&inupdat_clustering);
	scc_free_SeedClustering(&exclusi_tmp_clustering);
	scc_free_Clustering(&exclusi_clustering);
	scc_free_SeedClustering(&exupdat_tmp_clustering);
	scc_free_Clustering(&exupdat_clustering);
}


void scc_ut_nng_caliper_assign_selfpointing_withdiag(void** state)
{
	(void) state;

	scc_Digraph nng = scc_digraph_from_string("#.... ...../"
	                                          ".#... #.#../"
	                                          "..#.. #..../"
	                                          "...#. .##../"
	                                          "..... ...../"

	                                          "..... #..../"
	                                          ".#... .##../"
	                                          "..#.# ..#../"
	                                          "...## ...##/"
	                                          "...#. .#..#/");

	scc_Clabel ref_lexical_cluster_label[10] = {0, 1, 1, 2, 2, 1, 1, 1, 2, 2};
	scc_SeedClustering lexical_tmp_clustering = scc_get_seed_clustering(&nng, SCC_LEXICAL, 1, NULL);
	scc_Clustering lexical_clustering = scc_assign_remaining_lexical(&lexical_tmp_clustering, &nng);
	assert_int_equal(lexical_clustering.vertices, 10);
	assert_int_equal(lexical_clustering.num_clusters, 3);
	assert_false(lexical_clustering.external_labels);
	assert_memory_equal(lexical_clustering.cluster_label, ref_lexical_cluster_label, 10 * sizeof(scc_Clabel));

	scc_Clabel ref_inwards_cluster_label[10] = {0, 2, 2, 1, 1, 2, 2, 2, 1, 1};
	scc_SeedClustering inwards_tmp_clustering = scc_get_seed_clustering(&nng, SCC_INWARDS_ORDER, 1, NULL);
	scc_Clustering inwards_clustering = scc_assign_remaining_lexical(&inwards_tmp_clustering, &nng);
	assert_int_equal(inwards_clustering.vertices, 10);
	assert_int_equal(inwards_clustering.num_clusters, 3);
	assert_false(inwards_clustering.external_labels);
	assert_memory_equal(inwards_clustering.cluster_label, ref_inwards_cluster_label, 10 * sizeof(scc_Clabel));

	scc_Clabel ref_inupdat_cluster_label[10] = {0, 2, 3, 1, 1, 3, 2, 2, 1, 1};
	scc_SeedClustering inupdat_tmp_clustering = scc_get_seed_clustering(&nng, SCC_INWARDS_UPDATING, 1, NULL);
	scc_Clustering inupdat_clustering = scc_assign_remaining_lexical(&inupdat_tmp_clustering, &nng);
	assert_int_equal(inupdat_clustering.vertices, 10);
	assert_int_equal(inupdat_clustering.num_clusters, 4);
	assert_false(inupdat_clustering.external_labels);
	assert_memory_equal(inupdat_clustering.cluster_label, ref_inupdat_cluster_label, 10 * sizeof(scc_Clabel));

	scc_Clabel ref_exclusi_cluster_label[10] = {0, 3, 1, 2, 2, 1, 3, 3, 2, 2};
	scc_SeedClustering exclusi_tmp_clustering = scc_get_seed_clustering(&nng, SCC_EXCLUSION_ORDER, 1, NULL);
	scc_Clustering exclusi_clustering = scc_assign_remaining_lexical(&exclusi_tmp_clustering, &nng);
	assert_int_equal(exclusi_clustering.vertices, 10);
	assert_int_equal(exclusi_clustering.num_clusters, 4);
	assert_false(exclusi_clustering.external_labels);
	assert_memory_equal(exclusi_clustering.cluster_label, ref_exclusi_cluster_label, 10 * sizeof(scc_Clabel));

	scc_Clabel ref_exupdat_cluster_label[10] = {0, 3, 1, 2, 2, 1, 3, 3, 2, 2};
	scc_SeedClustering exupdat_tmp_clustering = scc_get_seed_clustering(&nng, SCC_EXCLUSION_UPDATING, 1, NULL);
	scc_Clustering exupdat_clustering = scc_assign_remaining_lexical(&exupdat_tmp_clustering, &nng);
	assert_int_equal(exupdat_clustering.vertices, 10);
	assert_int_equal(exupdat_clustering.num_clusters, 4);
	assert_false(exupdat_clustering.external_labels);
	assert_memory_equal(exupdat_clustering.cluster_label, ref_exupdat_cluster_label, 10 * sizeof(scc_Clabel));

	scc_free_digraph(&nng);
	scc_free_SeedClustering(&lexical_tmp_clustering);
	scc_free_Clustering(&lexical_clustering);
	scc_free_SeedClustering(&inwards_tmp_clustering);
	scc_free_Clustering(&inwards_clustering);
	scc_free_SeedClustering(&inupdat_tmp_clustering);
	scc_free_Clustering(&inupdat_clustering);
	scc_free_SeedClustering(&exclusi_tmp_clustering);
	scc_free_Clustering(&exclusi_clustering);
	scc_free_SeedClustering(&exupdat_tmp_clustering);
	scc_free_Clustering(&exupdat_clustering);
}


void scc_ut_clustering_lexical_withdiag(void** state)
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

	bool fp_assigned[10] = {true, true, false, false, true, true, true, true, false, false};
	scc_Vid fp_seeds[3] = {0, 4, 7};
	scc_Clabel fp_cluster_label[10] = {0, 0, SCC_CLABEL_NA, SCC_CLABEL_NA, 1, 1, 2, 2, SCC_CLABEL_NA, SCC_CLABEL_NA};

	scc_SeedClustering seed_clustering = scc_get_seed_clustering(&nng, SCC_LEXICAL, 6, NULL);
	assert_int_equal(seed_clustering.vertices, 10);
	assert_int_equal(seed_clustering.num_clusters, 3);
	assert_int_equal(seed_clustering.seed_capacity, 3);
	assert_false(seed_clustering.external_labels);
	assert_memory_equal(seed_clustering.assigned, fp_assigned, 10 * sizeof(bool));
	assert_memory_equal(seed_clustering.seeds, fp_seeds, 3 * sizeof(scc_Vid));
	assert_int_equal(seed_clustering.cluster_label[0], fp_cluster_label[0]);
	assert_int_equal(seed_clustering.cluster_label[1], fp_cluster_label[1]);
	assert_int_equal(seed_clustering.cluster_label[4], fp_cluster_label[4]);
	assert_int_equal(seed_clustering.cluster_label[5], fp_cluster_label[5]);
	assert_int_equal(seed_clustering.cluster_label[6], fp_cluster_label[6]);
	assert_int_equal(seed_clustering.cluster_label[7], fp_cluster_label[7]);

	scc_Clabel external_cluster_label[10];
	scc_SeedClustering seed_clustering2 = scc_get_seed_clustering(&nng, SCC_LEXICAL, 6, external_cluster_label);
	assert_int_equal(seed_clustering2.vertices, 10);
	assert_int_equal(seed_clustering2.num_clusters, 3);
	assert_int_equal(seed_clustering2.seed_capacity, 3);
	assert_true(seed_clustering2.external_labels);
	assert_memory_equal(seed_clustering2.assigned, fp_assigned, 10 * sizeof(bool));
	assert_memory_equal(seed_clustering2.seeds, fp_seeds, 3 * sizeof(scc_Vid));
	assert_ptr_equal(external_cluster_label, seed_clustering2.cluster_label);
	assert_int_equal(external_cluster_label[0], fp_cluster_label[0]);
	assert_int_equal(external_cluster_label[1], fp_cluster_label[1]);
	assert_int_equal(external_cluster_label[4], fp_cluster_label[4]);
	assert_int_equal(external_cluster_label[5], fp_cluster_label[5]);
	assert_int_equal(external_cluster_label[6], fp_cluster_label[6]);
	assert_int_equal(external_cluster_label[7], fp_cluster_label[7]);

	scc_free_SeedClustering(&seed_clustering);
	scc_free_SeedClustering(&seed_clustering2);

	scc_free_digraph(&nng);
}


void scc_ut_clustering_inwards_withdiag(void** state)
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

	bool fp_assigned[10] = {true, true, true, true, true, true, true, true, false, false};
	scc_Vid fp_seeds[4] = {2, 7, 4, 1};
	scc_Clabel fp_cluster_label[10] = {0, 3, 0, 3, 2, 2, 1, 1, SCC_CLABEL_NA, SCC_CLABEL_NA};

	scc_SeedClustering seed_clustering = scc_get_seed_clustering(&nng, SCC_INWARDS_ORDER, 6, NULL);
	assert_int_equal(seed_clustering.vertices, 10);
	assert_int_equal(seed_clustering.num_clusters, 4);
	assert_int_equal(seed_clustering.seed_capacity, 4);
	assert_false(seed_clustering.external_labels);
	assert_memory_equal(seed_clustering.assigned, fp_assigned, 10 * sizeof(bool));
	assert_memory_equal(seed_clustering.seeds, fp_seeds, 4 * sizeof(scc_Vid));
	assert_memory_equal(seed_clustering.cluster_label, fp_cluster_label, 8 * sizeof(scc_Clabel));

	scc_Clabel external_cluster_label[10];
	scc_SeedClustering seed_clustering2 = scc_get_seed_clustering(&nng, SCC_INWARDS_ORDER, 6, external_cluster_label);
	assert_int_equal(seed_clustering2.vertices, 10);
	assert_int_equal(seed_clustering2.num_clusters, 4);
	assert_int_equal(seed_clustering2.seed_capacity, 4);
	assert_true(seed_clustering2.external_labels);
	assert_memory_equal(seed_clustering2.assigned, fp_assigned, 10 * sizeof(bool));
	assert_memory_equal(seed_clustering2.seeds, fp_seeds, 4 * sizeof(scc_Vid));
	assert_ptr_equal(external_cluster_label, seed_clustering2.cluster_label);
	assert_memory_equal(external_cluster_label, fp_cluster_label, 8 * sizeof(scc_Clabel));

	scc_free_SeedClustering(&seed_clustering);
	scc_free_SeedClustering(&seed_clustering2);

	scc_free_digraph(&nng);
}


void scc_ut_clustering_inwards_updating_withdiag(void** state)
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

	bool fp_assigned[10] = {true, true, true, true, true, true, true, true, false, false};
	scc_Vid fp_seeds[4] = {2, 7, 4, 3};
	scc_Clabel fp_cluster_label[10] = {0, 3, 0, 3, 2, 2, 1, 1, SCC_CLABEL_NA, SCC_CLABEL_NA};

	scc_SeedClustering seed_clustering = scc_get_seed_clustering(&nng, SCC_INWARDS_UPDATING, 6, NULL);
	assert_int_equal(seed_clustering.vertices, 10);
	assert_int_equal(seed_clustering.num_clusters, 4);
	assert_int_equal(seed_clustering.seed_capacity, 4);
	assert_false(seed_clustering.external_labels);
	assert_memory_equal(seed_clustering.assigned, fp_assigned, 10 * sizeof(bool));
	assert_memory_equal(seed_clustering.seeds, fp_seeds, 4 * sizeof(scc_Vid));
	assert_memory_equal(seed_clustering.cluster_label, fp_cluster_label, 8 * sizeof(scc_Clabel));

	scc_Clabel external_cluster_label[10];
	scc_SeedClustering seed_clustering2 = scc_get_seed_clustering(&nng, SCC_INWARDS_UPDATING, 6, external_cluster_label);
	assert_int_equal(seed_clustering2.vertices, 10);
	assert_int_equal(seed_clustering2.num_clusters, 4);
	assert_int_equal(seed_clustering2.seed_capacity, 4);
	assert_true(seed_clustering2.external_labels);
	assert_memory_equal(seed_clustering2.assigned, fp_assigned, 10 * sizeof(bool));
	assert_memory_equal(seed_clustering2.seeds, fp_seeds, 4 * sizeof(scc_Vid));
	assert_ptr_equal(external_cluster_label, seed_clustering2.cluster_label);
	assert_memory_equal(external_cluster_label, fp_cluster_label, 8 * sizeof(scc_Clabel));

	scc_free_SeedClustering(&seed_clustering);
	scc_free_SeedClustering(&seed_clustering2);

	scc_free_digraph(&nng);
}


void scc_ut_clustering_exclusion_withdiag(void** state)
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

	bool fp_assigned[10] = {true, true, true, true, true, true, true, true, false, false};
	scc_Vid fp_seeds[4] = {0, 4, 2, 7};
	scc_Clabel fp_cluster_label[10] = {0, 2, 2, 0, 1, 1, 3, 3, SCC_CLABEL_NA, SCC_CLABEL_NA};

	scc_SeedClustering seed_clustering = scc_get_seed_clustering(&nng, SCC_EXCLUSION_ORDER, 6, NULL);
	assert_int_equal(seed_clustering.vertices, 10);
	assert_int_equal(seed_clustering.num_clusters, 4);
	assert_int_equal(seed_clustering.seed_capacity, 4);
	assert_false(seed_clustering.external_labels);
	assert_memory_equal(seed_clustering.assigned, fp_assigned, 10 * sizeof(bool));
	assert_memory_equal(seed_clustering.seeds, fp_seeds, 4 * sizeof(scc_Vid));
	assert_memory_equal(seed_clustering.cluster_label, fp_cluster_label, 8 * sizeof(scc_Clabel));

	scc_Clabel external_cluster_label[10];
	scc_SeedClustering seed_clustering2 = scc_get_seed_clustering(&nng, SCC_EXCLUSION_ORDER, 6, external_cluster_label);
	assert_int_equal(seed_clustering2.vertices, 10);
	assert_int_equal(seed_clustering2.num_clusters, 4);
	assert_int_equal(seed_clustering2.seed_capacity, 4);
	assert_true(seed_clustering2.external_labels);
	assert_memory_equal(seed_clustering2.assigned, fp_assigned, 10 * sizeof(bool));
	assert_memory_equal(seed_clustering2.seeds, fp_seeds, 4 * sizeof(scc_Vid));
	assert_ptr_equal(external_cluster_label, seed_clustering2.cluster_label);
	assert_memory_equal(external_cluster_label, fp_cluster_label, 8 * sizeof(scc_Clabel));

	scc_free_SeedClustering(&seed_clustering);
	scc_free_SeedClustering(&seed_clustering2);

	scc_free_digraph(&nng);
}


void scc_ut_clustering_exclusion_updating_withdiag(void** state)
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

	bool fp_assigned[10] = {true, true, true, true, true, true, true, true, false, false};
	scc_Vid fp_seeds[4] = {0, 2, 4, 7};
	scc_Clabel fp_cluster_label[10] = {0, 1, 1, 0, 2, 2, 3, 3, SCC_CLABEL_NA, SCC_CLABEL_NA};

	scc_SeedClustering seed_clustering = scc_get_seed_clustering(&nng, SCC_EXCLUSION_UPDATING, 6, NULL);
	assert_int_equal(seed_clustering.vertices, 10);
	assert_int_equal(seed_clustering.num_clusters, 4);
	assert_int_equal(seed_clustering.seed_capacity, 4);
	assert_false(seed_clustering.external_labels);
	assert_memory_equal(seed_clustering.assigned, fp_assigned, 10 * sizeof(bool));
	assert_memory_equal(seed_clustering.seeds, fp_seeds, 4 * sizeof(scc_Vid));
	assert_memory_equal(seed_clustering.cluster_label, fp_cluster_label, 8 * sizeof(scc_Clabel));

	scc_Clabel external_cluster_label[10];
	scc_SeedClustering seed_clustering2 = scc_get_seed_clustering(&nng, SCC_EXCLUSION_UPDATING, 6, external_cluster_label);
	assert_int_equal(seed_clustering2.vertices, 10);
	assert_int_equal(seed_clustering2.num_clusters, 4);
	assert_int_equal(seed_clustering2.seed_capacity, 4);
	assert_true(seed_clustering2.external_labels);
	assert_memory_equal(seed_clustering2.assigned, fp_assigned, 10 * sizeof(bool));
	assert_memory_equal(seed_clustering2.seeds, fp_seeds, 4 * sizeof(scc_Vid));
	assert_ptr_equal(external_cluster_label, seed_clustering2.cluster_label);
	assert_memory_equal(external_cluster_label, fp_cluster_label, 8 * sizeof(scc_Clabel));

	scc_free_SeedClustering(&seed_clustering);
	scc_free_SeedClustering(&seed_clustering2);

	scc_free_digraph(&nng);
}


int main(void)
{
	const struct CMUnitTest test_withdiag[] = {
		cmocka_unit_test(scc_ut_findseeds_lexical_withdiag),
		cmocka_unit_test(scc_ut_findseeds_inwards_withdiag),
		cmocka_unit_test(scc_ut_findseeds_inwards_updating_withdiag),
		cmocka_unit_test(scc_ut_findseeds_exclusion_withdiag),
		cmocka_unit_test(scc_ut_findseeds_exclusion_updating_withdiag),
		cmocka_unit_test(scc_ut_findseeds_checkseeds18_withdiag),
		cmocka_unit_test(scc_ut_findseeds_caliper_withdiag),
		cmocka_unit_test(scc_ut_nng_caliper_ignore_withdiag),
		cmocka_unit_test(scc_ut_nng_caliper_assign_withdiag),
		cmocka_unit_test(scc_ut_findseeds_caliper_selfpointing_withdiag),
		cmocka_unit_test(scc_ut_nng_caliper_ignore_selfpointing_withdiag),
		cmocka_unit_test(scc_ut_nng_caliper_assign_selfpointing_withdiag),
		cmocka_unit_test(scc_ut_clustering_lexical_withdiag),
		cmocka_unit_test(scc_ut_clustering_inwards_withdiag),
		cmocka_unit_test(scc_ut_clustering_inwards_updating_withdiag),
		cmocka_unit_test(scc_ut_clustering_exclusion_withdiag),
		cmocka_unit_test(scc_ut_clustering_exclusion_updating_withdiag),
	};

	return cmocka_run_group_tests_name("NNG clustering with loops module", test_withdiag, NULL, NULL);
}
