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
 * License along with this library. If not, see http://www.gnu.org/licenses/
 * ============================================================================== */

#include "test_suite.h"
#include "../include/scclust.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "../src/scclust_int.h"
#include "data_object_test.h"


void scc_ut_top_down_greedy_clustering_stress(void** state)
{
	(void) state;

	double* const data_matrix = malloc(sizeof(double[300000]));

	srand(123456789); 
	for (size_t i = 0; i < 300000; ++i) {
		data_matrix[i] = (double) rand();
	}

	scc_ErrorCode ec;
	scc_DataSetObject* data_set_object;
	ec = scc_get_data_set_object(100000, 3, 300000, data_matrix, false, &data_set_object);
	assert_int_equal(ec, SCC_ER_OK);

	scc_Clustering* clustering;
	ec = scc_init_empty_clustering(100000, NULL, &clustering);
	assert_int_equal(ec, SCC_ER_OK);

	ec = scc_top_down_greedy_clustering(clustering, data_set_object, 10, false);

	scc_free_clustering(&clustering);
	scc_free_data_set_object(&data_set_object);

	free(data_matrix);
}


int main(void)
{
	const struct CMUnitTest test_cases[] = {
		cmocka_unit_test(scc_ut_top_down_greedy_clustering_stress),
	};
	
	return cmocka_run_group_tests_name("greedy_clustering.c", test_cases, NULL, NULL);
}
