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
#include <src/clustering_struct.h>
#include <src/nng_batch_clustering.h>
#include <src/scclust_types.h>
#include "data_object_test.h"

static const int32_t ISCC_UT_OPTIONS_STRUCT_VERSION = 722678001;

void iscc_run_nonval_tests_batches(scc_UnassignedMethod unassigned_method,
                                   bool radius_constraint,
                                   double radius,
                                   size_t len_primary_data_points,
                                   const scc_PointIndex primary_data_points[]);

static scc_ClusterOptions iscc_translate_options(const uint32_t size_constraint,
                                                 const uint32_t num_types,
                                                 const uint32_t* const type_constraints,
                                                 const size_t len_type_labels,
                                                 const scc_TypeLabel* const type_labels,
                                                 const scc_SeedMethod seed_method,
                                                 const scc_UnassignedMethod unassigned_method,
                                                 const bool radius_constraint,
                                                 const double radius,
                                                 const size_t len_primary_data_points,
                                                 const scc_PointIndex* const primary_data_points,
                                                 const scc_UnassignedMethod secondary_unassigned_method,
                                                 const bool secondary_radius_constraint,
                                                 const double secondary_radius,
                                                const  uint32_t batch_size)
{
	return (scc_ClusterOptions) {
		.options_version = ISCC_UT_OPTIONS_STRUCT_VERSION,
		.size_constraint = size_constraint,
		.num_types = num_types,
		.type_constraints = type_constraints,
		.len_type_labels = len_type_labels,
		.type_labels = type_labels,
		.seed_method = seed_method,
		.len_primary_data_points = len_primary_data_points,
		.primary_data_points = primary_data_points,
		.primary_unassigned_method = unassigned_method,
		.secondary_unassigned_method = secondary_unassigned_method,
		.seed_radius = radius_constraint,
		.seed_supplied_radius = radius,
		.primary_radius = SCC_RM_USE_SEED_RADIUS,
		.primary_supplied_radius = 0.0,
		.secondary_radius = secondary_radius_constraint,
		.secondary_supplied_radius = secondary_radius,
		.batch_size = batch_size,
	};
}


void scc_ut_nng_clustering_batches_internal(void** state)
{
	(void) state;

	/*
	true <- TRUE
	false <- FALSE
	dist_mat <- aaa
	num_points <- nrow(dist_mat)
	size_constraint <- 2
	ignore_remaining <- TRUE
	radius <- Inf
	primary_data_points <- rep(TRUE, num_points)

	res <- apply(dist_mat, 1, order)[1:size_constraint, ]
	within_radius <- (apply(dist_mat, 1, sort)[size_constraint, ] < radius)

	clustering <- rep(NA, num_points)
	assign <- rep(FALSE, num_points)

	cluster_label <- 0

	for (i in which(primary_data_points)) {
	  if (!assign[i] && within_radius[i]) {
	    if (!any(assign[res[, i]])) {
	      assign[res[, i]] <- TRUE
	      clustering[res[, i]] <- cluster_label
	      cluster_label <- cluster_label + 1
	    } else if(!ignore_remaining) {
	      clustering[i] <- clustering[res[, i][which(assign[res[, i]])[1]]]
	    }
	  }
	}

	cluster_label
	paste0(clustering, collapse = ", ")
	*/

	bool cl_is_OK;
	scc_Clustering* cl;
	scc_Clustering* cl_compare;
	scc_ClusterOptions options;
	scc_ErrorCode ec;
	const scc_Clabel NA = SCC_CLABEL_NA;
	scc_Clabel external_cluster_labels[100];
	scc_Clabel external_cluster_labels_compare[100];
	const size_t len_primary_data_points = 50;
	const scc_PointIndex primary_data_points[50] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 40,
	                                        41, 42, 43, 44, 45, 46, 47, 48, 49, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
	                                        80, 81, 82, 83, 84, 85, 86, 87, 88, 89 };

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 3,
	                                SCC_UM_IGNORE, false, 0.0, 0, NULL, 10);
	assert_int_equal(ec, SCC_ER_OK);
	scc_free_clustering(&cl);

	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 3,
	                                SCC_UM_IGNORE, false, 0.0, 0, NULL, 10);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_small_struct, 3,
	                                SCC_UM_IGNORE, false, 0.0, 0, NULL, 10);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);
	scc_free_clustering(&cl);

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 1,
	                                SCC_UM_IGNORE, false, 0.0, 0, NULL, 10);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);
	scc_free_clustering(&cl);

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 101,
	                                SCC_UM_IGNORE, false, 0.0, 0, NULL, 10);
	assert_int_equal(ec, SCC_ER_NO_SOLUTION);
	scc_free_clustering(&cl);

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 3,
	                                100, false, 0.0, 0, NULL, 10);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);
	scc_free_clustering(&cl);

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 3,
	                                SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0, NULL, 10);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);
	scc_free_clustering(&cl);

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 3,
	                                SCC_UM_CLOSEST_SEED, false, 0.0, 0, NULL, 10);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);
	scc_free_clustering(&cl);

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 3,
	                                SCC_UM_IGNORE, true, 0.0, 0, NULL, 10);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);
	scc_free_clustering(&cl);

	scc_init_existing_clustering(100, 10, external_cluster_labels, false, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 3,
	                                SCC_UM_IGNORE, false, 0.0, 0, NULL, 10);
	assert_int_equal(ec, SCC_ER_NOT_IMPLEMENTED);
	scc_free_clustering(&cl);

	scc_init_empty_clustering(100, NULL, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 3,
	                                SCC_UM_IGNORE, true, 1.0, 0, NULL, 10);
	assert_int_equal(ec, SCC_ER_NO_SOLUTION);
	scc_free_clustering(&cl);

/*
	DOES NOT WORK WITH ANN, CHECK WHY!

	const scc_Clabel ref0_cluster_label[100] = { 0, 1, 2, 3, 3, NA, 4, 5, 6, 20, 0, 4, 7, NA, 8, 17, 9, 10, 11, 8, 20, 12, 13, 14, NA, NA, 15, 14, 15, NA, 3, NA, NA, NA, 9, NA, 8, 19, NA, NA, NA, 16, 17, NA, NA, NA, 5, 13, 7, 15, 17, NA, NA, NA, NA, 11, 22, 18, 2, 4, NA, 18, 0, 6, NA, NA, 19, 20, NA, 2, 12, 19, 16, 1, NA, NA, 5, 10, NA, 18, NA, 21, 13, 14, 16, 12, 9, 21, 10, NA, 6, 22, NA, 22, 21, 7, 1, NA, NA, 11 };
	const size_t ref0_num_clusters = 23;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 3,
	                                SCC_UM_IGNORE, false, 0.0,
	                                0, NULL, 0);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_init_empty_clustering(100, external_cluster_labels_compare, &cl_compare);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_make_clustering(&scc_ut_test_data_large_struct, cl_compare, &options);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->num_clusters, cl_compare->num_clusters);
	assert_memory_equal(cl->cluster_label, cl_compare->cluster_label, 100 * sizeof(scc_Clabel));
	scc_free_clustering(&cl_compare);
	scc_free_clustering(&cl);
*/

	const scc_Clabel ref1_cluster_label[100] = { 0, 1, 2, 3, 3, 4, 5, 6, 7, 31, 0, 8, 9, 10, 11, 12, 13, 14, 15, NA, NA, 16, 17, 18, NA, 19, 20, 19, 20, 21, 36, 22, NA, 23, 22, NA, 11, NA, 8, 10, 24, 25, NA, 26, 27, NA, 6, 17, 28, NA, 12, NA, NA, NA, NA, 15, NA, NA, 4, 5, 28, 29, NA, 7, 24, NA, 30, 31, NA, 2, 23, 30, 25, NA, 32, 33, 33, 32, 34, 29, NA, 35, NA, 18, NA, 16, 13, NA, 14, 26, NA, 27, 34, NA, 35, 9, 1, NA, 36, 21 };
	const size_t ref1_num_clusters = 37;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 2,
	                                SCC_UM_IGNORE, false, 0.0,
	                                0, NULL, 10);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_int_equal(cl->num_clusters, ref1_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_memory_equal(cl->cluster_label, ref1_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering(cl, 2, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_init_empty_clustering(100, external_cluster_labels_compare, &cl_compare);
	options = iscc_translate_options(2,
	                        0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_make_clustering(&scc_ut_test_data_large_struct, cl_compare, &options);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->num_clusters, cl_compare->num_clusters);
	assert_memory_equal(cl->cluster_label, cl_compare->cluster_label, 100 * sizeof(scc_Clabel));
	scc_free_clustering(&cl_compare);
	scc_free_clustering(&cl);


	const scc_Clabel ref2_cluster_label[100] = { 0, 1, 2, NA, NA, 2, 3, 3, NA, 2, 0, 3, NA, NA, NA, 1, 5, 0, 4, 5, 2, 6, NA, 4, 4, 5, NA, 5, NA, 4, 1, 5, 2, 6, 5, NA, NA, 5, 3, 3, 0, NA, 1, 0, 0, 4, 3, NA, 6, NA, 1, 5, 2, 0, 3, 4, NA, 6, 2, 3, 6, 6, 0, NA, 0, NA, 5, 2, NA, 2, 6, 5, NA, 1, NA, 3, 3, NA, 1, 6, 2, 4, 6, 4, NA, 6, NA, NA, NA, NA, NA, 0, 1, NA, 4, NA, 1, NA, 1, 4 };
	const size_t ref2_num_clusters = 7;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 10,
	                                SCC_UM_IGNORE, false, 0.0,
	                                0, NULL, 1);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_int_equal(cl->num_clusters, ref2_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_memory_equal(cl->cluster_label, ref2_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering(cl, 10, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_init_empty_clustering(100, external_cluster_labels_compare, &cl_compare);
	options = iscc_translate_options(10,
	                        0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_make_clustering(&scc_ut_test_data_large_struct, cl_compare, &options);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->num_clusters, cl_compare->num_clusters);
	assert_memory_equal(cl->cluster_label, cl_compare->cluster_label, 100 * sizeof(scc_Clabel));
	scc_free_clustering(&cl_compare);
	scc_free_clustering(&cl);

/*
	DOES NOT WORK WITH ANN, CHECK WHY!

	const scc_Clabel ref3_cluster_label[100] = { 0, 1, 2, 3, 3, 2, 4, 5, 6, 20, 0, 4, 7, 4, 8, 17, 9, 10, 11, 8, 20, 12, 13, 14, 5, 14, 15, 14, 15, 11, 3, 9, 2, 12, 9, 9, 8, 19, 4, 4, 13, 16, 17, 10, 10, 11, 5, 13, 7, 15, 17, 14, 2, 8, 4, 11, 22, 18, 2, 4, 7, 18, 0, 6, 0, 6, 19, 20, 3, 2, 12, 19, 16, 1, 10, 5, 5, 10, 1, 18, 2, 21, 13, 14, 16, 12, 9, 21, 10, 10, 6, 22, 1, 22, 21, 7, 1, 6, 3, 11 };
	const size_t ref3_num_clusters = 23;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 3,
	                                SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                                0, NULL, 0);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_int_equal(cl->num_clusters, ref3_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_memory_equal(cl->cluster_label, ref3_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);
*/

	const scc_Clabel ref4_cluster_label[100] = { 0, 1, 2, 3, 3, 4, 5, 6, 7, 31, 0, 8, 9, 10, 11, 12, 13, 14, 15, 11, 4, 16, 17, 18, 6, 19, 20, 19, 20, 21, 36, 22, 4, 23, 22, 13, 11, 22, 8, 10, 24, 25, 12, 26, 27, 21, 6, 17, 28, 20, 12, 22, 4, 24, 8, 15, 20, 23, 4, 5, 28, 29, 24, 7, 24, 7, 30, 31, 3, 2, 23, 30, 25, 1, 32, 33, 33, 32, 34, 29, 2, 35, 17, 18, 25, 16, 13, 35, 14, 26, 7, 27, 34, 27, 35, 9, 1, 7, 36, 21 };
	const size_t ref4_num_clusters = 37;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 2,
	                                SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                                0, NULL, 10);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_int_equal(cl->num_clusters, ref4_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_memory_equal(cl->cluster_label, ref4_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering(cl, 2, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	const scc_Clabel ref5_cluster_label[100] = { 0, 1, 2, 1, 1, 2, 3, 3, 0, 2, 0, 3, 3, 3, 0, 1, 5, 0, 4, 5, 2, 6, 0, 4, 4, 5, 0, 5, 0, 4, 1, 5, 2, 6, 5, 5, 5, 5, 3, 3, 0, 4, 1, 0, 0, 4, 3, 1, 6, 0, 1, 5, 2, 0, 3, 4, 0, 6, 2, 3, 6, 6, 0, 5, 0, 5, 5, 2, 1, 2, 6, 5, 0, 1, 0, 3, 3, 0, 1, 6, 2, 4, 6, 4, 0, 6, 3, 4, 0, 0, 5, 0, 1, 0, 4, 6, 1, 5, 1, 4 };
	const size_t ref5_num_clusters = 7;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 10,
	                                SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                                0, NULL, 1);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_int_equal(cl->num_clusters, ref5_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_memory_equal(cl->cluster_label, ref5_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering(cl, 10, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);

/*
	DOES NOT WORK WITH ANN, CHECK WHY!

	const scc_Clabel ref6_cluster_label[100] = { 0, 1, 2, 3, 3, NA, 4, 5, 6, NA, 0, 4, 7, NA, 8, 16, NA, 9, NA, 8, NA, 10, 11, NA, NA, 12, 13, 12, 13, 14, 3, 12, NA, NA, NA, NA, 8, NA, NA, NA, NA, 15, 16, NA, NA, 14, 5, 11, 7, 13, 16, NA, NA, NA, NA, NA, NA, 17, 2, 4, NA, 17, 0, 6, NA, NA, NA, NA, NA, 2, 10, NA, 15, 1, NA, NA, 5, 9, NA, 17, NA, 18, 11, NA, 15, 10, NA, 18, 9, NA, 6, NA, NA, NA, 18, 7, 1, NA, NA, 14 };
	const size_t ref6_num_clusters = 19;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 3,
	                                SCC_UM_IGNORE, true, 20.0,
	                                0, NULL, 0);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_int_equal(cl->num_clusters, ref6_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_memory_equal(cl->cluster_label, ref6_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_init_empty_clustering(100, external_cluster_labels_compare, &cl_compare);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_IGNORE, true, 20.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_make_clustering(&scc_ut_test_data_large_struct, cl_compare, &options);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->num_clusters, cl_compare->num_clusters);
	assert_memory_equal(cl->cluster_label, cl_compare->cluster_label, 100 * sizeof(scc_Clabel));
	scc_free_clustering(&cl_compare);
	scc_free_clustering(&cl);
*/

	const scc_Clabel ref7_cluster_label[100] = { 0, 1, 2, 3, 3, 4, 5, 6, 7, NA, 0, 8, 9, 10, 11, 12, 13, 14, 15, NA, NA, 16, 17, 18, NA, 19, 20, 19, 20, 21, 34, 22, NA, 23, 22, NA, 11, NA, 8, 10, 24, 25, NA, 26, 27, NA, 6, 17, 28, NA, 12, NA, NA, NA, NA, 15, NA, NA, 4, 5, 28, 29, NA, 7, 24, NA, 30, NA, NA, 2, 23, 30, 25, NA, NA, 31, 31, NA, 32, 29, NA, 33, NA, 18, NA, 16, 13, NA, 14, 26, NA, 27, 32, NA, 33, 9, 1, NA, 34, 21 };
	const size_t ref7_num_clusters = 35;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 2,
	                                SCC_UM_IGNORE, true, 20.0,
	                                0, NULL, 10);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_int_equal(cl->num_clusters, ref7_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_memory_equal(cl->cluster_label, ref7_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering(cl, 2, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_init_empty_clustering(100, external_cluster_labels_compare, &cl_compare);
	options = iscc_translate_options(2,
	                        0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_IGNORE, true, 20.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_make_clustering(&scc_ut_test_data_large_struct, cl_compare, &options);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->num_clusters, cl_compare->num_clusters);
	assert_memory_equal(cl->cluster_label, cl_compare->cluster_label, 100 * sizeof(scc_Clabel));
	scc_free_clustering(&cl_compare);
	scc_free_clustering(&cl);


	const scc_Clabel ref8_cluster_label[100] = { 3, NA, 1, 0, 0, 1, 2, 2, 4, 1, NA, 2, NA, NA, NA, 0, NA, 3, NA, NA, 1, NA, NA, NA, NA, 4, 3, 4, 3, NA, 0, 4, 1, NA, 4, NA, NA, 4, 2, 2, NA, NA, 0, 3, 3, NA, 2, 0, NA, NA, 0, 4, 1, NA, 2, NA, 1, NA, 1, 2, NA, 0, NA, 4, NA, NA, NA, NA, 0, 1, NA, NA, NA, NA, NA, 2, 2, 3, NA, NA, 1, NA, NA, NA, NA, NA, NA, NA, 3, 3, 4, 3, NA, NA, NA, NA, NA, 4, 0, NA };
	const size_t ref8_num_clusters = 5;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 10,
	                                SCC_UM_IGNORE, true, 30.0,
	                                0, NULL, 1);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_int_equal(cl->num_clusters, ref8_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_memory_equal(cl->cluster_label, ref8_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering(cl, 10, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_init_empty_clustering(100, external_cluster_labels_compare, &cl_compare);
	options = iscc_translate_options(10,
	                        0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_IGNORE, true, 30.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_make_clustering(&scc_ut_test_data_large_struct, cl_compare, &options);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->num_clusters, cl_compare->num_clusters);
	assert_memory_equal(cl->cluster_label, cl_compare->cluster_label, 100 * sizeof(scc_Clabel));
	scc_free_clustering(&cl_compare);
	scc_free_clustering(&cl);

/*
	DOES NOT WORK WITH ANN, CHECK WHY!

	const scc_Clabel ref9_cluster_label[100] = { 0, 1, 2, 3, 3, 2, 4, 5, 6, NA, 0, 4, 7, NA, 8, 16, NA, 9, NA, 8, 2, 10, 11, NA, NA, 12, 13, 12, 13, 14, 3, 12, 2, 10, 12, NA, 8, 12, 4, 4, 11, 15, 16, 9, 9, 14, 5, 11, 7, 13, 16, 12, NA, 8, 4, NA, NA, 17, 2, 4, 7, 17, 0, 6, 0, NA, NA, NA, 3, 2, 10, NA, 15, 1, NA, 5, 5, 9, 1, 17, NA, 18, 11, NA, 15, 10, 5, 18, 9, 9, 6, 13, 1, NA, 18, 7, 1, 6, 3, 14 };
	const size_t ref9_num_clusters = 19;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 3,
	                                SCC_UM_ANY_NEIGHBOR, true, 20.0,
	                                0, NULL, 0);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_int_equal(cl->num_clusters, ref9_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_memory_equal(cl->cluster_label, ref9_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);
*/

	const scc_Clabel ref10_cluster_label[100] = { 0, 1, 2, 3, 3, 4, 5, 6, 7, NA, 0, 8, 9, 10, 11, 12, 13, 14, 15, 11, 4, 16, 17, 18, 6, 19, 20, 19, 20, 21, 34, 22, 4, 23, 22, NA, 11, 22, 8, 10, 24, 25, 12, 26, 27, 21, 6, 17, 28, 20, 12, 22, NA, 24, 8, 15, NA, 23, 4, 5, 28, 29, 24, 7, 24, 7, 30, NA, 3, 2, 23, 30, 25, 1, NA, 31, 31, 14, 32, 29, 2, 33, 17, 18, 25, 16, 13, 33, 14, 26, 7, 27, 32, NA, 33, 9, 1, 7, 34, 21 };
	const size_t ref10_num_clusters = 35;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 2,
	                                SCC_UM_ANY_NEIGHBOR, true, 20.0,
	                                0, NULL, 10);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_int_equal(cl->num_clusters, ref10_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_memory_equal(cl->cluster_label, ref10_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering(cl, 2, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	const scc_Clabel ref11_cluster_label[100] = { 3, NA, 1, 0, 0, 1, 2, 2, 4, 1, NA, 2, 2, NA, 1, 0, NA, 3, NA, NA, 1, 0, 0, NA, NA, 4, 3, 4, 3, NA, 0, 4, 1, NA, 4, NA, NA, 4, 2, 2, 3, NA, 0, 3, 3, NA, 2, 0, 2, NA, 0, 4, 1, 3, 2, NA, 1, 0, 1, 2, 2, 0, NA, 4, 3, NA, NA, NA, 0, 1, NA, NA, NA, NA, NA, 2, 2, 3, NA, 0, 1, NA, 0, NA, NA, 0, 2, NA, 3, 3, 4, 3, 0, NA, NA, 2, NA, 4, 0, NA };
	const size_t ref11_num_clusters = 5;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 10,
	                                SCC_UM_ANY_NEIGHBOR, true, 30.0,
	                                0, NULL, 1);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_int_equal(cl->num_clusters, ref11_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_memory_equal(cl->cluster_label, ref11_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering(cl, 10, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);

/*
	DOES NOT WORK WITH ANN, CHECK WHY!

	const scc_Clabel ref12_cluster_label[100] = { 0, 1, 2, 3, 3, NA, 4, 5, 6, 17, 0, 4, NA, NA, NA, NA, NA, 13, NA, NA, 17, 7, 8, 9, NA, NA, 10, 9, 10, 11, 3, NA, NA, NA, NA, NA, NA, 16, NA, NA, NA, 12, NA, 13, NA, 11, 5, 8, 14, NA, NA, NA, NA, NA, 14, NA, NA, 15, 2, 4, 14, 15, 0, 6, NA, NA, 16, 17, NA, 2, 7, 16, 12, 1, NA, NA, 5, NA, NA, 15, NA, 18, 8, 9, 12, 7, NA, 18, 10, 13, 6, NA, NA, NA, 18, NA, 1, NA, NA, 11 };
	const size_t ref12_num_clusters = 19;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 3,
	                                SCC_UM_IGNORE, false, 0.0,
	                                len_primary_data_points, primary_data_points, 0);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_int_equal(cl->num_clusters, ref12_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_memory_equal(cl->cluster_label, ref12_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_init_empty_clustering(100, external_cluster_labels_compare, &cl_compare);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_make_clustering(&scc_ut_test_data_large_struct, cl_compare, &options);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->num_clusters, cl_compare->num_clusters);
	assert_memory_equal(cl->cluster_label, cl_compare->cluster_label, 100 * sizeof(scc_Clabel));
	scc_free_clustering(&cl_compare);
	scc_free_clustering(&cl);
*/

	const scc_Clabel ref13_cluster_label[100] = { 0, 1, 2, 3, 3, 4, 5, 6, 7, 22, 0, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, 8, 9, 10, NA, 11, 12, 11, 12, 13, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, 14, 15, 16, 17, 18, NA, 6, 9, 19, NA, 16, NA, NA, NA, NA, NA, NA, NA, 4, 5, 19, 20, NA, 7, 14, NA, 21, 22, NA, 2, NA, 21, 15, NA, NA, 24, NA, 25, NA, 20, NA, 23, NA, 10, NA, 8, 24, NA, 25, 17, NA, 18, NA, NA, 23, NA, 1, NA, NA, 13 };
	const size_t ref13_num_clusters = 26;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 2,
	                                SCC_UM_IGNORE, false, 0.0,
	                                len_primary_data_points, primary_data_points, 10);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_int_equal(cl->num_clusters, ref13_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_memory_equal(cl->cluster_label, ref13_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering(cl, 2, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_init_empty_clustering(100, external_cluster_labels_compare, &cl_compare);
	options = iscc_translate_options(2,
	                        0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_make_clustering(&scc_ut_test_data_large_struct, cl_compare, &options);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->num_clusters, cl_compare->num_clusters);
	assert_memory_equal(cl->cluster_label, cl_compare->cluster_label, 100 * sizeof(scc_Clabel));
	scc_free_clustering(&cl_compare);
	scc_free_clustering(&cl);


	const scc_Clabel ref14_cluster_label[100] = { 0, 1, 2, NA, NA, 2, 3, 3, 4, 2, 0, 3, NA, NA, NA, 1, NA, 0, 5, NA, 2, NA, NA, 4, 5, 4, NA, 4, NA, 5, 1, 4, 2, NA, 4, NA, NA, 4, 3, 3, 0, NA, 1, 0, 0, 5, 3, NA, NA, NA, 1, 4, 2, 0, 3, NA, NA, NA, 2, 3, NA, NA, 0, 4, 0, NA, NA, 2, NA, 2, NA, NA, NA, 1, NA, 3, 3, NA, 1, 5, 2, 5, 5, NA, NA, NA, NA, 5, NA, NA, 4, 0, 1, NA, 5, NA, 1, NA, 1, 5 };
	const size_t ref14_num_clusters = 6;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 10,
	                                SCC_UM_IGNORE, false, 0.0,
	                                len_primary_data_points, primary_data_points, 1);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_int_equal(cl->num_clusters, ref14_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_memory_equal(cl->cluster_label, ref14_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering(cl, 10, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_init_empty_clustering(100, external_cluster_labels_compare, &cl_compare);
	options = iscc_translate_options(10,
	                        0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_make_clustering(&scc_ut_test_data_large_struct, cl_compare, &options);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->num_clusters, cl_compare->num_clusters);
	assert_memory_equal(cl->cluster_label, cl_compare->cluster_label, 100 * sizeof(scc_Clabel));
	scc_free_clustering(&cl_compare);
	scc_free_clustering(&cl);

/*
	DOES NOT WORK WITH ANN, CHECK WHY!

	const scc_Clabel ref15_cluster_label[100] = { 0, 1, 2, 3, 3, 2, 4, 5, 6, 17, 0, 4, NA, NA, NA, NA, NA, 13, NA, NA, 17, 7, 8, 9, 5, 9, 10, 9, 10, 11, 3, NA, NA, NA, NA, NA, NA, 16, NA, NA, 8, 12, 3, 13, 13, 11, 5, 8, 14, 10, NA, NA, NA, NA, 14, NA, NA, 15, 2, 4, 14, 15, 0, 6, 0, 6, 16, 17, 3, 2, 7, 16, 12, 1, NA, NA, 5, NA, NA, 15, 2, 18, 8, 9, 12, 7, 5, 18, 10, 13, 6, NA, NA, NA, 18, NA, 1, NA, NA, 11 };
	const size_t ref15_num_clusters = 19;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 3,
	                                SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                                len_primary_data_points, primary_data_points, 0);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_int_equal(cl->num_clusters, ref15_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_memory_equal(cl->cluster_label, ref15_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);
*/

	const scc_Clabel ref16_cluster_label[100] = { 0, 1, 2, 3, 3, 4, 5, 6, 7, 22, 0, NA, NA, NA, NA, NA, NA, NA, NA, NA, 4, 8, 9, 10, 6, 11, 12, 11, 12, 13, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, 14, 15, 16, 17, 18, 13, 6, 9, 19, 12, 16, NA, NA, NA, NA, NA, NA, NA, 4, 5, 19, 20, 14, 7, 14, 7, 21, 22, 3, 2, NA, 21, 15, NA, NA, 24, NA, 25, NA, 20, 2, 23, 9, 10, 15, 8, 24, 23, 25, 17, NA, 18, NA, NA, 23, NA, 1, NA, NA, 13 };
	const size_t ref16_num_clusters = 26;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 2,
	                                SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                                len_primary_data_points, primary_data_points, 10);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_int_equal(cl->num_clusters, ref16_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_memory_equal(cl->cluster_label, ref16_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering(cl, 2, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	const scc_Clabel ref17_cluster_label[100] = { 0, 1, 2, 1, 1, 2, 3, 3, 4, 2, 0, 3, NA, NA, NA, 1, NA, 0, 5, NA, 2, 1, 0, 4, 5, 4, 0, 4, 0, 5, 1, 4, 2, NA, 4, NA, NA, 4, 3, 3, 0, 5, 1, 0, 0, 5, 3, 1, 3, 0, 1, 4, 2, 0, 3, NA, NA, NA, 2, 3, 3, 5, 0, 4, 0, 4, 4, 2, 1, 2, NA, NA, NA, 1, NA, 3, 3, NA, 1, 5, 2, 5, 5, 4, 0, 5, 3, 5, 0, 0, 4, 0, 1, NA, 5, NA, 1, NA, 1, 5 };
	const size_t ref17_num_clusters = 6;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 10,
	                                SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                                len_primary_data_points, primary_data_points, 1);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_int_equal(cl->num_clusters, ref17_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_memory_equal(cl->cluster_label, ref17_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering(cl, 10, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);

/*
	DOES NOT WORK WITH ANN, CHECK WHY!

	const scc_Clabel ref18_cluster_label[100] = { 0, 1, 2, 3, 3, NA, 4, 5, 6, NA, 0, 4, NA, NA, NA, NA, NA, 13, NA, NA, NA, 7, 8, NA, NA, 9, 10, 9, 10, 11, 3, 9, NA, NA, NA, NA, NA, NA, NA, NA, NA, 12, NA, 13, NA, 11, 5, 8, 14, NA, NA, NA, NA, NA, 14, NA, NA, 15, 2, 4, 14, 15, 0, 6, NA, NA, NA, NA, NA, 2, 7, NA, 12, 1, NA, NA, 5, NA, NA, 15, NA, 16, 8, NA, 12, 7, NA, 16, 10, 13, 6, NA, NA, NA, 16, NA, 1, NA, NA, 11 };
	const size_t ref18_num_clusters = 17;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 3,
	                                SCC_UM_IGNORE, true, 20.0,
	                                len_primary_data_points, primary_data_points, 0);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_int_equal(cl->num_clusters, ref18_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_memory_equal(cl->cluster_label, ref18_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_init_empty_clustering(100, external_cluster_labels_compare, &cl_compare);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_IGNORE, true, 20.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_make_clustering(&scc_ut_test_data_large_struct, cl_compare, &options);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->num_clusters, cl_compare->num_clusters);
	assert_memory_equal(cl->cluster_label, cl_compare->cluster_label, 100 * sizeof(scc_Clabel));
	scc_free_clustering(&cl_compare);
	scc_free_clustering(&cl);
*/

	const scc_Clabel ref19_cluster_label[100] = { 0, 1, 2, 3, 3, 4, 5, 6, 7, NA, 0, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, 8, 9, 10, NA, 11, 12, 11, 12, 13, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, 14, 15, 16, 17, 18, NA, 6, 9, 19, NA, 16, NA, NA, NA, NA, NA, NA, NA, 4, 5, 19, 20, NA, 7, 14, NA, 21, NA, NA, 2, NA, 21, 15, NA, NA, 23, NA, 24, NA, 20, NA, 22, NA, 10, NA, 8, 23, NA, 24, 17, NA, 18, NA, NA, 22, NA, 1, NA, NA, 13 };
	const size_t ref19_num_clusters = 25;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 2,
	                                SCC_UM_IGNORE, true, 20.0,
	                                len_primary_data_points, primary_data_points, 10);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_int_equal(cl->num_clusters, ref19_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_memory_equal(cl->cluster_label, ref19_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering(cl, 2, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_init_empty_clustering(100, external_cluster_labels_compare, &cl_compare);
	options = iscc_translate_options(2,
	                        0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_IGNORE, true, 20.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_make_clustering(&scc_ut_test_data_large_struct, cl_compare, &options);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->num_clusters, cl_compare->num_clusters);
	assert_memory_equal(cl->cluster_label, cl_compare->cluster_label, 100 * sizeof(scc_Clabel));
	scc_free_clustering(&cl_compare);
	scc_free_clustering(&cl);


	const scc_Clabel ref20_cluster_label[100] = { NA, NA, 1, 0, 0, 1, 2, 2, NA, 1, NA, 2, NA, NA, 3, 0, NA, 3, NA, NA, 1, NA, NA, NA, NA, NA, 3, NA, 3, NA, 0, NA, 1, NA, NA, NA, NA, NA, 2, 2, NA, NA, 0, NA, 3, NA, 2, 0, NA, 3, 0, NA, 1, NA, 2, NA, 1, NA, 1, 2, NA, 0, NA, NA, NA, NA, NA, NA, 0, 1, NA, NA, NA, NA, 3, 2, 2, 3, NA, NA, 1, NA, NA, NA, NA, NA, NA, NA, 3, NA, NA, 3, NA, NA, NA, NA, NA, NA, 0, NA };
	const size_t ref20_num_clusters = 4;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 10,
	                                SCC_UM_IGNORE, true, 30.0,
	                                len_primary_data_points, primary_data_points, 1);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_int_equal(cl->num_clusters, ref20_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_memory_equal(cl->cluster_label, ref20_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering(cl, 10, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_init_empty_clustering(100, external_cluster_labels_compare, &cl_compare);
	options = iscc_translate_options(10,
	                        0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_IGNORE, true, 30.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_make_clustering(&scc_ut_test_data_large_struct, cl_compare, &options);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->num_clusters, cl_compare->num_clusters);
	assert_memory_equal(cl->cluster_label, cl_compare->cluster_label, 100 * sizeof(scc_Clabel));
	scc_free_clustering(&cl_compare);
	scc_free_clustering(&cl);

/*
	DOES NOT WORK WITH ANN, CHECK WHY!

	const scc_Clabel ref21_cluster_label[100] = { 0, 1, 2, 3, 3, 2, 4, 5, 6, NA, 0, 4, NA, NA, NA, NA, NA, 13, NA, NA, 2, 7, 8, NA, NA, 9, 10, 9, 10, 11, 3, 9, NA, NA, NA, NA, NA, NA, NA, NA, 8, 12, 3, 13, 13, 11, 5, 8, 14, NA, NA, NA, NA, NA, 14, NA, NA, 15, 2, 4, 14, 15, 0, 6, 0, NA, NA, NA, 3, 2, 7, NA, 12, 1, NA, NA, 5, NA, NA, 15, NA, 16, 8, NA, 12, 7, 5, 16, 10, 13, 6, NA, NA, NA, 16, NA, 1, NA, NA, 11 };
	const size_t ref21_num_clusters = 17;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 3,
	                                SCC_UM_ANY_NEIGHBOR, true, 20.0,
	                                len_primary_data_points, primary_data_points, 0);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_int_equal(cl->num_clusters, ref21_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_memory_equal(cl->cluster_label, ref21_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);
*/

	const scc_Clabel ref22_cluster_label[100] = { 0, 1, 2, 3, 3, 4, 5, 6, 7, NA, 0, NA, NA, NA, NA, NA, NA, NA, NA, NA, 4, 8, 9, 10, 6, 11, 12, 11, 12, 13, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, 14, 15, 16, 17, 18, 13, 6, 9, 19, 12, 16, NA, NA, NA, NA, NA, NA, NA, 4, 5, 19, 20, 14, 7, 14, 7, 21, NA, 3, 2, NA, 21, 15, NA, NA, 23, NA, 24, NA, 20, 2, 22, 9, 10, 15, 8, 23, 22, 24, 17, NA, 18, NA, NA, 22, NA, 1, NA, NA, 13 };
	const size_t ref22_num_clusters = 25;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 2,
	                                SCC_UM_ANY_NEIGHBOR, true, 20.0,
	                                len_primary_data_points, primary_data_points, 10);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_int_equal(cl->num_clusters, ref22_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_memory_equal(cl->cluster_label, ref22_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering(cl, 2, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	const scc_Clabel ref23_cluster_label[100] = { NA, NA, 1, 0, 0, 1, 2, 2, NA, 1, NA, 2, NA, NA, 3, 0, NA, 3, NA, NA, 1, 0, 0, NA, NA, 2, 3, NA, 3, NA, 0, NA, 1, NA, NA, NA, NA, NA, 2, 2, 3, NA, 0, NA, 3, NA, 2, 0, 2, 3, 0, NA, 1, NA, 2, NA, 1, NA, 1, 2, 2, 0, NA, NA, 3, NA, NA, NA, 0, 1, NA, NA, NA, NA, 3, 2, 2, 3, NA, NA, 1, NA, 0, NA, NA, 0, 2, NA, 3, NA, NA, 3, NA, NA, NA, NA, NA, NA, 0, NA };
	const size_t ref23_num_clusters = 4;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 10,
	                                SCC_UM_ANY_NEIGHBOR, true, 30.0,
	                                len_primary_data_points, primary_data_points, 1);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_int_equal(cl->num_clusters, ref23_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_memory_equal(cl->cluster_label, ref23_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering(cl, 10, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);
}


void scc_ut_nng_clustering_batches_nonval_internal(void** state)
{
	(void) state;

	const size_t len_primary_data_points = 50;
	const scc_PointIndex primary_data_points[50] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 40,
	                                        41, 42, 43, 44, 45, 46, 47, 48, 49, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
	                                        80, 81, 82, 83, 84, 85, 86, 87, 88, 89 };

	iscc_run_nonval_tests_batches(SCC_UM_IGNORE, false, 0.0, 0, NULL);
	iscc_run_nonval_tests_batches(SCC_UM_ANY_NEIGHBOR, false, 0.0, 0, NULL);
	iscc_run_nonval_tests_batches(SCC_UM_IGNORE, true, 40.0, 0, NULL);
	iscc_run_nonval_tests_batches(SCC_UM_ANY_NEIGHBOR, true, 40.0, 0, NULL);

	iscc_run_nonval_tests_batches(SCC_UM_IGNORE, false, 0.0, len_primary_data_points, primary_data_points);
	iscc_run_nonval_tests_batches(SCC_UM_ANY_NEIGHBOR, false, 0.0, len_primary_data_points, primary_data_points);
	iscc_run_nonval_tests_batches(SCC_UM_IGNORE, true, 40.0, len_primary_data_points, primary_data_points);
	iscc_run_nonval_tests_batches(SCC_UM_ANY_NEIGHBOR, true, 40.0, len_primary_data_points, primary_data_points);
}


int main(void)
{
	if(!scc_ut_init_tests()) return 1;

	const struct CMUnitTest test_cases[] = {
		cmocka_unit_test(scc_ut_nng_clustering_batches_internal),
		cmocka_unit_test(scc_ut_nng_clustering_batches_nonval_internal),
	};

	return cmocka_run_group_tests_name("nng_clustering_batches_internal.c", test_cases, NULL, NULL);
}


void iscc_run_nonval_tests_batches(const scc_UnassignedMethod unassigned_method,
                                   const bool radius_constraint,
                                   const double radius,
                                   const size_t len_primary_data_points,
                                   const scc_PointIndex primary_data_points[const])
{
	bool cl_is_OK;
	scc_Clustering* cl;
	scc_ErrorCode ec;
	scc_Clabel external_cluster_labels[100];


	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 3,
	                                unassigned_method, radius_constraint, radius,
	                                len_primary_data_points, primary_data_points, 10);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_true(cl->external_labels);
	ec = scc_check_clustering(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 3,
	                                unassigned_method, radius_constraint, radius,
	                                len_primary_data_points, primary_data_points, 0);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_true(cl->external_labels);
	ec = scc_check_clustering(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 3,
	                                unassigned_method, radius_constraint, radius,
	                                len_primary_data_points, primary_data_points, 1);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_true(cl->external_labels);
	ec = scc_check_clustering(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 2,
	                                unassigned_method, radius_constraint, radius,
	                                len_primary_data_points, primary_data_points, 10);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_true(cl->external_labels);
	ec = scc_check_clustering(cl, 2, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 2,
	                                unassigned_method, radius_constraint, radius,
	                                len_primary_data_points, primary_data_points, 0);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_true(cl->external_labels);
	ec = scc_check_clustering(cl, 2, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 2,
	                                unassigned_method, radius_constraint, radius,
	                                len_primary_data_points, primary_data_points, 1);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_true(cl->external_labels);
	ec = scc_check_clustering(cl, 2, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 10,
	                                unassigned_method, radius_constraint, radius,
	                                len_primary_data_points, primary_data_points, 10);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_true(cl->external_labels);
	ec = scc_check_clustering(cl, 10, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 10,
	                                unassigned_method, radius_constraint, radius,
	                                len_primary_data_points, primary_data_points, 0);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_true(cl->external_labels);
	ec = scc_check_clustering(cl, 10, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	ec = scc_nng_clustering_batches(cl, &scc_ut_test_data_large_struct, 10,
	                                unassigned_method, radius_constraint, radius,
	                                len_primary_data_points, primary_data_points, 1);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_true(cl->external_labels);
	ec = scc_check_clustering(cl, 10, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);
}
