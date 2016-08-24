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


void scc_ut_stress_nng_clustering(void** state)
{
	(void) state;

	srand(123456789); 

	for (size_t r = 0; r < NUM_ROUNDS; ++r) {
		scc_ErrorCode ec;
		scc_Clustering* cl;
		bool cl_is_OK;

		double* const data_matrix = malloc(sizeof(double[DATA_DIMENSION * SAMPLE_SIZE]));
		scc_rand_double_array(0, 100, DATA_DIMENSION * SAMPLE_SIZE, data_matrix);

		bool* const main_data_points = malloc(sizeof(bool[SAMPLE_SIZE]));
		scc_rand_bool_array(SAMPLE_SIZE, main_data_points);

		scc_DataSetObject* data_set_object;
		ec = scc_get_data_set_object(SAMPLE_SIZE, DATA_DIMENSION, DATA_DIMENSION * SAMPLE_SIZE, data_matrix, false, &data_set_object);
		assert_int_equal(ec, SCC_ER_OK);

		const uint32_t size_constraint = scc_rand_uint(2, 10);
		const scc_SeedMethod seed_method = scc_rand_uint(SCC_SM_LEXICAL, SCC_SM_EXCLUSION_UPDATING);
		const scc_UnassignedMethod main_unassigned_method = scc_rand_uint(SCC_UM_IGNORE, SCC_UM_CLOSEST_SEED_EST_RADIUS);
		const scc_UnassignedMethod secondary_unassigned_method = scc_rand_uint(SCC_UM_IGNORE, SCC_UM_CLOSEST_SEED_EST_RADIUS);

		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		ec = scc_nng_clustering(cl, data_set_object, size_constraint,
	                            seed_method, main_unassigned_method, false, 0.0,
	                            0, NULL, SCC_UM_IGNORE, false, 0.0);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering(cl, size_constraint, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);

		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		ec = scc_nng_clustering(cl, data_set_object, size_constraint,
	                            seed_method, main_unassigned_method, true, 10.0,
	                            0, NULL, SCC_UM_IGNORE, false, 0.0);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering(cl, size_constraint, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);

		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		ec = scc_nng_clustering(cl, data_set_object, size_constraint,
	                            seed_method, main_unassigned_method, false, 0.0,
	                            SAMPLE_SIZE, main_data_points, secondary_unassigned_method, false, 0.0);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering(cl, size_constraint, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);

		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		ec = scc_nng_clustering(cl, data_set_object, size_constraint,
	                            seed_method, main_unassigned_method, true, 10.0,
	                            SAMPLE_SIZE, main_data_points, secondary_unassigned_method, false, 0.0);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering(cl, size_constraint, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);

		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		ec = scc_nng_clustering(cl, data_set_object, size_constraint,
	                            seed_method, main_unassigned_method, false, 0.0,
	                            SAMPLE_SIZE, main_data_points, secondary_unassigned_method, true, 10.0);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering(cl, size_constraint, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);

		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		ec = scc_nng_clustering(cl, data_set_object, size_constraint,
	                            seed_method, main_unassigned_method, true, 10.0,
	                            SAMPLE_SIZE, main_data_points, secondary_unassigned_method, true, 10.0);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering(cl, size_constraint, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);

		free(data_matrix);
		free(main_data_points);
		scc_free_data_set_object(&data_set_object);
	}
}


void scc_ut_stress_nng_clustering_batches(void** state)
{
	(void) state;

	srand(123456789); 

	for (size_t r = 0; r < NUM_ROUNDS; ++r) {
		scc_ErrorCode ec;
		scc_Clustering* cl;
		bool cl_is_OK;

		double* const data_matrix = malloc(sizeof(double[DATA_DIMENSION * SAMPLE_SIZE]));
		scc_rand_double_array(0, 100, DATA_DIMENSION * SAMPLE_SIZE, data_matrix);

		bool* const main_data_points = malloc(sizeof(bool[SAMPLE_SIZE]));
		scc_rand_bool_array(SAMPLE_SIZE, main_data_points);

		scc_DataSetObject* data_set_object;
		ec = scc_get_data_set_object(SAMPLE_SIZE, DATA_DIMENSION, DATA_DIMENSION * SAMPLE_SIZE, data_matrix, false, &data_set_object);
		assert_int_equal(ec, SCC_ER_OK);

		const uint32_t size_constraint = scc_rand_uint(2, 10);
		const scc_UnassignedMethod main_unassigned_method = scc_rand_uint(SCC_UM_IGNORE, SCC_UM_ASSIGN_BY_NNG);
		const uint32_t batch_size = scc_rand_uint(10, 100);


		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		ec = scc_nng_clustering_batches(cl, data_set_object, size_constraint,
                                        main_unassigned_method, false, 0.0,
                                        0, NULL, 0);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering(cl, size_constraint, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);


		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		ec = scc_nng_clustering_batches(cl, data_set_object, size_constraint,
                                        main_unassigned_method, false, 0.0,
                                        0, NULL, 1);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering(cl, size_constraint, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);


		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		ec = scc_nng_clustering_batches(cl, data_set_object, size_constraint,
                                        main_unassigned_method, false, 0.0,
                                        0, NULL, batch_size);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering(cl, size_constraint, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);


		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		ec = scc_nng_clustering_batches(cl, data_set_object, size_constraint,
                                        main_unassigned_method, true, 10.0,
                                        0, NULL, batch_size);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering(cl, size_constraint, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);


		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		ec = scc_nng_clustering_batches(cl, data_set_object, size_constraint,
                                        main_unassigned_method, false, 0.0,
                                        SAMPLE_SIZE, main_data_points, batch_size);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering(cl, size_constraint, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);


		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		ec = scc_nng_clustering_batches(cl, data_set_object, size_constraint,
                                        main_unassigned_method, true, 10.0,
                                        SAMPLE_SIZE, main_data_points, batch_size);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering(cl, size_constraint, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);


		free(data_matrix);
		free(main_data_points);
		scc_free_data_set_object(&data_set_object);
	}
}


void scc_ut_stress_nng_clustering_with_types(void** state)
{
	(void) state;

	srand(123456789); 

	for (size_t r = 0; r < NUM_ROUNDS; ++r) {
		scc_ErrorCode ec;
		scc_Clustering* cl;
		bool cl_is_OK;

		double* const data_matrix = malloc(sizeof(double[DATA_DIMENSION * SAMPLE_SIZE]));
		scc_rand_double_array(0, 100, DATA_DIMENSION * SAMPLE_SIZE, data_matrix);

		uint16_t num_types = scc_rand_uint(2, 10);
		uint32_t* const type_size_constraints = malloc(sizeof(uint32_t[num_types]));
		scc_rand_uint32_t_array(0, 5, num_types, type_size_constraints);
		scc_TypeLabel* const type_labels = malloc(sizeof(scc_TypeLabel[SAMPLE_SIZE]));
		uint16_t* const type_labels_tmp = malloc(sizeof(uint16_t[SAMPLE_SIZE]));
		scc_rand_uint16_t_array(0, (uint16_t) (num_types - 1), SAMPLE_SIZE, type_labels_tmp);
		for (size_t i = 0; i < SAMPLE_SIZE; ++i) {
			type_labels[i] = ((scc_TypeLabel) type_labels_tmp[i]);
		}
		free(type_labels_tmp);

		bool* const main_data_points = malloc(sizeof(bool[SAMPLE_SIZE]));
		scc_rand_bool_array(SAMPLE_SIZE, main_data_points);

		scc_DataSetObject* data_set_object;
		ec = scc_get_data_set_object(SAMPLE_SIZE, DATA_DIMENSION, DATA_DIMENSION * SAMPLE_SIZE, data_matrix, false, &data_set_object);
		assert_int_equal(ec, SCC_ER_OK);

		uint32_t sum_type_constraints = 0;
		for (uint16_t t = 0; t < num_types; ++t) {
			sum_type_constraints += type_size_constraints[t];
		}
		const uint32_t size_constraint = sum_type_constraints + scc_rand_uint(0, 2);
		const scc_SeedMethod seed_method = scc_rand_uint(SCC_SM_LEXICAL, SCC_SM_EXCLUSION_UPDATING);
		const scc_UnassignedMethod main_unassigned_method = scc_rand_uint(SCC_UM_IGNORE, SCC_UM_CLOSEST_SEED_EST_RADIUS);
		const scc_UnassignedMethod secondary_unassigned_method = scc_rand_uint(SCC_UM_IGNORE, SCC_UM_CLOSEST_SEED_EST_RADIUS);

		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		ec = scc_nng_clustering_types(cl, data_set_object, size_constraint,
	                                       num_types, type_size_constraints, SAMPLE_SIZE, type_labels,
	                                       seed_method, main_unassigned_method, false, 0.0,
	                                       0, NULL, SCC_UM_IGNORE, false, 0.0);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering_types(cl, size_constraint, num_types, type_size_constraints, SAMPLE_SIZE, type_labels, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);

		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		ec = scc_nng_clustering_types(cl, data_set_object, size_constraint,
	                                       num_types, type_size_constraints, SAMPLE_SIZE, type_labels,
	                                       seed_method, main_unassigned_method, true, 10.0,
	                                       0, NULL, SCC_UM_IGNORE, false, 0.0);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering_types(cl, size_constraint, num_types, type_size_constraints, SAMPLE_SIZE, type_labels, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);

		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		ec = scc_nng_clustering_types(cl, data_set_object, size_constraint,
	                                       num_types, type_size_constraints, SAMPLE_SIZE, type_labels,
	                                       seed_method, main_unassigned_method, false, 0.0,
	                                       SAMPLE_SIZE, main_data_points, secondary_unassigned_method, false, 0.0);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering_types(cl, size_constraint, num_types, type_size_constraints, SAMPLE_SIZE, type_labels, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);

		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		ec = scc_nng_clustering_types(cl, data_set_object, size_constraint,
	                                       num_types, type_size_constraints, SAMPLE_SIZE, type_labels,
	                                       seed_method, main_unassigned_method, true, 10.0,
	                                       SAMPLE_SIZE, main_data_points, secondary_unassigned_method, false, 0.0);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering_types(cl, size_constraint, num_types, type_size_constraints, SAMPLE_SIZE, type_labels, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);

		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		ec = scc_nng_clustering_types(cl, data_set_object, size_constraint,
	                                       num_types, type_size_constraints, SAMPLE_SIZE, type_labels,
	                                       seed_method, main_unassigned_method, false, 0.0,
	                                       SAMPLE_SIZE, main_data_points, secondary_unassigned_method, true, 10.0);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering_types(cl, size_constraint, num_types, type_size_constraints, SAMPLE_SIZE, type_labels, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);

		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		ec = scc_nng_clustering_types(cl, data_set_object, size_constraint,
	                                       num_types, type_size_constraints, SAMPLE_SIZE, type_labels,
	                                       seed_method, main_unassigned_method, true, 10.0,
	                                       SAMPLE_SIZE, main_data_points, secondary_unassigned_method, true, 10.0);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering_types(cl, size_constraint, num_types, type_size_constraints, SAMPLE_SIZE, type_labels, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);

		free(data_matrix);
		free(type_size_constraints);
		free(type_labels);
		free(main_data_points);
		scc_free_data_set_object(&data_set_object);
	}
}


int main(void)
{
	const struct CMUnitTest test_cases[] = {
		cmocka_unit_test(scc_ut_stress_nng_clustering),
		cmocka_unit_test(scc_ut_stress_nng_clustering_batches),
		cmocka_unit_test(scc_ut_stress_nng_clustering_with_types),
	};
	
	return cmocka_run_group_tests_name("stress nng_clustering.c", test_cases, NULL, NULL);
}
