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

#include <stdlib.h>
#include "../include/clustering.h"


void scc_ut_free_Clustering(void** state)
{
	(void) state;

	scc_Clustering test_cl1 =  {
		.vertices = 10,
		.num_clusters = 5,
		.external_labels = false,
		.cluster_label = malloc(sizeof(scc_Clabel[10])),
	};

	scc_Clabel ext_cluster_label[20];
	scc_Clustering test_cl2 =  {
		.vertices = 20,
		.num_clusters = 10,
		.external_labels = true,
		.cluster_label = ext_cluster_label,
	};

	scc_free_Clustering(&test_cl1);
	assert_int_equal(test_cl1.vertices, 0);
	assert_int_equal(test_cl1.num_clusters, 0);
	assert_false(test_cl1.external_labels);
	assert_null(test_cl1.cluster_label);

	scc_free_Clustering(&test_cl2);
	assert_int_equal(test_cl2.vertices, 0);
	assert_int_equal(test_cl2.num_clusters, 0);
	assert_false(test_cl2.external_labels);
	assert_null(test_cl2.cluster_label);
}


int main(void)
{
	const struct CMUnitTest test_clustering[] = {
		cmocka_unit_test(scc_ut_free_Clustering),
	};

	return cmocka_run_group_tests_name("clustering module", test_clustering, NULL, NULL);
}
