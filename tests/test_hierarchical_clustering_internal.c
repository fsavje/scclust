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
#include <src/clustering_struct.h>
#include <src/hierarchical_clustering.c>
#include <src/scclust_types.h>
#include "data_object_test.h"
#include "double_assert.h"


void scc_ut_hi_empty_cl_stack(void** state)
{
	(void) state;

	const iscc_Dpid ref1_cl[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	const iscc_Dpid ref2_cl[20] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };

	iscc_hi_ClusterStack cl_stack1;
	scc_ErrorCode ec1 = iscc_hi_empty_cl_stack(10, &cl_stack1);
	assert_int_equal(ec1, SCC_ER_OK);
	assert_int_equal(cl_stack1.capacity, 67);
	assert_int_equal(cl_stack1.items, 1);
	assert_non_null(cl_stack1.clusters);
	assert_non_null(cl_stack1.dpid_store);
	assert_memory_equal(cl_stack1.dpid_store, ref1_cl, 10 * sizeof(iscc_Dpid));
	assert_int_equal(cl_stack1.clusters[0].size, 10);
	assert_int_equal(cl_stack1.clusters[0].marker, 0);
	assert_ptr_equal(cl_stack1.clusters[0].members, cl_stack1.dpid_store);
	cl_stack1.clusters[cl_stack1.capacity - 1].size = 0;

	iscc_hi_ClusterStack cl_stack2;
	scc_ErrorCode ec2 = iscc_hi_empty_cl_stack(20, &cl_stack2);
	assert_int_equal(ec2, SCC_ER_OK);
	assert_int_equal(cl_stack2.capacity, 87);
	assert_int_equal(cl_stack2.items, 1);
	assert_non_null(cl_stack2.clusters);
	assert_non_null(cl_stack2.dpid_store);
	assert_memory_equal(cl_stack2.dpid_store, ref2_cl, 20 * sizeof(iscc_Dpid));
	assert_int_equal(cl_stack2.clusters[0].size, 20);
	assert_int_equal(cl_stack2.clusters[0].marker, 0);
	assert_ptr_equal(cl_stack2.clusters[0].members, cl_stack2.dpid_store);
	cl_stack2.clusters[cl_stack2.capacity - 1].size = 0;

	free(cl_stack1.clusters);
	free(cl_stack1.dpid_store);
	free(cl_stack2.clusters);
	free(cl_stack2.dpid_store);
}


void scc_ut_hi_init_cl_stack(void** state)
{
	(void) state;

	const scc_Clabel M = SCC_CLABEL_NA;
	scc_Clabel labels1[20] = { 1, 2, 4, 3, 1,
	                           2, 1, 5, 1, 2,
	                           3, 2, 5, 3, 4,
	                           3, 0, 4, 1, 5  };
	scc_Clabel labels2[25] = { 0, M, 5, 3, 3,
	                           5, 3, 0, 4, M,
	                           2, M, 4, 0, 5,
	                           5, 5, 2, 5, M,
	                           6, M, 6, 6, 3  };

	const iscc_Dpid ref1_cl[20] = { 16, 18, 8, 6, 4, 0, 11, 9, 5, 1, 15, 13, 10, 3, 17, 14, 2, 19, 12, 7 };

	scc_Clustering* cl1;
	iscc_hi_ClusterStack cl_stack1;
	size_t size_largest_cluster1;
	assert_int_equal(scc_init_existing_clustering(20, 6, labels1, false, &cl1), SCC_ER_OK);
	scc_ErrorCode ec1 = iscc_hi_init_cl_stack(cl1, &cl_stack1, &size_largest_cluster1);

	assert_int_equal(ec1, SCC_ER_OK);
	assert_int_equal(size_largest_cluster1, 5);

	assert_int_equal(cl_stack1.capacity, 50);
	assert_int_equal(cl_stack1.items, 6);
	assert_non_null(cl_stack1.clusters);
	assert_non_null(cl_stack1.dpid_store);
	assert_memory_equal(cl_stack1.dpid_store, ref1_cl, 20 * sizeof(iscc_Dpid));
	cl_stack1.clusters[cl_stack1.capacity - 1].size = 123;

	assert_int_equal(cl_stack1.clusters[0].size, 1);
	assert_int_equal(cl_stack1.clusters[1].size, 5);
	assert_int_equal(cl_stack1.clusters[2].size, 4);
	assert_int_equal(cl_stack1.clusters[3].size, 4);
	assert_int_equal(cl_stack1.clusters[4].size, 3);
	assert_int_equal(cl_stack1.clusters[5].size, 3);

	assert_int_equal(cl_stack1.clusters[0].marker, 0);
	assert_int_equal(cl_stack1.clusters[1].marker, 0);
	assert_int_equal(cl_stack1.clusters[2].marker, 0);
	assert_int_equal(cl_stack1.clusters[3].marker, 0);
	assert_int_equal(cl_stack1.clusters[4].marker, 0);
	assert_int_equal(cl_stack1.clusters[5].marker, 0);

	assert_ptr_equal(cl_stack1.clusters[0].members, cl_stack1.dpid_store + 0);
	assert_ptr_equal(cl_stack1.clusters[1].members, cl_stack1.dpid_store + 1);
	assert_ptr_equal(cl_stack1.clusters[2].members, cl_stack1.dpid_store + 6);
	assert_ptr_equal(cl_stack1.clusters[3].members, cl_stack1.dpid_store + 10);
	assert_ptr_equal(cl_stack1.clusters[4].members, cl_stack1.dpid_store + 14);
	assert_ptr_equal(cl_stack1.clusters[5].members, cl_stack1.dpid_store + 17);

	const iscc_Dpid ref2_cl[20] = { 13, 7, 0, 17, 10, 24, 6, 4, 3, 12, 8, 18, 16, 15, 14, 5, 2, 23, 22, 20 };

	scc_Clustering* cl2;
	iscc_hi_ClusterStack cl_stack2;
	size_t size_largest_cluster2;
	assert_int_equal(scc_init_existing_clustering(25, 7, labels2, true, &cl2), SCC_ER_OK);
	scc_ErrorCode ec2 = iscc_hi_init_cl_stack(cl2, &cl_stack2, &size_largest_cluster2);

	assert_int_equal(ec2, SCC_ER_OK);
	assert_int_equal(size_largest_cluster2, 6);

	assert_int_equal(cl_stack2.capacity, 54);
	assert_int_equal(cl_stack2.items, 7);
	assert_non_null(cl_stack2.clusters);
	assert_non_null(cl_stack2.dpid_store);
	assert_memory_equal(cl_stack2.dpid_store, ref2_cl, 20 * sizeof(iscc_Dpid));
	cl_stack2.clusters[cl_stack2.capacity - 1].size = 123;

	assert_int_equal(cl_stack2.clusters[0].size, 3);
	assert_int_equal(cl_stack2.clusters[1].size, 0);
	assert_int_equal(cl_stack2.clusters[2].size, 2);
	assert_int_equal(cl_stack2.clusters[3].size, 4);
	assert_int_equal(cl_stack2.clusters[4].size, 2);
	assert_int_equal(cl_stack2.clusters[5].size, 6);
	assert_int_equal(cl_stack2.clusters[6].size, 3);

	assert_int_equal(cl_stack2.clusters[0].marker, 0);
	assert_int_equal(cl_stack2.clusters[1].marker, 0);
	assert_int_equal(cl_stack2.clusters[2].marker, 0);
	assert_int_equal(cl_stack2.clusters[3].marker, 0);
	assert_int_equal(cl_stack2.clusters[4].marker, 0);
	assert_int_equal(cl_stack2.clusters[5].marker, 0);
	assert_int_equal(cl_stack2.clusters[6].marker, 0);

	assert_ptr_equal(cl_stack2.clusters[0].members, cl_stack2.dpid_store + 0);
	assert_ptr_equal(cl_stack2.clusters[1].members, cl_stack2.dpid_store + 3);
	assert_ptr_equal(cl_stack2.clusters[2].members, cl_stack2.dpid_store + 3);
	assert_ptr_equal(cl_stack2.clusters[3].members, cl_stack2.dpid_store + 5);
	assert_ptr_equal(cl_stack2.clusters[4].members, cl_stack2.dpid_store + 9);
	assert_ptr_equal(cl_stack2.clusters[5].members, cl_stack2.dpid_store + 11);
	assert_ptr_equal(cl_stack2.clusters[6].members, cl_stack2.dpid_store + 17);

	scc_free_clustering(&cl1);
	free(cl_stack1.clusters);
	free(cl_stack1.dpid_store);
	scc_free_clustering(&cl2);
	free(cl_stack2.clusters);
	free(cl_stack2.dpid_store);
}


void scc_ut_hi_run_hierarchical_clustering(void** state)
{
	(void) state;

	iscc_hi_WorkArea wa = {
		.dpid_array1 = malloc(sizeof(iscc_Dpid[100])),
		.dpid_array2 = malloc(sizeof(iscc_Dpid[100])),
		.dist_array = malloc(sizeof(double[200])),
		.vertex_markers = calloc(100, sizeof(uint_fast16_t)),
		.edge_store1 = malloc(sizeof(iscc_hi_DistanceEdge[100])),
		.edge_store2 = malloc(sizeof(iscc_hi_DistanceEdge[100])),
	};

	scc_Clustering cl1 = {
		.num_data_points = 100,
		.num_clusters = 0,
		.cluster_label = malloc(sizeof(scc_Clabel[100])),
		.external_labels = false,
		.clustering_version = ISCC_CLUSTERING_STRUCT_VERSION,
	};
	iscc_hi_ClusterStack cl_stack1;
	iscc_hi_empty_cl_stack(100, &cl_stack1);
	scc_ErrorCode ec1 = iscc_hi_run_hierarchical_clustering(&cl_stack1, &cl1, scc_ut_test_data_large, &wa, 20, true);
	assert_int_equal(ec1, SCC_ER_OK);
	scc_Clabel ref_label1[100] = { 2, 3, 3, 2, 2, 3, 0, 0, 4, 3, 2, 1, 1, 0, 4, 3, 0, 2, 0, 4, 3, 1, 3,
	                               0, 0, 0, 4, 0, 4, 0, 3, 4, 3, 1, 0, 0, 3, 4, 1, 0, 3, 2, 1, 2, 2, 2,
	                               0, 2, 1, 4, 3, 4, 4, 4, 1, 0, 3, 1, 3, 1, 1, 1, 4, 4, 3, 4, 4, 1, 2,
	                               3, 1, 4, 2, 3, 4, 0, 0, 4, 1, 1, 3, 2, 1, 0, 2, 1, 0, 2, 2, 2, 4, 2,
	                               1, 2, 2, 1, 3, 4, 3, 0 };
	assert_int_equal(cl_stack1.items, 0);
	assert_int_equal(cl1.num_data_points, 100);
	assert_int_equal(cl1.num_clusters, 5);
	assert_false(cl1.external_labels);
	assert_memory_equal(cl1.cluster_label, ref_label1, 100 * sizeof(scc_Clabel));
	free(cl1.cluster_label);
	free(cl_stack1.clusters);
	free(cl_stack1.dpid_store);

	for (size_t i = 0; i < 100; ++i) {
		wa.vertex_markers[i] = 0;
	}
	scc_Clustering cl2 = {
		.num_data_points = 100,
		.num_clusters = 0,
		.cluster_label = malloc(sizeof(scc_Clabel[100])),
		.external_labels = false,
		.clustering_version = ISCC_CLUSTERING_STRUCT_VERSION,
	};
	iscc_hi_ClusterStack cl_stack2;
	iscc_hi_empty_cl_stack(100, &cl_stack2);
	scc_ErrorCode ec2 = iscc_hi_run_hierarchical_clustering(&cl_stack2, &cl2, scc_ut_test_data_large, &wa, 20, false);
	assert_int_equal(ec2, SCC_ER_OK);
	scc_Clabel ref_label2[100] = { 3, 0, 2, 3, 3, 2, 1, 1, 3, 2, 3, 0, 1, 0, 2, 2, 1, 3, 0, 2, 2, 0, 1, 1, 0, 1, 2, 1, 2, 0,
	                               2, 2, 2, 0, 1, 1, 2, 2, 0, 0, 3, 3, 0, 3, 3, 0, 1, 3, 0, 2, 0, 2, 2, 2, 0, 0, 2, 0, 2, 1,
	                               0, 0, 3, 3, 3, 3, 1, 1, 3, 2, 0, 1, 3, 2, 2, 1, 1, 3, 0, 0, 2, 3, 0, 1, 3, 0, 1, 3, 3, 3,
	                               3, 2, 0, 3, 3, 1, 0, 3, 2, 0 };
	assert_int_equal(cl_stack2.items, 0);
	assert_int_equal(cl2.num_data_points, 100);
	assert_int_equal(cl2.num_clusters, 4);
	assert_false(cl2.external_labels);
	assert_memory_equal(cl2.cluster_label, ref_label2, 100 * sizeof(scc_Clabel));
	free(cl2.cluster_label);
	free(cl_stack2.clusters);
	free(cl_stack2.dpid_store);

	for (size_t i = 0; i < 100; ++i) {
		wa.vertex_markers[i] = 0;
	}
	scc_Clabel cluster_label3[100] = { 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0,
	                                   0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0,
	                                   1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 0,
	                                   0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0,
	                                   0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1,
	                                   0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0,
	                                   0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0 };
	scc_Clustering cl3 = {
		.num_data_points = 100,
		.num_clusters = 2,
		.cluster_label = cluster_label3,
		.external_labels = false,
		.clustering_version = ISCC_CLUSTERING_STRUCT_VERSION,
	};
	size_t size_largest_cluster3;
	iscc_hi_ClusterStack cl_stack3;
	iscc_hi_init_cl_stack(&cl3, &cl_stack3, &size_largest_cluster3);
	assert_int_equal(size_largest_cluster3, 50);
	scc_ErrorCode ec3 = iscc_hi_run_hierarchical_clustering(&cl_stack3, &cl3, scc_ut_test_data_large, &wa, 20, true);
	assert_int_equal(ec3, SCC_ER_OK);
	scc_Clabel ref_label3[100] = { 1, 1, 3, 3, 3, 0, 0, 2, 0, 0, 3, 1, 2, 2, 3, 1, 0, 3, 1, 0, 0, 2, 1, 1, 1,
	                               0, 1, 2, 0, 2, 3, 0, 0, 1, 2, 0, 3, 2, 1, 1, 1, 3, 2, 3, 1, 2, 2, 2, 1, 0,
	                               2, 2, 3, 3, 2, 2, 3, 1, 0, 2, 1, 2, 1, 0, 3, 3, 2, 2, 1, 0, 2, 0, 3, 1, 3,
	                               0, 2, 1, 1, 1, 2, 1, 1, 2, 3, 1, 2, 2, 1, 1, 0, 1, 2, 3, 2, 2, 1, 3, 0, 2 };
	assert_int_equal(cl_stack3.items, 0);
	assert_int_equal(cl3.num_data_points, 100);
	assert_int_equal(cl3.num_clusters, 4);
	assert_false(cl3.external_labels);
	assert_memory_equal(cl3.cluster_label, ref_label3, 100 * sizeof(scc_Clabel));
	free(cl_stack3.clusters);
	free(cl_stack3.dpid_store);

	for (size_t i = 0; i < 100; ++i) {
		wa.vertex_markers[i] = 0;
	}
	scc_Clabel cluster_label4[100] = { 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0,
	                                   0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0,
	                                   1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 0,
	                                   0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0,
	                                   0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1,
	                                   0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0,
	                                   0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0 };
	scc_Clustering cl4 = {
		.num_data_points = 100,
		.num_clusters = 2,
		.cluster_label = cluster_label4,
		.external_labels = false,
		.clustering_version = ISCC_CLUSTERING_STRUCT_VERSION,
	};
	size_t size_largest_cluster4;
	iscc_hi_ClusterStack cl_stack4;
	iscc_hi_init_cl_stack(&cl4, &cl_stack4, &size_largest_cluster4);
	assert_int_equal(size_largest_cluster4, 50);
	scc_ErrorCode ec4 = iscc_hi_run_hierarchical_clustering(&cl_stack4, &cl4, scc_ut_test_data_large, &wa, 20, false);
	assert_int_equal(ec4, SCC_ER_OK);
	scc_Clabel ref_label4[100] = { 1, 0, 3, 3, 3, 0, 0, 2, 0, 0, 3, 0, 2, 2, 3, 1, 0, 3, 1, 0, 0, 2, 1, 1, 1,
	                               0, 0, 2, 0, 2, 3, 0, 0, 1, 2, 0, 3, 3, 0, 0, 1, 3, 2, 3, 1, 2, 2, 2, 0, 0,
	                               2, 3, 3, 3, 2, 2, 3, 1, 0, 2, 1, 2, 1, 0, 3, 3, 2, 2, 1, 0, 2, 0, 3, 0, 3,
	                               0, 2, 1, 1, 1, 3, 1, 1, 2, 3, 1, 2, 3, 1, 1, 0, 1, 2, 3, 3, 2, 1, 3, 0, 2 };
	assert_int_equal(cl_stack4.items, 0);
	assert_int_equal(cl4.num_data_points, 100);
	assert_int_equal(cl4.num_clusters, 4);
	assert_false(cl4.external_labels);
	assert_memory_equal(cl4.cluster_label, ref_label4, 100 * sizeof(scc_Clabel));
	free(cl_stack4.clusters);
	free(cl_stack4.dpid_store);

	free(wa.dpid_array1);
	free(wa.dpid_array2);
	free(wa.dist_array);
	free(wa.vertex_markers);
	free(wa.edge_store1);
	free(wa.edge_store2);
}


void scc_ut_hi_push_to_stack(void** state)
{
	(void) state;

	iscc_Dpid* ref_clusters = calloc(3, sizeof(iscc_hi_ClusterItem));
	iscc_Dpid* ref_dpid_store = calloc(20, sizeof(iscc_Dpid));

	iscc_hi_ClusterStack cl_stack = {
		.capacity = 3,
		.items = 2,
		.clusters = calloc(3, sizeof(iscc_hi_ClusterItem)),
		.dpid_store = calloc(20, sizeof(iscc_Dpid)),
	};

	iscc_hi_ClusterItem* new_cl_item1 = NULL;
	scc_ErrorCode ec1 = iscc_hi_push_to_stack(&cl_stack, &new_cl_item1);
	assert_int_equal(ec1, SCC_ER_OK);
	assert_int_equal(cl_stack.capacity, 3);
	assert_int_equal(cl_stack.items, 3);
	assert_ptr_equal(new_cl_item1, cl_stack.clusters + 2);
	assert_memory_equal(cl_stack.clusters, ref_clusters, 3 * sizeof(iscc_hi_ClusterItem));
	assert_memory_equal(cl_stack.dpid_store, ref_dpid_store, 20 * sizeof(iscc_Dpid));

	iscc_hi_ClusterItem* new_cl_item2 = NULL;
	scc_ErrorCode ec2 = iscc_hi_push_to_stack(&cl_stack, &new_cl_item2);
	assert_int_equal(ec2, SCC_ER_OK);
	assert_int_equal(cl_stack.capacity, 19);
	assert_int_equal(cl_stack.items, 4);
	assert_ptr_equal(new_cl_item2, cl_stack.clusters + 3);
	assert_memory_equal(cl_stack.clusters, ref_clusters, 3 * sizeof(iscc_hi_ClusterItem));
	assert_memory_equal(cl_stack.dpid_store, ref_dpid_store, 20 * sizeof(iscc_Dpid));

	iscc_hi_ClusterItem* new_cl_item3 = NULL;
	scc_ErrorCode ec3 = iscc_hi_push_to_stack(&cl_stack, &new_cl_item3);
	assert_int_equal(ec3, SCC_ER_OK);
	assert_int_equal(cl_stack.capacity, 19);
	assert_int_equal(cl_stack.items, 5);
	assert_ptr_equal(new_cl_item3, cl_stack.clusters + 4);
	assert_memory_equal(cl_stack.clusters, ref_clusters, 3 * sizeof(iscc_hi_ClusterItem));
	assert_memory_equal(cl_stack.dpid_store, ref_dpid_store, 20 * sizeof(iscc_Dpid));

	iscc_hi_ClusterStack cl_stack_fail = {
		.capacity = SIZE_MAX,
		.items = SIZE_MAX,
		.clusters = calloc(3, sizeof(iscc_hi_ClusterItem)),
		.dpid_store = calloc(20, sizeof(iscc_Dpid)),
	};

	iscc_hi_ClusterItem* new_cl_item_fail = NULL;
	scc_ErrorCode ec_fail = iscc_hi_push_to_stack(&cl_stack_fail, &new_cl_item_fail);
	assert_int_equal(ec_fail, SCC_ER_TOO_LARGE_PROBLEM);

	free(ref_clusters);
	free(ref_dpid_store);
	free(cl_stack.clusters);
	free(cl_stack.dpid_store);
	free(cl_stack_fail.clusters);
	free(cl_stack_fail.dpid_store);
}


void scc_ut_hi_break_cluster_into_two(void** state)
{
	(void) state;

	iscc_hi_WorkArea wa = {
		.dpid_array1 = malloc(sizeof(iscc_Dpid[100])),
		.dpid_array2 = malloc(sizeof(iscc_Dpid[100])),
		.dist_array = malloc(sizeof(double[100])),
		.vertex_markers = calloc(100, sizeof(uint_fast16_t)),
		.edge_store1 = malloc(sizeof(iscc_hi_DistanceEdge[40])),
		.edge_store2 = malloc(sizeof(iscc_hi_DistanceEdge[40])),
	};

	iscc_Dpid members1[10] = { 2, 4, 6, 8, 10, 12, 14, 16, 18, 20 };
	iscc_hi_ClusterItem cl1 = {
		.size = 10,
		.marker = 0,
		.members = members1,
	};

	iscc_Dpid ref_cl1[6] = { 2, 20, 14, 4, 12, 16 };
	iscc_Dpid ref_cl2[4] = { 8, 6, 10, 18 };
	iscc_hi_ClusterItem cl1_a;
	scc_ErrorCode ec1 = iscc_hi_break_cluster_into_two(&cl1, scc_ut_test_data_large, &wa, 3, true, &cl1_a);
	assert_int_equal(ec1, SCC_ER_OK);
	assert_int_equal(cl1.size, 6);
	assert_int_equal(cl1.marker, 2);
	assert_memory_equal(cl1.members, ref_cl1, cl1.size * sizeof(iscc_Dpid));
	assert_int_equal(cl1_a.size, 4);
	assert_int_equal(cl1_a.marker, 2);
	assert_memory_equal(cl1_a.members, ref_cl2, cl1_a.size * sizeof(iscc_Dpid));
	assert_int_equal(wa.vertex_markers[2], 2);
	assert_int_equal(wa.vertex_markers[4], 2);
	assert_int_equal(wa.vertex_markers[6], 2);
	assert_int_equal(wa.vertex_markers[8], 2);
	assert_int_equal(wa.vertex_markers[10], 2);
	assert_int_equal(wa.vertex_markers[12], 2);
	assert_int_equal(wa.vertex_markers[14], 2);
	assert_int_equal(wa.vertex_markers[16], 2);
	assert_int_equal(wa.vertex_markers[18], 2);
	assert_int_equal(wa.vertex_markers[20], 2);

	iscc_Dpid ref_cl3[3] = { 2, 20, 4 };
	iscc_Dpid ref_cl4[3] = { 14, 12, 16 };
	iscc_hi_ClusterItem cl1_b;
	scc_ErrorCode ec2 = iscc_hi_break_cluster_into_two(&cl1, scc_ut_test_data_large, &wa, 3, true, &cl1_b);
	assert_int_equal(ec2, SCC_ER_OK);
	assert_int_equal(cl1.size, 3);
	assert_int_equal(cl1.marker, 4);
	assert_memory_equal(cl1.members, ref_cl3, cl1.size * sizeof(iscc_Dpid));
	assert_int_equal(cl1_b.size, 3);
	assert_int_equal(cl1_b.marker, 4);
	assert_memory_equal(cl1_b.members, ref_cl4, cl1_b.size * sizeof(iscc_Dpid));
	assert_int_equal(wa.vertex_markers[2], 4);
	assert_int_equal(wa.vertex_markers[4], 4);
	assert_int_equal(wa.vertex_markers[6], 2);
	assert_int_equal(wa.vertex_markers[8], 2);
	assert_int_equal(wa.vertex_markers[10], 2);
	assert_int_equal(wa.vertex_markers[12], 4);
	assert_int_equal(wa.vertex_markers[14], 4);
	assert_int_equal(wa.vertex_markers[16], 4);
	assert_int_equal(wa.vertex_markers[18], 2);
	assert_int_equal(wa.vertex_markers[20], 4);

	iscc_Dpid ref_members1[10] = { 2, 20, 4, 14, 12, 16, 8, 6, 10, 18 };
	assert_memory_equal(members1, ref_members1, 10 * sizeof(iscc_Dpid));

	iscc_Dpid members2[10] = { 22, 21, 23, 24, 25, 26, 27, 28, 29, 30 };
	iscc_hi_ClusterItem cl2 = {
		.size = 10,
		.marker = 0,
		.members = members2,
	};

	iscc_Dpid ref_cl5[5] = { 21, 30, 22, 29, 24 };
	iscc_Dpid ref_cl6[5] = { 23, 25, 27, 26, 28 };
	iscc_hi_ClusterItem cl2_a;
	scc_ErrorCode ec3 = iscc_hi_break_cluster_into_two(&cl2, scc_ut_test_data_large, &wa, 2, false, &cl2_a);
	assert_int_equal(ec3, SCC_ER_OK);
	assert_int_equal(cl2.size, 5);
	assert_int_equal(cl2.marker, 2);
	assert_memory_equal(cl2.members, ref_cl5, cl2.size * sizeof(iscc_Dpid));
	assert_int_equal(cl2_a.size, 5);
	assert_int_equal(cl2_a.marker, 2);
	assert_memory_equal(cl2_a.members, ref_cl6, cl2_a.size * sizeof(iscc_Dpid));
	assert_int_equal(wa.vertex_markers[21], 2);
	assert_int_equal(wa.vertex_markers[22], 2);
	assert_int_equal(wa.vertex_markers[23], 2);
	assert_int_equal(wa.vertex_markers[24], 2);
	assert_int_equal(wa.vertex_markers[25], 2);
	assert_int_equal(wa.vertex_markers[26], 2);
	assert_int_equal(wa.vertex_markers[27], 2);
	assert_int_equal(wa.vertex_markers[28], 2);
	assert_int_equal(wa.vertex_markers[29], 2);
	assert_int_equal(wa.vertex_markers[30], 2);

	iscc_Dpid ref_cl7[2] = { 29, 24 };
	iscc_Dpid ref_cl8[3] = { 21, 22, 30 };
	iscc_hi_ClusterItem cl2_b;
	scc_ErrorCode ec4 = iscc_hi_break_cluster_into_two(&cl2, scc_ut_test_data_large, &wa, 2, false, &cl2_b);
	assert_int_equal(ec4, SCC_ER_OK);
	assert_int_equal(cl2.size, 2);
	assert_int_equal(cl2.marker, 4);
	assert_memory_equal(cl2.members, ref_cl7, cl2.size * sizeof(iscc_Dpid));
	assert_int_equal(cl2_b.size, 3);
	assert_int_equal(cl2_b.marker, 4);
	assert_memory_equal(cl2_b.members, ref_cl8, cl2_b.size * sizeof(iscc_Dpid));
	assert_int_equal(wa.vertex_markers[21], 4);
	assert_int_equal(wa.vertex_markers[22], 4);
	assert_int_equal(wa.vertex_markers[23], 2);
	assert_int_equal(wa.vertex_markers[24], 4);
	assert_int_equal(wa.vertex_markers[25], 2);
	assert_int_equal(wa.vertex_markers[26], 2);
	assert_int_equal(wa.vertex_markers[27], 2);
	assert_int_equal(wa.vertex_markers[28], 2);
	assert_int_equal(wa.vertex_markers[29], 4);
	assert_int_equal(wa.vertex_markers[30], 4);

	iscc_Dpid ref_cl9[3] = { 23, 27, 25 };
	iscc_Dpid ref_cl10[2] = { 28, 26 };
	iscc_hi_ClusterItem cl2_c;
	scc_ErrorCode ec5 = iscc_hi_break_cluster_into_two(&cl2_a, scc_ut_test_data_large, &wa, 2, false, &cl2_c);
	assert_int_equal(ec5, SCC_ER_OK);
	assert_int_equal(cl2_a.size, 3);
	assert_int_equal(cl2_a.marker, 4);
	assert_memory_equal(cl2_a.members, ref_cl9, cl2_a.size * sizeof(iscc_Dpid));
	assert_int_equal(cl2_c.size, 2);
	assert_int_equal(cl2_c.marker, 4);
	assert_memory_equal(cl2_c.members, ref_cl10, cl2_c.size * sizeof(iscc_Dpid));
	assert_int_equal(wa.vertex_markers[21], 4);
	assert_int_equal(wa.vertex_markers[22], 4);
	assert_int_equal(wa.vertex_markers[23], 4);
	assert_int_equal(wa.vertex_markers[24], 4);
	assert_int_equal(wa.vertex_markers[25], 4);
	assert_int_equal(wa.vertex_markers[26], 4);
	assert_int_equal(wa.vertex_markers[27], 4);
	assert_int_equal(wa.vertex_markers[28], 4);
	assert_int_equal(wa.vertex_markers[29], 4);
	assert_int_equal(wa.vertex_markers[30], 4);

	iscc_Dpid members3[40] = { 34, 42, 78, 27, 99, 67, 29, 18, 92, 25,
	                           33, 23, 55, 82, 21, 79, 70, 85, 83, 61,
	                           16, 57, 24, 35, 12, 95, 60, 7, 86, 48,
	                           46, 76, 75, 54, 59, 38, 11, 6, 39, 13 };
	iscc_hi_ClusterItem cl3 = {
		.size = 40,
		.marker = 10,
		.members = members3,
	};
	iscc_Dpid ref_cl11[20] = { 78, 92, 85, 42, 21, 33, 70, 57, 60, 48, 61, 79, 12, 95, 54, 38, 11, 82, 67, 59 };
	iscc_Dpid ref_cl12[20] = { 35, 34, 16, 39, 86, 25, 13, 27, 75, 76, 6, 46, 7, 23, 29, 99, 24, 83, 18, 55 };
	iscc_hi_ClusterItem cl3_a;
	scc_ErrorCode ec6 = iscc_hi_break_cluster_into_two(&cl3, scc_ut_test_data_large, &wa, 20, true, &cl3_a);
	assert_int_equal(ec6, SCC_ER_OK);
	assert_int_equal(cl3.size, 20);
	assert_int_equal(cl3.marker, 12);
	assert_memory_equal(cl3.members, ref_cl11, cl3.size * sizeof(iscc_Dpid));
	assert_int_equal(cl3_a.size, 20);
	assert_int_equal(cl3_a.marker, 12);
	assert_memory_equal(cl3_a.members, ref_cl12, cl3_a.size * sizeof(iscc_Dpid));

	free(wa.dpid_array1);
	free(wa.dpid_array2);
	free(wa.dist_array);
	free(wa.vertex_markers);
	free(wa.edge_store1);
	free(wa.edge_store2);
}


void scc_ut_hi_get_next_marker(void** state)
{
	(void) state;

	const uint_fast16_t vertex_markers_ref[20] = { 0, 2, 3, 4, 0,
	                                               6, 0, 8, 0, 10,
	                                               11, 12, 13, 14, 15,
	                                               16, 17, 18, 0, 20 };
	uint_fast16_t vertex_markers[20] = { 0, 2, 3, 4, 0,
	                                     6, 0, 8, 0, 10,
	                                     11, 12, 13, 14, 15,
	                                     16, 17, 18, 0, 20 };

	const iscc_Dpid mems_ref[5] = { 0, 4, 6, 8, 18 };
	iscc_Dpid mems[5] = { 0, 4, 6, 8, 18 };
	iscc_hi_ClusterItem cl_item = {
		.size = 5,
		.marker = 0,
		.members = mems,
	};

	uint_fast16_t next_marker1 = iscc_hi_get_next_marker(&cl_item, vertex_markers);
	assert_int_equal(next_marker1, 1);
	assert_int_equal(cl_item.size, 5);
	assert_int_equal(cl_item.marker, 1);
	assert_memory_equal(mems, mems_ref, 5 * sizeof(iscc_Dpid));
	assert_memory_equal(vertex_markers, vertex_markers_ref, 20 * sizeof(uint_fast16_t));

	uint_fast16_t next_marker2 = iscc_hi_get_next_marker(&cl_item, vertex_markers);
	assert_int_equal(next_marker2, 2);
	assert_int_equal(cl_item.size, 5);
	assert_int_equal(cl_item.marker, 2);
	assert_memory_equal(mems, mems_ref, 5 * sizeof(iscc_Dpid));
	assert_memory_equal(vertex_markers, vertex_markers_ref, 20 * sizeof(uint_fast16_t));

	vertex_markers[0] = 4;
	vertex_markers[4] = 3;
	vertex_markers[6] = 2;
	vertex_markers[8] = 8;
	vertex_markers[18] = 4;
	cl_item.marker = UINT_FAST16_MAX;

	uint_fast16_t next_marker3 = iscc_hi_get_next_marker(&cl_item, vertex_markers);
	assert_int_equal(next_marker3, 1);
	assert_int_equal(cl_item.size, 5);
	assert_int_equal(cl_item.marker, 1);
	assert_memory_equal(mems, mems_ref, 5 * sizeof(iscc_Dpid));
	assert_memory_equal(vertex_markers, vertex_markers_ref, 20 * sizeof(uint_fast16_t));

	uint_fast16_t next_marker4 = iscc_hi_get_next_marker(&cl_item, vertex_markers);
	assert_int_equal(next_marker4, 2);
	assert_int_equal(cl_item.size, 5);
	assert_int_equal(cl_item.marker, 2);
	assert_memory_equal(mems, mems_ref, 5 * sizeof(iscc_Dpid));
	assert_memory_equal(vertex_markers, vertex_markers_ref, 20 * sizeof(uint_fast16_t));
}


void scc_ut_hi_get_next_k_nn(void** state)
{
	(void) state;

	iscc_Dpid members[10] = { 2, 4, 6, 8, 10, 12, 14, 16, 18, 20 };
	iscc_hi_ClusterItem cl = {
		.size = 10,
		.marker = 0,
		.members = members,
	};

	iscc_hi_WorkArea wa = {
		.dpid_array1 = NULL,
		.dpid_array2 = NULL,
		.dist_array = malloc(sizeof(double[20])),
		.vertex_markers = calloc(100, sizeof(uint_fast16_t)),
		.edge_store1 = malloc(sizeof(iscc_hi_DistanceEdge[10])),
		.edge_store2 = malloc(sizeof(iscc_hi_DistanceEdge[10])),
	};

	assert_int_equal(iscc_hi_populate_edge_lists(&cl, scc_ut_test_data_large, 6, 16, &wa), SCC_ER_OK);

	iscc_Dpid out_dist_array0[4];
	const iscc_Dpid ref_dist_array0[4] = { 12, 16, 18, 4 };
	iscc_hi_DistanceEdge* prev_dist0 = iscc_hi_get_next_k_nn(wa.edge_store1, 4, wa.vertex_markers, 1, out_dist_array0);
	assert_memory_equal(out_dist_array0, ref_dist_array0, 4 * sizeof(iscc_Dpid));
	assert_int_equal(prev_dist0->head, 4);
	assert_double_equal(prev_dist0->distance, 72.125847);
	assert_ptr_equal(prev_dist0->next_dist, prev_dist0 + 1);

	iscc_Dpid out_dist_array1[4];
	const iscc_Dpid ref_dist_array1[4] = { 18, 4, 10, 14 };
	iscc_hi_DistanceEdge* prev_dist1 = iscc_hi_get_next_k_nn(&wa.edge_store1[2], 4, wa.vertex_markers, 1, out_dist_array1);
	assert_memory_equal(out_dist_array1, ref_dist_array1, 4 * sizeof(iscc_Dpid));
	assert_int_equal(prev_dist1->head, 14);
	assert_double_equal(prev_dist1->distance, 80.566800);
	assert_ptr_equal(prev_dist1->next_dist, prev_dist1 + 1);

	wa.vertex_markers[18] = 1;
	wa.vertex_markers[4] = 1;
	wa.vertex_markers[8] = 1;

	iscc_Dpid out_dist_arrayY[4];
	const iscc_Dpid ref_dist_arrayY[4] = { 12, 16, 10, 14 };
	iscc_hi_DistanceEdge* prev_distY = iscc_hi_get_next_k_nn(wa.edge_store1, 4, wa.vertex_markers, 1, out_dist_arrayY);
	assert_memory_equal(out_dist_arrayY, ref_dist_arrayY, 4 * sizeof(iscc_Dpid));
	assert_int_equal(prev_distY->head, 14);
	assert_double_equal(prev_distY->distance, 80.566800);
	assert_ptr_equal(prev_distY->next_dist, prev_distY + 1);

	iscc_Dpid out_dist_arrayX[2];
	const iscc_Dpid ref_dist_arrayX[2] = { 12, 16 };
	iscc_hi_DistanceEdge* prev_distX = iscc_hi_get_next_k_nn(wa.edge_store1, 2, wa.vertex_markers, 1, out_dist_arrayX);
	assert_memory_equal(out_dist_arrayX, ref_dist_arrayX, 2 * sizeof(iscc_Dpid));
	assert_int_equal(prev_distX->head, 16);
	assert_double_equal(prev_distX->distance, 43.918798);
	assert_ptr_equal(prev_distX->next_dist, prev_distX + 3);

	iscc_Dpid out_dist_array2[4];
	const iscc_Dpid ref_dist_array2[4] = { 10, 14, 20, 2 };
	iscc_hi_DistanceEdge* prev_dist2 = iscc_hi_get_next_k_nn(&wa.edge_store1[2], 4, wa.vertex_markers, 1, out_dist_array2);
	assert_memory_equal(out_dist_array2, ref_dist_array2, 4 * sizeof(iscc_Dpid));
	assert_int_equal(prev_dist2->head, 2);
	assert_double_equal(prev_dist2->distance, 103.030113);
	assert_null(prev_dist2->next_dist);

	wa.vertex_markers[20] = 1;

	iscc_Dpid out_dist_array3[4];
	const iscc_Dpid ref_dist_array3[4] = { 16, 10, 14, 2 };
	iscc_hi_DistanceEdge* prev_dist3 = iscc_hi_get_next_k_nn(&wa.edge_store1[1], 4, wa.vertex_markers, 1, out_dist_array3);
	assert_memory_equal(out_dist_array3, ref_dist_array3, 4 * sizeof(iscc_Dpid));
	assert_int_equal(prev_dist3->head, 2);
	assert_double_equal(prev_dist3->distance, 103.030113);
	assert_null(prev_dist3->next_dist);


	iscc_Dpid out_dist_array4[1];
	const iscc_Dpid ref_dist_array4[1] = { 8 };
	iscc_hi_DistanceEdge* prev_dist4 = iscc_hi_get_next_k_nn(&wa.edge_store2[4], 1, wa.vertex_markers, 2, out_dist_array4);
	assert_memory_equal(out_dist_array4, ref_dist_array4, 1 * sizeof(iscc_Dpid));
	assert_int_equal(prev_dist4->head, 8);
	assert_double_equal(prev_dist4->distance, 62.616031);
	assert_ptr_equal(prev_dist4->next_dist, prev_dist4 + 1);

	wa.vertex_markers[4] = 2;
	wa.vertex_markers[14] = 2;
	wa.vertex_markers[20] = 2;

	iscc_Dpid out_dist_array5[3];
	const iscc_Dpid ref_dist_array5[3] = { 8, 10, 2 };
	iscc_hi_DistanceEdge* prev_dist5 = iscc_hi_get_next_k_nn(&wa.edge_store2[2], 3, wa.vertex_markers, 2, out_dist_array5);
	assert_memory_equal(out_dist_array5, ref_dist_array5, 3 * sizeof(iscc_Dpid));
	assert_int_equal(prev_dist5->head, 2);
	assert_double_equal(prev_dist5->distance, 83.120587);
	assert_ptr_equal(prev_dist5->next_dist, prev_dist5 + 1);

	free(wa.dist_array);
	free(wa.vertex_markers);
	free(wa.edge_store1);
	free(wa.edge_store2);
}


void scc_ut_hi_get_next_dist(void** state)
{
	(void) state;

	iscc_Dpid members[5] = { 2, 4, 6, 8, 10 };
	iscc_hi_ClusterItem cl = {
		.size = 5,
		.marker = 0,
		.members = members,
	};

	iscc_hi_WorkArea wa = {
		.dpid_array1 = NULL,
		.dpid_array2 = NULL,
		.dist_array = malloc(sizeof(double[10])),
		.vertex_markers = calloc(100, sizeof(uint_fast16_t)),
		.edge_store1 = malloc(sizeof(iscc_hi_DistanceEdge[5])),
		.edge_store2 = malloc(sizeof(iscc_hi_DistanceEdge[5])),
	};

	assert_int_equal(iscc_hi_populate_edge_lists(&cl, scc_ut_test_data_large, 6, 4, &wa), SCC_ER_OK);

	assert_int_equal(wa.edge_store1[1].head, 4);
	assert_double_equal(wa.edge_store1[1].distance, 72.125847);
	assert_int_equal(wa.edge_store1[2].head, 10);
	assert_double_equal(wa.edge_store1[2].distance, 76.285875);
	assert_int_equal(wa.edge_store1[3].head, 8);
	assert_double_equal(wa.edge_store1[3].distance, 82.249050);
	assert_int_equal(wa.edge_store1[4].head, 2);
	assert_double_equal(wa.edge_store1[4].distance, 103.030113);
	for (size_t i = 0; i < 4; ++i) {
		assert_ptr_equal(wa.edge_store1[i].next_dist, &wa.edge_store1[i + 1]);
    }
    assert_null(wa.edge_store1[4].next_dist);

	assert_int_equal(wa.edge_store2[1].head, 2);
	assert_double_equal(wa.edge_store2[1].distance, 63.103580);
	assert_int_equal(wa.edge_store2[2].head, 10);
	assert_double_equal(wa.edge_store2[2].distance, 67.606177);
	assert_int_equal(wa.edge_store2[3].head, 6);
	assert_double_equal(wa.edge_store2[3].distance, 72.125847);
	assert_int_equal(wa.edge_store2[4].head, 8);
	assert_double_equal(wa.edge_store2[4].distance, 89.098152);
	for (size_t i = 0; i < 4; ++i) {
		assert_ptr_equal(wa.edge_store2[i].next_dist, &wa.edge_store2[i + 1]);
    }
    assert_null(wa.edge_store2[4].next_dist);


    iscc_hi_DistanceEdge* next0 = iscc_hi_get_next_dist(wa.edge_store1, wa.vertex_markers, 1);
	assert_int_equal(next0->head, 4);
	assert_double_equal(next0->distance, 72.125847);
	assert_ptr_equal(next0->next_dist, &wa.edge_store1[2]);

	iscc_hi_DistanceEdge* next1 = iscc_hi_get_next_dist(&wa.edge_store1[2], wa.vertex_markers, 1);
	assert_int_equal(next1->head, 8);
	assert_double_equal(next1->distance, 82.249050);
	assert_ptr_equal(next1->next_dist, &wa.edge_store1[4]);

	iscc_hi_DistanceEdge* next2 = iscc_hi_get_next_dist(&wa.edge_store1[3], wa.vertex_markers, 1);
	assert_int_equal(next2->head, 2);
	assert_double_equal(next2->distance, 103.030113);
	assert_null(next2->next_dist);

	wa.vertex_markers[8] = 1;

	iscc_hi_DistanceEdge* next3 = iscc_hi_get_next_dist(&wa.edge_store1[2], wa.vertex_markers, 1);
	assert_int_equal(next3->head, 2);
	assert_double_equal(next3->distance, 103.030113);
	assert_null(next3->next_dist);

	iscc_hi_DistanceEdge* next4 = iscc_hi_get_next_dist(&wa.edge_store1[2], wa.vertex_markers, 1);
	assert_int_equal(next4->head, 2);
	assert_double_equal(next4->distance, 103.030113);
	assert_null(next4->next_dist);

	assert_int_equal(wa.edge_store1[1].head, 4);
	assert_double_equal(wa.edge_store1[1].distance, 72.125847);
	assert_int_equal(wa.edge_store1[2].head, 10);
	assert_double_equal(wa.edge_store1[2].distance, 76.285875);
	assert_int_equal(wa.edge_store1[3].head, 8);
	assert_double_equal(wa.edge_store1[3].distance, 82.249050);
	assert_int_equal(wa.edge_store1[4].head, 2);
	assert_double_equal(wa.edge_store1[4].distance, 103.030113);

	assert_ptr_equal(wa.edge_store1[0].next_dist, &wa.edge_store1[1]);
	assert_ptr_equal(wa.edge_store1[1].next_dist, &wa.edge_store1[2]);
	assert_ptr_equal(wa.edge_store1[2].next_dist, &wa.edge_store1[4]);
	assert_ptr_equal(wa.edge_store1[3].next_dist, &wa.edge_store1[4]);
	assert_null(wa.edge_store1[4].next_dist);

	wa.vertex_markers[2] = 1;
	wa.vertex_markers[10] = 1;

	iscc_hi_DistanceEdge* next5 = iscc_hi_get_next_dist(wa.edge_store2, wa.vertex_markers, 1);
	assert_int_equal(next5->head, 6);
	assert_double_equal(next5->distance, 72.125847);
	assert_ptr_equal(next5->next_dist, &wa.edge_store2[4]);

	iscc_hi_DistanceEdge* next6 = iscc_hi_get_next_dist(wa.edge_store2, wa.vertex_markers, 1);
	assert_int_equal(next6->head, 6);
	assert_double_equal(next6->distance, 72.125847);
	assert_ptr_equal(next6->next_dist, &wa.edge_store2[4]);

	assert_int_equal(wa.edge_store2[1].head, 2);
	assert_double_equal(wa.edge_store2[1].distance, 63.103580);
	assert_int_equal(wa.edge_store2[2].head, 10);
	assert_double_equal(wa.edge_store2[2].distance, 67.606177);
	assert_int_equal(wa.edge_store2[3].head, 6);
	assert_double_equal(wa.edge_store2[3].distance, 72.125847);
	assert_int_equal(wa.edge_store2[4].head, 8);
	assert_double_equal(wa.edge_store2[4].distance, 89.098152);

	assert_ptr_equal(wa.edge_store2[0].next_dist, &wa.edge_store2[3]);
	assert_ptr_equal(wa.edge_store2[1].next_dist, &wa.edge_store2[2]);
	assert_ptr_equal(wa.edge_store2[2].next_dist, &wa.edge_store2[3]);
	assert_ptr_equal(wa.edge_store2[3].next_dist, &wa.edge_store2[4]);
	assert_null(wa.edge_store2[4].next_dist);

	free(wa.dist_array);
	free(wa.vertex_markers);
	free(wa.edge_store1);
	free(wa.edge_store2);
}


void scc_ut_hi_move_point_to_cluster1(void** state)
{
	(void) state;

	uint_fast16_t* vertex_markers = calloc(20, sizeof(uint_fast16_t));
	iscc_Dpid* members = calloc(20, sizeof(iscc_Dpid));
	members[0] = 4;
	members[1] = 1;
	members[2] = 2;
	vertex_markers[4] = 1;
	vertex_markers[1] = 1;
	vertex_markers[2] = 1;

	iscc_hi_ClusterItem cl = {
		.size = 3,
		.marker = 1,
		.members = members,
	};

	uint_fast16_t* ref_vertex_markers = calloc(20, sizeof(uint_fast16_t));
	iscc_Dpid* ref_members = calloc(20, sizeof(iscc_Dpid));
	ref_members[0] = 4;
	ref_members[1] = 1;
	ref_members[2] = 2;
	ref_vertex_markers[4] = 1;
	ref_vertex_markers[1] = 1;
	ref_vertex_markers[2] = 1;

	assert_int_equal(cl.size, 3);
	assert_int_equal(cl.marker, 1);
	assert_ptr_equal(cl.members, members);
	assert_memory_equal(cl.members, ref_members, 20 * sizeof(iscc_Dpid));
	assert_memory_equal(vertex_markers, ref_vertex_markers, 20 * sizeof(uint_fast16_t));


	ref_members[3] = 11;
	ref_vertex_markers[11] = 1;
	iscc_hi_move_point_to_cluster1(11, &cl, vertex_markers, 1);
	assert_int_equal(cl.size, 4);
	assert_int_equal(cl.marker, 1);
	assert_ptr_equal(cl.members, members);
	assert_memory_equal(cl.members, ref_members, 20 * sizeof(iscc_Dpid));
	assert_memory_equal(vertex_markers, ref_vertex_markers, 20 * sizeof(uint_fast16_t));

	ref_members[4] = 3;
	ref_vertex_markers[3] = 1;
	iscc_hi_move_point_to_cluster1(3, &cl, vertex_markers, 1);
	assert_int_equal(cl.size, 5);
	assert_int_equal(cl.marker, 1);
	assert_ptr_equal(cl.members, members);
	assert_memory_equal(cl.members, ref_members, 20 * sizeof(iscc_Dpid));
	assert_memory_equal(vertex_markers, ref_vertex_markers, 20 * sizeof(uint_fast16_t));

	ref_members[5] = 6;
	ref_vertex_markers[6] = 1;
	iscc_hi_move_point_to_cluster1(6, &cl, vertex_markers, 1);
	assert_int_equal(cl.size, 6);
	assert_int_equal(cl.marker, 1);
	assert_ptr_equal(cl.members, members);
	assert_memory_equal(cl.members, ref_members, 20 * sizeof(iscc_Dpid));
	assert_memory_equal(vertex_markers, ref_vertex_markers, 20 * sizeof(uint_fast16_t));

	ref_members[6] = 10;
	ref_vertex_markers[10] = 1;
	iscc_hi_move_point_to_cluster1(10, &cl, vertex_markers, 1);
	assert_int_equal(cl.size, 7);
	assert_int_equal(cl.marker, 1);
	assert_ptr_equal(cl.members, members);
	assert_memory_equal(cl.members, ref_members, 20 * sizeof(iscc_Dpid));
	assert_memory_equal(vertex_markers, ref_vertex_markers, 20 * sizeof(uint_fast16_t));

	ref_members[7] = 14;
	ref_vertex_markers[14] = 1;
	iscc_hi_move_point_to_cluster1(14, &cl, vertex_markers, 1);
	assert_int_equal(cl.size, 8);
	assert_int_equal(cl.marker, 1);
	assert_ptr_equal(cl.members, members);
	assert_memory_equal(cl.members, ref_members, 20 * sizeof(iscc_Dpid));
	assert_memory_equal(vertex_markers, ref_vertex_markers, 20 * sizeof(uint_fast16_t));

	iscc_Dpid end_members[8] = { 4, 1, 2, 11, 3, 6, 10, 14 };
	assert_int_equal(cl.size, 8);
	assert_int_equal(cl.marker, 1);
	assert_memory_equal(cl.members, end_members, 8 * sizeof(iscc_Dpid));

	free(vertex_markers);
	free(members);
	free(ref_vertex_markers);
	free(ref_members);
}


void scc_ut_hi_move_point_to_cluster2(void** state)
{
	(void) state;

	uint_fast16_t* vertex_markers = calloc(20, sizeof(uint_fast16_t));
	iscc_Dpid* members = calloc(20, sizeof(iscc_Dpid));
	members[19] = 4;
	members[18] = 1;
	members[17] = 2;
	vertex_markers[4] = 1;
	vertex_markers[1] = 1;
	vertex_markers[2] = 1;

	iscc_hi_ClusterItem cl = {
		.size = 3,
		.marker = 1,
		.members = &members[17],
	};

	uint_fast16_t* ref_vertex_markers = calloc(20, sizeof(uint_fast16_t));
	iscc_Dpid* ref_members = calloc(20, sizeof(iscc_Dpid));
	ref_members[19] = 4;
	ref_members[18] = 1;
	ref_members[17] = 2;
	ref_vertex_markers[4] = 1;
	ref_vertex_markers[1] = 1;
	ref_vertex_markers[2] = 1;

	assert_int_equal(cl.size, 3);
	assert_int_equal(cl.marker, 1);
	assert_ptr_equal(cl.members, &members[17]);
	assert_memory_equal(members, ref_members, 20 * sizeof(iscc_Dpid));
	assert_memory_equal(vertex_markers, ref_vertex_markers, 20 * sizeof(uint_fast16_t));


	ref_members[16] = 11;
	ref_vertex_markers[11] = 1;
	iscc_hi_move_point_to_cluster2(11, &cl, vertex_markers, 1);
	assert_int_equal(cl.size, 4);
	assert_int_equal(cl.marker, 1);
	assert_ptr_equal(cl.members, &members[16]);
	assert_memory_equal(members, ref_members, 20 * sizeof(iscc_Dpid));
	assert_memory_equal(vertex_markers, ref_vertex_markers, 20 * sizeof(uint_fast16_t));

	ref_members[15] = 3;
	ref_vertex_markers[3] = 1;
	iscc_hi_move_point_to_cluster2(3, &cl, vertex_markers, 1);
	assert_int_equal(cl.size, 5);
	assert_int_equal(cl.marker, 1);
	assert_ptr_equal(cl.members, &members[15]);
	assert_memory_equal(members, ref_members, 20 * sizeof(iscc_Dpid));
	assert_memory_equal(vertex_markers, ref_vertex_markers, 20 * sizeof(uint_fast16_t));

	ref_members[14] = 6;
	ref_vertex_markers[6] = 1;
	iscc_hi_move_point_to_cluster2(6, &cl, vertex_markers, 1);
	assert_int_equal(cl.size, 6);
	assert_int_equal(cl.marker, 1);
	assert_ptr_equal(cl.members, &members[14]);
	assert_memory_equal(members, ref_members, 20 * sizeof(iscc_Dpid));
	assert_memory_equal(vertex_markers, ref_vertex_markers, 20 * sizeof(uint_fast16_t));

	ref_members[13] = 10;
	ref_vertex_markers[10] = 1;
	iscc_hi_move_point_to_cluster2(10, &cl, vertex_markers, 1);
	assert_int_equal(cl.size, 7);
	assert_int_equal(cl.marker, 1);
	assert_ptr_equal(cl.members, &members[13]);
	assert_memory_equal(members, ref_members, 20 * sizeof(iscc_Dpid));
	assert_memory_equal(vertex_markers, ref_vertex_markers, 20 * sizeof(uint_fast16_t));

	ref_members[12] = 14;
	ref_vertex_markers[14] = 1;
	iscc_hi_move_point_to_cluster2(14, &cl, vertex_markers, 1);
	assert_int_equal(cl.size, 8);
	assert_int_equal(cl.marker, 1);
	assert_ptr_equal(cl.members, &members[12]);
	assert_memory_equal(members, ref_members, 20 * sizeof(iscc_Dpid));
	assert_memory_equal(vertex_markers, ref_vertex_markers, 20 * sizeof(uint_fast16_t));

	iscc_Dpid end_members[8] = { 14, 10, 6, 3, 11, 2, 1, 4 };
	assert_int_equal(cl.size, 8);
	assert_int_equal(cl.marker, 1);
	assert_memory_equal(cl.members, end_members, 8 * sizeof(iscc_Dpid));

	free(vertex_markers);
	free(members);
	free(ref_vertex_markers);
	free(ref_members);
}


void scc_ut_hi_move_array_to_cluster1(void** state)
{
	(void) state;

	uint_fast16_t* vertex_markers = calloc(20, sizeof(uint_fast16_t));
	iscc_Dpid* members = calloc(20, sizeof(iscc_Dpid));
	members[0] = 4;
	members[1] = 1;
	members[2] = 2;
	vertex_markers[4] = 1;
	vertex_markers[1] = 1;
	vertex_markers[2] = 1;

	iscc_hi_ClusterItem cl = {
		.size = 3,
		.marker = 1,
		.members = members,
	};

	uint_fast16_t* ref_vertex_markers = calloc(20, sizeof(uint_fast16_t));
	iscc_Dpid* ref_members = calloc(20, sizeof(iscc_Dpid));
	ref_members[0] = 4;
	ref_members[1] = 1;
	ref_members[2] = 2;
	ref_vertex_markers[4] = 1;
	ref_vertex_markers[1] = 1;
	ref_vertex_markers[2] = 1;

	assert_int_equal(cl.size, 3);
	assert_int_equal(cl.marker, 1);
	assert_ptr_equal(cl.members, members);
	assert_memory_equal(cl.members, ref_members, 20 * sizeof(iscc_Dpid));
	assert_memory_equal(vertex_markers, ref_vertex_markers, 20 * sizeof(uint_fast16_t));

	ref_members[3] = 11;
	ref_members[4] = 3;
	ref_vertex_markers[11] = 1;
	ref_vertex_markers[3] = 1;
	iscc_Dpid add1[2] = { 11, 3 };
	iscc_hi_move_array_to_cluster1(2, add1, &cl, vertex_markers, 1);
	assert_int_equal(cl.size, 5);
	assert_int_equal(cl.marker, 1);
	assert_ptr_equal(cl.members, members);
	assert_memory_equal(cl.members, ref_members, 20 * sizeof(iscc_Dpid));
	assert_memory_equal(vertex_markers, ref_vertex_markers, 20 * sizeof(uint_fast16_t));

	ref_members[5] = 6;
	ref_vertex_markers[6] = 1;
	iscc_Dpid add2[1] = { 6 };
	iscc_hi_move_array_to_cluster1(1, add2, &cl, vertex_markers, 1);
	assert_int_equal(cl.size, 6);
	assert_int_equal(cl.marker, 1);
	assert_ptr_equal(cl.members, members);
	assert_memory_equal(cl.members, ref_members, 20 * sizeof(iscc_Dpid));
	assert_memory_equal(vertex_markers, ref_vertex_markers, 20 * sizeof(uint_fast16_t));

	ref_members[6] = 10;
	ref_members[7] = 14;
	ref_members[8] = 17;
	ref_vertex_markers[10] = 1;
	ref_vertex_markers[14] = 1;
	ref_vertex_markers[17] = 1;
	iscc_Dpid add3[3] = { 10, 14, 17 };
	iscc_hi_move_array_to_cluster1(3, add3, &cl, vertex_markers, 1);
	assert_int_equal(cl.size, 9);
	assert_int_equal(cl.marker, 1);
	assert_ptr_equal(cl.members, members);
	assert_memory_equal(cl.members, ref_members, 20 * sizeof(iscc_Dpid));
	assert_memory_equal(vertex_markers, ref_vertex_markers, 20 * sizeof(uint_fast16_t));

	iscc_Dpid end_members[9] = { 4, 1, 2, 11, 3, 6, 10, 14, 17 };
	assert_int_equal(cl.size, 9);
	assert_int_equal(cl.marker, 1);
	assert_memory_equal(cl.members, end_members, 9 * sizeof(iscc_Dpid));

	free(vertex_markers);
	free(members);
	free(ref_vertex_markers);
	free(ref_members);
}


void scc_ut_hi_move_array_to_cluster2(void** state)
{
	(void) state;

	uint_fast16_t* vertex_markers = calloc(20, sizeof(uint_fast16_t));
	iscc_Dpid* members = calloc(20, sizeof(iscc_Dpid));
	members[19] = 4;
	members[18] = 1;
	members[17] = 2;
	vertex_markers[4] = 1;
	vertex_markers[1] = 1;
	vertex_markers[2] = 1;

	iscc_hi_ClusterItem cl = {
		.size = 3,
		.marker = 1,
		.members = &members[17],
	};

	uint_fast16_t* ref_vertex_markers = calloc(20, sizeof(uint_fast16_t));
	iscc_Dpid* ref_members = calloc(20, sizeof(iscc_Dpid));
	ref_members[19] = 4;
	ref_members[18] = 1;
	ref_members[17] = 2;
	ref_vertex_markers[4] = 1;
	ref_vertex_markers[1] = 1;
	ref_vertex_markers[2] = 1;

	assert_int_equal(cl.size, 3);
	assert_int_equal(cl.marker, 1);
	assert_ptr_equal(cl.members, &members[17]);
	assert_memory_equal(members, ref_members, 20 * sizeof(iscc_Dpid));
	assert_memory_equal(vertex_markers, ref_vertex_markers, 20 * sizeof(uint_fast16_t));

	ref_members[16] = 11;
	ref_members[15] = 3;
	ref_vertex_markers[11] = 1;
	ref_vertex_markers[3] = 1;
	iscc_Dpid add1[2] = { 11, 3 };
	iscc_hi_move_array_to_cluster2(2, add1, &cl, vertex_markers, 1);
	assert_int_equal(cl.size, 5);
	assert_int_equal(cl.marker, 1);
	assert_ptr_equal(cl.members, &members[15]);
	assert_memory_equal(members, ref_members, 20 * sizeof(iscc_Dpid));
	assert_memory_equal(vertex_markers, ref_vertex_markers, 20 * sizeof(uint_fast16_t));

	ref_members[14] = 6;
	ref_vertex_markers[6] = 1;
	iscc_Dpid add2[1] = { 6 };
	iscc_hi_move_array_to_cluster2(1, add2, &cl, vertex_markers, 1);
	assert_int_equal(cl.size, 6);
	assert_int_equal(cl.marker, 1);
	assert_ptr_equal(cl.members, &members[14]);
	assert_memory_equal(members, ref_members, 20 * sizeof(iscc_Dpid));
	assert_memory_equal(vertex_markers, ref_vertex_markers, 20 * sizeof(uint_fast16_t));

	ref_members[13] = 10;
	ref_members[12] = 14;
	ref_members[11] = 17;
	ref_vertex_markers[10] = 1;
	ref_vertex_markers[14] = 1;
	ref_vertex_markers[17] = 1;
	iscc_Dpid add3[3] = { 10, 14, 17 };
	iscc_hi_move_array_to_cluster2(3, add3, &cl, vertex_markers, 1);
	assert_int_equal(cl.size, 9);
	assert_int_equal(cl.marker, 1);
	assert_ptr_equal(cl.members, &members[11]);
	assert_memory_equal(members, ref_members, 20 * sizeof(iscc_Dpid));
	assert_memory_equal(vertex_markers, ref_vertex_markers, 20 * sizeof(uint_fast16_t));

	iscc_Dpid end_members[9] = { 17, 14, 10, 6, 3, 11, 2, 1, 4 };
	assert_int_equal(cl.size, 9);
	assert_int_equal(cl.marker, 1);
	assert_memory_equal(cl.members, end_members, 9 * sizeof(iscc_Dpid));

	free(vertex_markers);
	free(members);
	free(ref_vertex_markers);
	free(ref_members);
}


void scc_ut_hi_find_centers(void** state)
{
	(void) state;

	iscc_hi_WorkArea wa = {
		.dpid_array1 = malloc(sizeof(iscc_Dpid[100])),
		.dpid_array2 = malloc(sizeof(iscc_Dpid[100])),
		.dist_array = malloc(sizeof(double[100])),
		.vertex_markers = calloc(100, sizeof(uint_fast16_t)),
		.edge_store1 = NULL,
		.edge_store2 = NULL,
	};

	iscc_Dpid ref_members1[10] = { 2, 4, 6, 8, 10, 12, 14, 16, 18, 20 };
	iscc_Dpid members1[10] = { 2, 4, 6, 8, 10, 12, 14, 16, 18, 20 };
	iscc_hi_ClusterItem cl1 = {
		.size = 10,
		.marker = 0,
		.members = members1,
	};

	iscc_Dpid ref_members2[5] = { 2, 4, 6, 8, 12 };
	iscc_Dpid members2[5] = { 2, 4, 6, 8, 12 };
	iscc_hi_ClusterItem cl2 = {
		.size = 5,
		.marker = 10,
		.members = members2,
	};

	iscc_Dpid ref_members3[10] = { 22, 21, 23, 24, 25, 26, 27, 28, 29, 30 };
	iscc_Dpid members3[10] = { 22, 21, 23, 24, 25, 26, 27, 28, 29, 30 };
	iscc_hi_ClusterItem cl3 = {
		.size = 10,
		.marker = 20,
		.members = members3,
	};

	iscc_Dpid ref_members4[4] = { 21, 23, 25, 27 };
	iscc_Dpid members4[4] = { 21, 23, 25, 27 };
	iscc_hi_ClusterItem cl4 = {
		.size = 4,
		.marker = 30,
		.members = members4,
	};

	iscc_Dpid center1, center2;
	scc_ErrorCode ec1 = iscc_hi_find_centers(&cl1, scc_ut_test_data_large, &wa, &center1, &center2);
	assert_int_equal(ec1, SCC_ER_OK);
	assert_int_equal(center1, 2);
	assert_int_equal(center2, 18);
	assert_int_equal(cl1.size, 10);
	assert_int_equal(cl1.marker, 1);
	assert_memory_equal(cl1.members, ref_members1, 10 * sizeof(iscc_Dpid));

	scc_ErrorCode ec2 = iscc_hi_find_centers(&cl2, scc_ut_test_data_large, &wa, &center1, &center2);
	assert_int_equal(ec2, SCC_ER_OK);
	assert_int_equal(center1, 2);
	assert_int_equal(center2, 8);
	assert_int_equal(cl2.size, 5);
	assert_int_equal(cl2.marker, 11);
	assert_memory_equal(cl2.members, ref_members2, 5 * sizeof(iscc_Dpid));

	scc_ErrorCode ec3 = iscc_hi_find_centers(&cl3, scc_ut_test_data_large, &wa, &center1, &center2);
	assert_int_equal(ec3, SCC_ER_OK);
	assert_int_equal(center1, 21);
	assert_int_equal(center2, 28);
	assert_int_equal(cl3.size, 10);
	assert_int_equal(cl3.marker, 21);
	assert_memory_equal(cl3.members, ref_members3, 10 * sizeof(iscc_Dpid));

	scc_ErrorCode ec4 = iscc_hi_find_centers(&cl4, scc_ut_test_data_large, &wa, &center1, &center2);
	assert_int_equal(ec4, SCC_ER_OK);
	assert_int_equal(center1, 21);
	assert_int_equal(center2, 27);
	assert_int_equal(cl4.size, 4);
	assert_int_equal(cl4.marker, 31);
	assert_memory_equal(cl4.members, ref_members4, 4 * sizeof(iscc_Dpid));

	free(wa.dpid_array1);
	free(wa.dpid_array2);
	free(wa.dist_array);
	free(wa.vertex_markers);
}


void scc_ut_hi_find_centers_second(void** state)
{
	(void) state;

	iscc_hi_WorkArea wa = {
		.dpid_array1 = malloc(sizeof(iscc_Dpid[100])),
		.dpid_array2 = malloc(sizeof(iscc_Dpid[100])),
		.dist_array = malloc(sizeof(double[100])),
		.vertex_markers = calloc(100, sizeof(uint_fast16_t)),
		.edge_store1 = NULL,
		.edge_store2 = NULL,
	};

	iscc_Dpid members1[40] = { 34, 42, 78, 27, 99, 67, 29, 18, 92, 25,
	                           33, 23, 55, 82, 21, 79, 70, 85, 83, 61,
	                           16, 57, 24, 35, 12, 95, 60, 7, 86, 48,
	                           46, 76, 75, 54, 59, 38, 11, 6, 39, 13 };
	iscc_hi_ClusterItem cl1 = {
		.size = 40,
		.marker = 10,
		.members = members1,
	};

	iscc_Dpid center1, center2;
	scc_ErrorCode ec1 = iscc_hi_find_centers(&cl1, scc_ut_test_data_large, &wa, &center1, &center2);
	assert_int_equal(ec1, SCC_ER_OK);
	assert_int_equal(center1, 78);
	assert_int_equal(center2, 55);

	free(wa.dpid_array1);
	free(wa.dpid_array2);
	free(wa.dist_array);
	free(wa.vertex_markers);
}


void scc_ut_hi_populate_edge_lists(void** state)
{
	(void) state;

	iscc_Dpid members[4] = { 10, 3, 5, 2 };
	iscc_hi_ClusterItem cl = {
		.size = 4,
		.marker = 0,
		.members = members,
	};

	iscc_hi_WorkArea wa = {
		.dpid_array1 = NULL,
		.dpid_array2 = NULL,
		.dist_array = malloc(sizeof(double[8])),
		.vertex_markers = NULL,
		.edge_store1 = malloc(sizeof(iscc_hi_DistanceEdge[4])),
		.edge_store2 = malloc(sizeof(iscc_hi_DistanceEdge[4])),
	};

	scc_ErrorCode ec = iscc_hi_populate_edge_lists(&cl, scc_ut_test_data_large, 10, 5, &wa);
	assert_int_equal(ec, SCC_ER_OK);

	assert_int_equal(wa.edge_store1[1].head, 3);
	assert_double_equal(wa.edge_store1[1].distance, 65.042314);
	assert_int_equal(wa.edge_store1[2].head, 5);
	assert_double_equal(wa.edge_store1[2].distance, 82.967209);
	assert_int_equal(wa.edge_store1[3].head, 2);
	assert_double_equal(wa.edge_store1[3].distance, 102.986773);
	for (size_t i = 0; i < 3; ++i) {
		assert_ptr_equal(wa.edge_store1[i].next_dist, &wa.edge_store1[i + 1]);
    }
    assert_null(wa.edge_store1[3].next_dist);

	assert_int_equal(wa.edge_store2[1].head, 2);
	assert_double_equal(wa.edge_store2[1].distance, 21.423179);
	assert_int_equal(wa.edge_store2[2].head, 3);
	assert_double_equal(wa.edge_store2[2].distance, 52.901061);
	assert_int_equal(wa.edge_store2[3].head, 10);
	assert_double_equal(wa.edge_store2[3].distance, 82.967209);
	for (size_t i = 0; i < 3; ++i) {
		assert_ptr_equal(wa.edge_store2[i].next_dist, &wa.edge_store2[i + 1]);
    }
    assert_null(wa.edge_store2[3].next_dist);

    free(wa.dist_array);
    free(wa.edge_store1);
    free(wa.edge_store2);
}


void scc_ut_hi_sort_edge_list(void** state)
{
	(void) state;

	iscc_Dpid mem[10] = { 1, 6, 3, 8, 9, 4, 10, 12, 13, 14 };

	iscc_hi_ClusterItem ci = {
		.size = 10,
		.marker = 0,
		.members = mem,
	};

	double output_dists[10] = { 10.4, 1.4, 6.2, 5.2, 0.0, 1.2, 9.5, 3.3, 9.6, 3.1 };

	iscc_hi_DistanceEdge* const edge_store = malloc(sizeof(iscc_hi_DistanceEdge[10]));

	iscc_hi_sort_edge_list(&ci, 9, output_dists, edge_store);

	assert_int_equal(edge_store[1].head, 4);
	assert_double_equal(edge_store[1].distance, 1.2);
	assert_int_equal(edge_store[2].head, 6);
	assert_double_equal(edge_store[2].distance, 1.4);
	assert_int_equal(edge_store[3].head, 14);
	assert_double_equal(edge_store[3].distance, 3.1);
	assert_int_equal(edge_store[4].head, 12);
	assert_double_equal(edge_store[4].distance, 3.3);
	assert_int_equal(edge_store[5].head, 8);
	assert_double_equal(edge_store[5].distance, 5.2);
	assert_int_equal(edge_store[6].head, 3);
	assert_double_equal(edge_store[6].distance, 6.2);
	assert_int_equal(edge_store[7].head, 10);
	assert_double_equal(edge_store[7].distance, 9.5);
	assert_int_equal(edge_store[8].head, 13);
	assert_double_equal(edge_store[8].distance, 9.6);
	assert_int_equal(edge_store[9].head, 1);
	assert_double_equal(edge_store[9].distance, 10.4);

	for (size_t i = 0; i < 9; ++i) {
		assert_ptr_equal(edge_store[i].next_dist, &edge_store[i + 1]);
    }
    assert_null(edge_store[9].next_dist);

    free(edge_store);
}


void scc_ut_hi_compare_dist_edges(void** state)
{
	(void) state;

	iscc_hi_DistanceEdge d1 = {
		.head = 1,
		.distance = 0.5,
		.next_dist = NULL,
	};
	iscc_hi_DistanceEdge d2 = {
		.head = 2,
		.distance = 0.5,
		.next_dist = NULL,
	};
	iscc_hi_DistanceEdge d3 = {
		.head = 3,
		.distance = 2.5,
		.next_dist = NULL,
	};

	assert_int_equal(iscc_hi_compare_dist_edges(&d1, &d2), 0);
	assert_int_equal(iscc_hi_compare_dist_edges(&d1, &d3), -1);
	assert_int_equal(iscc_hi_compare_dist_edges(&d3, &d2), 1);
}


int main(void)
{
	scc_ut_init_tests();

	const struct CMUnitTest test_cases[] = {
		cmocka_unit_test(scc_ut_hi_empty_cl_stack),
		cmocka_unit_test(scc_ut_hi_init_cl_stack),
		cmocka_unit_test(scc_ut_hi_push_to_stack),
		cmocka_unit_test(scc_ut_hi_get_next_marker),
		cmocka_unit_test(scc_ut_hi_compare_dist_edges),
		cmocka_unit_test(scc_ut_hi_sort_edge_list),
		cmocka_unit_test(scc_ut_hi_populate_edge_lists),
		cmocka_unit_test(scc_ut_hi_get_next_dist),
		cmocka_unit_test(scc_ut_hi_get_next_k_nn),
		cmocka_unit_test(scc_ut_hi_move_point_to_cluster1),
		cmocka_unit_test(scc_ut_hi_move_point_to_cluster2),
		cmocka_unit_test(scc_ut_hi_move_array_to_cluster1),
		cmocka_unit_test(scc_ut_hi_move_array_to_cluster2),
		cmocka_unit_test(scc_ut_hi_find_centers),
		cmocka_unit_test(scc_ut_hi_find_centers_second),
		cmocka_unit_test(scc_ut_hi_break_cluster_into_two),
		cmocka_unit_test(scc_ut_hi_run_hierarchical_clustering),
	};

	return cmocka_run_group_tests_name("internal hierarchical_clustering.c", test_cases, NULL, NULL);
}
