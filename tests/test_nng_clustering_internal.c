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

#include "test_suite.h"
#include "assert_digraph.h"
#include "../src/nng_clustering.c"

#include "data_object_test.h"


void scc_ut_make_clustering_from_nng(void** state)
{
	(void) state;

	scc_Clabel external_cluster_labels[15];
	const scc_Clabel M = SCC_CLABEL_NA;
	const bool main_data_points[15] = { true, true, true, true, true,
	                                    false, false, false, false, false,
	                                    true, true, true, true, true };

	iscc_Digraph nng1;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..#.. ..#.. ...../"
	                         "..... ..... ...##/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... #..../", &nng1);
	scc_Clustering* cl1;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl1), SCC_ER_OK);
	scc_ErrorCode ec1 = iscc_make_clustering_from_nng(cl1, &scc_ut_test_data_small_struct,
	                                                  &nng1, false, 3, SCC_SM_LEXICAL,
	                                                  SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                                                  main_data_points, SCC_UM_IGNORE, false, 0.0);
	const scc_Clabel ref_cluster_label1[15] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	assert_int_equal(ec1, SCC_ER_OK);
	assert_int_equal(cl1->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl1->num_data_points, 15);
	assert_int_equal(cl1->num_clusters, 3);
	assert_non_null(cl1->cluster_label);
	assert_ptr_equal(cl1->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label1, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl1->cluster_label, ref_cluster_label1, 15 * sizeof(scc_Clabel));
	assert_true(cl1->external_labels);
	scc_free_clustering(&cl1);
	iscc_free_digraph(&nng1);


	iscc_Digraph nng2;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..#.. ..#.. ...../"
	                         "..... ..... ...##/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... #..../", &nng2);
	scc_Clustering* cl2;
	assert_int_equal(scc_init_empty_clustering(15, NULL, &cl2), SCC_ER_OK);
	scc_ErrorCode ec2 = iscc_make_clustering_from_nng(cl2, &scc_ut_test_data_small_struct,
	                                                  &nng2, false, 3, SCC_SM_LEXICAL,
	                                                  SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                                                  main_data_points, SCC_UM_IGNORE, false, 0.0);
	const scc_Clabel ref_cluster_label2[15] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	assert_int_equal(ec2, SCC_ER_OK);
	assert_int_equal(cl2->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl2->num_data_points, 15);
	assert_int_equal(cl2->num_clusters, 3);
	assert_non_null(cl2->cluster_label);
	assert_memory_equal(cl2->cluster_label, ref_cluster_label2, 15 * sizeof(scc_Clabel));
	assert_false(cl2->external_labels);
	scc_free_clustering(&cl2);
	iscc_free_digraph(&nng2);


	iscc_Digraph nng3;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..#.. ..#.. ...../"
	                         "..... ..... ...##/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... #..../", &nng3);
	scc_Clustering* cl3;
	assert_int_equal(scc_init_empty_clustering(15, NULL, &cl3), SCC_ER_OK);
	scc_ErrorCode ec3 = iscc_make_clustering_from_nng(cl3, &scc_ut_test_data_small_struct,
	                                                  &nng3, false, 3, SCC_SM_LEXICAL,
	                                                  SCC_UM_CLOSEST_SEED_EST_RADIUS, false, 0.0,
	                                                  main_data_points, SCC_UM_CLOSEST_SEED_EST_RADIUS, false, 0.0);
	const scc_Clabel ref_cluster_label3[15] = { 0, 0, 1, 1, 2,   1, 0, 0, 2, 1,   1, M, 2, 1, 2 };
	assert_int_equal(ec3, SCC_ER_OK);
	assert_int_equal(cl3->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl3->num_data_points, 15);
	assert_int_equal(cl3->num_clusters, 3);
	assert_non_null(cl3->cluster_label);
	assert_memory_equal(cl3->cluster_label, ref_cluster_label3, 15 * sizeof(scc_Clabel));
	assert_false(cl3->external_labels);
	scc_free_clustering(&cl3);
	iscc_free_digraph(&nng3);


	iscc_Digraph nng4;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..#.. ..#.. ...../"
	                         "..... ..... ...##/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... #..../", &nng4);
	scc_Clustering* cl4;
	assert_int_equal(scc_init_empty_clustering(15, NULL, &cl4), SCC_ER_OK);
	scc_ErrorCode ec4 = iscc_make_clustering_from_nng(cl4, &scc_ut_test_data_small_struct,
	                                                  &nng4, false, 3, SCC_SM_LEXICAL,
	                                                  SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                                                  main_data_points, SCC_UM_CLOSEST_SEED_EST_RADIUS, false, 0.0);
	const scc_Clabel ref_cluster_label4[15] = { 0, 0, 1, 1, 2,   1, 0, 0, 2, 1,   1, 1, 2, 1, 0 };
	assert_int_equal(ec4, SCC_ER_OK);
	assert_int_equal(cl4->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl4->num_data_points, 15);
	assert_int_equal(cl4->num_clusters, 3);
	assert_non_null(cl4->cluster_label);
	assert_memory_equal(cl4->cluster_label, ref_cluster_label4, 15 * sizeof(scc_Clabel));
	assert_false(cl4->external_labels);
	scc_free_clustering(&cl4);
	iscc_free_digraph(&nng4);


	iscc_Digraph nng5;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..#.. ..#.. ...../"
	                         "..... ..... ...##/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... #..../", &nng5);
	scc_Clustering* cl5;
	assert_int_equal(scc_init_empty_clustering(15, NULL, &cl5), SCC_ER_OK);
	scc_ErrorCode ec5 = iscc_make_clustering_from_nng(cl5, &scc_ut_test_data_small_struct,
	                                                  &nng5, false, 3, SCC_SM_LEXICAL,
	                                                  SCC_UM_CLOSEST_SEED_EST_RADIUS, false, 0.0,
	                                                  main_data_points, SCC_UM_IGNORE, false, 0.0);
	const scc_Clabel ref_cluster_label5[15] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, M, 2, 1, 2 };
	assert_int_equal(ec5, SCC_ER_OK);
	assert_int_equal(cl5->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl5->num_data_points, 15);
	assert_int_equal(cl5->num_clusters, 3);
	assert_non_null(cl5->cluster_label);
	assert_memory_equal(cl5->cluster_label, ref_cluster_label5, 15 * sizeof(scc_Clabel));
	assert_false(cl5->external_labels);
	scc_free_clustering(&cl5);
	iscc_free_digraph(&nng5);
}


int main(void)
{
	const struct CMUnitTest test_cases[] = {
		cmocka_unit_test(scc_ut_make_clustering_from_nng),
	};

	return cmocka_run_group_tests_name("internal nng_clustering.c", test_cases, NULL, NULL);
}
