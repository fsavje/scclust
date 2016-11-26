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
#include <stdlib.h>
#include <include/scclust.h>
#include "data_object_test.h"
#include "rand.h"

static const size_t SAMPLE_SIZE = 10000;
static const size_t DATA_DIMENSION = 3;
static const size_t NUM_ROUNDS = 10;

static const int32_t ISCC_UT_OPTIONS_STRUCT_VERSION = 722678001;

static void iscc_make_batch_options(scc_ClusterOptions* out_options,
                                    uint32_t size_constraint,
                                    scc_UnassignedMethod unassigned_method,
                                    bool radius_constraint,
                                    double radius,
                                    size_t len_primary_data_points,
                                    const bool primary_data_points[],
                                    uint32_t batch_size)
{
	*out_options = (scc_ClusterOptions) {
		.options_version = ISCC_UT_OPTIONS_STRUCT_VERSION,
		.size_constraint = size_constraint,
		.num_types = 0,
		.type_constraints = NULL,
		.len_type_labels = 0,
		.type_labels = NULL,
		.seed_method = SCC_SM_BATCHES,
		.unassigned_method = unassigned_method,
		.radius_constraint = radius_constraint,
		.radius = radius,
		.len_primary_data_points = len_primary_data_points,
		.primary_data_points = primary_data_points,
		.secondary_unassigned_method = SCC_UM_IGNORE,
		.secondary_radius_constraint = false,
		.secondary_radius = 0.0,
		.batch_size = batch_size,
	};
}


void scc_ut_stress_nng_clustering(void** state)
{
	(void) state;

	srand(123456789);

	for (size_t r = 0; r < NUM_ROUNDS; ++r) {
		scc_ErrorCode ec;
		scc_Clustering* cl;
		scc_ClusterOptions options;
		bool cl_is_OK;

		double* const data_matrix = malloc(sizeof(double[DATA_DIMENSION * SAMPLE_SIZE]));
		scc_rand_double_array(0, 100, DATA_DIMENSION * SAMPLE_SIZE, data_matrix);

		bool* const primary_data_points = malloc(sizeof(bool[SAMPLE_SIZE]));
		scc_rand_bool_array(SAMPLE_SIZE, primary_data_points);

		scc_DataSet* data_set;
		ec = scc_init_data_set(SAMPLE_SIZE, DATA_DIMENSION, DATA_DIMENSION * SAMPLE_SIZE, data_matrix, &data_set);
		assert_int_equal(ec, SCC_ER_OK);

		const uint32_t size_constraint = scc_rand_uint(2, 10);
		const scc_SeedMethod seed_method = scc_rand_uint(SCC_SM_LEXICAL, SCC_SM_EXCLUSION_UPDATING);
		const scc_UnassignedMethod unassigned_method = scc_rand_uint(SCC_UM_IGNORE, SCC_UM_CLOSEST_SEED_EST_RADIUS);
		const scc_UnassignedMethod secondary_unassigned_method = scc_rand_uint(SCC_UM_IGNORE, SCC_UM_CLOSEST_SEED_EST_RADIUS);

		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		options = (scc_ClusterOptions) { ISCC_UT_OPTIONS_STRUCT_VERSION, size_constraint,
	                        0, NULL, 0, NULL,
	                            seed_method, unassigned_method, false, 0.0,
	                            0, NULL, SCC_UM_IGNORE, false, 0.0, 0 };
		ec = scc_make_clustering(data_set, cl, &options);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering(cl, size_constraint, 0, NULL, 0, NULL, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);

		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		options = (scc_ClusterOptions) { ISCC_UT_OPTIONS_STRUCT_VERSION, size_constraint,
	                        0, NULL, 0, NULL,
	                            seed_method, unassigned_method, true, 10.0,
	                            0, NULL, SCC_UM_IGNORE, false, 0.0, 0 };
		ec = scc_make_clustering(data_set, cl, &options);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering(cl, size_constraint, 0, NULL, 0, NULL, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);

		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		options = (scc_ClusterOptions) { ISCC_UT_OPTIONS_STRUCT_VERSION, size_constraint,
	                        0, NULL, 0, NULL,
	                            seed_method, unassigned_method, false, 0.0,
	                            SAMPLE_SIZE, primary_data_points, secondary_unassigned_method, false, 0.0, 0 };
		ec = scc_make_clustering(data_set, cl, &options);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering(cl, size_constraint, 0, NULL, 0, NULL, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);

		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		options = (scc_ClusterOptions) { ISCC_UT_OPTIONS_STRUCT_VERSION, size_constraint,
	                        0, NULL, 0, NULL,
	                            seed_method, unassigned_method, true, 10.0,
	                            SAMPLE_SIZE, primary_data_points, secondary_unassigned_method, false, 0.0, 0 };
		ec = scc_make_clustering(data_set, cl, &options);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering(cl, size_constraint, 0, NULL, 0, NULL, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);

		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		options = (scc_ClusterOptions) { ISCC_UT_OPTIONS_STRUCT_VERSION, size_constraint,
	                        0, NULL, 0, NULL,
	                            seed_method, unassigned_method, false, 0.0,
	                            SAMPLE_SIZE, primary_data_points, secondary_unassigned_method, true, 10.0, 0 };
		ec = scc_make_clustering(data_set, cl, &options);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering(cl, size_constraint, 0, NULL, 0, NULL, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);

		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		options = (scc_ClusterOptions) { ISCC_UT_OPTIONS_STRUCT_VERSION, size_constraint,
	                        0, NULL, 0, NULL,
	                            seed_method, unassigned_method, true, 10.0,
	                            SAMPLE_SIZE, primary_data_points, secondary_unassigned_method, true, 10.0, 0 };
		ec = scc_make_clustering(data_set, cl, &options);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering(cl, size_constraint, 0, NULL, 0, NULL, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);

		free(data_matrix);
		free(primary_data_points);
		scc_free_data_set(&data_set);
	}
}


void scc_ut_stress_nng_clustering_batches(void** state)
{
	(void) state;

	srand(123456789);

	for (size_t r = 0; r < NUM_ROUNDS; ++r) {
		scc_ErrorCode ec;
		scc_Clustering* cl;
		scc_ClusterOptions options;
		bool cl_is_OK;

		double* const data_matrix = malloc(sizeof(double[DATA_DIMENSION * SAMPLE_SIZE]));
		scc_rand_double_array(0, 100, DATA_DIMENSION * SAMPLE_SIZE, data_matrix);

		bool* const primary_data_points = malloc(sizeof(bool[SAMPLE_SIZE]));
		scc_rand_bool_array(SAMPLE_SIZE, primary_data_points);

		scc_DataSet* data_set;
		ec = scc_init_data_set(SAMPLE_SIZE, DATA_DIMENSION, DATA_DIMENSION * SAMPLE_SIZE, data_matrix, &data_set);
		assert_int_equal(ec, SCC_ER_OK);

		const uint32_t size_constraint = scc_rand_uint(2, 10);
		const scc_UnassignedMethod unassigned_method = scc_rand_uint(SCC_UM_IGNORE, SCC_UM_ANY_NEIGHBOR);
		const uint32_t batch_size = scc_rand_uint(10, 100);


		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		iscc_make_batch_options(&options, size_constraint,
                                        unassigned_method, false, 0.0,
                                        0, NULL, 0);
		ec = scc_make_clustering(data_set, cl, &options);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering(cl, size_constraint, 0, NULL, 0, NULL, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);


		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		iscc_make_batch_options(&options, size_constraint,
                                        unassigned_method, false, 0.0,
                                        0, NULL, 1);
		ec = scc_make_clustering(data_set, cl, &options);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering(cl, size_constraint, 0, NULL, 0, NULL, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);


		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		iscc_make_batch_options(&options, size_constraint,
                                        unassigned_method, false, 0.0,
                                        0, NULL, batch_size);
		ec = scc_make_clustering(data_set, cl, &options);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering(cl, size_constraint, 0, NULL, 0, NULL, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);


		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		iscc_make_batch_options(&options, size_constraint,
                                        unassigned_method, true, 10.0,
                                        0, NULL, batch_size);
		ec = scc_make_clustering(data_set, cl, &options);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering(cl, size_constraint, 0, NULL, 0, NULL, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);


		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		iscc_make_batch_options(&options, size_constraint,
                                        unassigned_method, false, 0.0,
                                        SAMPLE_SIZE, primary_data_points, batch_size);
		ec = scc_make_clustering(data_set, cl, &options);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering(cl, size_constraint, 0, NULL, 0, NULL, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);


		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		iscc_make_batch_options(&options, size_constraint,
                                        unassigned_method, true, 10.0,
                                        SAMPLE_SIZE, primary_data_points, batch_size);
		ec = scc_make_clustering(data_set, cl, &options);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering(cl, size_constraint, 0, NULL, 0, NULL, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);


		free(data_matrix);
		free(primary_data_points);
		scc_free_data_set(&data_set);
	}
}


void scc_ut_stress_nng_clustering_with_types(void** state)
{
	(void) state;

	srand(123456789);

	for (size_t r = 0; r < NUM_ROUNDS; ++r) {
		scc_ErrorCode ec;
		scc_Clustering* cl;
		scc_ClusterOptions options;
		bool cl_is_OK;

		double* const data_matrix = malloc(sizeof(double[DATA_DIMENSION * SAMPLE_SIZE]));
		scc_rand_double_array(0, 100, DATA_DIMENSION * SAMPLE_SIZE, data_matrix);

		uint16_t num_types = scc_rand_uint(2, 10);
		uint32_t* const type_constraints = malloc(sizeof(uint32_t[num_types]));
		scc_rand_uint32_t_array(0, 5, num_types, type_constraints);
		scc_TypeLabel* const type_labels = malloc(sizeof(scc_TypeLabel[SAMPLE_SIZE]));
		uint16_t* const type_labels_tmp = malloc(sizeof(uint16_t[SAMPLE_SIZE]));
		scc_rand_uint16_t_array(0, (uint16_t) (num_types - 1), SAMPLE_SIZE, type_labels_tmp);
		for (size_t i = 0; i < SAMPLE_SIZE; ++i) {
			type_labels[i] = ((scc_TypeLabel) type_labels_tmp[i]);
		}
		free(type_labels_tmp);

		bool* const primary_data_points = malloc(sizeof(bool[SAMPLE_SIZE]));
		scc_rand_bool_array(SAMPLE_SIZE, primary_data_points);

		scc_DataSet* data_set;
		ec = scc_init_data_set(SAMPLE_SIZE, DATA_DIMENSION, DATA_DIMENSION * SAMPLE_SIZE, data_matrix, &data_set);
		assert_int_equal(ec, SCC_ER_OK);

		uint32_t sum_type_constraints = 0;
		for (uint16_t t = 0; t < num_types; ++t) {
			sum_type_constraints += type_constraints[t];
		}
		const uint32_t size_constraint = sum_type_constraints + scc_rand_uint(0, 2);
		const scc_SeedMethod seed_method = scc_rand_uint(SCC_SM_LEXICAL, SCC_SM_EXCLUSION_UPDATING);
		const scc_UnassignedMethod unassigned_method = scc_rand_uint(SCC_UM_IGNORE, SCC_UM_CLOSEST_SEED_EST_RADIUS);
		const scc_UnassignedMethod secondary_unassigned_method = scc_rand_uint(SCC_UM_IGNORE, SCC_UM_CLOSEST_SEED_EST_RADIUS);

		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		options = (scc_ClusterOptions) { ISCC_UT_OPTIONS_STRUCT_VERSION, size_constraint,
	                                       num_types, type_constraints, SAMPLE_SIZE, type_labels,
	                                       seed_method, unassigned_method, false, 0.0,
	                                       0, NULL, SCC_UM_IGNORE, false, 0.0, 0 };
		ec = scc_make_clustering(data_set, cl, &options);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering(cl, size_constraint, num_types, type_constraints, SAMPLE_SIZE, type_labels, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);

		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		options = (scc_ClusterOptions) { ISCC_UT_OPTIONS_STRUCT_VERSION, size_constraint,
	                                       num_types, type_constraints, SAMPLE_SIZE, type_labels,
	                                       seed_method, unassigned_method, true, 10.0,
	                                       0, NULL, SCC_UM_IGNORE, false, 0.0, 0 };
		ec = scc_make_clustering(data_set, cl, &options);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering(cl, size_constraint, num_types, type_constraints, SAMPLE_SIZE, type_labels, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);

		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		options = (scc_ClusterOptions) { ISCC_UT_OPTIONS_STRUCT_VERSION, size_constraint,
	                                       num_types, type_constraints, SAMPLE_SIZE, type_labels,
	                                       seed_method, unassigned_method, false, 0.0,
	                                       SAMPLE_SIZE, primary_data_points, secondary_unassigned_method, false, 0.0, 0 };
		ec = scc_make_clustering(data_set, cl, &options);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering(cl, size_constraint, num_types, type_constraints, SAMPLE_SIZE, type_labels, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);

		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		options = (scc_ClusterOptions) { ISCC_UT_OPTIONS_STRUCT_VERSION, size_constraint,
	                                       num_types, type_constraints, SAMPLE_SIZE, type_labels,
	                                       seed_method, unassigned_method, true, 10.0,
	                                       SAMPLE_SIZE, primary_data_points, secondary_unassigned_method, false, 0.0, 0 };
		ec = scc_make_clustering(data_set, cl, &options);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering(cl, size_constraint, num_types, type_constraints, SAMPLE_SIZE, type_labels, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);

		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		options = (scc_ClusterOptions) { ISCC_UT_OPTIONS_STRUCT_VERSION, size_constraint,
	                                       num_types, type_constraints, SAMPLE_SIZE, type_labels,
	                                       seed_method, unassigned_method, false, 0.0,
	                                       SAMPLE_SIZE, primary_data_points, secondary_unassigned_method, true, 10.0, 0 };
		ec = scc_make_clustering(data_set, cl, &options);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering(cl, size_constraint, num_types, type_constraints, SAMPLE_SIZE, type_labels, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);

		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		options = (scc_ClusterOptions) { ISCC_UT_OPTIONS_STRUCT_VERSION, size_constraint,
	                                       num_types, type_constraints, SAMPLE_SIZE, type_labels,
	                                       seed_method, unassigned_method, true, 10.0,
	                                       SAMPLE_SIZE, primary_data_points, secondary_unassigned_method, true, 10.0, 0 };
		ec = scc_make_clustering(data_set, cl, &options);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering(cl, size_constraint, num_types, type_constraints, SAMPLE_SIZE, type_labels, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);

		free(data_matrix);
		free(type_constraints);
		free(type_labels);
		free(primary_data_points);
		scc_free_data_set(&data_set);
	}
}


int main(void)
{
	if(!scc_ut_init_tests()) return 1;

	const struct CMUnitTest test_cases[] = {
		cmocka_unit_test(scc_ut_stress_nng_clustering),
		cmocka_unit_test(scc_ut_stress_nng_clustering_batches),
		cmocka_unit_test(scc_ut_stress_nng_clustering_with_types),
	};

	return cmocka_run_group_tests_name("stress nng_clustering.c", test_cases, NULL, NULL);
}
