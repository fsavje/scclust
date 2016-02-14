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
#include <stdlib.h>
#include "../include/greedy_clustering.h"
#include "data_object_test.h"


void scc_ut_greedy_break_clustering(void** state)
{
	(void) state;

	scc_Clabel cluster_label1[100] = { 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0,
	                                   0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0,
	                                   1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 0,
	                                   0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0,
	                                   0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1,
	                                   0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0,
	                                   0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0 };
	scc_Clustering cl1 = {
		.vertices = 100,
		.num_clusters = 2,
		.external_labels = true,
		.cluster_label = cluster_label1,
	};
	assert_true(scc_greedy_break_clustering(&cl1, &scc_ut_test_data_large, 20, true));
	scc_Clabel ref_label1[100] = { 1, 1, 2, 2, 2, 0, 0, 3, 0, 0, 2, 1, 3, 3, 2,
	                               1, 0, 2, 1, 0, 0, 3, 1, 1, 1, 0, 1, 3, 0, 3,
	                               2, 0, 0, 1, 3, 0, 2, 3, 1, 1, 1, 2, 3, 2, 1,
	                               3, 3, 3, 1, 0, 3, 3, 2, 2, 3, 3, 2, 1, 0, 3,
	                               1, 3, 1, 0, 2, 2, 3, 3, 1, 0, 3, 0, 2, 1, 2,
	                               0, 3, 1, 1, 1, 3, 1, 1, 3, 2, 1, 3, 3, 1, 1,
	                               0, 1, 3, 2, 3, 3, 1, 2, 0, 3 };
	assert_int_equal(cl1.vertices, 100);
	assert_int_equal(cl1.num_clusters, 4);
	assert_true(cl1.external_labels);
	assert_memory_equal(cl1.cluster_label, ref_label1, 100 * sizeof(scc_Clabel));


	scc_Clabel cluster_label2[100] = { 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0,
	                                   0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0,
	                                   1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 0,
	                                   0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0,
	                                   0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1,
	                                   0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0,
	                                   0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0 };
	scc_Clustering cl2 = {
		.vertices = 100,
		.num_clusters = 2,
		.external_labels = true,
		.cluster_label = cluster_label2,
	};
	assert_true(scc_greedy_break_clustering(&cl2, &scc_ut_test_data_large, 20, false));
	scc_Clabel ref_label2[100] = { 1, 0, 2, 2, 2, 0, 0, 3, 0, 0, 2, 0, 3, 3, 2, 1, 0,
	                               2, 1, 0, 0, 3, 1, 1, 1, 0, 0, 3, 0, 3, 2, 0, 0, 1,
	                               3, 0, 2, 2, 0, 0, 1, 2, 3, 2, 1, 3, 3, 3, 0, 0, 3,
	                               2, 2, 2, 3, 3, 2, 1, 0, 3, 1, 3, 1, 0, 2, 2, 3, 3,
	                               1, 0, 3, 0, 2, 0, 2, 0, 3, 1, 1, 1, 2, 1, 1, 3, 2,
	                               1, 3, 2, 1, 1, 0, 1, 3, 2, 2, 3, 1, 2, 0, 3 };
	assert_int_equal(cl2.vertices, 100);
	assert_int_equal(cl2.num_clusters, 4);
	assert_true(cl2.external_labels);
	assert_memory_equal(cl2.cluster_label, ref_label2, 100 * sizeof(scc_Clabel));

	scc_free_Clustering(&cl1);
	scc_free_Clustering(&cl2);
}


void scc_ut_get_greedy_clustering(void** state)
{
	(void) state;

	scc_Clabel* ext_cluster_label1 = malloc(sizeof(scc_Clabel[100]));
	scc_Clustering cl1a = scc_get_greedy_clustering(&scc_ut_test_data_large, 20, true, ext_cluster_label1);
	scc_Clustering cl1b = scc_get_greedy_clustering(&scc_ut_test_data_large, 20, true, NULL);
	scc_Clabel ref_label1[100] = { 2, 3, 3, 2, 2, 3, 0, 0, 4, 3, 2, 1, 1, 0, 4,
	                               3, 0, 2, 0, 4, 3, 1, 3, 0, 0, 0, 4, 0, 4, 0,
	                               3, 4, 3, 1, 0, 0, 3, 4, 1, 0, 3, 2, 1, 2, 2,
	                               2, 0, 2, 1, 4, 3, 4, 4, 4, 1, 0, 3, 1, 3, 1,
	                               1, 1, 4, 4, 3, 4, 4, 1, 2, 3, 1, 4, 2, 3, 4,
	                               0, 0, 4, 1, 1, 3, 2, 1, 0, 2, 1, 0, 2, 2, 2,
	                               4, 2, 1, 2, 2, 1, 3, 4, 3, 0 };
	
	assert_int_equal(cl1a.vertices, 100);
	assert_int_equal(cl1a.num_clusters, 5);
	assert_true(cl1a.external_labels);
	assert_ptr_equal(cl1a.cluster_label, ext_cluster_label1);
	assert_memory_equal(cl1a.cluster_label, ref_label1, 100 * sizeof(scc_Clabel));

	assert_int_equal(cl1b.vertices, 100);
	assert_int_equal(cl1b.num_clusters, 5);
	assert_false(cl1b.external_labels);
	assert_memory_equal(cl1b.cluster_label, ref_label1, 100 * sizeof(scc_Clabel));


	scc_Clabel* ext_cluster_label2 = malloc(sizeof(scc_Clabel[100]));
	scc_Clustering cl2a = scc_get_greedy_clustering(&scc_ut_test_data_large, 20, false, ext_cluster_label2);
	scc_Clustering cl2b = scc_get_greedy_clustering(&scc_ut_test_data_large, 20, false, NULL);
	scc_Clabel ref_label2[100] = { 3, 0, 2, 3, 3, 2, 1, 1, 3, 2, 3, 0, 1, 0, 2, 2, 1, 3,
	                               0, 2, 2, 0, 1, 1, 0, 1, 2, 1, 2, 0, 2, 2, 2, 0, 1, 1,
	                               2, 2, 0, 0, 3, 3, 0, 3, 3, 0, 1, 3, 0, 2, 0, 2, 2, 2,
	                               0, 0, 2, 0, 2, 1, 0, 0, 3, 3, 3, 3, 1, 1, 3, 2, 0, 1,
	                               3, 2, 2, 1, 1, 3, 0, 0, 2, 3, 0, 1, 3, 0, 1, 3, 3, 3,
	                               3, 2, 0, 3, 3, 1, 0, 3, 2, 0 };
	
	assert_int_equal(cl2a.vertices, 100);
	assert_int_equal(cl2a.num_clusters, 4);
	assert_true(cl2a.external_labels);
	assert_ptr_equal(cl2a.cluster_label, ext_cluster_label2);
	assert_memory_equal(cl2a.cluster_label, ref_label2, 100 * sizeof(scc_Clabel));

	assert_int_equal(cl2b.vertices, 100);
	assert_int_equal(cl2b.num_clusters, 4);
	assert_false(cl2b.external_labels);
	assert_memory_equal(cl2b.cluster_label, ref_label2, 100 * sizeof(scc_Clabel));


	free(ext_cluster_label1);
	scc_free_Clustering(&cl1a);
	scc_free_Clustering(&cl1b);
	free(ext_cluster_label2);
	scc_free_Clustering(&cl2a);
	scc_free_Clustering(&cl2b);
}


int main(void)
{
	const struct CMUnitTest test_greedy[] = {
		cmocka_unit_test(scc_ut_greedy_break_clustering),
		cmocka_unit_test(scc_ut_get_greedy_clustering),
	};

	return cmocka_run_group_tests_name("greedy module", test_greedy, NULL, NULL);
}
