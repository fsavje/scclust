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
#include "../include/scc_data_obj.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "data_object_test.h"
#include "rand.h"

static const size_t SAMPLE_SIZE = 10000;
static const size_t DATA_DIMENSION = 3;
static const size_t NUM_ROUNDS = 10;


void scc_ut_stress_top_down_greedy_clustering(void** state)
{
	(void) state;

	srand(123456789);

	for (size_t r = 0; r < NUM_ROUNDS; ++r) {
		scc_ErrorCode ec;
		scc_Clustering* cl;
		bool cl_is_OK;

		double* const data_matrix = malloc(sizeof(double[DATA_DIMENSION * SAMPLE_SIZE]));
		scc_rand_double_array(0, 100, DATA_DIMENSION * SAMPLE_SIZE, data_matrix);

		scc_DataSetObject* data_set_object;
		ec = scc_get_data_set_object(SAMPLE_SIZE, DATA_DIMENSION, DATA_DIMENSION * SAMPLE_SIZE, data_matrix, false, &data_set_object);
		assert_int_equal(ec, SCC_ER_OK);

		const uint32_t size_constraint = scc_rand_uint(2, 10);

		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		ec = scc_top_down_greedy_clustering(cl, data_set_object, size_constraint, true);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering(cl, size_constraint, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);

		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		ec = scc_top_down_greedy_clustering(cl, data_set_object, size_constraint, false);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering(cl, size_constraint, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);

		free(data_matrix);
		scc_free_data_set_object(&data_set_object);
	}
}


int main(void)
{
	const struct CMUnitTest test_cases[] = {
		cmocka_unit_test(scc_ut_stress_top_down_greedy_clustering),
	};

	return cmocka_run_group_tests_name("stress greedy_top_clustering.c", test_cases, NULL, NULL);
}
