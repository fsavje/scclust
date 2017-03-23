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
                                    const scc_PointIndex primary_data_points[],
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
		.len_primary_data_points = len_primary_data_points,
		.primary_data_points = primary_data_points,
		.primary_unassigned_method = unassigned_method,
		.secondary_unassigned_method = SCC_UM_IGNORE,
		.seed_radius = radius_constraint,
		.seed_supplied_radius = radius,
		.primary_radius = SCC_RM_USE_SEED_RADIUS,
		.primary_supplied_radius = 0.0,
		.secondary_radius = SCC_RM_NO_RADIUS,
		.secondary_supplied_radius = 0.0,
		.batch_size = batch_size,
	};
}


static scc_ErrorCode scc_check_clustering_wrap(const scc_Clustering* const clustering,
                                               const uint32_t size_constraint,
                                               const uintmax_t num_types,
                                               const uint32_t type_constraints[const],
                                               const size_t len_type_labels,
                                               const scc_TypeLabel type_labels[const],
                                               bool* const out_is_OK)
{
	scc_ClusterOptions options = scc_default_options();
	options.size_constraint = size_constraint;
	options.num_types = num_types;
	options.type_constraints = type_constraints;
	options.len_type_labels = len_type_labels;
	options.type_labels = type_labels;

	return scc_check_clustering(clustering, options, out_is_OK);
}


static scc_ClusterOptions iscc_translate_options(const uint32_t size_constraint,
                                                 const uint32_t num_types,
                                                 const uint32_t* const type_constraints,
                                                 const size_t len_type_labels,
                                                 const scc_TypeLabel* const type_labels,
                                                 const scc_SeedMethod seed_method,
                                                 const scc_UnassignedMethod unassigned_method,
                                                 const bool radius_constraint,
                                                 const double radius,
                                                 const size_t len_primary_data_points,
                                                 const scc_PointIndex* const primary_data_points,
                                                 const scc_UnassignedMethod secondary_unassigned_method,
                                                 const bool secondary_radius_constraint,
                                                 const double secondary_radius,
                                                const  uint32_t batch_size)
{
	return (scc_ClusterOptions) {
		.options_version = ISCC_UT_OPTIONS_STRUCT_VERSION,
		.size_constraint = size_constraint,
		.num_types = num_types,
		.type_constraints = type_constraints,
		.len_type_labels = len_type_labels,
		.type_labels = type_labels,
		.seed_method = seed_method,
		.len_primary_data_points = len_primary_data_points,
		.primary_data_points = primary_data_points,
		.primary_unassigned_method = unassigned_method,
		.secondary_unassigned_method = secondary_unassigned_method,
		.seed_radius = radius_constraint,
		.seed_supplied_radius = radius,
		.primary_radius = SCC_RM_USE_SEED_RADIUS,
		.primary_supplied_radius = 0.0,
		.secondary_radius = secondary_radius_constraint,
		.secondary_supplied_radius = secondary_radius,
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

		bool* const construct_primary_data_points = malloc(sizeof(bool[SAMPLE_SIZE]));
		scc_rand_bool_array(SAMPLE_SIZE, construct_primary_data_points);

		size_t len_primary_data_points = 0;
		scc_PointIndex* const primary_data_points = malloc(sizeof(scc_PointIndex[SAMPLE_SIZE]));
		for (size_t i = 0, index = 0; i < SAMPLE_SIZE; ++i) {
			primary_data_points[index] = (scc_PointIndex) i;
			index += construct_primary_data_points[i];
		}

		scc_DataSet* data_set;
		ec = scc_init_data_set(SAMPLE_SIZE, DATA_DIMENSION, DATA_DIMENSION * SAMPLE_SIZE, data_matrix, &data_set);
		assert_int_equal(ec, SCC_ER_OK);

		const uint32_t size_constraint = scc_rand_uint(2, 10);
		const scc_SeedMethod seed_method = scc_rand_uint(SCC_SM_LEXICAL, SCC_SM_EXCLUSION_UPDATING);
		const scc_UnassignedMethod unassigned_method = scc_rand_uint(SCC_UM_IGNORE, SCC_UM_CLOSEST_SEED);
		const scc_UnassignedMethod secondary_unassigned_method = scc_rand_uint(SCC_UM_IGNORE, SCC_UM_CLOSEST_SEED);

		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		options = iscc_translate_options(size_constraint,
	                        0, NULL, 0, NULL,
	                            seed_method, unassigned_method, false, 0.0,
	                            0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
		ec = scc_sc_clustering(data_set, &options, cl);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering_wrap(cl, size_constraint, 0, NULL, 0, NULL, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);

		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		options = iscc_translate_options(size_constraint,
	                        0, NULL, 0, NULL,
	                            seed_method, unassigned_method, true, 10.0,
	                            0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
		ec = scc_sc_clustering(data_set, &options, cl);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering_wrap(cl, size_constraint, 0, NULL, 0, NULL, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);

		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		options = iscc_translate_options(size_constraint,
	                        0, NULL, 0, NULL,
	                            seed_method, unassigned_method, false, 0.0,
	                            len_primary_data_points, primary_data_points, secondary_unassigned_method, false, 0.0, 0);
		ec = scc_sc_clustering(data_set, &options, cl);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering_wrap(cl, size_constraint, 0, NULL, 0, NULL, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);

		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		options = iscc_translate_options(size_constraint,
	                        0, NULL, 0, NULL,
	                            seed_method, unassigned_method, true, 10.0,
	                            len_primary_data_points, primary_data_points, secondary_unassigned_method, false, 0.0, 0);
		ec = scc_sc_clustering(data_set, &options, cl);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering_wrap(cl, size_constraint, 0, NULL, 0, NULL, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);

		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		options = iscc_translate_options(size_constraint,
	                        0, NULL, 0, NULL,
	                            seed_method, unassigned_method, false, 0.0,
	                            len_primary_data_points, primary_data_points, secondary_unassigned_method, true, 10.0, 0);
		ec = scc_sc_clustering(data_set, &options, cl);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering_wrap(cl, size_constraint, 0, NULL, 0, NULL, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);

		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		options = iscc_translate_options(size_constraint,
	                        0, NULL, 0, NULL,
	                            seed_method, unassigned_method, true, 10.0,
	                            len_primary_data_points, primary_data_points, secondary_unassigned_method, true, 10.0, 0);
		ec = scc_sc_clustering(data_set, &options, cl);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering_wrap(cl, size_constraint, 0, NULL, 0, NULL, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);

		free(data_matrix);
		free(construct_primary_data_points);
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

		bool* const construct_primary_data_points = malloc(sizeof(bool[SAMPLE_SIZE]));
		scc_rand_bool_array(SAMPLE_SIZE, construct_primary_data_points);

		size_t len_primary_data_points = 0;
		scc_PointIndex* const primary_data_points = malloc(sizeof(scc_PointIndex[SAMPLE_SIZE]));
		for (size_t i = 0, index = 0; i < SAMPLE_SIZE; ++i) {
			primary_data_points[index] = (scc_PointIndex) i;
			index += construct_primary_data_points[i];
		}

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
		ec = scc_sc_clustering(data_set, &options, cl);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering_wrap(cl, size_constraint, 0, NULL, 0, NULL, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);


		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		iscc_make_batch_options(&options, size_constraint,
                                        unassigned_method, false, 0.0,
                                        0, NULL, 1);
		ec = scc_sc_clustering(data_set, &options, cl);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering_wrap(cl, size_constraint, 0, NULL, 0, NULL, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);


		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		iscc_make_batch_options(&options, size_constraint,
                                        unassigned_method, false, 0.0,
                                        0, NULL, batch_size);
		ec = scc_sc_clustering(data_set, &options, cl);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering_wrap(cl, size_constraint, 0, NULL, 0, NULL, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);


		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		iscc_make_batch_options(&options, size_constraint,
                                        unassigned_method, true, 10.0,
                                        0, NULL, batch_size);
		ec = scc_sc_clustering(data_set, &options, cl);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering_wrap(cl, size_constraint, 0, NULL, 0, NULL, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);


		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		iscc_make_batch_options(&options, size_constraint,
                                        unassigned_method, false, 0.0,
                                        len_primary_data_points, primary_data_points, batch_size);
		ec = scc_sc_clustering(data_set, &options, cl);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering_wrap(cl, size_constraint, 0, NULL, 0, NULL, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);


		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		iscc_make_batch_options(&options, size_constraint,
                                        unassigned_method, true, 10.0,
                                        len_primary_data_points, primary_data_points, batch_size);
		ec = scc_sc_clustering(data_set, &options, cl);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering_wrap(cl, size_constraint, 0, NULL, 0, NULL, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);


		free(data_matrix);
		free(construct_primary_data_points);
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

		bool* const construct_primary_data_points = malloc(sizeof(bool[SAMPLE_SIZE]));
		scc_rand_bool_array(SAMPLE_SIZE, construct_primary_data_points);

		size_t len_primary_data_points = 0;
		scc_PointIndex* const primary_data_points = malloc(sizeof(scc_PointIndex[SAMPLE_SIZE]));
		for (size_t i = 0, index = 0; i < SAMPLE_SIZE; ++i) {
			primary_data_points[index] = (scc_PointIndex) i;
			index += construct_primary_data_points[i];
		}

		scc_DataSet* data_set;
		ec = scc_init_data_set(SAMPLE_SIZE, DATA_DIMENSION, DATA_DIMENSION * SAMPLE_SIZE, data_matrix, &data_set);
		assert_int_equal(ec, SCC_ER_OK);

		uint32_t sum_type_constraints = 0;
		for (uint16_t t = 0; t < num_types; ++t) {
			sum_type_constraints += type_constraints[t];
		}
		const uint32_t size_constraint = sum_type_constraints + scc_rand_uint(0, 2);
		const scc_SeedMethod seed_method = scc_rand_uint(SCC_SM_LEXICAL, SCC_SM_EXCLUSION_UPDATING);
		const scc_UnassignedMethod unassigned_method = scc_rand_uint(SCC_UM_IGNORE, SCC_UM_CLOSEST_SEED);
		const scc_UnassignedMethod secondary_unassigned_method = scc_rand_uint(SCC_UM_IGNORE, SCC_UM_CLOSEST_SEED);

		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		options = iscc_translate_options(size_constraint,
	                                       num_types, type_constraints, SAMPLE_SIZE, type_labels,
	                                       seed_method, unassigned_method, false, 0.0,
	                                       0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
		ec = scc_sc_clustering(data_set, &options, cl);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering_wrap(cl, size_constraint, num_types, type_constraints, SAMPLE_SIZE, type_labels, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);

		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		options = iscc_translate_options(size_constraint,
	                                       num_types, type_constraints, SAMPLE_SIZE, type_labels,
	                                       seed_method, unassigned_method, true, 10.0,
	                                       0, NULL, SCC_UM_IGNORE, false, 0.0, 0);
		ec = scc_sc_clustering(data_set, &options, cl);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering_wrap(cl, size_constraint, num_types, type_constraints, SAMPLE_SIZE, type_labels, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);

		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		options = iscc_translate_options(size_constraint,
	                                       num_types, type_constraints, SAMPLE_SIZE, type_labels,
	                                       seed_method, unassigned_method, false, 0.0,
	                                       len_primary_data_points, primary_data_points, secondary_unassigned_method, false, 0.0, 0);
		ec = scc_sc_clustering(data_set, &options, cl);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering_wrap(cl, size_constraint, num_types, type_constraints, SAMPLE_SIZE, type_labels, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);

		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		options = iscc_translate_options(size_constraint,
	                                       num_types, type_constraints, SAMPLE_SIZE, type_labels,
	                                       seed_method, unassigned_method, true, 10.0,
	                                       len_primary_data_points, primary_data_points, secondary_unassigned_method, false, 0.0, 0);
		ec = scc_sc_clustering(data_set, &options, cl);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering_wrap(cl, size_constraint, num_types, type_constraints, SAMPLE_SIZE, type_labels, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);

		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		options = iscc_translate_options(size_constraint,
	                                       num_types, type_constraints, SAMPLE_SIZE, type_labels,
	                                       seed_method, unassigned_method, false, 0.0,
	                                       len_primary_data_points, primary_data_points, secondary_unassigned_method, true, 10.0, 0);
		ec = scc_sc_clustering(data_set, &options, cl);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering_wrap(cl, size_constraint, num_types, type_constraints, SAMPLE_SIZE, type_labels, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);

		ec = scc_init_empty_clustering(SAMPLE_SIZE, NULL, &cl);
		assert_int_equal(ec, SCC_ER_OK);
		options = iscc_translate_options(size_constraint,
	                                       num_types, type_constraints, SAMPLE_SIZE, type_labels,
	                                       seed_method, unassigned_method, true, 10.0,
	                                       len_primary_data_points, primary_data_points, secondary_unassigned_method, true, 10.0, 0);
		ec = scc_sc_clustering(data_set, &options, cl);
		if (ec == SCC_ER_OK) {
			ec = scc_check_clustering_wrap(cl, size_constraint, num_types, type_constraints, SAMPLE_SIZE, type_labels, &cl_is_OK);
			assert_int_equal(ec, SCC_ER_OK);
			assert_true(cl_is_OK);
		}
		scc_free_clustering(&cl);

		free(data_matrix);
		free(type_constraints);
		free(type_labels);
		free(construct_primary_data_points);
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
