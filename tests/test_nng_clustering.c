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
#include <stddef.h>

#include "../include/config.h"
#include "../include/digraph.h"
#include "../include/digraph_debug.h"
#include "../include/clustering.h"
#include "../include/nng_clustering.h"


#ifdef SCC_STABLE_CLUSTERING
    #error Please run this test without the SCC_STABLE_CLUSTERING flag
#endif


void scc_ut_nng_free_TempSeedClustering(void** state)
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
	
	scc_TempSeedClustering seed_clustering = scc_get_seed_clustering(&nng, SCC_LEXICAL, 6, NULL);
	assert_int_equal(seed_clustering.vertices, 10);
	assert_int_equal(seed_clustering.num_clusters, 3);
	assert_int_equal(seed_clustering.seed_capacity, 3);
	assert_false(seed_clustering.external_labels);
	assert_non_null(seed_clustering.assigned);
	assert_non_null(seed_clustering.seeds);
	assert_non_null(seed_clustering.cluster_label);

	scc_free_TempSeedClustering(&seed_clustering);

	assert_int_equal(seed_clustering.vertices, 0);
	assert_int_equal(seed_clustering.num_clusters, 0);
	assert_int_equal(seed_clustering.seed_capacity, 0);
	assert_false(seed_clustering.external_labels);
	assert_null(seed_clustering.assigned);
	assert_null(seed_clustering.seeds);
	assert_null(seed_clustering.cluster_label);

	scc_Clabel ext_cluster_label[10];

	scc_TempSeedClustering seed_clustering2 = scc_get_seed_clustering(&nng, SCC_LEXICAL, 6, ext_cluster_label);
	assert_int_equal(seed_clustering2.vertices, 10);
	assert_int_equal(seed_clustering2.num_clusters, 3);
	assert_int_equal(seed_clustering2.seed_capacity, 3);
	assert_true(seed_clustering2.external_labels);
	assert_non_null(seed_clustering2.assigned);
	assert_non_null(seed_clustering2.seeds);
	assert_non_null(seed_clustering2.cluster_label);

	scc_free_TempSeedClustering(&seed_clustering2);

	assert_int_equal(seed_clustering2.vertices, 0);
	assert_int_equal(seed_clustering2.num_clusters, 0);
	assert_int_equal(seed_clustering2.seed_capacity, 0);
	assert_false(seed_clustering2.external_labels);
	assert_null(seed_clustering2.assigned);
	assert_null(seed_clustering2.seeds);
	assert_null(seed_clustering2.cluster_label);

	scc_free_digraph(&nng);
}


void scc_ut_nng_copy_TempSeedClustering(void** state)
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
	
	scc_TempSeedClustering seed_clustering = scc_get_seed_clustering(&nng, SCC_LEXICAL, 6, NULL);

	scc_TempSeedClustering copy = scc_copy_TempSeedClustering(&seed_clustering);

	assert_int_equal(seed_clustering.vertices, copy.vertices);
	assert_int_equal(seed_clustering.num_clusters, copy.num_clusters);
	assert_int_equal(seed_clustering.seed_capacity, copy.seed_capacity);
	assert_false(copy.external_labels);
	assert_non_null(copy.assigned);
	assert_non_null(copy.seeds);
	assert_non_null(copy.cluster_label);
	assert_memory_equal(seed_clustering.assigned, copy.assigned, seed_clustering.vertices * sizeof(bool));
	assert_memory_equal(seed_clustering.seeds, copy.seeds, seed_clustering.seed_capacity * sizeof(scc_Vid));
	assert_memory_equal(seed_clustering.cluster_label, copy.cluster_label, seed_clustering.vertices * sizeof(scc_Clabel));

	seed_clustering.external_labels = true;
	scc_TempSeedClustering copy2 = scc_copy_TempSeedClustering(&seed_clustering);
	seed_clustering.external_labels = false;

	assert_int_equal(seed_clustering.vertices, copy2.vertices);
	assert_int_equal(seed_clustering.num_clusters, copy2.num_clusters);
	assert_int_equal(seed_clustering.seed_capacity, copy2.seed_capacity);
	assert_true(copy2.external_labels);
	assert_non_null(copy2.assigned);
	assert_non_null(copy2.seeds);
	assert_non_null(copy2.cluster_label);
	assert_memory_equal(seed_clustering.assigned, copy2.assigned, seed_clustering.vertices * sizeof(bool));
	assert_memory_equal(seed_clustering.seeds, copy2.seeds, seed_clustering.seed_capacity * sizeof(scc_Vid));
	assert_memory_equal(seed_clustering.cluster_label, copy2.cluster_label, seed_clustering.vertices * sizeof(scc_Clabel));
	assert_ptr_equal(seed_clustering.cluster_label, copy2.cluster_label);

	scc_free_digraph(&nng);
	scc_free_TempSeedClustering(&copy);
	scc_free_TempSeedClustering(&copy2);
	scc_free_TempSeedClustering(&seed_clustering);
}


void scc_ut_clustering_lexical(void** state)
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

	bool fp_assigned[10] = {true, true, false, false, true, true, true, true, false, false};
	scc_Vid fp_seeds[3] = {0, 4, 7};
	scc_Clabel fp_cluster_label[10] = {0, 0, SCC_CLABEL_NA, SCC_CLABEL_NA, 1, 1, 2, 2, SCC_CLABEL_NA, SCC_CLABEL_NA};

	scc_TempSeedClustering seed_clustering = scc_get_seed_clustering(&nng, SCC_LEXICAL, 6, NULL);
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
	scc_TempSeedClustering seed_clustering2 = scc_get_seed_clustering(&nng, SCC_LEXICAL, 6, external_cluster_label);
	assert_int_equal(seed_clustering2.vertices, 10);
	assert_int_equal(seed_clustering2.num_clusters, 3);
	assert_int_equal(seed_clustering2.seed_capacity, 3);
	assert_true(seed_clustering2.external_labels);
	assert_memory_equal(seed_clustering2.assigned, fp_assigned, 10 * sizeof(bool));
	assert_memory_equal(seed_clustering2.seeds, fp_seeds, 3 * sizeof(scc_Vid));
	assert_int_equal(external_cluster_label[0], fp_cluster_label[0]);
	assert_int_equal(external_cluster_label[1], fp_cluster_label[1]);
	assert_int_equal(external_cluster_label[4], fp_cluster_label[4]);
	assert_int_equal(external_cluster_label[5], fp_cluster_label[5]);
	assert_int_equal(external_cluster_label[6], fp_cluster_label[6]);
	assert_int_equal(external_cluster_label[7], fp_cluster_label[7]);

	scc_free_TempSeedClustering(&seed_clustering);
	scc_free_TempSeedClustering(&seed_clustering2);

	scc_free_digraph(&nng);
}


void scc_ut_clustering_inwards(void** state)
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

	bool fp_assigned[10] = {true, true, true, true, true, true, true, true, false, false};
	scc_Vid fp_seeds[4] = {2, 7, 4, 1};
	scc_Clabel fp_cluster_label[10] = {0, 3, 0, 3, 2, 2, 1, 1, SCC_CLABEL_NA, SCC_CLABEL_NA};

	scc_TempSeedClustering seed_clustering = scc_get_seed_clustering(&nng, SCC_INWARDS_ORDER, 6, NULL);
	assert_int_equal(seed_clustering.vertices, 10);
	assert_int_equal(seed_clustering.num_clusters, 4);
	assert_int_equal(seed_clustering.seed_capacity, 4);
	assert_false(seed_clustering.external_labels);
	assert_memory_equal(seed_clustering.assigned, fp_assigned, 10 * sizeof(bool));
	assert_memory_equal(seed_clustering.seeds, fp_seeds, 4 * sizeof(scc_Vid));
	assert_memory_equal(seed_clustering.cluster_label, fp_cluster_label, 8 * sizeof(scc_Clabel));

	scc_Clabel external_cluster_label[10];
	scc_TempSeedClustering seed_clustering2 = scc_get_seed_clustering(&nng, SCC_INWARDS_ORDER, 6, external_cluster_label);
	assert_int_equal(seed_clustering2.vertices, 10);
	assert_int_equal(seed_clustering2.num_clusters, 4);
	assert_int_equal(seed_clustering2.seed_capacity, 4);
	assert_true(seed_clustering2.external_labels);
	assert_memory_equal(seed_clustering2.assigned, fp_assigned, 10 * sizeof(bool));
	assert_memory_equal(seed_clustering2.seeds, fp_seeds, 4 * sizeof(scc_Vid));
	assert_memory_equal(external_cluster_label, fp_cluster_label, 8 * sizeof(scc_Clabel));

	scc_free_TempSeedClustering(&seed_clustering);
	scc_free_TempSeedClustering(&seed_clustering2);

	scc_free_digraph(&nng);
}


void scc_ut_clustering_inwards_updating(void** state)
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

	bool fp_assigned[10] = {true, true, true, true, true, true, true, true, false, false};
	scc_Vid fp_seeds[4] = {2, 7, 4, 3};
	scc_Clabel fp_cluster_label[10] = {0, 3, 0, 3, 2, 2, 1, 1, SCC_CLABEL_NA, SCC_CLABEL_NA};

	scc_TempSeedClustering seed_clustering = scc_get_seed_clustering(&nng, SCC_INWARDS_UPDATING, 6, NULL);
	assert_int_equal(seed_clustering.vertices, 10);
	assert_int_equal(seed_clustering.num_clusters, 4);
	assert_int_equal(seed_clustering.seed_capacity, 4);
	assert_false(seed_clustering.external_labels);
	assert_memory_equal(seed_clustering.assigned, fp_assigned, 10 * sizeof(bool));
	assert_memory_equal(seed_clustering.seeds, fp_seeds, 4 * sizeof(scc_Vid));
	assert_memory_equal(seed_clustering.cluster_label, fp_cluster_label, 8 * sizeof(scc_Clabel));

	scc_Clabel external_cluster_label[10];
	scc_TempSeedClustering seed_clustering2 = scc_get_seed_clustering(&nng, SCC_INWARDS_UPDATING, 6, external_cluster_label);
	assert_int_equal(seed_clustering2.vertices, 10);
	assert_int_equal(seed_clustering2.num_clusters, 4);
	assert_int_equal(seed_clustering2.seed_capacity, 4);
	assert_true(seed_clustering2.external_labels);
	assert_memory_equal(seed_clustering2.assigned, fp_assigned, 10 * sizeof(bool));
	assert_memory_equal(seed_clustering2.seeds, fp_seeds, 4 * sizeof(scc_Vid));
	assert_memory_equal(external_cluster_label, fp_cluster_label, 8 * sizeof(scc_Clabel));

	scc_free_TempSeedClustering(&seed_clustering);
	scc_free_TempSeedClustering(&seed_clustering2);

	scc_free_digraph(&nng);
}


void scc_ut_clustering_exclusion(void** state)
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

	bool fp_assigned[10] = {true, true, true, true, true, true, true, true, false, false};
	scc_Vid fp_seeds[4] = {0, 4, 2, 7};
	scc_Clabel fp_cluster_label[10] = {0, 2, 2, 0, 1, 1, 3, 3, SCC_CLABEL_NA, SCC_CLABEL_NA};

	scc_TempSeedClustering seed_clustering = scc_get_seed_clustering(&nng, SCC_EXCLUSION_ORDER, 6, NULL);
	assert_int_equal(seed_clustering.vertices, 10);
	assert_int_equal(seed_clustering.num_clusters, 4);
	assert_int_equal(seed_clustering.seed_capacity, 4);
	assert_false(seed_clustering.external_labels);
	assert_memory_equal(seed_clustering.assigned, fp_assigned, 10 * sizeof(bool));
	assert_memory_equal(seed_clustering.seeds, fp_seeds, 4 * sizeof(scc_Vid));
	assert_memory_equal(seed_clustering.cluster_label, fp_cluster_label, 8 * sizeof(scc_Clabel));

	scc_Clabel external_cluster_label[10];
	scc_TempSeedClustering seed_clustering2 = scc_get_seed_clustering(&nng, SCC_EXCLUSION_ORDER, 6, external_cluster_label);
	assert_int_equal(seed_clustering2.vertices, 10);
	assert_int_equal(seed_clustering2.num_clusters, 4);
	assert_int_equal(seed_clustering2.seed_capacity, 4);
	assert_true(seed_clustering2.external_labels);
	assert_memory_equal(seed_clustering2.assigned, fp_assigned, 10 * sizeof(bool));
	assert_memory_equal(seed_clustering2.seeds, fp_seeds, 4 * sizeof(scc_Vid));
	assert_memory_equal(external_cluster_label, fp_cluster_label, 8 * sizeof(scc_Clabel));

	scc_free_TempSeedClustering(&seed_clustering);
	scc_free_TempSeedClustering(&seed_clustering2);

	scc_free_digraph(&nng);
}


void scc_ut_clustering_exclusion_updating(void** state)
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

	bool fp_assigned[10] = {true, true, true, true, true, true, true, true, false, false};
	scc_Vid fp_seeds[4] = {0, 2, 4, 7};
	scc_Clabel fp_cluster_label[10] = {0, 1, 1, 0, 2, 2, 3, 3, SCC_CLABEL_NA, SCC_CLABEL_NA};

	scc_TempSeedClustering seed_clustering = scc_get_seed_clustering(&nng, SCC_EXCLUSION_UPDATING, 6, NULL);
	assert_int_equal(seed_clustering.vertices, 10);
	assert_int_equal(seed_clustering.num_clusters, 4);
	assert_int_equal(seed_clustering.seed_capacity, 4);
	assert_false(seed_clustering.external_labels);
	assert_memory_equal(seed_clustering.assigned, fp_assigned, 10 * sizeof(bool));
	assert_memory_equal(seed_clustering.seeds, fp_seeds, 4 * sizeof(scc_Vid));
	assert_memory_equal(seed_clustering.cluster_label, fp_cluster_label, 8 * sizeof(scc_Clabel));

	scc_Clabel external_cluster_label[10];
	scc_TempSeedClustering seed_clustering2 = scc_get_seed_clustering(&nng, SCC_EXCLUSION_UPDATING, 6, external_cluster_label);
	assert_int_equal(seed_clustering2.vertices, 10);
	assert_int_equal(seed_clustering2.num_clusters, 4);
	assert_int_equal(seed_clustering2.seed_capacity, 4);
	assert_true(seed_clustering2.external_labels);
	assert_memory_equal(seed_clustering2.assigned, fp_assigned, 10 * sizeof(bool));
	assert_memory_equal(seed_clustering2.seeds, fp_seeds, 4 * sizeof(scc_Vid));
	assert_memory_equal(external_cluster_label, fp_cluster_label, 8 * sizeof(scc_Clabel));

	scc_free_TempSeedClustering(&seed_clustering);
	scc_free_TempSeedClustering(&seed_clustering2);

	scc_free_digraph(&nng);
}


void scc_ut_ignore_remaining(void** state)
{
	(void) state;

	bool assigned[20] = { true,  false, true,  false, true, 
	                      false, true,  false, true,  false,
	                      true,  false, true,  false, true,
	                      false, true,  false, true,  false  };
	scc_Vid seeds[5] = { 0, 18, 2, 6, 4 };
	scc_Clabel cluster_label[20] = { 0, 0, 2, 1, 4,
	                                 2, 3, 3, 0, 4,
	                                 1, 5, 4, 6, 3,
	                                 7, 2, 8, 1, 9  };
	scc_TempSeedClustering cl_template = {
		.vertices = 20,
		.num_clusters = 5,
		.seed_capacity = 5,
		.external_labels = true,
		.assigned = assigned,
		.seeds = seeds,
		.cluster_label = cluster_label,
	};

	const scc_Clabel M = SCC_CLABEL_NA;
	scc_Clabel ref_cluster_label[20] = { 0, M, 2, M, 4,
	                                     M, 3, M, 0, M,
	                                     1, M, 4, M, 3,
	                                     M, 2, M, 1, M  };

	scc_TempSeedClustering cl = scc_copy_TempSeedClustering(&cl_template);
	scc_Clustering out = scc_ignore_remaining(&cl);
	assert_int_equal(cl.vertices, 0);
	assert_int_equal(cl.num_clusters, 0);
	assert_int_equal(cl.seed_capacity, 0);
	assert_false(cl.external_labels);
	assert_null(cl.assigned);
	assert_null(cl.seeds);
	assert_null(cl.cluster_label);
	assert_int_equal(out.vertices, 20);
	assert_int_equal(out.num_clusters, 5);
	assert_true(out.external_labels);
	assert_non_null(out.cluster_label);
	assert_memory_equal(out.cluster_label, ref_cluster_label, 20 * sizeof(scc_Clabel));

	scc_free_TempSeedClustering(&cl);
	scc_free_Clustering(&out);
}


void scc_ut_assign_remaining_lexical(void** state)
{
	(void) state;

	bool assigned[20] = { true,  false, true,  false, true, 
	                      false, true,  false, true,  false,
	                      true,  false, true,  false, true,
	                      false, true,  false, true,  false  };
	scc_Vid seeds[5] = { 0, 18, 2, 6, 4 };
	scc_Clabel cluster_label[20] = { 0, 0, 2, 1, 4,
	                                 2, 3, 3, 0, 4,
	                                 1, 5, 4, 6, 3,
	                                 7, 2, 8, 1, 9  };
	scc_TempSeedClustering cl_template = {
		.vertices = 20,
		.num_clusters = 5,
		.seed_capacity = 5,
		.external_labels = false,
		.assigned = assigned,
		.seeds = seeds,
		.cluster_label = cluster_label,
	};

	scc_Arci tail_ptr1[21] = {  0,  1,  2,  3,  4, 
	                            5,  6,  7,  8,  9, 
	                           10, 11, 12, 13, 14, 
	                           15, 16, 17, 18, 19, 20 };
	scc_Vid head1[20] = { 4,
	                      0,
	                      2,
	                     18,
	                      7,
	                      1,
	                     10,
	                      1,
	                      1,
	                     12,
	                      2,
	                     14,
	                      7,
	                     10,
	                      3,
	                      6,
	                     15,
	                      2,
	                     12,
	                      0 };
	scc_Digraph prio1 = scc_digraph_from_pieces(20, 20, tail_ptr1, head1);
	assert_true(scc_digraph_is_initialized(&prio1));


	scc_Arci tail_ptr2[21] = {  0,  2,  3,  4,  5, 
	                            7, 10, 12, 14, 14, 
	                           14, 16, 17, 19, 19, 
	                           20, 23, 23, 26, 28, 34 };
	scc_Vid head2[34] = {  0, 19,
	                      18,
	                       1,
	                      17,
	                       2, 19,
	                       0,  6, 18,
	                       3, 18,
	                       5,  0,
	                      
	                      
	                       5, 17,
	                       0,
	                       6, 16,
	                      
	                       7,
	                      13, 15, 17,
	                      
	                       6,  0, 15,
	                       9, 15,
	                       0,  2, 4, 6, 8, 10 };
	scc_Digraph prio2 = scc_digraph_from_pieces(20, 34, tail_ptr2, head2);
	assert_true(scc_digraph_is_initialized(&prio2));

	const scc_Clabel M = SCC_CLABEL_NA;
	scc_Clabel ref_cluster_label1[20] = { 0, 0, 2, 1, 4,
	                                      M, 3, M, 0, 4,
	                                      1, 3, 4, 1, 3,
	                                      3, 2, 2, 1, 0  };
	scc_Clabel ref_cluster_label2[20] = { 0, 1, 2, M, 4,
	                                      0, 3, 0, 0, M,
	                                      1, 0, 4, M, 3,
	                                      M, 2, 3, 1, 0  };

	scc_TempSeedClustering cl1 = scc_copy_TempSeedClustering(&cl_template);
	scc_Clustering out1 = scc_assign_remaining_lexical(&cl1, &prio1);
	assert_int_equal(cl1.vertices, 0);
	assert_int_equal(cl1.num_clusters, 0);
	assert_int_equal(cl1.seed_capacity, 0);
	assert_false(cl1.external_labels);
	assert_null(cl1.assigned);
	assert_null(cl1.seeds);
	assert_null(cl1.cluster_label);
	assert_int_equal(out1.vertices, 20);
	assert_int_equal(out1.num_clusters, 5);
	assert_false(out1.external_labels);
	assert_non_null(out1.cluster_label);
	assert_memory_equal(out1.cluster_label, ref_cluster_label1, 20 * sizeof(scc_Clabel));

	scc_TempSeedClustering cl2 = scc_copy_TempSeedClustering(&cl_template);
	scc_Clustering out2 = scc_assign_remaining_lexical(&cl2, &prio2);
	assert_int_equal(cl2.vertices, 0);
	assert_int_equal(cl2.num_clusters, 0);
	assert_int_equal(cl2.seed_capacity, 0);
	assert_false(cl2.external_labels);
	assert_null(cl2.assigned);
	assert_null(cl2.seeds);
	assert_null(cl2.cluster_label);
	assert_int_equal(out2.vertices, 20);
	assert_int_equal(out2.num_clusters, 5);
	assert_false(out2.external_labels);
	assert_non_null(out2.cluster_label);
	assert_memory_equal(out2.cluster_label, ref_cluster_label2, 20 * sizeof(scc_Clabel));

	scc_free_digraph(&prio1);
	scc_free_TempSeedClustering(&cl1);
	scc_free_Clustering(&out1);
	scc_free_digraph(&prio2);
	scc_free_TempSeedClustering(&cl2);
	scc_free_Clustering(&out2);
}


void scc_ut_assign_remaining_desired_size(void** state)
{
	(void) state;


	bool assigned[20] = { true,  false, true,  false, true, 
	                      false, true,  false, true,  false,
	                      true,  false, true,  false, true,
	                      false, true,  false, true,  false  };
	scc_Vid seeds[5] = { 0, 18, 2, 6, 4 };
	scc_Clabel cluster_label[20] = { 0, 0, 2, 1, 4,
	                                 2, 3, 3, 0, 4,
	                                 1, 5, 4, 6, 3,
	                                 7, 2, 8, 1, 9  };
	scc_TempSeedClustering cl_template = {
		.vertices = 20,
		.num_clusters = 5,
		.seed_capacity = 5,
		.external_labels = true,
		.assigned = assigned,
		.seeds = seeds,
		.cluster_label = cluster_label,
	};

	scc_Arci tail_ptr1[21] = {  0,  1,  2,  3,  4, 
	                            5,  6,  7,  8,  9, 
	                           10, 11, 12, 13, 14, 
	                           15, 16, 17, 18, 19, 20 };
	scc_Vid head1[20] = { 4,
	                      0,
	                      2,
	                     18,
	                      7,
	                      1,
	                     10,
	                      1,
	                      1,
	                     12,
	                      2,
	                     14,
	                      7,
	                     10,
	                      3,
	                      6,
	                     15,
	                      2,
	                     12,
	                      0 };
	scc_Digraph prio1 = scc_digraph_from_pieces(20, 20, tail_ptr1, head1);
	assert_true(scc_digraph_is_initialized(&prio1));


	scc_Arci tail_ptr2[21] = {  0,  2,  3,  4,  5, 
	                            7, 10, 12, 14, 14, 
	                           14, 16, 17, 19, 19, 
	                           20, 23, 23, 26, 28, 34 };
	scc_Vid head2[34] = {  0, 19,
	                      18,
	                       1,
	                      17,
	                       2, 19,
	                       0,  6, 18,
	                       3, 18,
	                       5,  0,
	                      
	                      
	                       5, 17,
	                       0,
	                       6, 16,
	                      
	                       7,
	                      13, 15, 17,
	                      
	                       6,  0, 15,
	                       9, 15,
	                       0,  2, 4, 6, 8, 10 };
	scc_Digraph prio2 = scc_digraph_from_pieces(20, 34, tail_ptr2, head2);
	assert_true(scc_digraph_is_initialized(&prio2));

	const scc_Clabel M = SCC_CLABEL_NA;
	scc_Clabel ref_cluster_label1[20] = { 0, 0, 2, 1, 4,
	                                      M, 3, M, 0, 4,
	                                      1, 3, 4, 1, 3,
	                                      3, 2, 2, 1, 0  };
	scc_Clabel ref_cluster_label2[20] = { 0, 1, 2, M, 4,
	                                      1, 3, 0, 0, M,
	                                      1, 0, 4, M, 3,
	                                      M, 2, 3, 1, 3  };
	scc_Clabel ref_cluster_label3[20] = { 0, 1, 2, M, 4,
	                                      1, 3, 0, 0, M,
	                                      1, 0, 4, M, 3,
	                                      M, 2, 0, 1, 1  };

	scc_TempSeedClustering cl1 = scc_copy_TempSeedClustering(&cl_template);
	scc_Clustering out1 = scc_assign_remaining_desired_size(&cl1, &prio1, 2);
	assert_int_equal(cl1.vertices, 0);
	assert_int_equal(cl1.num_clusters, 0);
	assert_int_equal(cl1.seed_capacity, 0);
	assert_false(cl1.external_labels);
	assert_null(cl1.assigned);
	assert_null(cl1.seeds);
	assert_null(cl1.cluster_label);
	assert_int_equal(out1.vertices, 20);
	assert_int_equal(out1.num_clusters, 5);
	assert_true(out1.external_labels);
	assert_non_null(out1.cluster_label);
	assert_memory_equal(out1.cluster_label, ref_cluster_label1, 20 * sizeof(scc_Clabel));

	scc_TempSeedClustering cl2 = scc_copy_TempSeedClustering(&cl_template);
	scc_Clustering out2 = scc_assign_remaining_desired_size(&cl2, &prio2, 2);
	assert_int_equal(cl2.vertices, 0);
	assert_int_equal(cl2.num_clusters, 0);
	assert_int_equal(cl2.seed_capacity, 0);
	assert_false(cl2.external_labels);
	assert_null(cl2.assigned);
	assert_null(cl2.seeds);
	assert_null(cl2.cluster_label);
	assert_int_equal(out2.vertices, 20);
	assert_int_equal(out2.num_clusters, 5);
	assert_true(out2.external_labels);
	assert_non_null(out2.cluster_label);
	assert_memory_equal(out2.cluster_label, ref_cluster_label2, 20 * sizeof(scc_Clabel));

	scc_TempSeedClustering cl3 = scc_copy_TempSeedClustering(&cl_template);
	scc_Clustering out3 = scc_assign_remaining_desired_size(&cl3, &prio2, 3);
	assert_int_equal(cl3.vertices, 0);
	assert_int_equal(cl3.num_clusters, 0);
	assert_int_equal(cl3.seed_capacity, 0);
	assert_false(cl3.external_labels);
	assert_null(cl3.assigned);
	assert_null(cl3.seeds);
	assert_null(cl3.cluster_label);
	assert_int_equal(out3.vertices, 20);
	assert_int_equal(out3.num_clusters, 5);
	assert_true(out3.external_labels);
	assert_non_null(out3.cluster_label);
	assert_memory_equal(out3.cluster_label, ref_cluster_label3, 20 * sizeof(scc_Clabel));

	scc_free_digraph(&prio1);
	scc_free_TempSeedClustering(&cl1);
	scc_free_Clustering(&out1);
	scc_free_digraph(&prio2);
	scc_free_TempSeedClustering(&cl2);
	scc_free_Clustering(&out2);
	scc_free_TempSeedClustering(&cl3);
	scc_free_Clustering(&out3);
}


int main(void)
{
	const struct CMUnitTest test_nng_clustering[] = {
		cmocka_unit_test(scc_ut_nng_free_TempSeedClustering),
		cmocka_unit_test(scc_ut_nng_copy_TempSeedClustering),
		cmocka_unit_test(scc_ut_clustering_lexical),
		cmocka_unit_test(scc_ut_clustering_inwards),
		cmocka_unit_test(scc_ut_clustering_inwards_updating),
		cmocka_unit_test(scc_ut_clustering_exclusion),
		cmocka_unit_test(scc_ut_clustering_exclusion_updating),
		cmocka_unit_test(scc_ut_ignore_remaining),
		cmocka_unit_test(scc_ut_assign_remaining_lexical),
		cmocka_unit_test(scc_ut_assign_remaining_desired_size),
	};

	return cmocka_run_group_tests_name("nng clustering module", test_nng_clustering, NULL, NULL);
}
