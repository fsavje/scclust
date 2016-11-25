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
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <include/scclust.h>
#include <src/clustering_struct.h>
#include <src/scclust_types.h>
#include "data_object_test.h"
#include "double_assert.h"


void scc_ut_get_compiled_version(void** state)
{
	(void) state;

	uint32_t major, minor, patch;

	major = minor = patch = 123456;
	scc_get_compiled_version(&major, &minor, &patch);
	assert_int_equal(major, SCC_SCCLUST_MAJOR_VERSION);
	assert_int_equal(minor, SCC_SCCLUST_MINOR_VERSION);
	assert_int_equal(patch, SCC_SCCLUST_PATCH_VERSION);

	major = minor = patch = 123456;
	scc_get_compiled_version(NULL, &minor, &patch);
	assert_int_equal(major, 123456);
	assert_int_equal(minor, SCC_SCCLUST_MINOR_VERSION);
	assert_int_equal(patch, SCC_SCCLUST_PATCH_VERSION);

	major = minor = patch = 123456;
	scc_get_compiled_version(&major, NULL, &patch);
	assert_int_equal(major, SCC_SCCLUST_MAJOR_VERSION);
	assert_int_equal(minor, 123456);
	assert_int_equal(patch, SCC_SCCLUST_PATCH_VERSION);

	major = minor = patch = 123456;
	scc_get_compiled_version(&major, &minor, NULL);
	assert_int_equal(major, SCC_SCCLUST_MAJOR_VERSION);
	assert_int_equal(minor, SCC_SCCLUST_MINOR_VERSION);
	assert_int_equal(patch, 123456);
}


void scc_ut_free_clustering(void** state)
{
	(void) state;

	scc_Clustering* null = NULL;
	scc_Clabel* const labels1 = malloc(sizeof(scc_Clabel[5]));
	scc_Clabel labels2[5] = { 1, 2, 3, 4, 5 };

	scc_Clustering* cl1 = malloc(sizeof(scc_Clustering));
	*cl1 = (scc_Clustering) {
		.num_data_points = 123,
		.num_clusters = 0,
		.cluster_label = NULL,
		.external_labels = false,
		.clustering_version = ISCC_CLUSTERING_STRUCT_VERSION,
	};

	scc_Clustering* cl2 = malloc(sizeof(scc_Clustering));
	*cl2 = (scc_Clustering) {
		.num_data_points = 123,
		.num_clusters = 3,
		.cluster_label = labels1,
		.external_labels = false,
		.clustering_version = ISCC_CLUSTERING_STRUCT_VERSION,
	};

	scc_Clustering* cl3 = malloc(sizeof(scc_Clustering));
	*cl3 = (scc_Clustering) {
		.num_data_points = 123,
		.num_clusters = 3,
		.cluster_label = labels2,
		.external_labels = true,
		.clustering_version = ISCC_CLUSTERING_STRUCT_VERSION,
	};

	scc_free_clustering(NULL);
	scc_free_clustering(&null);
	scc_free_clustering(&cl1);
	scc_free_clustering(&cl2);
	scc_free_clustering(&cl3);

	assert_null(null);
	assert_null(cl1);
	assert_null(cl2);
	assert_null(cl3);
}


void scc_ut_init_empty_clustering(void** state)
{
	(void) state;

	scc_Clabel external_cluster_labels[10];

	scc_ErrorCode ec1 = scc_init_empty_clustering(10, external_cluster_labels, NULL);
	assert_int_equal(ec1, SCC_ER_INVALID_INPUT);

	scc_Clustering* cl2;
	scc_ErrorCode ec2 = scc_init_empty_clustering(0, external_cluster_labels, &cl2);
	assert_null(cl2);
	assert_int_equal(ec2, SCC_ER_INVALID_INPUT);

	#if ISCC_M_POINTINDEX_MAX < UINTMAX_MAX
		scc_Clustering* cl4;
		scc_ErrorCode ec4 = scc_init_empty_clustering(((uintmax_t) ISCC_POINTINDEX_MAX) + 1, external_cluster_labels, &cl4);
		assert_null(cl4);
		assert_int_equal(ec4, SCC_ER_TOO_LARGE_PROBLEM);
	#endif

	scc_Clustering* cl6;
	scc_ErrorCode ec6 = scc_init_empty_clustering(10, external_cluster_labels, &cl6);
	assert_non_null(cl6);
	assert_int_equal(cl6->num_data_points, 10);
	assert_int_equal(cl6->num_clusters, 0);
	assert_non_null(cl6->cluster_label);
	assert_ptr_equal(cl6->cluster_label, external_cluster_labels);
	assert_true(cl6->external_labels);
	assert_int_equal(cl6->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(ec6, SCC_ER_OK);

	scc_Clustering* cl6a;
	scc_ErrorCode ec6a = scc_init_empty_clustering(5, external_cluster_labels, &cl6a);
	assert_non_null(cl6a);
	assert_int_equal(cl6a->num_data_points, 5);
	assert_int_equal(cl6a->num_clusters, 0);
	assert_non_null(cl6a->cluster_label);
	assert_ptr_equal(cl6a->cluster_label, external_cluster_labels);
	assert_true(cl6a->external_labels);
	assert_int_equal(cl6a->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(ec6a, SCC_ER_OK);

	scc_Clustering* cl7;
	scc_ErrorCode ec7 = scc_init_empty_clustering(1000, NULL, &cl7);
	assert_non_null(cl7);
	assert_int_equal(cl7->num_data_points, 1000);
	assert_int_equal(cl7->num_clusters, 0);
	assert_null(cl7->cluster_label);
	assert_false(cl7->external_labels);
	assert_int_equal(cl7->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(ec7, SCC_ER_OK);

	scc_free_clustering(&cl6);
	scc_free_clustering(&cl6a);
	scc_free_clustering(&cl7);
}


void scc_ut_init_existing_clustering(void** state)
{
	(void) state;

	scc_Clabel cluster_labels[10] = { 1, 4, 3, 0, 2, 2, 0, 1, 4, 4 };
	const scc_Clabel ref_cluster_labels[10] = { 1, 4, 3, 0, 2, 2, 0, 1, 4, 4 };

	scc_ErrorCode ec1 = scc_init_existing_clustering(10, 5, cluster_labels, true, NULL);
	assert_int_equal(ec1, SCC_ER_INVALID_INPUT);

	scc_Clustering* cl2;
	scc_ErrorCode ec2 = scc_init_existing_clustering(10, 0, cluster_labels, true, &cl2);
	assert_null(cl2);
	assert_int_equal(ec2, SCC_ER_INVALID_INPUT);

	scc_Clustering* cl3;
	scc_ErrorCode ec3 = scc_init_existing_clustering(0, 5, cluster_labels, true, &cl3);
	assert_null(cl3);
	assert_int_equal(ec3, SCC_ER_INVALID_INPUT);

	#if ISCC_M_CLABEL_MAX < UINTMAX_MAX
		scc_Clustering* cl4;
		scc_ErrorCode ec4 = scc_init_existing_clustering(10, ((uintmax_t) SCC_CLABEL_MAX) + 1, cluster_labels, true, &cl4);
		assert_null(cl4);
		assert_int_equal(ec4, SCC_ER_TOO_LARGE_PROBLEM);
	#endif

	#if ISCC_M_POINTINDEX_MAX < UINTMAX_MAX
		scc_Clustering* cl6;
		scc_ErrorCode ec6 = scc_init_existing_clustering(((uintmax_t) ISCC_POINTINDEX_MAX) + 1, 5, cluster_labels, true, &cl6);
		assert_null(cl6);
		assert_int_equal(ec6, SCC_ER_TOO_LARGE_PROBLEM);
	#endif

	scc_Clustering* cl8;
	scc_ErrorCode ec8 = scc_init_existing_clustering(10, 5, NULL, true, &cl8);
	assert_null(cl8);
	assert_int_equal(ec8, SCC_ER_INVALID_INPUT);

	scc_Clustering* cl9;
	scc_ErrorCode ec9 = scc_init_existing_clustering(10, 5, cluster_labels, false, &cl9);
	assert_non_null(cl9);
	assert_int_equal(cl9->num_data_points, 10);
	assert_int_equal(cl9->num_clusters, 5);
	assert_non_null(cl9->cluster_label);
	assert_ptr_equal(cl9->cluster_label, cluster_labels);
	assert_memory_equal(cl9->cluster_label, ref_cluster_labels, 10 * sizeof(scc_Clabel));
	assert_true(cl9->external_labels);
	assert_int_equal(cl9->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(ec9, SCC_ER_OK);

	scc_Clustering* cl10;
	scc_ErrorCode ec10 = scc_init_existing_clustering(6, 5, cluster_labels, false, &cl10);
	assert_non_null(cl10);
	assert_int_equal(cl10->num_data_points, 6);
	assert_int_equal(cl10->num_clusters, 5);
	assert_non_null(cl10->cluster_label);
	assert_ptr_equal(cl10->cluster_label, cluster_labels);
	assert_memory_equal(cl10->cluster_label, ref_cluster_labels, 6 * sizeof(scc_Clabel));
	assert_true(cl10->external_labels);
	assert_int_equal(cl10->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(ec10, SCC_ER_OK);

	scc_Clustering* cl11;
	scc_ErrorCode ec11 = scc_init_existing_clustering(10, 5, cluster_labels, true, &cl11);
	assert_non_null(cl11);
	assert_int_equal(cl11->num_data_points, 10);
	assert_int_equal(cl11->num_clusters, 5);
	assert_non_null(cl11->cluster_label);
	assert_ptr_not_equal(cl11->cluster_label, cluster_labels);
	assert_memory_equal(cl11->cluster_label, ref_cluster_labels, 10 * sizeof(scc_Clabel));
	assert_false(cl11->external_labels);
	assert_int_equal(cl11->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(ec11, SCC_ER_OK);

	scc_Clustering* cl12;
	scc_ErrorCode ec12 = scc_init_existing_clustering(6, 5, cluster_labels, true, &cl12);
	assert_non_null(cl12);
	assert_int_equal(cl12->num_data_points, 6);
	assert_int_equal(cl12->num_clusters, 5);
	assert_non_null(cl12->cluster_label);
	assert_ptr_not_equal(cl12->cluster_label, cluster_labels);
	assert_memory_equal(cl12->cluster_label, ref_cluster_labels, 6 * sizeof(scc_Clabel));
	assert_false(cl12->external_labels);
	assert_int_equal(cl12->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(ec12, SCC_ER_OK);

	scc_free_clustering(&cl9);
	scc_free_clustering(&cl10);
	scc_free_clustering(&cl11);
	scc_free_clustering(&cl12);
}


void scc_ut_copy_clustering(void** state)
{
	(void) state;

	scc_Clabel cluster_labels[10] = { 1, 4, 3, 0, 2, 2, 0, 1, 4, 4 };
	const scc_Clabel ref_cluster_labels[10] = { 1, 4, 3, 0, 2, 2, 0, 1, 4, 4 };

	scc_Clustering in_cl = {
		.num_data_points = 10,
		.num_clusters = 5,
		.cluster_label = cluster_labels,
		.external_labels = true,
		.clustering_version = ISCC_CLUSTERING_STRUCT_VERSION,
	};

	scc_Clustering* out_cl;
	scc_ErrorCode ec = scc_copy_clustering(&in_cl, &out_cl);
	assert_non_null(out_cl);
	assert_int_equal(out_cl->num_data_points, 10);
	assert_int_equal(out_cl->num_clusters, 5);
	assert_non_null(out_cl->cluster_label);
	assert_ptr_not_equal(out_cl->cluster_label, cluster_labels);
	assert_memory_equal(out_cl->cluster_label, ref_cluster_labels, 10 * sizeof(scc_Clabel));
	assert_false(out_cl->external_labels);
	assert_int_equal(out_cl->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(ec, SCC_ER_OK);

	scc_free_clustering(&out_cl);


	scc_Clustering in_cl2 = {
		.num_data_points = 10,
		.num_clusters = 0,
		.cluster_label = NULL,
		.external_labels = false,
		.clustering_version = ISCC_CLUSTERING_STRUCT_VERSION,
	};

	scc_Clustering* out_cl2;
	scc_ErrorCode ec2 = scc_copy_clustering(&in_cl2, &out_cl2);
	assert_non_null(out_cl2);
	assert_int_equal(out_cl2->num_data_points, 10);
	assert_int_equal(out_cl2->num_clusters, 0);
	assert_null(out_cl2->cluster_label);
	assert_false(out_cl2->external_labels);
	assert_int_equal(out_cl2->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(ec2, SCC_ER_OK);

	scc_free_clustering(&out_cl2);
}


void scc_ut_is_initialized_clustering(void** state)
{
	(void) state;

	scc_Clabel cluster_labels[10] = { 1, 4, 3, 0, 2, 2, 0, 1, 4, 4 };

	scc_Clustering in_cl = {
		.num_data_points = 10,
		.num_clusters = 5,
		.cluster_label = cluster_labels,
		.external_labels = true,
		.clustering_version = ISCC_CLUSTERING_STRUCT_VERSION,
	};

	assert_false(scc_is_initialized_clustering(NULL));

	assert_true(scc_is_initialized_clustering(&in_cl));

	in_cl.num_data_points = 0;
	assert_false(scc_is_initialized_clustering(&in_cl));

	in_cl.num_data_points = 10;
	assert_true(scc_is_initialized_clustering(&in_cl));

	#if ISCC_M_POINTINDEX_MAX < SIZE_MAX
		in_cl.num_data_points = ((size_t) ISCC_POINTINDEX_MAX) + 1;
		assert_false(scc_is_initialized_clustering(&in_cl));
		in_cl.num_data_points = 10;
		assert_true(scc_is_initialized_clustering(&in_cl));
	#endif

	#if ISCC_M_CLABEL_MAX < SIZE_MAX
		in_cl.num_clusters = ((size_t) SCC_CLABEL_MAX) + 1;
		assert_false(scc_is_initialized_clustering(&in_cl));
		in_cl.num_clusters = 5;
		assert_true(scc_is_initialized_clustering(&in_cl));
	#endif

	in_cl.clustering_version = 0;
	assert_false(scc_is_initialized_clustering(&in_cl));
	in_cl.clustering_version = ISCC_CLUSTERING_STRUCT_VERSION;
	assert_true(scc_is_initialized_clustering(&in_cl));

	in_cl.num_clusters = 0;
	in_cl.cluster_label = NULL;
	assert_true(scc_is_initialized_clustering(&in_cl));

	in_cl.num_clusters = 5;
	assert_false(scc_is_initialized_clustering(&in_cl));
	in_cl.cluster_label = cluster_labels;
	assert_true(scc_is_initialized_clustering(&in_cl));
}


void scc_ut_check_clustering(void** state)
{
	(void) state;

	scc_Clabel cluster_labels1[15] = { 0, 1, 3, 2, 2, 3, 2, 1, 1, 0, 3, 3, 2, 1, 1 };

	scc_Clustering cl1 = {
		.num_data_points = 15,
		.num_clusters = 4,
		.cluster_label = cluster_labels1,
		.external_labels = true,
		.clustering_version = ISCC_CLUSTERING_STRUCT_VERSION,
	};

	bool is_OK;
	scc_ErrorCode ec;

	ec = scc_check_clustering(&cl1, 2, 0, NULL, 0, NULL, NULL);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);

	ec = scc_check_clustering(&cl1, 2, 0, NULL, 0, NULL, &is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(is_OK);

	ec = scc_check_clustering(&cl1, 2, 1, NULL, 0, NULL, &is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(is_OK);

	ec = scc_check_clustering(&cl1, 2, 0, NULL, 0, NULL, &is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(is_OK);

	cl1.num_clusters = 0;
	ec = scc_check_clustering(&cl1, 2, 0, NULL, 0, NULL, &is_OK);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);
	assert_false(is_OK);
	cl1.num_clusters = 4;

 	uint32_t test_type_constraints[5];
	ec = scc_check_clustering(&cl1, 2, 0, test_type_constraints, 0, NULL, &is_OK);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);
	assert_false(is_OK);

	ec = scc_check_clustering(&cl1, 2, 0, NULL, 5, NULL, &is_OK);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);
	assert_false(is_OK);

 	scc_TypeLabel test_type_labels[5];
	ec = scc_check_clustering(&cl1, 2, 0, NULL, 0, test_type_labels, &is_OK);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);
	assert_false(is_OK);

	ec = scc_check_clustering(&cl1, 2, 0, NULL, 0, NULL, &is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(is_OK);

	cluster_labels1[2] = 5;
	ec = scc_check_clustering(&cl1, 2, 0, NULL, 0, NULL, &is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_false(is_OK);

	cluster_labels1[2] = SCC_CLABEL_NA;
	ec = scc_check_clustering(&cl1, 2, 0, NULL, 0, NULL, &is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(is_OK);
	cluster_labels1[2] = 3;

	ec = scc_check_clustering(&cl1, 5, 0, NULL, 0, NULL, &is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_false(is_OK);

	ec = scc_check_clustering(&cl1, 2, 0, NULL, 0, NULL, &is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(is_OK);
}


void scc_ut_check_clustering_types(void** state)
{
	(void) state;

	scc_Clabel cluster_labels1[15] = { 0, 1, 3, 2, 2, 3, 2, 1, 1, 0, 3, 3, 2, 1, 1 };
	scc_TypeLabel type_labels[15] =  { 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0 };
	uint32_t type_constraints[2] = { 1, 1 };

	scc_Clustering cl1 = {
		.num_data_points = 15,
		.num_clusters = 4,
		.cluster_label = cluster_labels1,
		.external_labels = true,
		.clustering_version = ISCC_CLUSTERING_STRUCT_VERSION,
	};

	bool is_OK;
	scc_ErrorCode ec;

	ec = scc_check_clustering(&cl1, 2, 2, type_constraints, 15, type_labels, NULL);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);

	ec = scc_check_clustering(&cl1, 2, 2, type_constraints, 15, type_labels, &is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(is_OK);

	ec = scc_check_clustering(&cl1, 2, 2, type_constraints, 15, type_labels, &is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(is_OK);

	cl1.num_clusters = 0;
	ec = scc_check_clustering(&cl1, 2, 2, type_constraints, 15, type_labels, &is_OK);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);
	assert_false(is_OK);
	cl1.num_clusters = 4;

	ec = scc_check_clustering(&cl1, 2, 2, type_constraints, 15, type_labels, &is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(is_OK);

	ec = scc_check_clustering(&cl1, 2, UINT16_MAX + 1, type_constraints, 15, type_labels, &is_OK);
	assert_int_equal(ec, SCC_ER_TOO_LARGE_PROBLEM);
	assert_false(is_OK);

	ec = scc_check_clustering(&cl1, 2, 2, NULL, 15, type_labels, &is_OK);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);
	assert_false(is_OK);

	ec = scc_check_clustering(&cl1, 2, 2, type_constraints, 14, type_labels, &is_OK);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);
	assert_false(is_OK);

	ec = scc_check_clustering(&cl1, 2, 2, type_constraints, 15, NULL, &is_OK);
	assert_int_equal(ec, SCC_ER_INVALID_INPUT);
	assert_false(is_OK);


	ec = scc_check_clustering(&cl1, 2, 2, type_constraints, 15, type_labels, &is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(is_OK);

	ec = scc_check_clustering(&cl1, 5, 2, type_constraints, 15, type_labels, &is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_false(is_OK);

	type_labels[0] = 1;
	ec = scc_check_clustering(&cl1, 2, 2, type_constraints, 15, type_labels, &is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_false(is_OK);
	type_labels[0] = 0;

	cluster_labels1[2] = 5;
	ec = scc_check_clustering(&cl1, 2, 2, type_constraints, 15, type_labels, &is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_false(is_OK);

	cluster_labels1[2] = SCC_CLABEL_NA;
	ec = scc_check_clustering(&cl1, 2, 2, type_constraints, 15, type_labels, &is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(is_OK);
	cluster_labels1[2] = 3;

	ec = scc_check_clustering(&cl1, 2, 2, type_constraints, 15, type_labels, &is_OK);
	assert_int_equal(ec, SCC_ER_OK);
	assert_true(is_OK);
}


void scc_ut_get_clustering_info(void** state)
{
	(void) state;

	scc_Clabel cluster_labels[10] = { 1, 4, 3, 0, 2, 2, 0, 1, 4, 4 };

	scc_Clustering in_cl = {
		.num_data_points = 10,
		.num_clusters = 5,
		.cluster_label = cluster_labels,
		.external_labels = true,
		.clustering_version = ISCC_CLUSTERING_STRUCT_VERSION,
	};

	uintmax_t out_num_data_points, out_num_clusters;

	scc_ErrorCode ec1 = scc_get_clustering_info(&in_cl, NULL, NULL);
	assert_int_equal(ec1, SCC_ER_OK);

	out_num_data_points = out_num_clusters = 0;
	scc_ErrorCode ec2 = scc_get_clustering_info(&in_cl, &out_num_data_points, NULL);
	assert_int_equal(ec2, SCC_ER_OK);
	assert_int_equal(out_num_data_points, 10);

	out_num_data_points = out_num_clusters = 0;
	scc_ErrorCode ec3 = scc_get_clustering_info(&in_cl, NULL, &out_num_clusters);
	assert_int_equal(ec3, SCC_ER_OK);
	assert_int_equal(out_num_clusters, 5);

	out_num_data_points = out_num_clusters = 0;
	scc_ErrorCode ec4 = scc_get_clustering_info(&in_cl, &out_num_data_points, &out_num_clusters);
	assert_int_equal(ec4, SCC_ER_OK);
	assert_int_equal(out_num_data_points, 10);
	assert_int_equal(out_num_clusters, 5);
}


void scc_ut_get_cluster_labels(void** state)
{
	(void) state;

	scc_Clabel cluster_labels[10] = { 1, 4, 3, 0, 2, 2, 0, 1, 4, 4 };

	scc_Clustering in_cl = {
		.num_data_points = 10,
		.num_clusters = 0,
		.cluster_label = cluster_labels,
		.external_labels = true,
		.clustering_version = ISCC_CLUSTERING_STRUCT_VERSION,
	};

	scc_Clabel out_cluster_labels_tmp[10];

	scc_ErrorCode ec1 = scc_get_cluster_labels(&in_cl, 10, out_cluster_labels_tmp);
	assert_int_equal(ec1, SCC_ER_INVALID_INPUT);
	in_cl.num_clusters = 5;

	scc_ErrorCode ec2 = scc_get_cluster_labels(&in_cl, 0, out_cluster_labels_tmp);
	assert_int_equal(ec2, SCC_ER_INVALID_INPUT);

	scc_ErrorCode ec3 = scc_get_cluster_labels(&in_cl, 10, NULL);
	assert_int_equal(ec3, SCC_ER_INVALID_INPUT);

	scc_Clabel out_cluster_labels1[6];
	out_cluster_labels1[5] = 123;
	const scc_Clabel ref_cluster_labels1[6] = { 1, 4, 3, 0, 2, 123 };
	scc_ErrorCode ec4 = scc_get_cluster_labels(&in_cl, 5, out_cluster_labels1);
	assert_int_equal(ec4, SCC_ER_OK);
	assert_memory_equal(out_cluster_labels1, ref_cluster_labels1, 6 * sizeof(scc_Clabel));

	scc_Clabel out_cluster_labels2[11];
	out_cluster_labels2[10] = 123;
	const scc_Clabel ref_cluster_labels2[11] = { 1, 4, 3, 0, 2, 2, 0, 1, 4, 4, 123 };
	scc_ErrorCode ec5 = scc_get_cluster_labels(&in_cl, 10, out_cluster_labels2);
	assert_int_equal(ec5, SCC_ER_OK);
	assert_memory_equal(out_cluster_labels2, ref_cluster_labels2, 11 * sizeof(scc_Clabel));

	scc_Clabel out_cluster_labels3[21];
	out_cluster_labels3[20] = 123;
	const scc_Clabel ref_cluster_labels3[21] = { 1, 4, 3, 0, 2, 2, 0, 1, 4, 4,
	                                             SCC_CLABEL_NA, SCC_CLABEL_NA, SCC_CLABEL_NA, SCC_CLABEL_NA, SCC_CLABEL_NA,
	                                             SCC_CLABEL_NA, SCC_CLABEL_NA, SCC_CLABEL_NA, SCC_CLABEL_NA, SCC_CLABEL_NA, 123 };
	scc_ErrorCode ec6 = scc_get_cluster_labels(&in_cl, 20, out_cluster_labels3);
	assert_int_equal(ec6, SCC_ER_OK);
	assert_memory_equal(out_cluster_labels3, ref_cluster_labels3, 21 * sizeof(scc_Clabel));
}


void scc_ut_get_clustering_stats(void** state)
{
	(void) state;

	const scc_ClusteringStats ISCC_NULL_CLUSTERING_STATS = { 0, 0, 0, 0, 0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

	scc_Clabel cluster_labels1[15] = { 0, 1, 3, 2, 2, 3, 2, 1, 1, 0, 3, 3, 2, 1, 1 };

	scc_Clustering cl1 = {
		.num_data_points = 15,
		.num_clusters = 4,
		.cluster_label = cluster_labels1,
		.external_labels = true,
		.clustering_version = ISCC_CLUSTERING_STRUCT_VERSION,
	};

	scc_ErrorCode ec1 = scc_get_clustering_stats(&cl1, scc_ut_test_data_small, NULL);
	assert_int_equal(ec1, SCC_ER_INVALID_INPUT);

	scc_ClusteringStats out_stats1;
	cl1.num_clusters = 0;
	scc_ErrorCode ec2 = scc_get_clustering_stats(&cl1, scc_ut_test_data_small, &out_stats1);
	assert_int_equal(ec2, SCC_ER_INVALID_INPUT);
	assert_memory_equal(&out_stats1, &ISCC_NULL_CLUSTERING_STATS, sizeof(scc_ClusteringStats));
	cl1.num_clusters = 4;

	scc_ErrorCode ec3 = scc_get_clustering_stats(&cl1, NULL, &out_stats1);
	assert_int_equal(ec3, SCC_ER_INVALID_INPUT);
	assert_memory_equal(&out_stats1, &ISCC_NULL_CLUSTERING_STATS, sizeof(scc_ClusteringStats));

	scc_ErrorCode ec4 = scc_get_clustering_stats(&cl1, scc_ut_test_data_small, &out_stats1);
	assert_int_equal(ec4, SCC_ER_OK);
	assert_int_equal(out_stats1.num_data_points, 15);
	assert_int_equal(out_stats1.num_assigned, 15);
	assert_int_equal(out_stats1.num_clusters, 4);
	assert_int_equal(out_stats1.num_populated_clusters, 4);
	assert_int_equal(out_stats1.min_cluster_size, 2);
	assert_int_equal(out_stats1.max_cluster_size, 5);
	assert_double_equal(out_stats1.avg_cluster_size, 3.75);
	assert_double_equal(out_stats1.sum_dists, 20.776226);
	assert_double_equal(out_stats1.min_dist, 0.019146);
	assert_double_equal(out_stats1.max_dist, 2.195331);
	assert_double_equal(out_stats1.cl_avg_min_dist, 0.646028);
	assert_double_equal(out_stats1.cl_avg_max_dist, 1.763760);
	assert_double_equal(out_stats1.cl_avg_dist_weighted, 1.031564);
	assert_double_equal(out_stats1.cl_avg_dist_unweighted, 1.182866);

	const scc_Clabel M = SCC_CLABEL_NA;
	scc_Clabel cluster_labels2[100] = { M, 0, M, M, M, 1, M, M, 3, M,
	                                    M, M, M, M, M, M, 0, M, M, M,
	                                    M, 3, M, 0, M, 5, M, 5, M, M,
	                                    M, M, M, M, M, 1, M, M, 3, M,
	                                    M, 3, M, M, M, M, M, M, M, M,
	                                    M, M, M, 1, 1, M, M, 4, M, M,
	                                    M, M, M, M, M, M, M, M, M, M,
	                                    M, M, M, M, M, M, M, M, M, M,
	                                    M, M, M, M, M, M, M, M, M, M,
	                                    M, M, M, M, M, M, M, M, M, M };

	scc_Clustering cl2 = {
		.num_data_points = 100,
		.num_clusters = 6,
		.cluster_label = cluster_labels2,
		.external_labels = true,
		.clustering_version = ISCC_CLUSTERING_STRUCT_VERSION,
	};

	scc_ClusteringStats out_stats2;
	scc_ErrorCode ec5 = scc_get_clustering_stats(&cl2, scc_ut_test_data_large, &out_stats2);
	assert_int_equal(ec5, SCC_ER_OK);
	assert_int_equal(out_stats2.num_data_points, 100);
	assert_int_equal(out_stats2.num_assigned, 14);
	assert_int_equal(out_stats2.num_clusters, 6);
	assert_int_equal(out_stats2.num_populated_clusters, 5);
	assert_int_equal(out_stats2.min_cluster_size, 1);
	assert_int_equal(out_stats2.max_cluster_size, 4);
	assert_double_equal(out_stats2.avg_cluster_size, 2.8);
	assert_double_equal(out_stats2.sum_dists, 1126.431356);
	assert_double_equal(out_stats2.min_dist, 0.0);
	assert_double_equal(out_stats2.max_dist, 102.899636);
	assert_double_equal(out_stats2.cl_avg_min_dist, 28.301867);
	assert_double_equal(out_stats2.cl_avg_max_dist, 59.867698);
	assert_double_equal(out_stats2.cl_avg_dist_weighted, 60.25076);
	assert_double_equal(out_stats2.cl_avg_dist_unweighted, 47.155168);
}


int main(void)
{
	if(!scc_ut_init_tests()) return 1;

	const struct CMUnitTest test_cases[] = {
		cmocka_unit_test(scc_ut_get_compiled_version),
		cmocka_unit_test(scc_ut_free_clustering),
		cmocka_unit_test(scc_ut_init_empty_clustering),
		cmocka_unit_test(scc_ut_init_existing_clustering),
		cmocka_unit_test(scc_ut_copy_clustering),
		cmocka_unit_test(scc_ut_is_initialized_clustering),
		cmocka_unit_test(scc_ut_check_clustering),
		cmocka_unit_test(scc_ut_check_clustering_types),
		cmocka_unit_test(scc_ut_get_clustering_info),
		cmocka_unit_test(scc_ut_get_cluster_labels),
		cmocka_unit_test(scc_ut_get_clustering_stats),
	};

	return cmocka_run_group_tests_name("scclust.c", test_cases, NULL, NULL);
}
