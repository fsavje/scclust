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
#include <src/scclust_types.h>
#include "data_object_test.h"


static const int32_t ISCC_UT_OPTIONS_STRUCT_VERSION = 722678001;


void iscc_run_nonval_tests(scc_SeedMethod seed_method,
                           int unassigned_method,
                           bool radius_constraint,
                           double radius,
                           size_t len_primary_data_points,
                           const scc_PointIndex primary_data_points[],
                           int secondary_unassigned_method,
                           bool secondary_radius_constraint,
                           double secondary_radius);

void iscc_run_type_nonval_tests(scc_SeedMethod seed_method,
                                int unassigned_method,
                                bool radius_constraint,
                                double radius,
                                size_t len_primary_data_points,
                                const scc_PointIndex primary_data_points[],
                                int secondary_unassigned_method,
                                bool secondary_radius_constraint,
                                double secondary_radius);

static scc_ClusterOptions iscc_translate_options(const uint32_t size_constraint,
                                                 const uint32_t num_types,
                                                 const uint32_t* const type_constraints,
                                                 const size_t len_type_labels,
                                                 const scc_TypeLabel* const type_labels,
                                                 const scc_SeedMethod seed_method,
                                                 int unassigned_method,
                                                 const bool radius_constraint,
                                                 const double radius,
                                                 const size_t len_primary_data_points,
                                                 const scc_PointIndex* const primary_data_points,
                                                 int secondary_unassigned_method,
                                                 const bool secondary_radius_constraint,
                                                 const double secondary_radius,
                                                 const  uint32_t batch_size)
{
	scc_RadiusMethod primary_radius = SCC_RM_USE_SEED_RADIUS;
	if (unassigned_method == SCC_UM_CLOSEST_SEED + 100) {
		primary_radius = SCC_RM_USE_ESTIMATED;
		unassigned_method = SCC_UM_CLOSEST_SEED;
	}
	scc_RadiusMethod __secondary_radius_constraint = secondary_radius_constraint;
	if (secondary_unassigned_method == SCC_UM_CLOSEST_SEED + 100) {
		__secondary_radius_constraint = SCC_RM_USE_ESTIMATED;
		secondary_unassigned_method = SCC_UM_CLOSEST_SEED;
	}

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
		.primary_unassigned_method = (scc_UnassignedMethod) unassigned_method,
		.secondary_unassigned_method = (scc_UnassignedMethod) secondary_unassigned_method,
		.seed_radius = radius_constraint,
		.seed_supplied_radius = radius,
		.primary_radius = primary_radius,
		.primary_supplied_radius = 0.0,
		.secondary_radius = __secondary_radius_constraint,
		.secondary_supplied_radius = secondary_radius,
		.batch_size = batch_size,
	};
}


static scc_ErrorCode scc_check_clustering_wrap(const scc_Clustering* const clustering,
                                               const uint32_t size_constraint,
                                               const uint32_t num_types,
                                               const uint32_t type_constraints[const],
                                               const size_t len_type_labels,
                                               const scc_TypeLabel type_labels[const],
                                               bool* const out_is_OK)
{
	scc_ClusterOptions options = scc_default_options();
	options.size_constraint = size_constraint;
	options.num_types = num_types;
	options.type_constraints = type_constraints;
	options.len_type_labels = len_type_labels;
	options.type_labels = type_labels;

	return scc_check_clustering(clustering, &options, out_is_OK);
}


void scc_ut_nng_clustering(void** state)
{
	(void) state;

	bool cl_is_OK;
	scc_Clustering* cl;
	scc_ClusterOptions options;
	scc_ErrorCode ec;
	//const scc_Clabel M = SCC_CLABEL_NA;
	scc_Clabel external_cluster_labels[100];
	const size_t len_primary_data_points = 50;
	const scc_PointIndex primary_data_points[50] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 40,
	                                        41, 42, 43, 44, 45, 46, 47, 48, 49, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
	                                        80, 81, 82, 83, 84, 85, 86, 87, 88, 89 };

	const scc_PointIndex err1_primary_data_points[50] = { 0, 1, 2, 3, 4, 5, 7, 7, 8, 9, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 40,
	                                        41, 42, 43, 44, 45, 46, 47, 48, 49, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
	                                        80, 81, 82, 83, 84, 85, 86, 87, 88, 89 };
	const scc_PointIndex err2_primary_data_points[50] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 21, 20, 22, 23, 24, 25, 26, 27, 28, 29, 40,
	                                        41, 42, 43, 44, 45, 46, 47, 48, 49, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
	                                        80, 81, 82, 83, 84, 85, 86, 87, 88, 89 };

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	scc_free_clustering(&cl);

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           1, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	scc_free_clustering(&cl);

	options = iscc_translate_options(3,
                           0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_small_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);
	scc_free_clustering(&cl);

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(1,
                           0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);
	scc_free_clustering(&cl);

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(101,
                           0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_NO_SOLUTION);
	scc_free_clustering(&cl);

	uint32_t test_type_constraints[5];
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           0, test_type_constraints, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);
	scc_free_clustering(&cl);

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 10, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);
	scc_free_clustering(&cl);

	scc_TypeLabel test_type_labels[5];
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 0, test_type_labels,
	                        SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);
	scc_free_clustering(&cl);

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 0, NULL,
	                        100, SCC_UM_IGNORE, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);
	scc_free_clustering(&cl);

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, 100, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);
	scc_free_clustering(&cl);

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_IGNORE, true, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);
	scc_free_clustering(&cl);

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	scc_free_clustering(&cl);

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
	                        len_primary_data_points, err1_primary_data_points, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);
	scc_free_clustering(&cl);

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
	                        len_primary_data_points, err2_primary_data_points, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);
	scc_free_clustering(&cl);

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
	                        len_primary_data_points, primary_data_points, 100, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);
	scc_free_clustering(&cl);

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_ANY_NEIGHBOR, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);
	scc_free_clustering(&cl);

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);
	scc_free_clustering(&cl);

	scc_init_existing_clustering(100, 10, external_cluster_labels, false, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_NOT_IMPLEMENTED);
	scc_free_clustering(&cl);

	scc_init_empty_clustering(100, NULL, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_IGNORE, true, 1,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_NO_SOLUTION);
	scc_free_clustering(&cl);



	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref1_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref1_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(2,
                           0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref1_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref1_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 2, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref2_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref2_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(10,
                           0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref2_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref2_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 10, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref3_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref3_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_ORDER, SCC_UM_IGNORE, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref3_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref3_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref4_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref4_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_UPDATING, SCC_UM_IGNORE, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref4_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref4_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref4_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref4_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_IGNORE, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref4_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref4_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref5_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref5_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 0, NULL,
	                        SCC_SM_EXCLUSION_ORDER, SCC_UM_IGNORE, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref5_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref5_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref6_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref6_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 0, NULL,
	                        SCC_SM_EXCLUSION_UPDATING, SCC_UM_IGNORE, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref6_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref6_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref7_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref7_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref7_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref7_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref8_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref8_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_ORDER, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref8_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref8_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref9_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref9_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref9_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref9_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref9_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref9_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref9_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref9_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);

	//const scc_Clabel ref10_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref10_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 0, NULL,
	                        SCC_SM_EXCLUSION_ORDER, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref10_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref10_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref11_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref11_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 0, NULL,
	                        SCC_SM_EXCLUSION_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref11_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref11_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref12_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref12_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref12_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref12_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref13_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref13_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref13_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref13_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref14_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref14_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref14_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref14_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref14_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref14_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref14_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref14_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref15_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref15_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 0, NULL,
	                        SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref15_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref15_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref16_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref16_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 0, NULL,
	                        SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref16_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref16_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref17_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref17_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref17_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref17_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref18_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref18_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref18_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref18_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref19_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref19_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref19_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref19_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref19_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref19_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref19_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref19_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref20_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref20_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 0, NULL,
	                        SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref20_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref20_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref21_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref21_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 0, NULL,
	                        SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref21_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref21_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref22_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref22_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref22_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref22_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref23_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref23_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref23_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref23_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref24_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref24_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                           0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref24_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref24_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref24_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref24_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref24_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref24_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref25_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref25_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref25_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref25_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref26_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref26_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref26_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref26_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refB0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refB0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_IGNORE, true, 20.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refB0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refB0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refB1_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refB1_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(2,
	                        0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_IGNORE, true, 20.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refB1_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refB1_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 2, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refB2_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refB2_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(10,
	                        0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_IGNORE, true, 40.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refB2_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refB2_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 10, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refB3_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refB3_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_ORDER, SCC_UM_IGNORE, true, 20.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refB3_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refB3_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refB4_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refB4_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_UPDATING, SCC_UM_IGNORE, true, 20.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refB4_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refB4_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refB4_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refB4_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_IGNORE, true, 20.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refB4_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refB4_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refB5_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refB5_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_EXCLUSION_ORDER, SCC_UM_IGNORE, true, 20.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refB5_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refB5_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refB6_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refB6_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_EXCLUSION_UPDATING, SCC_UM_IGNORE, true, 20.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refB6_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refB6_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refB7_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refB7_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_ANY_NEIGHBOR, true, 20.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refB7_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refB7_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refB8_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refB8_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_ORDER, SCC_UM_ANY_NEIGHBOR, true, 20.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refB8_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refB8_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refB9_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refB9_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 20.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refB9_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refB9_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refB9_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refB9_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 20.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refB9_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refB9_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refB10_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refB10_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_EXCLUSION_ORDER, SCC_UM_ANY_NEIGHBOR, true, 20.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refB10_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refB10_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refB11_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refB11_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_EXCLUSION_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 20.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refB11_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refB11_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refB12_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refB12_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_CLOSEST_ASSIGNED, true, 20.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refB12_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refB12_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refB13_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refB13_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 20.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refB13_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refB13_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refB14_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refB14_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 20.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refB14_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refB14_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refB14_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refB14_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 20.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refB14_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refB14_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refB15_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refB15_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 20.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refB15_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refB15_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refB16_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refB16_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 20.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refB16_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refB16_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refB17_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refB17_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, true, 20.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refB17_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refB17_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refB18_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refB18_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED, true, 20.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refB18_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refB18_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refB19_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refB19_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED, true, 20.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refB19_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refB19_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refB19_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refB19_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED, true, 20.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refB19_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refB19_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refB20_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refB20_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED, true, 20.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refB20_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refB20_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refB21_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refB21_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, true, 20.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refB21_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refB21_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refB22_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refB22_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED + 100, true, 20.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refB22_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refB22_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refB23_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refB23_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 20.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refB23_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refB23_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refB24_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refB24_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 20.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refB24_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refB24_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refB24_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refB24_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 20.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refB24_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refB24_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refB25_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refB25_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 20.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refB25_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refB25_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refB26_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refB26_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 20.0,
	                        0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refB26_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refB26_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refC0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refC0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refC0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refC0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refC1_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refC1_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(2,
	                        0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refC1_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refC1_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 2, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refC2_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refC2_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(10,
	                        0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refC2_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refC2_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 10, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refC3_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refC3_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_ORDER, SCC_UM_IGNORE, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refC3_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refC3_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refC4_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refC4_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_UPDATING, SCC_UM_IGNORE, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refC4_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refC4_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refC4_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refC4_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_IGNORE, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refC4_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refC4_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refC5_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refC5_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_EXCLUSION_ORDER, SCC_UM_IGNORE, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refC5_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refC5_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refC6_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refC6_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_EXCLUSION_UPDATING, SCC_UM_IGNORE, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refC6_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refC6_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refC7_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refC7_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refC7_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refC7_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refC8_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refC8_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_ORDER, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refC8_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refC8_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refC9_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refC9_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refC9_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refC9_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refC9_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refC9_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refC9_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refC9_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refC10_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refC10_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_EXCLUSION_ORDER, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refC10_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refC10_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refC11_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refC11_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_EXCLUSION_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refC11_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refC11_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refC12_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refC12_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refC12_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refC12_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refC13_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refC13_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refC13_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refC13_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refC14_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refC14_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refC14_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refC14_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refC14_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refC14_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refC14_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refC14_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refC15_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refC15_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refC15_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refC15_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refC16_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refC16_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refC16_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refC16_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refC17_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refC17_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refC17_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refC17_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refC18_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refC18_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refC18_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refC18_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refC19_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refC19_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refC19_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refC19_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refC19_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refC19_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refC19_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refC19_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refC20_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refC20_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refC20_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refC20_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refC21_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refC21_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refC21_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refC21_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refC22_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refC22_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refC22_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refC22_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refC23_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refC23_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refC23_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refC23_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refC24_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refC24_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refC24_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refC24_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refC24_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refC24_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refC24_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refC24_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refC25_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refC25_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refC25_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refC25_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refC26_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refC26_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refC26_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refC26_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refD1_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refD1_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refD1_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refD1_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refD2_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refD2_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refD2_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refD2_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refD3_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refD3_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refD3_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refD3_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refD4_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refD4_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refD4_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refD4_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refE1_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refE1_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refE1_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refE1_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refE2_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refE2_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refE2_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refE2_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refE3_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refE3_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refE3_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refE3_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel refE4_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t refE4_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
	                        SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, false, 0.0,
	                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, refE4_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, refE4_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);
}


void scc_ut_nng_clustering_nonval(void** state)
{
	(void) state;

	const size_t len_primary_data_points = 50;
	const scc_PointIndex primary_data_points[50] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 40,
	                                        41, 42, 43, 44, 45, 46, 47, 48, 49, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
	                                        80, 81, 82, 83, 84, 85, 86, 87, 88, 89 };

	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_IGNORE, false, 0.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_IGNORE, false, 0.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_IGNORE, false, 0.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_IGNORE, false, 0.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_IGNORE, false, 0.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_IGNORE, true, 40.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_IGNORE, true, 40.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_IGNORE, true, 40.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_IGNORE, true, 40.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_IGNORE, true, 40.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_IGNORE, true, 40.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      0, NULL, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_IGNORE, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_IGNORE, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_INWARDS_ALT_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
	iscc_run_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
	                      len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
}


void scc_ut_nng_clustering_with_types(void** state)
{
	(void) state;

	bool cl_is_OK;
	scc_Clustering* cl;
	scc_ClusterOptions options;
	scc_ErrorCode ec;
	//const scc_Clabel M = SCC_CLABEL_NA;
	scc_Clabel external_cluster_labels[100];
	const size_t len_primary_data_points = 50;
	const scc_PointIndex primary_data_points[50] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 40,
	                                        41, 42, 43, 44, 45, 46, 47, 48, 49, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
	                                        80, 81, 82, 83, 84, 85, 86, 87, 88, 89 };
	const uint32_t type_constraints_one[3] = { 0, 1, 0 };
	const uint32_t type_constraints_three[3] = { 1, 1, 1 };
	const uint32_t type_constraints_four[3] = { 1, 0, 3 };
	const scc_TypeLabel type_labels_three[100] = { 1, 0, 2, 0, 0, 2, 0, 0, 2, 0,
	                                               2, 1, 1, 2, 2, 2, 2, 0, 0, 2,
	                                               0, 0, 2, 0, 0, 0, 2, 0, 2, 1,
	                                               2, 1, 2, 1, 1, 1, 1, 0, 1, 2,
	                                               1, 2, 0, 1, 1, 1, 0, 0, 2, 0,
	                                               0, 2, 1, 2, 1, 2, 2, 2, 1, 1,
	                                               0, 0, 1, 2, 1, 2, 1, 0, 0, 1,
	                                               2, 2, 1, 0, 2, 2, 1, 1, 2, 1,
	                                               1, 0, 2, 0, 0, 1, 1, 0, 1, 1,
	                                               1, 2, 2, 0, 2, 0, 2, 0, 0, 1 };

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                                       3, type_constraints_three, 100, type_labels_three,
                                       SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
                                       0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	scc_free_clustering(&cl);

	options = iscc_translate_options(3,
                                       3, type_constraints_three, 100, type_labels_three,
                                       SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
                                       0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                                       3, type_constraints_three, 100, type_labels_three,
                                       SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
                                       0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_small_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);
	scc_free_clustering(&cl);

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(1,
                                       3, type_constraints_three, 100, type_labels_three,
                                       SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
                                       0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);
	scc_free_clustering(&cl);

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(101,
                                       3, type_constraints_three, 100, type_labels_three,
                                       SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
                                       0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_NO_SOLUTION);
	scc_free_clustering(&cl);

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                                       4294967295, type_constraints_three, 100, type_labels_three,
                                       SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
                                       0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_TOO_LARGE_PROBLEM);
	scc_free_clustering(&cl);

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                                       3, NULL, 100, type_labels_three,
                                       SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
                                       0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);
	scc_free_clustering(&cl);

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                                       3, type_constraints_three, 50, type_labels_three,
                                       SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
                                       0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);
	scc_free_clustering(&cl);

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                                       3, type_constraints_three, 100, NULL,
                                       SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
                                       0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);
	scc_free_clustering(&cl);

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                                       3, type_constraints_three, 100, type_labels_three,
                                       100, SCC_UM_IGNORE, false, 0.0,
                                       0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);
	scc_free_clustering(&cl);

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                                       3, type_constraints_three, 100, type_labels_three,
                                       SCC_SM_LEXICAL, 100, false, 0.0,
                                       0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);
	scc_free_clustering(&cl);

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                                       3, type_constraints_three, 100, type_labels_three,
                                       SCC_SM_LEXICAL, SCC_UM_IGNORE, true, 0.0,
                                       0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);
	scc_free_clustering(&cl);

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                                       3, type_constraints_three, 100, type_labels_three,
                                       SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
                                       len_primary_data_points, primary_data_points, 100, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);
	scc_free_clustering(&cl);

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                                       3, type_constraints_three, 100, type_labels_three,
                                       SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
                                       len_primary_data_points, primary_data_points, SCC_UM_ANY_NEIGHBOR, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);
	scc_free_clustering(&cl);

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                                       3, type_constraints_three, 100, type_labels_three,
                                       SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
                                       len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);
	scc_free_clustering(&cl);

	scc_init_existing_clustering(100, 10, external_cluster_labels, false, &cl);
	options = iscc_translate_options(3,
                                       3, type_constraints_three, 100, type_labels_three,
                                       SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
                                       0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_NOT_IMPLEMENTED);
	scc_free_clustering(&cl);

	const uint32_t type_constraints_three_mod1[3] = { 35, 1, 1 };
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                                       3, type_constraints_three_mod1, 100, type_labels_three,
                                       SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
                                       0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_NO_SOLUTION);
	scc_free_clustering(&cl);

	const uint32_t type_constraints_three_mod2[3] = { 2, 2, 2 };
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                                       3, type_constraints_three_mod2, 100, type_labels_three,
                                       SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
                                       0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);
	scc_free_clustering(&cl);

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                                       3, type_constraints_three, 100, type_labels_three,
                                       SCC_SM_LEXICAL, SCC_UM_IGNORE, true, 10.0,
                                       0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	scc_free_clustering(&cl);

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                                       3, type_constraints_three, 100, type_labels_three,
                                       SCC_SM_LEXICAL, SCC_UM_IGNORE, true, 1.0,
                                       0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_NO_SOLUTION);
	scc_free_clustering(&cl);

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(10,
                                       3, type_constraints_three, 100, type_labels_three,
                                       SCC_SM_LEXICAL, SCC_UM_IGNORE, true, 10.0,
                                       0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_NO_SOLUTION);
	scc_free_clustering(&cl);

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(10,
                                       3, type_constraints_three, 100, type_labels_three,
                                       SCC_SM_LEXICAL, SCC_UM_IGNORE, true, 30.0,
                                       0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	scc_free_clustering(&cl);



	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                                       3, type_constraints_three, 100, type_labels_three,
                                       SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
                                       0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 3, type_constraints_three, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(5,
                                       3, type_constraints_three, 100, type_labels_three,
                                       SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
                                       0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 5, 3, type_constraints_three, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                                       3, type_constraints_one, 100, type_labels_three,
                                       SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
                                       0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 3, type_constraints_one, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(2,
                                       3, type_constraints_one, 100, type_labels_three,
                                       SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
                                       0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 2, 3, type_constraints_one, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(5,
                                       3, type_constraints_one, 100, type_labels_three,
                                       SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
                                       0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 5, 3, type_constraints_one, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(4,
                                       3, type_constraints_four, 100, type_labels_three,
                                       SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
                                       0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 4, 3, type_constraints_four, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(8,
                                       3, type_constraints_four, 100, type_labels_three,
                                       SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
                                       0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 8, 3, type_constraints_four, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                                   3, type_constraints_three, 100, type_labels_three,
	                                   SCC_SM_LEXICAL, SCC_UM_IGNORE, true, 20.0,
	                                   0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 3, type_constraints_three, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(5,
	                                   3, type_constraints_three, 100, type_labels_three,
	                                   SCC_SM_LEXICAL, SCC_UM_IGNORE, true, 20.0,
	                                   0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 5, 3, type_constraints_three, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                                   3, type_constraints_one, 100, type_labels_three,
	                                   SCC_SM_LEXICAL, SCC_UM_IGNORE, true, 20.0,
	                                   0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 3, type_constraints_one, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(2,
	                                   3, type_constraints_one, 100, type_labels_three,
	                                   SCC_SM_LEXICAL, SCC_UM_IGNORE, true, 20.0,
	                                   0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 2, 3, type_constraints_one, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(5,
	                                   3, type_constraints_one, 100, type_labels_three,
	                                   SCC_SM_LEXICAL, SCC_UM_IGNORE, true, 20.0,
	                                   0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 5, 3, type_constraints_one, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(4,
	                                   3, type_constraints_four, 100, type_labels_three,
	                                   SCC_SM_LEXICAL, SCC_UM_IGNORE, true, 20.0,
	                                   0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 4, 3, type_constraints_four, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(8,
	                                   3, type_constraints_four, 100, type_labels_three,
	                                   SCC_SM_LEXICAL, SCC_UM_IGNORE, true, 20.0,
	                                   0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 8, 3, type_constraints_four, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                                       3, type_constraints_three, 100, type_labels_three,
                                       SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                                       0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 3, type_constraints_three, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(5,
                                       3, type_constraints_three, 100, type_labels_three,
                                       SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                                       0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 5, 3, type_constraints_three, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                                       3, type_constraints_one, 100, type_labels_three,
                                       SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                                       0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 3, type_constraints_one, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(2,
                                       3, type_constraints_one, 100, type_labels_three,
                                       SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                                       0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 2, 3, type_constraints_one, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(5,
                                       3, type_constraints_one, 100, type_labels_three,
                                       SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                                       0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 5, 3, type_constraints_one, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(4,
                                       3, type_constraints_four, 100, type_labels_three,
                                       SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                                       0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 4, 3, type_constraints_four, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(8,
                                       3, type_constraints_four, 100, type_labels_three,
                                       SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                                       0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 8, 3, type_constraints_four, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                                   3, type_constraints_three, 100, type_labels_three,
	                                   SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 20.0,
	                                   0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 3, type_constraints_three, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(5,
	                                   3, type_constraints_three, 100, type_labels_three,
	                                   SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 20.0,
	                                   0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 5, 3, type_constraints_three, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                                   3, type_constraints_one, 100, type_labels_three,
	                                   SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 20.0,
	                                   0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 3, type_constraints_one, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(2,
	                                   3, type_constraints_one, 100, type_labels_three,
	                                   SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 20.0,
	                                   0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 2, 3, type_constraints_one, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(5,
	                                   3, type_constraints_one, 100, type_labels_three,
	                                   SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 20.0,
	                                   0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 5, 3, type_constraints_one, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(4,
	                                   3, type_constraints_four, 100, type_labels_three,
	                                   SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 20.0,
	                                   0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 4, 3, type_constraints_four, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(8,
	                                   3, type_constraints_four, 100, type_labels_three,
	                                   SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 20.0,
	                                   0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 8, 3, type_constraints_four, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                                       3, type_constraints_three, 100, type_labels_three,
                                       SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                                       len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 3, type_constraints_three, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(5,
                                       3, type_constraints_three, 100, type_labels_three,
                                       SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                                       len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 5, 3, type_constraints_three, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                                       3, type_constraints_one, 100, type_labels_three,
                                       SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                                       len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 3, type_constraints_one, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(2,
                                       3, type_constraints_one, 100, type_labels_three,
                                       SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                                       len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 2, 3, type_constraints_one, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(5,
                                       3, type_constraints_one, 100, type_labels_three,
                                       SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                                       len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 5, 3, type_constraints_one, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(4,
                                       3, type_constraints_four, 100, type_labels_three,
                                       SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                                       len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 4, 3, type_constraints_four, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(8,
                                       3, type_constraints_four, 100, type_labels_three,
                                       SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                                       len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 8, 3, type_constraints_four, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                                   3, type_constraints_three, 100, type_labels_three,
	                                   SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 20.0,
	                                   len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 3, type_constraints_three, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(5,
	                                   3, type_constraints_three, 100, type_labels_three,
	                                   SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 20.0,
	                                   len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 5, 3, type_constraints_three, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                                   3, type_constraints_one, 100, type_labels_three,
	                                   SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 20.0,
	                                   len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 3, type_constraints_one, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(2,
	                                   3, type_constraints_one, 100, type_labels_three,
	                                   SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 20.0,
	                                   len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 2, 3, type_constraints_one, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(5,
	                                   3, type_constraints_one, 100, type_labels_three,
	                                   SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 20.0,
	                                   len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 5, 3, type_constraints_one, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(4,
	                                   3, type_constraints_four, 100, type_labels_three,
	                                   SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 20.0,
	                                   len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 4, 3, type_constraints_four, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(8,
	                                   3, type_constraints_four, 100, type_labels_three,
	                                   SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 20.0,
	                                   len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 8, 3, type_constraints_four, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                                       3, type_constraints_three, 100, type_labels_three,
                                       SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
                                       len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 3, type_constraints_three, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(5,
                                       3, type_constraints_three, 100, type_labels_three,
                                       SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
                                       len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 5, 3, type_constraints_three, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                                       3, type_constraints_one, 100, type_labels_three,
                                       SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
                                       len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 3, type_constraints_one, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(2,
                                       3, type_constraints_one, 100, type_labels_three,
                                       SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
                                       len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 2, 3, type_constraints_one, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(5,
                                       3, type_constraints_one, 100, type_labels_three,
                                       SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
                                       len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 5, 3, type_constraints_one, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(4,
                                       3, type_constraints_four, 100, type_labels_three,
                                       SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
                                       len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 4, 3, type_constraints_four, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(8,
                                       3, type_constraints_four, 100, type_labels_three,
                                       SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
                                       len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 8, 3, type_constraints_four, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                                   3, type_constraints_three, 100, type_labels_three,
	                                   SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, true, 20.0,
	                                   len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 3, type_constraints_three, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(5,
	                                   3, type_constraints_three, 100, type_labels_three,
	                                   SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, true, 20.0,
	                                   len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 5, 3, type_constraints_three, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                                   3, type_constraints_one, 100, type_labels_three,
	                                   SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, true, 20.0,
	                                   len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 3, type_constraints_one, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(2,
	                                   3, type_constraints_one, 100, type_labels_three,
	                                   SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, true, 20.0,
	                                   len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 2, 3, type_constraints_one, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(5,
	                                   3, type_constraints_one, 100, type_labels_three,
	                                   SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, true, 20.0,
	                                   len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 5, 3, type_constraints_one, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(4,
	                                   3, type_constraints_four, 100, type_labels_three,
	                                   SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, true, 20.0,
	                                   len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 4, 3, type_constraints_four, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(8,
	                                   3, type_constraints_four, 100, type_labels_three,
	                                   SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, true, 20.0,
	                                   len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 8, 3, type_constraints_four, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                                       3, type_constraints_three, 100, type_labels_three,
                                       SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                                       len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 3, type_constraints_three, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(5,
                                       3, type_constraints_three, 100, type_labels_three,
                                       SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                                       len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 5, 3, type_constraints_three, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                                       3, type_constraints_one, 100, type_labels_three,
                                       SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                                       len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 3, type_constraints_one, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(2,
                                       3, type_constraints_one, 100, type_labels_three,
                                       SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                                       len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 2, 3, type_constraints_one, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(5,
                                       3, type_constraints_one, 100, type_labels_three,
                                       SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                                       len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 5, 3, type_constraints_one, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(4,
                                       3, type_constraints_four, 100, type_labels_three,
                                       SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                                       len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 4, 3, type_constraints_four, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(8,
                                       3, type_constraints_four, 100, type_labels_three,
                                       SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                                       len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 8, 3, type_constraints_four, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                                   3, type_constraints_three, 100, type_labels_three,
	                                   SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 20.0,
	                                   len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 3, type_constraints_three, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(5,
	                                   3, type_constraints_three, 100, type_labels_three,
	                                   SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 20.0,
	                                   len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 5, 3, type_constraints_three, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                                   3, type_constraints_one, 100, type_labels_three,
	                                   SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 20.0,
	                                   len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 3, type_constraints_one, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(2,
	                                   3, type_constraints_one, 100, type_labels_three,
	                                   SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 20.0,
	                                   len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 2, 3, type_constraints_one, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(5,
	                                   3, type_constraints_one, 100, type_labels_three,
	                                   SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 20.0,
	                                   len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 5, 3, type_constraints_one, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(4,
	                                   3, type_constraints_four, 100, type_labels_three,
	                                   SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 20.0,
	                                   len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 4, 3, type_constraints_four, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(8,
	                                   3, type_constraints_four, 100, type_labels_three,
	                                   SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 20.0,
	                                   len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 8, 3, type_constraints_four, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                                       3, type_constraints_three, 100, type_labels_three,
                                       SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                                       len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 3, type_constraints_three, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(5,
                                       3, type_constraints_three, 100, type_labels_three,
                                       SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                                       len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 5, 3, type_constraints_three, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                                       3, type_constraints_one, 100, type_labels_three,
                                       SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                                       len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 3, type_constraints_one, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(2,
                                       3, type_constraints_one, 100, type_labels_three,
                                       SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                                       len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 2, 3, type_constraints_one, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(5,
                                       3, type_constraints_one, 100, type_labels_three,
                                       SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                                       len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 5, 3, type_constraints_one, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(4,
                                       3, type_constraints_four, 100, type_labels_three,
                                       SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                                       len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 4, 3, type_constraints_four, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(8,
                                       3, type_constraints_four, 100, type_labels_three,
                                       SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                                       len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 8, 3, type_constraints_four, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                                   3, type_constraints_three, 100, type_labels_three,
	                                   SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 20.0,
	                                   len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 3, type_constraints_three, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(5,
	                                   3, type_constraints_three, 100, type_labels_three,
	                                   SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 20.0,
	                                   len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 5, 3, type_constraints_three, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                                   3, type_constraints_one, 100, type_labels_three,
	                                   SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 20.0,
	                                   len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 3, type_constraints_one, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(2,
	                                   3, type_constraints_one, 100, type_labels_three,
	                                   SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 20.0,
	                                   len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 2, 3, type_constraints_one, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(5,
	                                   3, type_constraints_one, 100, type_labels_three,
	                                   SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 20.0,
	                                   len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 5, 3, type_constraints_one, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(4,
	                                   3, type_constraints_four, 100, type_labels_three,
	                                   SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 20.0,
	                                   len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 4, 3, type_constraints_four, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	//const scc_Clabel ref0_cluster_label[100] = { 0, 0, 1, 1, 2,   M, 0, M, 2, M,   1, 1, 2, 1, 0 };
	//const size_t ref0_num_clusters = 3;
	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(8,
	                                   3, type_constraints_four, 100, type_labels_three,
	                                   SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 20.0,
	                                   len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	//assert_int_equal(cl->num_clusters, ref0_num_clusters);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	//assert_memory_equal(cl->cluster_label, ref0_cluster_label, 100 * sizeof(scc_Clabel));
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 8, 3, type_constraints_four, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);
}


void scc_ut_nng_clustering_with_types_nonval(void** state)
{
  (void) state;

	const size_t len_primary_data_points = 50;
	const scc_PointIndex primary_data_points[50] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 40,
	                                        41, 42, 43, 44, 45, 46, 47, 48, 49, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
	                                        80, 81, 82, 83, 84, 85, 86, 87, 88, 89 };

  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_IGNORE, false, 0.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_IGNORE, false, 0.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_IGNORE, false, 0.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_IGNORE, false, 0.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, false, 0.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED, false, 0.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED, false, 0.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_IGNORE, true, 40.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_IGNORE, true, 40.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_IGNORE, true, 40.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_IGNORE, true, 40.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_IGNORE, true, 40.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, true, 40.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED, true, 40.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED, true, 40.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        0, NULL, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_IGNORE, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_IGNORE, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_IGNORE, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_IGNORE, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_IGNORE, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_IGNORE, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_IGNORE, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_IGNORE, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_IGNORE, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_IGNORE, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_IGNORE, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_IGNORE, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_IGNORE, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_IGNORE, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_IGNORE, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_IGNORE, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_IGNORE, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_IGNORE, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_IGNORE, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_IGNORE, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_IGNORE, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_IGNORE, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_IGNORE, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_IGNORE, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_IGNORE, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_IGNORE, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_IGNORE, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_IGNORE, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_IGNORE, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_IGNORE, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_IGNORE, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_IGNORE, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_IGNORE, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_IGNORE, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_IGNORE, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_IGNORE, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_IGNORE, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_IGNORE, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_IGNORE, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_IGNORE, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_IGNORE, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_IGNORE, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_IGNORE, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_IGNORE, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_IGNORE, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_IGNORE, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_IGNORE, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_IGNORE, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_IGNORE, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_IGNORE, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_IGNORE, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_IGNORE, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_IGNORE, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_IGNORE, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_IGNORE, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_IGNORE, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_IGNORE, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_IGNORE, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_IGNORE, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_IGNORE, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_IGNORE, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_IGNORE, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_IGNORE, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_IGNORE, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, false, 0.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_IGNORE, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_IGNORE, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_IGNORE, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_IGNORE, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_IGNORE, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_ANY_NEIGHBOR, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED + 100, false, 0.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_IGNORE, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_IGNORE, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_IGNORE, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_IGNORE, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_IGNORE, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_ANY_NEIGHBOR, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_ASSIGNED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
  iscc_run_type_nonval_tests(SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED + 100, true, 40.0,
                        len_primary_data_points, primary_data_points, SCC_UM_CLOSEST_SEED + 100, true, 20.0);
}


int main(void)
{
	if(!scc_ut_init_tests()) return 1;

	const struct CMUnitTest test_cases[] = {
		cmocka_unit_test(scc_ut_nng_clustering),
		cmocka_unit_test(scc_ut_nng_clustering_nonval),
		cmocka_unit_test(scc_ut_nng_clustering_with_types),
		cmocka_unit_test(scc_ut_nng_clustering_with_types_nonval),
	};

	return cmocka_run_group_tests_name("nng_clustering.c", test_cases, NULL, NULL);
}


void iscc_run_nonval_tests(const scc_SeedMethod seed_method,
                           const int unassigned_method,
                           const bool radius_constraint,
                           const double radius,
                           const size_t len_primary_data_points,
                           const scc_PointIndex primary_data_points[const],
                           const int secondary_unassigned_method,
                           const bool secondary_radius_constraint,
                           const double secondary_radius)
{
	bool cl_is_OK;
	scc_Clustering* cl;
	scc_ClusterOptions options;
	scc_ErrorCode ec;
	scc_Clabel external_cluster_labels[100];


	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
	                        0, NULL, 0, NULL,
                            seed_method, unassigned_method, radius_constraint, radius,
                            len_primary_data_points, primary_data_points, secondary_unassigned_method, secondary_radius_constraint, secondary_radius, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(2,
	                        0, NULL, 0, NULL,
                            seed_method, unassigned_method, radius_constraint, radius,
                            len_primary_data_points, primary_data_points, secondary_unassigned_method, secondary_radius_constraint, secondary_radius, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 2, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(10,
	                        0, NULL, 0, NULL,
                            seed_method, unassigned_method, radius_constraint, radius,
                            len_primary_data_points, primary_data_points, secondary_unassigned_method, secondary_radius_constraint, secondary_radius, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 10, 0, NULL, 0, NULL, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);
}


void iscc_run_type_nonval_tests(const scc_SeedMethod seed_method,
                                const int unassigned_method,
                                const bool radius_constraint,
                                const double radius,
                                const size_t len_primary_data_points,
                                const scc_PointIndex primary_data_points[const],
                                const int secondary_unassigned_method,
                                const bool secondary_radius_constraint,
                                const double secondary_radius)
{
	bool cl_is_OK;
	scc_Clustering* cl;
	scc_ClusterOptions options;
	scc_ErrorCode ec;
	scc_Clabel external_cluster_labels[100];
	const uint32_t type_constraints_one[3] = { 0, 1, 0 };
	const uint32_t type_constraints_three[3] = { 1, 1, 1 };
	const uint32_t type_constraints_four[3] = { 1, 0, 3 };
	const scc_TypeLabel type_labels_three[100] = { 1, 0, 2, 0, 0, 2, 0, 0, 2, 0,
	                                               2, 1, 1, 2, 2, 2, 2, 0, 0, 2,
	                                               0, 0, 2, 0, 0, 0, 2, 0, 2, 1,
	                                               2, 1, 2, 1, 1, 1, 1, 0, 1, 2,
	                                               1, 2, 0, 1, 1, 1, 0, 0, 2, 0,
	                                               0, 2, 1, 2, 1, 2, 2, 2, 1, 1,
	                                               0, 0, 1, 2, 1, 2, 1, 0, 0, 1,
	                                               2, 2, 1, 0, 2, 2, 1, 1, 2, 1,
	                                               1, 0, 2, 0, 0, 1, 1, 0, 1, 1,
	                                               1, 2, 2, 0, 2, 0, 2, 0, 0, 1 };

	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                                       3, type_constraints_three, 100, type_labels_three,
                                       seed_method, unassigned_method, radius_constraint, radius,
                                       len_primary_data_points, primary_data_points, secondary_unassigned_method,
                                       secondary_radius_constraint, secondary_radius, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 3, type_constraints_three, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(5,
                                       3, type_constraints_three, 100, type_labels_three,
                                       seed_method, unassigned_method, radius_constraint, radius,
                                       len_primary_data_points, primary_data_points, secondary_unassigned_method,
                                       secondary_radius_constraint, secondary_radius, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 5, 3, type_constraints_three, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(3,
                                       3, type_constraints_one, 100, type_labels_three,
                                       seed_method, unassigned_method, radius_constraint, radius,
                                       len_primary_data_points, primary_data_points, secondary_unassigned_method,
                                       secondary_radius_constraint, secondary_radius, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 3, 3, type_constraints_one, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(2,
                                       3, type_constraints_one, 100, type_labels_three,
                                       seed_method, unassigned_method, radius_constraint, radius,
                                       len_primary_data_points, primary_data_points, secondary_unassigned_method,
                                       secondary_radius_constraint, secondary_radius, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 2, 3, type_constraints_one, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(5,
                                       3, type_constraints_one, 100, type_labels_three,
                                       seed_method, unassigned_method, radius_constraint, radius,
                                       len_primary_data_points, primary_data_points, secondary_unassigned_method,
                                       secondary_radius_constraint, secondary_radius, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 5, 3, type_constraints_one, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(4,
                                       3, type_constraints_four, 100, type_labels_three,
                                       seed_method, unassigned_method, radius_constraint, radius,
                                       len_primary_data_points, primary_data_points, secondary_unassigned_method,
                                       secondary_radius_constraint, secondary_radius, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 4, 3, type_constraints_four, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);


	scc_init_empty_clustering(100, external_cluster_labels, &cl);
	options = iscc_translate_options(8,
                                       3, type_constraints_four, 100, type_labels_three,
                                       seed_method, unassigned_method, radius_constraint, radius,
                                       len_primary_data_points, primary_data_points, secondary_unassigned_method,
                                       secondary_radius_constraint, secondary_radius, 0);
	ec = scc_sc_clustering(&scc_ut_test_data_large_struct, &options, cl);
	assert_int_equal(ec, SCC_ER_OK);
	assert_int_equal(cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl->num_data_points, 100);
	assert_non_null(cl->cluster_label);
	assert_ptr_equal(cl->cluster_label, external_cluster_labels);
	assert_true(cl->external_labels);
	ec = scc_check_clustering_wrap(cl, 8, 3, type_constraints_four, 100, type_labels_three, &cl_is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(cl_is_OK);
	scc_free_clustering(&cl);
}
