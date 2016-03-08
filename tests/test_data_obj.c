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
#include "../include/data_obj.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include "../include/scclust.h"
#include "../src/config.h"
#include "../src/data_obj_int.h"

// ISCC_DPID_MAX should be UINT32_MAX
// SIZE_MAX is redefined to (UINT32_MAX - 10) by test_suite.h

void scc_ut_free_data_set_object(void** state)
{
	(void) state;

	scc_DataSetObject* null = NULL;
	double* const coord1 = malloc(sizeof(double[5]));
	double coord2[5] = { 58.339591, 14.339080, 54.090796, 32.886948, 92.021040 };

	scc_DataSetObject* dso1 = malloc(sizeof(scc_DataSetObject));
	*dso1 = (scc_DataSetObject) {
		.num_data_points = 123,
		.num_dimensions = 3,
		.data_matrix = NULL,
		.external_matrix = false,
		.data_set_object_version = ISCC_CURRENT_DATASETOBJ_VERSION,
	};

	scc_DataSetObject* dso2 = malloc(sizeof(scc_DataSetObject));
	*dso2 = (scc_DataSetObject) {
		.num_data_points = 123,
		.num_dimensions = 3,
		.data_matrix = coord1,
		.external_matrix = false,
		.data_set_object_version = ISCC_CURRENT_DATASETOBJ_VERSION,
	};

	scc_DataSetObject* dso3 = malloc(sizeof(scc_DataSetObject));
	*dso3 = (scc_DataSetObject) {
		.num_data_points = 123,
		.num_dimensions = 3,
		.data_matrix = coord2,
		.external_matrix = true,
		.data_set_object_version = ISCC_CURRENT_DATASETOBJ_VERSION,
	};

	scc_free_data_set_object(NULL);
	scc_free_data_set_object(&null);
	scc_free_data_set_object(&dso1);
	scc_free_data_set_object(&dso2);
	scc_free_data_set_object(&dso3);

	assert_null(null);
	assert_null(dso1);
	assert_null(dso2);
	assert_null(dso3);
}


void scc_ut_get_data_set_object(void** state)
{
	(void) state;

	double coord[10] = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0 };
	double ref_coord[10] = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0 };

	scc_ErrorCode ec1 = scc_get_data_set_object(5, 2, 10, coord, false, NULL);
	assert_int_equal(ec1, SCC_ER_NULL_INPUT);

	scc_DataSetObject* dso2;
	scc_ErrorCode ec2 = scc_get_data_set_object(0, 2, 10, coord, false, &dso2);
	assert_null(dso2);
	assert_int_equal(ec2, SCC_ER_INVALID_INPUT);

	scc_DataSetObject* dso3;
	scc_ErrorCode ec3 = scc_get_data_set_object(((uint64_t) UINT32_MAX) + 1, 2, 10, coord, false, &dso3);
	assert_null(dso3);
	assert_int_equal(ec3, SCC_ER_TOO_LARGE_PROBLEM);

	scc_DataSetObject* dso4;
	scc_ErrorCode ec4 = scc_get_data_set_object(((uint64_t) UINT32_MAX) - 5, 2, 10, coord, false, &dso4);
	assert_null(dso4);
	assert_int_equal(ec4, SCC_ER_TOO_LARGE_PROBLEM);

	scc_DataSetObject* dso5;
	scc_ErrorCode ec5 = scc_get_data_set_object(5, 0, 10, coord, false, &dso5);
	assert_null(dso5);
	assert_int_equal(ec5, SCC_ER_INVALID_INPUT);

	scc_DataSetObject* dso6;
	scc_ErrorCode ec6 = scc_get_data_set_object(5, ((uint64_t) UINT16_MAX) + 1, 10, coord, false, &dso6);
	assert_null(dso6);
	assert_int_equal(ec6, SCC_ER_TOO_LARGE_PROBLEM);

	scc_DataSetObject* dso7;
	scc_ErrorCode ec7 = scc_get_data_set_object(5, 2, 8, coord, false, &dso7);
	assert_null(dso7);
	assert_int_equal(ec7, SCC_ER_INVALID_INPUT);

	scc_DataSetObject* dso8;
	scc_ErrorCode ec8 = scc_get_data_set_object(5, 2, 10, NULL, false, &dso8);
	assert_null(dso8);
	assert_int_equal(ec8, SCC_ER_NULL_INPUT);

	scc_DataSetObject* dso9;
	scc_ErrorCode ec9 = scc_get_data_set_object(5, 2, 10, coord, false, &dso9);
	assert_non_null(dso9);
	assert_int_equal(dso9->num_data_points, 5);
	assert_int_equal(dso9->num_dimensions, 2);
	assert_non_null(dso9->data_matrix);
	assert_ptr_equal(dso9->data_matrix, coord);
	assert_memory_equal(dso9->data_matrix, ref_coord, 10 * sizeof(double));
	assert_true(dso9->external_matrix);
	assert_int_equal(dso9->data_set_object_version, ISCC_CURRENT_DATASETOBJ_VERSION);
	assert_int_equal(ec9, SCC_ER_OK);
	
	scc_DataSetObject* dso10;
	scc_ErrorCode ec10 = scc_get_data_set_object(5, 2, 10, coord, true, &dso10);
	assert_non_null(dso10);
	assert_int_equal(dso10->num_data_points, 5);
	assert_int_equal(dso10->num_dimensions, 2);
	assert_non_null(dso10->data_matrix);
	assert_ptr_not_equal(dso10->data_matrix, coord);
	assert_memory_equal(dso10->data_matrix, ref_coord, 10 * sizeof(double));
	assert_false(dso10->external_matrix);
	assert_int_equal(dso10->data_set_object_version, ISCC_CURRENT_DATASETOBJ_VERSION);
	assert_int_equal(ec10, SCC_ER_OK);

	scc_DataSetObject* dso11;
	scc_ErrorCode ec11 = scc_get_data_set_object(3, 1, 10, coord, false, &dso11);
	assert_non_null(dso11);
	assert_int_equal(dso11->num_data_points, 3);
	assert_int_equal(dso11->num_dimensions, 1);
	assert_non_null(dso11->data_matrix);
	assert_ptr_equal(dso11->data_matrix, coord);
	assert_memory_equal(dso11->data_matrix, ref_coord, 3 * sizeof(double));
	assert_true(dso11->external_matrix);
	assert_int_equal(dso11->data_set_object_version, ISCC_CURRENT_DATASETOBJ_VERSION);
	assert_int_equal(ec11, SCC_ER_OK);
	
	scc_DataSetObject* dso12;
	scc_ErrorCode ec12 = scc_get_data_set_object(3, 1, 10, coord, true, &dso12);
	assert_non_null(dso12);
	assert_int_equal(dso12->num_data_points, 3);
	assert_int_equal(dso12->num_dimensions, 1);
	assert_non_null(dso12->data_matrix);
	assert_ptr_not_equal(dso12->data_matrix, coord);
	assert_memory_equal(dso12->data_matrix, ref_coord, 3 * sizeof(double));
	assert_false(dso12->external_matrix);
	assert_int_equal(dso12->data_set_object_version, ISCC_CURRENT_DATASETOBJ_VERSION);
	assert_int_equal(ec12, SCC_ER_OK);

	scc_free_data_set_object(&dso9);
	scc_free_data_set_object(&dso10);
	scc_free_data_set_object(&dso11);
	scc_free_data_set_object(&dso12);
}


int main(void)
{
	const struct CMUnitTest test_cases[] = {
		cmocka_unit_test(scc_ut_free_data_set_object),
		cmocka_unit_test(scc_ut_get_data_set_object),
	};
	
	return cmocka_run_group_tests_name("data_obj.c", test_cases, NULL, NULL);
}
