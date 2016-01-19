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
#include "../include/clustering.h"
#include "../include/nng_clustering.h"


#ifdef SCC_STABLE_CLUSTERING
    #error Please run this test without the SCC_STABLE_CLUSTERING flag
#endif


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
	const struct CMUnitTest test_nng_clustering_withdiag[] = {
		cmocka_unit_test(scc_ut_clustering_lexical_withdiag),
		cmocka_unit_test(scc_ut_clustering_inwards_withdiag),
		cmocka_unit_test(scc_ut_clustering_inwards_updating_withdiag),
		cmocka_unit_test(scc_ut_clustering_exclusion_withdiag),
		cmocka_unit_test(scc_ut_clustering_exclusion_updating_withdiag),
	};

	return cmocka_run_group_tests_name("nng clustering module with diagonal", test_nng_clustering_withdiag, NULL, NULL);
}
