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
#include "data_object_test.h"


void scc_nng_clustering_stress(void** state)
{
	(void) state;

	double* const data_matrix = malloc(sizeof(double[30000]));

	srand(123456789); 
	for (size_t i = 0; i < 30000; ++i) {
		data_matrix[i] = (double) rand();
	}

	scc_ErrorCode ec;
	scc_DataSetObject* data_set_object;
	ec = scc_get_data_set_object(10000, 3, 30000, data_matrix, false, &data_set_object);
	assert_int_equal(ec, SCC_ER_OK);

	scc_Clustering* clustering_to_run;
	scc_Clustering* clustering_templete;
	ec = scc_init_empty_clustering(10000, NULL, &clustering_templete);
	assert_int_equal(ec, SCC_ER_OK);


	ec = scc_copy_clustering(clustering_templete, &clustering_to_run);
	assert_int_equal(ec, SCC_ER_OK);
	ec = scc_nng_clustering(clustering_to_run, data_set_object, 8, SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED,
                            false, 0.0, 0, NULL, SCC_UM_IGNORE, false, 0.0);
	assert_int_equal(ec, SCC_ER_OK);
	scc_free_clustering(&clustering_to_run);
	
	ec = scc_copy_clustering(clustering_templete, &clustering_to_run);
	assert_int_equal(ec, SCC_ER_OK);
	ec = scc_nng_clustering(clustering_to_run, data_set_object, 8, SCC_SM_INWARDS_ORDER, SCC_UM_CLOSEST_SEED,
                            false, 0.0, 0, NULL, SCC_UM_IGNORE, false, 0.0);
	assert_int_equal(ec, SCC_ER_OK);
	scc_free_clustering(&clustering_to_run);
	
	ec = scc_copy_clustering(clustering_templete, &clustering_to_run);
	assert_int_equal(ec, SCC_ER_OK);
	ec = scc_nng_clustering(clustering_to_run, data_set_object, 8, SCC_SM_INWARDS_UPDATING, SCC_UM_CLOSEST_SEED,
                            false, 0.0, 0, NULL, SCC_UM_IGNORE, false, 0.0);
	assert_int_equal(ec, SCC_ER_OK);
	scc_free_clustering(&clustering_to_run);
	
	ec = scc_copy_clustering(clustering_templete, &clustering_to_run);
	assert_int_equal(ec, SCC_ER_OK);
	ec = scc_nng_clustering(clustering_to_run, data_set_object, 8, SCC_SM_EXCLUSION_ORDER, SCC_UM_CLOSEST_SEED,
                            false, 0.0, 0, NULL, SCC_UM_IGNORE, false, 0.0);
	assert_int_equal(ec, SCC_ER_OK);
	scc_free_clustering(&clustering_to_run);
	
	ec = scc_copy_clustering(clustering_templete, &clustering_to_run);
	assert_int_equal(ec, SCC_ER_OK);
	ec = scc_nng_clustering(clustering_to_run, data_set_object, 8, SCC_SM_EXCLUSION_UPDATING, SCC_UM_CLOSEST_SEED,
                            false, 0.0, 0, NULL, SCC_UM_IGNORE, false, 0.0);
	assert_int_equal(ec, SCC_ER_OK);
	scc_free_clustering(&clustering_to_run);
	

	ec = scc_copy_clustering(clustering_templete, &clustering_to_run);
	assert_int_equal(ec, SCC_ER_OK);
	ec = scc_nng_clustering(clustering_to_run, data_set_object, 8, SCC_SM_LEXICAL, SCC_UM_IGNORE,
                            false, 0.0, 0, NULL, SCC_UM_IGNORE, false, 0.0);
	assert_int_equal(ec, SCC_ER_OK);
	scc_free_clustering(&clustering_to_run);
	
	ec = scc_copy_clustering(clustering_templete, &clustering_to_run);
	assert_int_equal(ec, SCC_ER_OK);
	ec = scc_nng_clustering(clustering_to_run, data_set_object, 8, SCC_SM_LEXICAL, SCC_UM_ASSIGN_BY_NNG,
                            false, 0.0, 0, NULL, SCC_UM_IGNORE, false, 0.0);
	assert_int_equal(ec, SCC_ER_OK);
	scc_free_clustering(&clustering_to_run);
	
	ec = scc_copy_clustering(clustering_templete, &clustering_to_run);
	assert_int_equal(ec, SCC_ER_OK);
	ec = scc_nng_clustering(clustering_to_run, data_set_object, 8, SCC_SM_LEXICAL, SCC_UM_CLOSEST_ASSIGNED,
                            false, 0.0, 0, NULL, SCC_UM_IGNORE, false, 0.0);
	assert_int_equal(ec, SCC_ER_OK);
	scc_free_clustering(&clustering_to_run);
	
	ec = scc_copy_clustering(clustering_templete, &clustering_to_run);
	assert_int_equal(ec, SCC_ER_OK);
	ec = scc_nng_clustering(clustering_to_run, data_set_object, 8, SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED,
                            false, 0.0, 0, NULL, SCC_UM_IGNORE, false, 0.0);
	assert_int_equal(ec, SCC_ER_OK);
	scc_free_clustering(&clustering_to_run);
	
	ec = scc_copy_clustering(clustering_templete, &clustering_to_run);
	assert_int_equal(ec, SCC_ER_OK);
	ec = scc_nng_clustering(clustering_to_run, data_set_object, 8, SCC_SM_LEXICAL, SCC_UM_CLOSEST_SEED_EST_RADIUS,
                            false, 0.0, 0, NULL, SCC_UM_IGNORE, false, 0.0);
	assert_int_equal(ec, SCC_ER_OK);
	scc_free_clustering(&clustering_to_run);


	scc_free_clustering(&clustering_templete);
	scc_free_data_set_object(&data_set_object);
	free(data_matrix);
}


int main(void)
{
	const struct CMUnitTest test_cases[] = {
		cmocka_unit_test(scc_nng_clustering_stress),
	};
	
	return cmocka_run_group_tests_name("stress nng_clustering.c", test_cases, NULL, NULL);
}
