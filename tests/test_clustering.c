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

#include <stdlib.h>
#include "../include/scclust.h"
#include "../src/clustering.h"
#include "data_object_test.h"


void scc_ut_free_Clustering(void** state)
{
	(void) state;

	scc_Clustering test_cl1 =  {
		.num_data_points = 10,
		.num_clusters = 5,
		.external_labels = false,
		.cluster_label = malloc(sizeof(scc_Clabel[10])),
	};

	scc_Clabel ext_cluster_label[20];
	scc_Clustering test_cl2 =  {
		.num_data_points = 20,
		.num_clusters = 10,
		.external_labels = true,
		.cluster_label = ext_cluster_label,
	};

	scc_free_clustering(&test_cl1);
	assert_int_equal(test_cl1.num_data_points, 0);
	assert_int_equal(test_cl1.num_clusters, 0);
	assert_false(test_cl1.external_labels);
	assert_null(test_cl1.cluster_label);

	scc_free_clustering(&test_cl2);
	assert_int_equal(test_cl2.num_data_points, 0);
	assert_int_equal(test_cl2.num_clusters, 0);
	assert_false(test_cl2.external_labels);
	assert_null(test_cl2.cluster_label);
}


void scc_ut_get_clustering_stats(void** state)
{
	(void) state;

	scc_Clabel clabels[15] = { 0, 1, 3, 2, 2,
	                           3, 2, 1, 1, 0,
	                           3, 3, 2, 1, 1 };
	scc_Clustering cl = {
		.num_data_points = 15,
		.num_clusters = 4,
		.external_labels = true,
		.cluster_label = clabels,
	};

	scc_ClusteringStats out_stat = scc_get_clustering_stats(&cl, &scc_ut_test_data_small);

	scc_ClusteringStats cs_ref =  {
		.num_populated_clusters = 4,
		.num_assigned = 15,
		.min_cluster_size = 2,
		.max_cluster_size = 5,
		.avg_cluster_size = 3.75,
		.sum_dists = 20.776226,
		.min_dist = 0.019146,
		.max_dist = 2.195331,
		.cl_avg_min_dist = 0.646028,
		.cl_avg_max_dist = 1.763760,
		.cl_avg_dist_weighted = 1.031564,
		.cl_avg_dist_unweighted = 1.182866,
	};

	assert_int_equal(out_stat.num_populated_clusters, cs_ref.num_populated_clusters);
	assert_int_equal(out_stat.num_assigned, cs_ref.num_assigned);
	assert_int_equal(out_stat.min_cluster_size, cs_ref.min_cluster_size);
	assert_int_equal(out_stat.max_cluster_size, cs_ref.max_cluster_size);
	assert_double_equal(out_stat.avg_cluster_size, cs_ref.avg_cluster_size);
	assert_double_equal(out_stat.sum_dists, cs_ref.sum_dists);
	assert_double_equal(out_stat.min_dist, cs_ref.min_dist);
	assert_double_equal(out_stat.max_dist, cs_ref.max_dist);
	assert_double_equal(out_stat.cl_avg_min_dist, cs_ref.cl_avg_min_dist);
	assert_double_equal(out_stat.cl_avg_max_dist, cs_ref.cl_avg_max_dist);
	assert_double_equal(out_stat.cl_avg_dist_weighted, cs_ref.cl_avg_dist_weighted);
	assert_double_equal(out_stat.cl_avg_dist_unweighted, cs_ref.cl_avg_dist_unweighted);


	const scc_Clabel M = SCC_CLABEL_NA;
	scc_Clabel clabels2[100] = { M, 0, M, M, M, 1, M, M, 3, M,
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
		.external_labels = true,
		.cluster_label = clabels2,
	};

	scc_ClusteringStats out_stat2 = scc_get_clustering_stats(&cl2, &scc_ut_test_data_large);

	scc_ClusteringStats cs_ref2 =  {
		.num_populated_clusters = 5,
		.num_assigned = 14,
		.min_cluster_size = 1,
		.max_cluster_size = 4,
		.avg_cluster_size = 2.8,
		.sum_dists = 1126.431356,
		.min_dist = 0.0,
		.max_dist = 102.899636,
		.cl_avg_min_dist = 28.301867,
		.cl_avg_max_dist = 59.867698,
		.cl_avg_dist_weighted = 60.25076,
		.cl_avg_dist_unweighted = 47.155168,
	};

	assert_int_equal(out_stat2.num_populated_clusters, cs_ref2.num_populated_clusters);
	assert_int_equal(out_stat2.num_assigned, cs_ref2.num_assigned);
	assert_int_equal(out_stat2.min_cluster_size, cs_ref2.min_cluster_size);
	assert_int_equal(out_stat2.max_cluster_size, cs_ref2.max_cluster_size);
	assert_double_equal(out_stat2.avg_cluster_size, cs_ref2.avg_cluster_size);
	assert_double_equal(out_stat2.sum_dists, cs_ref2.sum_dists);
	assert_double_equal(out_stat2.min_dist, cs_ref2.min_dist);
	assert_double_equal(out_stat2.max_dist, cs_ref2.max_dist);
	assert_double_equal(out_stat2.cl_avg_min_dist, cs_ref2.cl_avg_min_dist);
	assert_double_equal(out_stat2.cl_avg_max_dist, cs_ref2.cl_avg_max_dist);
	assert_double_equal(out_stat2.cl_avg_dist_weighted, cs_ref2.cl_avg_dist_weighted);
	assert_double_equal(out_stat2.cl_avg_dist_unweighted, cs_ref2.cl_avg_dist_unweighted);
}


int main(void)
{
	const struct CMUnitTest test_clustering[] = {
		cmocka_unit_test(scc_ut_free_Clustering),
		cmocka_unit_test(scc_ut_get_clustering_stats),
	};

	return cmocka_run_group_tests_name("clustering module", test_clustering, NULL, NULL);
}
