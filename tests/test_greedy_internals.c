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

#define SCC_DOUBLE_ASSERT

#include "test_suite.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "../src/clustering_greedy.c"
#include "data_object_test.h"


void scc_ut_init_cl_stack(void** state)
{
	(void) state;

	const scc_Clabel M = SCC_CLABEL_NA;
	scc_Clabel labels1[20] = { 1, 2, 4, 3, 1,
	                           2, 1, 5, 1, 2,
	                           3, 2, 5, 3, 4,
	                           3, 0, 4, 1, 5  };
	scc_Clabel labels2[25] = { 0, M, 2, 3, 3,
	                           5, 3, 0, 4, M,
	                           2, M, 4, 0, 5,
	                           5, 5, 2, 5, M,
	                           6, M, 6, 6, 3  };

	scc_Clustering cl1 = {
		.num_data_points = 20,
		.num_clusters = 6,
		.external_labels = true,
		.cluster_label = labels1,
	};
	scc_Clustering cl2 = {
		.num_data_points = 25,
		.num_clusters = 7,
		.external_labels = true,
		.cluster_label = labels2,
	};

	iscc_gr_ClusterStack cl_stack1 = iscc_gr_init_cl_stack(&cl1);

	assert_int_equal(cl_stack1.capacity, 49);
	assert_int_equal(cl_stack1.items, 6);
	assert_non_null(cl_stack1.clusters);

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

	scc_Dpid ref1_cl0[1] = { 16 };
	scc_Dpid ref1_cl1[5] = { 0, 4, 6, 8, 18 };
	scc_Dpid ref1_cl2[4] = { 1, 5, 9, 11 };
	scc_Dpid ref1_cl3[4] = { 3, 10, 13, 15 };
	scc_Dpid ref1_cl4[3] = { 2, 14, 17 };
	scc_Dpid ref1_cl5[3] = { 7, 12, 19 };

	assert_memory_equal(cl_stack1.clusters[0].members, ref1_cl0, 1 * sizeof(scc_Dpid));
	assert_memory_equal(cl_stack1.clusters[1].members, ref1_cl1, 5 * sizeof(scc_Dpid));
	assert_memory_equal(cl_stack1.clusters[2].members, ref1_cl2, 4 * sizeof(scc_Dpid));
	assert_memory_equal(cl_stack1.clusters[3].members, ref1_cl3, 4 * sizeof(scc_Dpid));
	assert_memory_equal(cl_stack1.clusters[4].members, ref1_cl4, 3 * sizeof(scc_Dpid));
	assert_memory_equal(cl_stack1.clusters[5].members, ref1_cl5, 3 * sizeof(scc_Dpid));

	for (size_t i = 6; i < cl_stack1.capacity; ++i) {
		assert_int_equal(cl_stack1.clusters[i].size, 0);
		assert_null(cl_stack1.clusters[i].members);
	}


	iscc_gr_ClusterStack cl_stack2 = iscc_gr_init_cl_stack(&cl2);

	assert_int_equal(cl_stack2.capacity, 53);
	assert_int_equal(cl_stack2.items, 7);
	assert_non_null(cl_stack2.clusters);

	assert_int_equal(cl_stack2.clusters[0].size, 3);
	assert_int_equal(cl_stack2.clusters[1].size, 0);
	assert_int_equal(cl_stack2.clusters[2].size, 3);
	assert_int_equal(cl_stack2.clusters[3].size, 4);
	assert_int_equal(cl_stack2.clusters[4].size, 2);
	assert_int_equal(cl_stack2.clusters[5].size, 5);
	assert_int_equal(cl_stack2.clusters[6].size, 3);

	assert_int_equal(cl_stack2.clusters[0].marker, 0);
	assert_int_equal(cl_stack2.clusters[1].marker, 0);
	assert_int_equal(cl_stack2.clusters[2].marker, 0);
	assert_int_equal(cl_stack2.clusters[3].marker, 0);
	assert_int_equal(cl_stack2.clusters[4].marker, 0);
	assert_int_equal(cl_stack2.clusters[5].marker, 0);
	assert_int_equal(cl_stack2.clusters[6].marker, 0);

	scc_Dpid ref2_cl0[3] = { 0, 7, 13 };
	scc_Dpid ref2_cl2[3] = { 2, 10, 17 };
	scc_Dpid ref2_cl3[4] = { 3, 4, 6, 24 };
	scc_Dpid ref2_cl4[2] = { 8, 12 };
	scc_Dpid ref2_cl5[5] = { 5, 14, 15, 16, 18 };
	scc_Dpid ref2_cl6[3] = { 20, 22, 23 };

	assert_memory_equal(cl_stack2.clusters[0].members, ref2_cl0, 3 * sizeof(scc_Dpid));
	assert_null(cl_stack2.clusters[1].members);
	assert_memory_equal(cl_stack2.clusters[2].members, ref2_cl2, 3 * sizeof(scc_Dpid));
	assert_memory_equal(cl_stack2.clusters[3].members, ref2_cl3, 4 * sizeof(scc_Dpid));
	assert_memory_equal(cl_stack2.clusters[4].members, ref2_cl4, 2 * sizeof(scc_Dpid));
	assert_memory_equal(cl_stack2.clusters[5].members, ref2_cl5, 5 * sizeof(scc_Dpid));
	assert_memory_equal(cl_stack2.clusters[6].members, ref2_cl6, 3 * sizeof(scc_Dpid));

	for (size_t i = 7; i < cl_stack2.capacity; ++i) {
		assert_int_equal(cl_stack2.clusters[i].size, 0);
		assert_null(cl_stack2.clusters[i].members);
	}

	free(cl_stack1.clusters[0].members);
	free(cl_stack1.clusters[1].members);
	free(cl_stack1.clusters[2].members);
	free(cl_stack1.clusters[3].members);
	free(cl_stack1.clusters[4].members);
	free(cl_stack1.clusters[5].members);
	free(cl_stack1.clusters);

	free(cl_stack2.clusters[0].members);
	free(cl_stack2.clusters[2].members);
	free(cl_stack2.clusters[3].members);
	free(cl_stack2.clusters[4].members);
	free(cl_stack2.clusters[5].members);
	free(cl_stack2.clusters[6].members);
	free(cl_stack2.clusters);
}


void scc_ut_empty_cl_stack(void** state)
{
	(void) state;

	iscc_gr_ClusterStack cl_stack1 = iscc_gr_empty_cl_stack(10);

	assert_int_equal(cl_stack1.capacity, 67);
	assert_int_equal(cl_stack1.items, 1);
	assert_non_null(cl_stack1.clusters);

	assert_int_equal(cl_stack1.clusters[0].size, 10);
	assert_int_equal(cl_stack1.clusters[0].marker, 0);

	scc_Dpid ref1_cl[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

	assert_memory_equal(cl_stack1.clusters[0].members, ref1_cl, 10 * sizeof(scc_Dpid));

	cl_stack1.clusters[cl_stack1.capacity - 1].size = 0;


	iscc_gr_ClusterStack cl_stack2 = iscc_gr_empty_cl_stack(20);

	assert_int_equal(cl_stack2.capacity, 87);
	assert_int_equal(cl_stack2.items, 1);
	assert_non_null(cl_stack2.clusters);

	assert_int_equal(cl_stack2.clusters[0].size, 20);
	assert_int_equal(cl_stack2.clusters[0].marker, 0);

	scc_Dpid ref2_cl[20] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };

	assert_memory_equal(cl_stack2.clusters[0].members, ref2_cl, 20 * sizeof(scc_Dpid));

	cl_stack2.clusters[cl_stack2.capacity - 1].size = 0;


	free(cl_stack1.clusters[0].members);
	free(cl_stack1.clusters);

	free(cl_stack2.clusters[0].members);
	free(cl_stack2.clusters);
}


void scc_ut_free_cl_strack(void** state)
{
	(void) state;

	const scc_Clabel M = SCC_CLABEL_NA;
	scc_Clabel labels1[20] = { 1, 2, 4, 3, 1,
	                           2, 1, 5, 1, 2,
	                           3, 2, 5, 3, 4,
	                           3, 0, 4, 1, 5  };
	scc_Clabel labels2[25] = { 0, M, 2, 3, 3,
	                           5, 3, 0, 4, M,
	                           2, M, 4, 0, 5,
	                           5, 5, 2, 5, M,
	                           6, M, 6, 6, 3  };

	scc_Clustering cl1 = {
		.num_data_points = 20,
		.num_clusters = 6,
		.external_labels = true,
		.cluster_label = labels1,
	};
	scc_Clustering cl2 = {
		.num_data_points = 25,
		.num_clusters = 7,
		.external_labels = true,
		.cluster_label = labels2,
	};

	iscc_gr_ClusterStack cl_stack1 = iscc_gr_init_cl_stack(&cl1);
	iscc_gr_ClusterStack cl_stack2 = iscc_gr_init_cl_stack(&cl2);

	iscc_gr_free_cl_strack(&cl_stack1);
	iscc_gr_free_cl_strack(&cl_stack2);
}


void scc_ut_run_greedy_clustering(void** state)
{
	(void) state;

	scc_Clustering cl1 = {
		.num_data_points = 100,
		.num_clusters = 0,
		.external_labels = false,
		.cluster_label = malloc(sizeof(scc_Clabel[100])),
	};
	iscc_gr_ClusterStack cl_stack1 = iscc_gr_empty_cl_stack(100);
	assert_true(iscc_gr_run_greedy_clustering(&scc_ut_test_data_large,
	                                          &cl_stack1,
	                                          &cl1,
	                                          20,
	                                          true));
	scc_Clabel ref_label1[100] = { 2, 3, 3, 2, 2, 3, 0, 0, 4, 3, 2, 1, 1, 0, 4,
	                               3, 0, 2, 0, 4, 3, 1, 3, 0, 0, 0, 4, 0, 4, 0,
	                               3, 4, 3, 1, 0, 0, 3, 4, 1, 0, 3, 2, 1, 2, 2,
	                               2, 0, 2, 1, 4, 3, 4, 4, 4, 1, 0, 3, 1, 3, 1,
	                               1, 1, 4, 4, 3, 4, 4, 1, 2, 3, 1, 4, 2, 3, 4,
	                               0, 0, 4, 1, 1, 3, 2, 1, 0, 2, 1, 0, 2, 2, 2,
	                               4, 2, 1, 2, 2, 1, 3, 4, 3, 0 };
	assert_int_equal(cl_stack1.items, 0);
	assert_int_equal(cl1.num_data_points, 100);
	assert_int_equal(cl1.num_clusters, 5);
	assert_false(cl1.external_labels);
	assert_memory_equal(cl1.cluster_label, ref_label1, 100 * sizeof(scc_Clabel));


	scc_Clustering cl2 = {
		.num_data_points = 100,
		.num_clusters = 0,
		.external_labels = false,
		.cluster_label = malloc(sizeof(scc_Clabel[100])),
	};
	iscc_gr_ClusterStack cl_stack2 = iscc_gr_empty_cl_stack(100);
	assert_true(iscc_gr_run_greedy_clustering(&scc_ut_test_data_large,
	                                          &cl_stack2,
	                                          &cl2,
	                                          20,
	                                          false));
	scc_Clabel ref_label2[100] = { 3, 0, 2, 3, 3, 2, 1, 1, 3, 2, 3, 0, 1, 0, 2, 2, 1, 3,
	                               0, 2, 2, 0, 1, 1, 0, 1, 2, 1, 2, 0, 2, 2, 2, 0, 1, 1,
	                               2, 2, 0, 0, 3, 3, 0, 3, 3, 0, 1, 3, 0, 2, 0, 2, 2, 2,
	                               0, 0, 2, 0, 2, 1, 0, 0, 3, 3, 3, 3, 1, 1, 3, 2, 0, 1,
	                               3, 2, 2, 1, 1, 3, 0, 0, 2, 3, 0, 1, 3, 0, 1, 3, 3, 3,
	                               3, 2, 0, 3, 3, 1, 0, 3, 2, 0 };
	assert_int_equal(cl_stack2.items, 0);
	assert_int_equal(cl2.num_data_points, 100);
	assert_int_equal(cl2.num_clusters, 4);
	assert_false(cl2.external_labels);
	assert_memory_equal(cl2.cluster_label, ref_label2, 100 * sizeof(scc_Clabel));


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
		.external_labels = true,
		.cluster_label = cluster_label3,
	};
	iscc_gr_ClusterStack cl_stack3 = iscc_gr_init_cl_stack(&cl3);
	assert_true(iscc_gr_run_greedy_clustering(&scc_ut_test_data_large,
	                                          &cl_stack3,
	                                          &cl3,
	                                          20,
	                                          true));
	scc_Clabel ref_label3[100] = { 1, 1, 2, 2, 2, 0, 0, 3, 0, 0, 2, 1, 3, 3, 2,
	                               1, 0, 2, 1, 0, 0, 3, 1, 1, 1, 0, 1, 3, 0, 3,
	                               2, 0, 0, 1, 3, 0, 2, 3, 1, 1, 1, 2, 3, 2, 1,
	                               3, 3, 3, 1, 0, 3, 3, 2, 2, 3, 3, 2, 1, 0, 3,
	                               1, 3, 1, 0, 2, 2, 3, 3, 1, 0, 3, 0, 2, 1, 2,
	                               0, 3, 1, 1, 1, 3, 1, 1, 3, 2, 1, 3, 3, 1, 1,
	                               0, 1, 3, 2, 3, 3, 1, 2, 0, 3 };
	assert_int_equal(cl_stack3.items, 0);
	assert_int_equal(cl3.num_data_points, 100);
	assert_int_equal(cl3.num_clusters, 4);
	assert_true(cl3.external_labels);
	assert_memory_equal(cl3.cluster_label, ref_label3, 100 * sizeof(scc_Clabel));


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
		.external_labels = true,
		.cluster_label = cluster_label4,
	};
	iscc_gr_ClusterStack cl_stack4 = iscc_gr_init_cl_stack(&cl4);
	assert_true(iscc_gr_run_greedy_clustering(&scc_ut_test_data_large,
	                                          &cl_stack4,
	                                          &cl4,
	                                          20,
	                                          false));
	scc_Clabel ref_label4[100] = { 1, 0, 2, 2, 2, 0, 0, 3, 0, 0, 2, 0, 3, 3, 2, 1, 0,
	                               2, 1, 0, 0, 3, 1, 1, 1, 0, 0, 3, 0, 3, 2, 0, 0, 1,
	                               3, 0, 2, 2, 0, 0, 1, 2, 3, 2, 1, 3, 3, 3, 0, 0, 3,
	                               2, 2, 2, 3, 3, 2, 1, 0, 3, 1, 3, 1, 0, 2, 2, 3, 3,
	                               1, 0, 3, 0, 2, 0, 2, 0, 3, 1, 1, 1, 2, 1, 1, 3, 2,
	                               1, 3, 2, 1, 1, 0, 1, 3, 2, 2, 3, 1, 2, 0, 3 };
	assert_int_equal(cl_stack4.items, 0);
	assert_int_equal(cl4.num_data_points, 100);
	assert_int_equal(cl4.num_clusters, 4);
	assert_true(cl4.external_labels);
	assert_memory_equal(cl4.cluster_label, ref_label4, 100 * sizeof(scc_Clabel));


	scc_free_clustering(&cl1);
	scc_free_clustering(&cl2);
	scc_free_clustering(&cl3);
	scc_free_clustering(&cl4);
	iscc_gr_free_cl_strack(&cl_stack1);
	iscc_gr_free_cl_strack(&cl_stack2);
	iscc_gr_free_cl_strack(&cl_stack3);
	iscc_gr_free_cl_strack(&cl_stack4);
}


void scc_ut_peek_at_stack(void** state)
{
	(void) state;

	scc_Clabel labels[20] = { 1, 2, 4, 3, 1,
	                          2, 1, 5, 1, 2,
	                          3, 2, 5, 3, 4,
	                          3, 0, 4, 1, 5  };
	scc_Clustering cl = {
		.num_data_points = 20,
		.num_clusters = 6,
		.external_labels = true,
		.cluster_label = labels,
	};

	iscc_gr_ClusterStack cl_stack = iscc_gr_init_cl_stack(&cl);

	assert_int_equal(cl_stack.capacity, 49);
	assert_int_equal(cl_stack.items, 6);
	assert_non_null(cl_stack.clusters);

	assert_ptr_equal(iscc_gr_peek_at_stack(&cl_stack), &cl_stack.clusters[5]);
	assert_int_equal(cl_stack.items, 6);
	assert_ptr_equal(iscc_gr_peek_at_stack(&cl_stack), &cl_stack.clusters[5]);
	assert_int_equal(cl_stack.items, 6);

	cl_stack.items = 4;

	assert_ptr_equal(iscc_gr_peek_at_stack(&cl_stack), &cl_stack.clusters[3]);
	assert_int_equal(cl_stack.items, 4);
	assert_ptr_equal(iscc_gr_peek_at_stack(&cl_stack), &cl_stack.clusters[3]);
	assert_int_equal(cl_stack.items, 4);

	cl_stack.items = 0;

	assert_null(iscc_gr_peek_at_stack(&cl_stack));
	assert_int_equal(cl_stack.items, 0);
	assert_null(iscc_gr_peek_at_stack(&cl_stack));
	assert_int_equal(cl_stack.items, 0);

	cl_stack.items = 6;


	assert_int_equal(cl_stack.capacity, 49);
	assert_int_equal(cl_stack.items, 6);
	assert_non_null(cl_stack.clusters);

	assert_int_equal(cl_stack.clusters[0].size, 1);
	assert_int_equal(cl_stack.clusters[1].size, 5);
	assert_int_equal(cl_stack.clusters[2].size, 4);
	assert_int_equal(cl_stack.clusters[3].size, 4);
	assert_int_equal(cl_stack.clusters[4].size, 3);
	assert_int_equal(cl_stack.clusters[5].size, 3);

	assert_int_equal(cl_stack.clusters[0].marker, 0);
	assert_int_equal(cl_stack.clusters[1].marker, 0);
	assert_int_equal(cl_stack.clusters[2].marker, 0);
	assert_int_equal(cl_stack.clusters[3].marker, 0);
	assert_int_equal(cl_stack.clusters[4].marker, 0);
	assert_int_equal(cl_stack.clusters[5].marker, 0);

	scc_Dpid ref1_cl0[1] = { 16 };
	scc_Dpid ref1_cl1[5] = { 0, 4, 6, 8, 18 };
	scc_Dpid ref1_cl2[4] = { 1, 5, 9, 11 };
	scc_Dpid ref1_cl3[4] = { 3, 10, 13, 15 };
	scc_Dpid ref1_cl4[3] = { 2, 14, 17 };
	scc_Dpid ref1_cl5[3] = { 7, 12, 19 };

	assert_memory_equal(cl_stack.clusters[0].members, ref1_cl0, 1 * sizeof(scc_Dpid));
	assert_memory_equal(cl_stack.clusters[1].members, ref1_cl1, 5 * sizeof(scc_Dpid));
	assert_memory_equal(cl_stack.clusters[2].members, ref1_cl2, 4 * sizeof(scc_Dpid));
	assert_memory_equal(cl_stack.clusters[3].members, ref1_cl3, 4 * sizeof(scc_Dpid));
	assert_memory_equal(cl_stack.clusters[4].members, ref1_cl4, 3 * sizeof(scc_Dpid));
	assert_memory_equal(cl_stack.clusters[5].members, ref1_cl5, 3 * sizeof(scc_Dpid));

	for (size_t i = 6; i < cl_stack.capacity; ++i) {
		assert_int_equal(cl_stack.clusters[i].size, 0);
		assert_null(cl_stack.clusters[i].members);
	}

	free(cl_stack.clusters[0].members);
	free(cl_stack.clusters[1].members);
	free(cl_stack.clusters[2].members);
	free(cl_stack.clusters[3].members);
	free(cl_stack.clusters[4].members);
	free(cl_stack.clusters[5].members);
	free(cl_stack.clusters);
}


void scc_ut_pop_from_stack(void** state)
{
	(void) state;

	scc_Clabel labels[20] = { 1, 2, 4, 3, 1,
	                          2, 1, 5, 1, 2,
	                          3, 2, 5, 3, 4,
	                          3, 0, 4, 1, 5  };
	scc_Clustering cl = {
		.num_data_points = 20,
		.num_clusters = 6,
		.external_labels = true,
		.cluster_label = labels,
	};

	iscc_gr_ClusterStack cl_stack = iscc_gr_init_cl_stack(&cl);

	assert_int_equal(cl_stack.capacity, 49);
	assert_int_equal(cl_stack.items, 6);
	assert_non_null(cl_stack.clusters);

	scc_Dpid ref1_cl0[1] = { 16 };
	scc_Dpid ref1_cl1[5] = { 0, 4, 6, 8, 18 };
	scc_Dpid ref1_cl2[4] = { 1, 5, 9, 11 };
	scc_Dpid ref1_cl3[4] = { 3, 10, 13, 15 };
	scc_Dpid ref1_cl4[3] = { 2, 14, 17 };
	scc_Dpid ref1_cl5[3] = { 7, 12, 19 };

	iscc_gr_ClusterItem next_cl;

	next_cl = iscc_gr_pop_from_stack(&cl_stack);
	assert_int_equal(cl_stack.capacity, 49);
	assert_int_equal(cl_stack.items, 5);
	assert_non_null(cl_stack.clusters);
	assert_int_equal(next_cl.size, 3);
	assert_int_equal(next_cl.marker, 0);
	assert_memory_equal(next_cl.members, ref1_cl5, 3 * sizeof(scc_Dpid));
	free(next_cl.members);

	next_cl = iscc_gr_pop_from_stack(&cl_stack);
	assert_int_equal(cl_stack.capacity, 49);
	assert_int_equal(cl_stack.items, 4);
	assert_non_null(cl_stack.clusters);
	assert_int_equal(next_cl.size, 3);
	assert_int_equal(next_cl.marker, 0);
	assert_memory_equal(next_cl.members, ref1_cl4, 3 * sizeof(scc_Dpid));
	free(next_cl.members);

	next_cl = iscc_gr_pop_from_stack(&cl_stack);
	assert_int_equal(cl_stack.capacity, 49);
	assert_int_equal(cl_stack.items, 3);
	assert_non_null(cl_stack.clusters);
	assert_int_equal(next_cl.size, 4);
	assert_int_equal(next_cl.marker, 0);
	assert_memory_equal(next_cl.members, ref1_cl3, 4 * sizeof(scc_Dpid));
	free(next_cl.members);

	next_cl = iscc_gr_pop_from_stack(&cl_stack);
	assert_int_equal(cl_stack.capacity, 49);
	assert_int_equal(cl_stack.items, 2);
	assert_non_null(cl_stack.clusters);
	assert_int_equal(next_cl.size, 4);
	assert_int_equal(next_cl.marker, 0);
	assert_memory_equal(next_cl.members, ref1_cl2, 4 * sizeof(scc_Dpid));
	free(next_cl.members);

	next_cl = iscc_gr_pop_from_stack(&cl_stack);
	assert_int_equal(cl_stack.capacity, 49);
	assert_int_equal(cl_stack.items, 1);
	assert_non_null(cl_stack.clusters);
	assert_int_equal(next_cl.size, 5);
	assert_int_equal(next_cl.marker, 0);
	assert_memory_equal(next_cl.members, ref1_cl1, 5 * sizeof(scc_Dpid));
	free(next_cl.members);

	next_cl = iscc_gr_pop_from_stack(&cl_stack);
	assert_int_equal(cl_stack.capacity, 49);
	assert_int_equal(cl_stack.items, 0);
	assert_non_null(cl_stack.clusters);
	assert_int_equal(next_cl.size, 1);
	assert_int_equal(next_cl.marker, 0);
	assert_memory_equal(next_cl.members, ref1_cl0, 1 * sizeof(scc_Dpid));
	free(next_cl.members);

	free(cl_stack.clusters);
}


void scc_ut_push_to_stack(void** state)
{
	(void) state;

	iscc_gr_ClusterStack cl_stack = {
		.capacity = 3,
		.items = 2,
		.clusters = calloc(3, sizeof(iscc_gr_ClusterItem)),
	};

	iscc_gr_ClusterItem cl1 = {
		.size = 10,
		.marker = 0,
		.members = malloc(sizeof(scc_Dpid[10])),
	};
	iscc_gr_ClusterItem cl2 = {
		.size = 15,
		.marker = 0,
		.members = malloc(sizeof(scc_Dpid[15])),
	};
	iscc_gr_ClusterItem cl3 = {
		.size = 20,
		.marker = 0,
		.members = malloc(sizeof(scc_Dpid[20])),
	};

	assert_true(iscc_gr_push_to_stack(&cl_stack, cl1));
	assert_int_equal(cl_stack.capacity, 3);
	assert_int_equal(cl_stack.items, 3);
	assert_non_null(cl_stack.clusters);
	assert_memory_equal(&cl_stack.clusters[2], &cl1, sizeof(iscc_gr_ClusterItem));

	assert_true(iscc_gr_push_to_stack(&cl_stack, cl2));
	assert_int_equal(cl_stack.capacity, 82);
	assert_int_equal(cl_stack.items, 4);
	assert_non_null(cl_stack.clusters);
	assert_memory_equal(&cl_stack.clusters[3], &cl2, sizeof(iscc_gr_ClusterItem));

	assert_true(iscc_gr_push_to_stack(&cl_stack, cl3));
	assert_int_equal(cl_stack.capacity, 82);
	assert_int_equal(cl_stack.items, 5);
	assert_non_null(cl_stack.clusters);
	assert_memory_equal(&cl_stack.clusters[4], &cl3, sizeof(iscc_gr_ClusterItem));

	free(cl_stack.clusters);
	free(cl1.members);
	free(cl2.members);
	free(cl3.members);
}


void scc_ut_break_cluster_into_two(void** state)
{
	(void) state;

	uint_fast16_t* vertex_markers = calloc(100, sizeof(uint_fast16_t));

	iscc_gr_ClusterItem cl1 = {
		.size = 10,
		.marker = 0,
		.members = malloc(sizeof(scc_Dpid[10])),
	};
	scc_Dpid members1_ref[10] = { 2, 4, 6, 8, 10, 12, 14, 16, 18, 20 };
	for (size_t i = 0; i < 10; ++i) {
		cl1.members[i] = members1_ref[i];
	}

	iscc_gr_ClusterItem cl2 = {
		.size = 10,
		.marker = 0,
		.members = malloc(sizeof(scc_Dpid[10])),
	};
	scc_Dpid members2_ref[10] = { 22, 21, 23, 24, 25, 26, 27, 28, 29, 30 };
	for (size_t i = 0; i < 10; ++i) {
		cl2.members[i] = members2_ref[i];
	}

	iscc_gr_ClusterItem cl1_a = iscc_gr_break_cluster_into_two(&scc_ut_test_data_large, &cl1, 3, true, vertex_markers);
	scc_Dpid ref_cl1[6] = { 2, 20, 14, 4, 12, 16 };
	scc_Dpid ref_cl2[4] = { 18, 10, 6, 8 };
	assert_int_equal(cl1.size, 6);
	assert_int_equal(cl1.marker, 2);
	assert_memory_equal(cl1.members, ref_cl1, cl1.size * sizeof(scc_Dpid));
	assert_int_equal(cl1_a.size, 4);
	assert_int_equal(cl1_a.marker, 2);
	assert_memory_equal(cl1_a.members, ref_cl2, cl1_a.size * sizeof(scc_Dpid));
	assert_int_equal(vertex_markers[2], 2);
	assert_int_equal(vertex_markers[4], 2);
	assert_int_equal(vertex_markers[6], 2);
	assert_int_equal(vertex_markers[8], 2);
	assert_int_equal(vertex_markers[10], 2);
	assert_int_equal(vertex_markers[12], 2);
	assert_int_equal(vertex_markers[14], 2);
	assert_int_equal(vertex_markers[16], 2);
	assert_int_equal(vertex_markers[18], 2);
	assert_int_equal(vertex_markers[20], 2);

	iscc_gr_ClusterItem cl1_b = iscc_gr_break_cluster_into_two(&scc_ut_test_data_large, &cl1, 3, true, vertex_markers);
	scc_Dpid ref_cl3[3] = { 2, 20, 4 };
	scc_Dpid ref_cl4[3] = { 16, 12, 14 };
	assert_int_equal(cl1.size, 3);
	assert_int_equal(cl1.marker, 4);
	assert_memory_equal(cl1.members, ref_cl3, cl1.size * sizeof(scc_Dpid));
	assert_int_equal(cl1_b.size, 3);
	assert_int_equal(cl1_b.marker, 4);
	assert_memory_equal(cl1_b.members, ref_cl4, cl1_b.size * sizeof(scc_Dpid));
	assert_int_equal(vertex_markers[2], 4);
	assert_int_equal(vertex_markers[4], 4);
	assert_int_equal(vertex_markers[6], 2);
	assert_int_equal(vertex_markers[8], 2);
	assert_int_equal(vertex_markers[10], 2);
	assert_int_equal(vertex_markers[12], 4);
	assert_int_equal(vertex_markers[14], 4);
	assert_int_equal(vertex_markers[16], 4);
	assert_int_equal(vertex_markers[18], 2);
	assert_int_equal(vertex_markers[20], 4);


	iscc_gr_ClusterItem cl2_a = iscc_gr_break_cluster_into_two(&scc_ut_test_data_large, &cl2, 2, false, vertex_markers);
	scc_Dpid ref_cl5[5] = { 21, 30, 22, 29, 24 };
	scc_Dpid ref_cl6[5] = { 28, 26, 27, 25, 23 };
	assert_int_equal(cl2.size, 5);
	assert_int_equal(cl2.marker, 2);
	assert_memory_equal(cl2.members, ref_cl5, cl2.size * sizeof(scc_Dpid));
	assert_int_equal(cl2_a.size, 5);
	assert_int_equal(cl2_a.marker, 2);
	assert_memory_equal(cl2_a.members, ref_cl6, cl2_a.size * sizeof(scc_Dpid));
	assert_int_equal(vertex_markers[21], 2);
	assert_int_equal(vertex_markers[22], 2);
	assert_int_equal(vertex_markers[23], 2);
	assert_int_equal(vertex_markers[24], 2);
	assert_int_equal(vertex_markers[25], 2);
	assert_int_equal(vertex_markers[26], 2);
	assert_int_equal(vertex_markers[27], 2);
	assert_int_equal(vertex_markers[28], 2);
	assert_int_equal(vertex_markers[29], 2);
	assert_int_equal(vertex_markers[30], 2);

	iscc_gr_ClusterItem cl2_b = iscc_gr_break_cluster_into_two(&scc_ut_test_data_large, &cl2_a, 2, false, vertex_markers);
	scc_Dpid ref_cl7[3] = { 23, 27, 25 };
	scc_Dpid ref_cl8[2] = { 26, 28 };
	assert_int_equal(cl2_a.size, 3);
	assert_int_equal(cl2_a.marker, 4);
	assert_memory_equal(cl2_a.members, ref_cl7, cl2_a.size * sizeof(scc_Dpid));
	assert_int_equal(cl2_b.size, 2);
	assert_int_equal(cl2_b.marker, 4);
	assert_memory_equal(cl2_b.members, ref_cl8, cl2_b.size * sizeof(scc_Dpid));
	assert_int_equal(vertex_markers[21], 2);
	assert_int_equal(vertex_markers[22], 2);
	assert_int_equal(vertex_markers[23], 4);
	assert_int_equal(vertex_markers[24], 2);
	assert_int_equal(vertex_markers[25], 4);
	assert_int_equal(vertex_markers[26], 4);
	assert_int_equal(vertex_markers[27], 4);
	assert_int_equal(vertex_markers[28], 4);
	assert_int_equal(vertex_markers[29], 2);
	assert_int_equal(vertex_markers[30], 2);

	iscc_gr_ClusterItem cl2_c = iscc_gr_break_cluster_into_two(&scc_ut_test_data_large, &cl2, 2, false, vertex_markers);
	scc_Dpid ref_cl9[2] = { 29, 24 };
	scc_Dpid ref_cl10[3] = { 30, 22, 21 };
	assert_int_equal(cl2.size, 2);
	assert_int_equal(cl2.marker, 4);
	assert_memory_equal(cl2.members, ref_cl9, cl2.size * sizeof(scc_Dpid));
	assert_int_equal(cl2_c.size, 3);
	assert_int_equal(cl2_c.marker, 4);
	assert_memory_equal(cl2_c.members, ref_cl10, cl2_c.size * sizeof(scc_Dpid));
	assert_int_equal(vertex_markers[21], 4);
	assert_int_equal(vertex_markers[22], 4);
	assert_int_equal(vertex_markers[23], 4);
	assert_int_equal(vertex_markers[24], 4);
	assert_int_equal(vertex_markers[25], 4);
	assert_int_equal(vertex_markers[26], 4);
	assert_int_equal(vertex_markers[27], 4);
	assert_int_equal(vertex_markers[28], 4);
	assert_int_equal(vertex_markers[29], 4);
	assert_int_equal(vertex_markers[30], 4);

	free(cl1.members);
	free(cl1_a.members);
	free(cl1_b.members);
	free(cl2.members);
	free(cl2_a.members);
	free(cl2_b.members);
	free(cl2_c.members);
	free(vertex_markers);
}


void scc_ut_find_centers(void** state)
{
	(void) state;

	uint_fast16_t* vertex_markers = calloc(100, sizeof(uint_fast16_t));

	scc_Dpid members1[10] = { 2, 4, 6, 8, 10, 12, 14, 16, 18, 20 };
	iscc_gr_ClusterItem cl1 = {
		.size = 10,
		.marker = 0,
		.members = members1,
	};
	
	scc_Dpid members2[5] = { 2, 4, 6, 8, 12 };
	iscc_gr_ClusterItem cl2 = {
		.size = 5,
		.marker = 10,
		.members = members2,
	};

	scc_Dpid members3[10] = { 22, 21, 23, 24, 25, 26, 27, 28, 29, 30 };
	iscc_gr_ClusterItem cl3 = {
		.size = 10,
		.marker = 20,
		.members = members3,
	};

	scc_Dpid members4[4] = { 21, 23, 25, 27 };
	iscc_gr_ClusterItem cl4 = {
		.size = 4,
		.marker = 30,
		.members = members4,
	};

	scc_Dpid center1, center2;

	assert_true(iscc_gr_find_centers(&scc_ut_test_data_large, &cl1, &center1, &center2, vertex_markers));
	assert_int_equal(center1, 2);
	assert_int_equal(center2, 18);

	assert_true(iscc_gr_find_centers(&scc_ut_test_data_large, &cl2, &center1, &center2, vertex_markers));
	assert_int_equal(center1, 2);
	assert_int_equal(center2, 8);

	assert_true(iscc_gr_find_centers(&scc_ut_test_data_large, &cl3, &center1, &center2, vertex_markers));
	assert_int_equal(center1, 21);
	assert_int_equal(center2, 28);

	assert_true(iscc_gr_find_centers(&scc_ut_test_data_large, &cl4, &center1, &center2, vertex_markers));
	assert_int_equal(center1, 21);
	assert_int_equal(center2, 27);

	free(vertex_markers);
}


void scc_ut_get_next_marker(void** state)
{
	(void) state;

	uint_fast16_t* vertex_markers_ref = malloc(sizeof(uint_fast16_t[100]));
	uint_fast16_t* vertex_markers = malloc(sizeof(uint_fast16_t[100]));
	vertex_markers_ref[0] = 0;
	vertex_markers_ref[4] = 0;
	vertex_markers_ref[6] = 0;
	vertex_markers_ref[8] = 0;
	vertex_markers_ref[18] = 0;
	for (size_t i = 0; i < 100; ++i) {
		vertex_markers[i] = vertex_markers_ref[i];
	}

	scc_Dpid mems_ref[5] = { 0, 4, 6, 8, 18 };
	scc_Dpid mems[5] = { 0, 4, 6, 8, 18 };
	iscc_gr_ClusterItem cl1 = {
		.size = 5,
		.marker = 0,
		.members = mems,
	};

	uint_fast16_t next_marker;

	next_marker = iscc_gr_get_next_marker(&cl1, vertex_markers);
	assert_int_equal(next_marker, 1);
	assert_int_equal(cl1.size, 5);
	assert_int_equal(cl1.marker, 1);
	assert_memory_equal(mems, mems_ref, 5 * sizeof(scc_Dpid));
	assert_memory_equal(vertex_markers, vertex_markers_ref, 100 * sizeof(uint_fast16_t));

	next_marker = iscc_gr_get_next_marker(&cl1, vertex_markers);
	assert_int_equal(next_marker, 2);
	assert_int_equal(cl1.size, 5);
	assert_int_equal(cl1.marker, 2);
	assert_memory_equal(mems, mems_ref, 5 * sizeof(scc_Dpid));
	assert_memory_equal(vertex_markers, vertex_markers_ref, 100 * sizeof(uint_fast16_t));

	vertex_markers[0] = 4;
	vertex_markers[4] = 3;
	vertex_markers[6] = 2;
	vertex_markers[8] = 8;
	vertex_markers[18] = 4;
	cl1.marker = UINT_FAST16_MAX;

	next_marker = iscc_gr_get_next_marker(&cl1, vertex_markers);
	assert_int_equal(next_marker, 1);
	assert_int_equal(cl1.size, 5);
	assert_int_equal(cl1.marker, 1);
	assert_memory_equal(mems, mems_ref, 5 * sizeof(scc_Dpid));
	assert_memory_equal(vertex_markers, vertex_markers_ref, 100 * sizeof(uint_fast16_t));

	next_marker = iscc_gr_get_next_marker(&cl1, vertex_markers);
	assert_int_equal(next_marker, 2);
	assert_int_equal(cl1.size, 5);
	assert_int_equal(cl1.marker, 2);
	assert_memory_equal(mems, mems_ref, 5 * sizeof(scc_Dpid));
	assert_memory_equal(vertex_markers, vertex_markers_ref, 100 * sizeof(uint_fast16_t));

	free(vertex_markers_ref);
	free(vertex_markers);
}


void scc_ut_populate_dist_lists(void** state)
{
	(void) state;

	scc_Dpid members[4] = { 10, 3, 5, 2 };
	iscc_gr_ClusterItem cl = {
		.size = 4,
		.marker = 0,
		.members = members,
	};

	iscc_gr_DistanceEdge* const dist_store1 = malloc(sizeof(iscc_gr_DistanceEdge[4]));
	iscc_gr_DistanceEdge* const dist_store2 = malloc(sizeof(iscc_gr_DistanceEdge[4]));

	assert_true(iscc_gr_populate_dist_lists(&scc_ut_test_data_large, &cl, 10, 5, dist_store1, dist_store2));

	assert_int_equal(dist_store1[1].head, 3);
	assert_double_equal(dist_store1[1].distance, 65.042314);
	assert_int_equal(dist_store1[2].head, 5);
	assert_double_equal(dist_store1[2].distance, 82.967209);
	assert_int_equal(dist_store1[3].head, 2);
	assert_double_equal(dist_store1[3].distance, 102.986773);
	for (size_t i = 0; i < 3; ++i) {
		assert_ptr_equal(dist_store1[i].next_dist, &dist_store1[i + 1]);
    }
    assert_null(dist_store1[3].next_dist);

	assert_int_equal(dist_store2[1].head, 2);
	assert_double_equal(dist_store2[1].distance, 21.423179);
	assert_int_equal(dist_store2[2].head, 3);
	assert_double_equal(dist_store2[2].distance, 52.901061);
	assert_int_equal(dist_store2[3].head, 10);
	assert_double_equal(dist_store2[3].distance, 82.967209);
	for (size_t i = 0; i < 3; ++i) {
		assert_ptr_equal(dist_store2[i].next_dist, &dist_store2[i + 1]);
    }
    assert_null(dist_store2[3].next_dist);

    free(dist_store1);
    free(dist_store2);
}


void scc_ut_sort_dist_list(void** state)
{
	(void) state;

	scc_Dpid mem[10] = { 1, 6, 3, 8, 9, 4, 10, 12, 13, 14 };

	iscc_gr_ClusterItem ci = {
		.size = 10,
		.marker = 0,
		.members = mem,
	};

	double output_dists[10] = { 10.4, 1.4, 6.2, 5.2, 0.0, 1.2, 9.5, 3.3, 9.6, 3.1 };

	iscc_gr_DistanceEdge* const dist_store = malloc(sizeof(iscc_gr_DistanceEdge[10]));

	iscc_gr_sort_dist_list(&ci, 9, output_dists, dist_store);

	assert_int_equal(dist_store[1].head, 4);
	assert_double_equal(dist_store[1].distance, 1.2);
	assert_int_equal(dist_store[2].head, 6);
	assert_double_equal(dist_store[2].distance, 1.4);
	assert_int_equal(dist_store[3].head, 14);
	assert_double_equal(dist_store[3].distance, 3.1);
	assert_int_equal(dist_store[4].head, 12);
	assert_double_equal(dist_store[4].distance, 3.3);
	assert_int_equal(dist_store[5].head, 8);
	assert_double_equal(dist_store[5].distance, 5.2);
	assert_int_equal(dist_store[6].head, 3);
	assert_double_equal(dist_store[6].distance, 6.2);
	assert_int_equal(dist_store[7].head, 10);
	assert_double_equal(dist_store[7].distance, 9.5);
	assert_int_equal(dist_store[8].head, 13);
	assert_double_equal(dist_store[8].distance, 9.6);
	assert_int_equal(dist_store[9].head, 1);
	assert_double_equal(dist_store[9].distance, 10.4);

	for (size_t i = 0; i < 9; ++i) {
		assert_ptr_equal(dist_store[i].next_dist, &dist_store[i + 1]);
    }
    assert_null(dist_store[9].next_dist);

    free(dist_store);
}


void scc_ut_compare_dist_edges(void** state)
{
	(void) state;

	iscc_gr_DistanceEdge d1 = {
		.head = 1,
		.distance = 0.5,
		.next_dist = NULL,
	};
	iscc_gr_DistanceEdge d2 = {
		.head = 2,
		.distance = 0.5,
		.next_dist = NULL,
	};
	iscc_gr_DistanceEdge d3 = {
		.head = 3,
		.distance = 2.5,
		.next_dist = NULL,
	};

	assert_int_equal(iscc_gr_compare_dist_edges(&d1, &d2), 0);
	assert_int_equal(iscc_gr_compare_dist_edges(&d1, &d3), -1);
	assert_int_equal(iscc_gr_compare_dist_edges(&d3, &d2), 1);
}


void scc_ut_move_v_to_cluster(void** state)
{
	(void) state;

	uint_fast16_t* vertex_markers = calloc(20, sizeof(uint_fast16_t));

	iscc_gr_ClusterItem cl = {
		.size = 3,
		.marker = 1,
		.members = calloc(20, sizeof(scc_Dpid)),
	};
	cl.members[0] = 4;
	vertex_markers[4] = 1;
	cl.members[1] = 1;
	vertex_markers[1] = 1;
	cl.members[2] = 2;
	vertex_markers[2] = 1;

	iscc_gr_move_v_to_cluster(11, &cl, vertex_markers, 1);
	assert_int_equal(cl.size, 4);
	iscc_gr_move_v_to_cluster(3, &cl, vertex_markers, 1);
	assert_int_equal(cl.size, 5);
	iscc_gr_move_v_to_cluster(6, &cl, vertex_markers, 1);
	assert_int_equal(cl.size, 6);
	iscc_gr_move_v_to_cluster(10, &cl, vertex_markers, 1);
	assert_int_equal(cl.size, 7);
	iscc_gr_move_v_to_cluster(14, &cl, vertex_markers, 1);
	assert_int_equal(cl.size, 8);

	uint_fast16_t v_marks[20] = { 0, 1, 1, 1, 1,
	                              0, 1, 0, 0, 0,
	                              1, 1, 0, 0, 1,
	                              0, 0, 0, 0, 0  };

	assert_memory_equal(vertex_markers, v_marks, 20 * sizeof(uint_fast16_t));

	scc_Dpid cl_members[20] = { 4, 1, 2, 11, 3,
	                           6, 10, 14, 0, 0,
	                           0, 0, 0, 0, 0,
	                           0, 0, 0, 0, 0  };

	assert_memory_equal(cl.members, cl_members, 20 * sizeof(scc_Dpid));

	free(cl.members);
	free(vertex_markers);
}


void scc_ut_get_next_dist(void** state)
{
	(void) state;

	uint_fast16_t* vertex_markers = calloc(100, sizeof(uint_fast16_t));

	scc_Dpid members[5] = { 2, 4, 6, 8, 10 };
	iscc_gr_ClusterItem cl = {
		.size = 5,
		.marker = 0,
		.members = members,
	};

	iscc_gr_DistanceEdge* const dist_store1 = malloc(sizeof(iscc_gr_DistanceEdge[5]));
	iscc_gr_DistanceEdge* const dist_store2 = malloc(sizeof(iscc_gr_DistanceEdge[5]));

	assert_true(iscc_gr_populate_dist_lists(&scc_ut_test_data_large, &cl, 6, 4, dist_store1, dist_store2));

	assert_int_equal(dist_store1[1].head, 4);
	assert_double_equal(dist_store1[1].distance, 72.125847);
	assert_int_equal(dist_store1[2].head, 10);
	assert_double_equal(dist_store1[2].distance, 76.285875);
	assert_int_equal(dist_store1[3].head, 8);
	assert_double_equal(dist_store1[3].distance, 82.249050);
	assert_int_equal(dist_store1[4].head, 2);
	assert_double_equal(dist_store1[4].distance, 103.030113);
	for (size_t i = 0; i < 4; ++i) {
		assert_ptr_equal(dist_store1[i].next_dist, &dist_store1[i + 1]);
    }
    assert_null(dist_store1[4].next_dist);

	assert_int_equal(dist_store2[1].head, 2);
	assert_double_equal(dist_store2[1].distance, 63.103580);
	assert_int_equal(dist_store2[2].head, 10);
	assert_double_equal(dist_store2[2].distance, 67.606177);
	assert_int_equal(dist_store2[3].head, 6);
	assert_double_equal(dist_store2[3].distance, 72.125847);
	assert_int_equal(dist_store2[4].head, 8);
	assert_double_equal(dist_store2[4].distance, 89.098152);
	for (size_t i = 0; i < 4; ++i) {
		assert_ptr_equal(dist_store2[i].next_dist, &dist_store2[i + 1]);
    }
    assert_null(dist_store2[4].next_dist);


	iscc_gr_DistanceEdge* next0 = iscc_gr_get_next_dist(dist_store1, vertex_markers, 1);
	assert_int_equal(next0->head, 4);
	assert_double_equal(next0->distance, 72.125847);
	assert_ptr_equal(next0->next_dist, &dist_store1[2]);

	iscc_gr_DistanceEdge* next1 = iscc_gr_get_next_dist(&dist_store1[2], vertex_markers, 1);
	assert_int_equal(next1->head, 8);
	assert_double_equal(next1->distance, 82.249050);
	assert_ptr_equal(next1->next_dist, &dist_store1[4]);

	iscc_gr_DistanceEdge* next2 = iscc_gr_get_next_dist(&dist_store1[3], vertex_markers, 1);
	assert_int_equal(next2->head, 2);
	assert_double_equal(next2->distance, 103.030113);
	assert_null(next2->next_dist);

	vertex_markers[8] = 1;

	iscc_gr_DistanceEdge* next3 = iscc_gr_get_next_dist(&dist_store1[2], vertex_markers, 1);
	assert_int_equal(next3->head, 2);
	assert_double_equal(next3->distance, 103.030113);
	assert_null(next3->next_dist);

	iscc_gr_DistanceEdge* next4 = iscc_gr_get_next_dist(&dist_store1[2], vertex_markers, 1);
	assert_int_equal(next4->head, 2);
	assert_double_equal(next4->distance, 103.030113);
	assert_null(next4->next_dist);

	assert_int_equal(dist_store1[1].head, 4);
	assert_double_equal(dist_store1[1].distance, 72.125847);
	assert_int_equal(dist_store1[2].head, 10);
	assert_double_equal(dist_store1[2].distance, 76.285875);
	assert_int_equal(dist_store1[3].head, 8);
	assert_double_equal(dist_store1[3].distance, 82.249050);
	assert_int_equal(dist_store1[4].head, 2);
	assert_double_equal(dist_store1[4].distance, 103.030113);

	assert_ptr_equal(dist_store1[0].next_dist, &dist_store1[1]);
	assert_ptr_equal(dist_store1[1].next_dist, &dist_store1[2]);
	assert_ptr_equal(dist_store1[2].next_dist, &dist_store1[4]);
	assert_ptr_equal(dist_store1[3].next_dist, &dist_store1[4]);
	assert_null(dist_store1[4].next_dist);

	vertex_markers[2] = 1;
	vertex_markers[10] = 1;

	iscc_gr_DistanceEdge* next5 = iscc_gr_get_next_dist(dist_store2, vertex_markers, 1);
	assert_int_equal(next5->head, 6);
	assert_double_equal(next5->distance, 72.125847);
	assert_ptr_equal(next5->next_dist, &dist_store2[4]);

	assert_int_equal(dist_store2[1].head, 2);
	assert_double_equal(dist_store2[1].distance, 63.103580);
	assert_int_equal(dist_store2[2].head, 10);
	assert_double_equal(dist_store2[2].distance, 67.606177);
	assert_int_equal(dist_store2[3].head, 6);
	assert_double_equal(dist_store2[3].distance, 72.125847);
	assert_int_equal(dist_store2[4].head, 8);
	assert_double_equal(dist_store2[4].distance, 89.098152);

	assert_ptr_equal(dist_store2[0].next_dist, &dist_store2[3]);
	assert_ptr_equal(dist_store2[1].next_dist, &dist_store2[2]);
	assert_ptr_equal(dist_store2[2].next_dist, &dist_store2[3]);
	assert_ptr_equal(dist_store2[3].next_dist, &dist_store2[4]);
	assert_null(dist_store2[4].next_dist);

    free(vertex_markers);
    free(dist_store1);
    free(dist_store2);
}


void scc_ut_get_next_k_nn(void** state)
{
	(void) state;

	uint_fast16_t* vertex_markers = calloc(100, sizeof(uint_fast16_t));

	scc_Dpid members[10] = { 2, 4, 6, 8, 10, 12, 14, 16, 18, 20 };
	iscc_gr_ClusterItem cl = {
		.size = 10,
		.marker = 0,
		.members = members,
	};

	iscc_gr_DistanceEdge* const dist_store1 = malloc(sizeof(iscc_gr_DistanceEdge[10]));
	iscc_gr_DistanceEdge* const dist_store2 = malloc(sizeof(iscc_gr_DistanceEdge[10]));

	assert_true(iscc_gr_populate_dist_lists(&scc_ut_test_data_large, &cl, 6, 16, dist_store1, dist_store2));

	scc_Dpid out_dist_array0[4];
	scc_Dpid ref_dist_array0[4] = { 12, 16, 18, 4 };
	iscc_gr_DistanceEdge* prev_dist0 = iscc_gr_get_next_k_nn(dist_store1, 4, out_dist_array0, vertex_markers, 1);
	assert_memory_equal(out_dist_array0, ref_dist_array0, 4 * sizeof(scc_Dpid));
	assert_int_equal(prev_dist0->head, 4);
	assert_double_equal(prev_dist0->distance, 72.125847);
	assert_ptr_equal(prev_dist0->next_dist, prev_dist0 + 1);

	scc_Dpid out_dist_array1[4];
	scc_Dpid ref_dist_array1[4] = { 18, 4, 10, 14 };
	iscc_gr_DistanceEdge* prev_dist1 = iscc_gr_get_next_k_nn(&dist_store1[2], 4, out_dist_array1, vertex_markers, 1);
	assert_memory_equal(out_dist_array1, ref_dist_array1, 4 * sizeof(scc_Dpid));
	assert_int_equal(prev_dist1->head, 14);
	assert_double_equal(prev_dist1->distance, 80.566800);
	assert_ptr_equal(prev_dist1->next_dist, prev_dist1 + 1);

	vertex_markers[18] = 1;
	vertex_markers[4] = 1;
	vertex_markers[8] = 1;

	scc_Dpid out_dist_arrayY[4];
	scc_Dpid ref_dist_arrayY[4] = { 12, 16, 10, 14 };
	iscc_gr_DistanceEdge* prev_distY = iscc_gr_get_next_k_nn(dist_store1, 4, out_dist_arrayY, vertex_markers, 1);
	assert_memory_equal(out_dist_arrayY, ref_dist_arrayY, 4 * sizeof(scc_Dpid));
	assert_int_equal(prev_distY->head, 14);
	assert_double_equal(prev_distY->distance, 80.566800);
	assert_ptr_equal(prev_distY->next_dist, prev_distY + 1);

	scc_Dpid out_dist_arrayX[2];
	scc_Dpid ref_dist_arrayX[2] = { 12, 16 };
	iscc_gr_DistanceEdge* prev_distX = iscc_gr_get_next_k_nn(dist_store1, 2, out_dist_arrayX, vertex_markers, 1);
	assert_memory_equal(out_dist_arrayX, ref_dist_arrayX, 2 * sizeof(scc_Dpid));
	assert_int_equal(prev_distX->head, 16);
	assert_double_equal(prev_distX->distance, 43.918798);
	assert_ptr_equal(prev_distX->next_dist, prev_distX + 3);

	scc_Dpid out_dist_array2[4];
	scc_Dpid ref_dist_array2[4] = { 10, 14, 20, 2 };
	iscc_gr_DistanceEdge* prev_dist2 = iscc_gr_get_next_k_nn(&dist_store1[2], 4, out_dist_array2, vertex_markers, 1);
	assert_memory_equal(out_dist_array2, ref_dist_array2, 4 * sizeof(scc_Dpid));
	assert_int_equal(prev_dist2->head, 2);
	assert_double_equal(prev_dist2->distance, 103.030113);
	assert_null(prev_dist2->next_dist);

	vertex_markers[20] = 1;

	scc_Dpid out_dist_array3[4];
	scc_Dpid ref_dist_array3[4] = { 16, 10, 14, 2 };
	iscc_gr_DistanceEdge* prev_dist3 = iscc_gr_get_next_k_nn(&dist_store1[1], 4, out_dist_array3, vertex_markers, 1);
	assert_memory_equal(out_dist_array3, ref_dist_array3, 4 * sizeof(scc_Dpid));
	assert_int_equal(prev_dist3->head, 2);
	assert_double_equal(prev_dist3->distance, 103.030113);
	assert_null(prev_dist3->next_dist);


	scc_Dpid out_dist_array4[1];
	scc_Dpid ref_dist_array4[1] = { 8 };
	iscc_gr_DistanceEdge* prev_dist4 = iscc_gr_get_next_k_nn(&dist_store2[4], 1, out_dist_array4, vertex_markers, 2);
	assert_memory_equal(out_dist_array4, ref_dist_array4, 1 * sizeof(scc_Dpid));
	assert_int_equal(prev_dist4->head, 8);
	assert_double_equal(prev_dist4->distance, 62.616031);
	assert_ptr_equal(prev_dist4->next_dist, prev_dist4 + 1);

	vertex_markers[4] = 2;
	vertex_markers[14] = 2;
	vertex_markers[20] = 2;

	scc_Dpid out_dist_array5[3];
	scc_Dpid ref_dist_array5[3] = { 8, 10, 2 };
	iscc_gr_DistanceEdge* prev_dist5 = iscc_gr_get_next_k_nn(&dist_store2[2], 3, out_dist_array5, vertex_markers, 2);
	assert_memory_equal(out_dist_array5, ref_dist_array5, 3 * sizeof(scc_Dpid));
	assert_int_equal(prev_dist5->head, 2);
	assert_double_equal(prev_dist5->distance, 83.120587);
	assert_ptr_equal(prev_dist5->next_dist, prev_dist5 + 1);

    free(vertex_markers);
    free(dist_store1);
    free(dist_store2);
}


void scc_ut_get_first_k_nn(void** state)
{
	(void) state;

	uint_fast16_t* vertex_markers = calloc(100, sizeof(uint_fast16_t));

	scc_Dpid members[10] = { 2, 4, 6, 8, 10, 12, 14, 16, 18, 20 };
	iscc_gr_ClusterItem cl = {
		.size = 10,
		.marker = 0,
		.members = members,
	};

	iscc_gr_DistanceEdge* const dist_store1 = malloc(sizeof(iscc_gr_DistanceEdge[10]));
	iscc_gr_DistanceEdge* const dist_store2 = malloc(sizeof(iscc_gr_DistanceEdge[10]));

	assert_true(iscc_gr_populate_dist_lists(&scc_ut_test_data_large, &cl, 6, 16, dist_store1, dist_store2));

	scc_Dpid out_dist_array1[5];
	scc_Dpid ref_dist_array1[5] = { 12, 16, 18, 4, 10 };
    iscc_gr_DistanceEdge* prev_dist1 = iscc_gr_get_next_k_nn(dist_store1, 5, out_dist_array1, vertex_markers, 1);
	assert_memory_equal(out_dist_array1, ref_dist_array1, 5 * sizeof(scc_Dpid));
	assert_int_equal(prev_dist1->head, 10);
	assert_double_equal(prev_dist1->distance, 76.285875);
	assert_ptr_equal(prev_dist1->next_dist, prev_dist1 + 1);

	vertex_markers[12] = 1;
	vertex_markers[18] = 1;
	vertex_markers[8] = 1;

	scc_Dpid out_dist_array2[5];
	scc_Dpid ref_dist_array2[5] = { 16, 4, 10, 14, 20 };
	iscc_gr_DistanceEdge* prev_dist2 = iscc_gr_get_next_k_nn(dist_store1, 5, out_dist_array2, vertex_markers, 1);
	assert_memory_equal(out_dist_array2, ref_dist_array2, 5 * sizeof(scc_Dpid));
	assert_int_equal(prev_dist2->head, 20);
	assert_double_equal(prev_dist2->distance, 81.300565);
	assert_ptr_equal(prev_dist2->next_dist, prev_dist2 + 1);

	vertex_markers[20] = 1;

	scc_Dpid out_dist_array3[5];
	scc_Dpid ref_dist_array3[5] = { 16, 4, 10, 14, 2 };
	iscc_gr_DistanceEdge* prev_dist3 = iscc_gr_get_next_k_nn(dist_store1, 5, out_dist_array3, vertex_markers, 1);
	assert_memory_equal(out_dist_array3, ref_dist_array3, 5 * sizeof(scc_Dpid));
	assert_int_equal(prev_dist3->head, 2);
	assert_double_equal(prev_dist3->distance, 103.030113);
	assert_null(prev_dist3->next_dist);

	scc_Dpid out_dist_array3b[5];
	scc_Dpid ref_dist_array3b[5] = { 16, 4, 10, 14, 2 };
	iscc_gr_DistanceEdge* prev_dist3b = iscc_gr_get_next_k_nn(dist_store1, 5, out_dist_array3b, vertex_markers, 1);
	assert_memory_equal(out_dist_array3b, ref_dist_array3b, 5 * sizeof(scc_Dpid));
	assert_int_equal(prev_dist3b->head, 2);
	assert_double_equal(prev_dist3b->distance, 103.030113);
	assert_null(prev_dist3b->next_dist);


	scc_Dpid out_dist_array4[1];
	scc_Dpid ref_dist_array4[1] = { 12 };
	iscc_gr_DistanceEdge* prev_dist4 = iscc_gr_get_next_k_nn(dist_store2, 1, out_dist_array4, vertex_markers, 2);
	assert_memory_equal(out_dist_array4, ref_dist_array4, 1 * sizeof(scc_Dpid));
	assert_int_equal(prev_dist4->head, 12);
	assert_double_equal(prev_dist4->distance, 43.831474);
	assert_ptr_equal(prev_dist4->next_dist, prev_dist4 + 1);

	vertex_markers[6] = 2;
	vertex_markers[14] = 2;
	vertex_markers[20] = 2;

	scc_Dpid out_dist_array5[5];
	scc_Dpid ref_dist_array5[5] = { 12, 8, 10, 4, 2 };
	iscc_gr_DistanceEdge* prev_dist5 = iscc_gr_get_next_k_nn(dist_store2, 5, out_dist_array5, vertex_markers, 2);
	assert_memory_equal(out_dist_array5, ref_dist_array5, 5 * sizeof(scc_Dpid));
	assert_int_equal(prev_dist5->head, 2);
	assert_double_equal(prev_dist5->distance, 83.120587);
	assert_ptr_equal(prev_dist5->next_dist, prev_dist5 + 1);

	scc_Dpid out_dist_array6[5];
	scc_Dpid ref_dist_array6[5] = { 12, 8, 10, 4, 2 };
	iscc_gr_DistanceEdge* prev_dist6 = iscc_gr_get_next_k_nn(dist_store2, 5, out_dist_array6, vertex_markers, 2);
	assert_memory_equal(out_dist_array6, ref_dist_array6, 5 * sizeof(scc_Dpid));
	assert_int_equal(prev_dist6->head, 2);
	assert_double_equal(prev_dist6->distance, 83.120587);
	assert_ptr_equal(prev_dist6->next_dist, prev_dist6 + 1);

    free(vertex_markers);
    free(dist_store1);
    free(dist_store2);
}


int main(void)
{
	const struct CMUnitTest test_greedy_internals[] = {
		cmocka_unit_test(scc_ut_init_cl_stack),
		cmocka_unit_test(scc_ut_empty_cl_stack),
		cmocka_unit_test(scc_ut_free_cl_strack),
		cmocka_unit_test(scc_ut_run_greedy_clustering),
		cmocka_unit_test(scc_ut_peek_at_stack),
		cmocka_unit_test(scc_ut_pop_from_stack),
		cmocka_unit_test(scc_ut_push_to_stack),
		cmocka_unit_test(scc_ut_break_cluster_into_two),
		cmocka_unit_test(scc_ut_find_centers),
		cmocka_unit_test(scc_ut_get_next_marker),
		cmocka_unit_test(scc_ut_populate_dist_lists),
		cmocka_unit_test(scc_ut_sort_dist_list),
		cmocka_unit_test(scc_ut_compare_dist_edges),
		cmocka_unit_test(scc_ut_move_v_to_cluster),
		cmocka_unit_test(scc_ut_get_next_dist),
		cmocka_unit_test(scc_ut_get_next_k_nn),
		cmocka_unit_test(scc_ut_get_first_k_nn),
	};

	return cmocka_run_group_tests_name("internal greedy module", test_greedy_internals, NULL, NULL);
}
