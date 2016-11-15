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

#include "test_suite.h"
#include "../include/scclust.h"
#include "../src/error.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


void scc_ut_get_compiled_version(void** state)
{
	(void) state;

	uint32_t major, minor, patch;

	major = minor = patch = 123456;
	scc_get_compiled_version(&major, &minor, &patch);
	assert_int_equal(major, SCC_SCCLUST_MAJOR_VERSION);
	assert_int_equal(minor, SCC_SCCLUST_MINOR_VERSION);
	assert_int_equal(patch, SCC_SCCLUST_PATCH_VERSION);

	major = minor = patch = 123456;
	scc_get_compiled_version(NULL, &minor, &patch);
	assert_int_equal(major, 123456);
	assert_int_equal(minor, SCC_SCCLUST_MINOR_VERSION);
	assert_int_equal(patch, SCC_SCCLUST_PATCH_VERSION);

	major = minor = patch = 123456;
	scc_get_compiled_version(&major, NULL, &patch);
	assert_int_equal(major, SCC_SCCLUST_MAJOR_VERSION);
	assert_int_equal(minor, 123456);
	assert_int_equal(patch, SCC_SCCLUST_PATCH_VERSION);

	major = minor = patch = 123456;
	scc_get_compiled_version(&major, &minor, NULL);
	assert_int_equal(major, SCC_SCCLUST_MAJOR_VERSION);
	assert_int_equal(minor, SCC_SCCLUST_MINOR_VERSION);
	assert_int_equal(patch, 123456);
}


void scc_ut_get_error_message(void** state)
{
	(void) state;

	const size_t buffer_size = 256;
	char text_buffer[buffer_size];

	bool err_res1 = scc_get_latest_error(0, text_buffer);
	assert_false(err_res1);

	bool err_res2 = scc_get_latest_error(buffer_size, NULL);
	assert_false(err_res2);

	scc_ErrorCode ec3 = iscc_no_error();
	bool err_res3 = scc_get_latest_error(buffer_size, text_buffer);
	assert_true(err_res3);
	assert_int_equal(ec3, SCC_ER_OK);
	assert_string_equal(text_buffer, "No error.");

	scc_ErrorCode ec4 = iscc_make_error(SCC_ER_NULL_INPUT);
	bool err_res4 = scc_get_latest_error(buffer_size, text_buffer);
	assert_true(err_res4);
	assert_int_equal(ec4, SCC_ER_NULL_INPUT);
	assert_string_equal(text_buffer, "(scclust:test_error.c:81) A required input pointer is NULL.");

	iscc_reset_error();
	bool err_res5 = scc_get_latest_error(buffer_size, text_buffer);
	assert_true(err_res5);
	assert_string_equal(text_buffer, "No error.");

	scc_ErrorCode ec6 = iscc_make_error_func(SCC_ER_INVALID_INPUT, "dummy1.c", 1);
	bool err_res6 = scc_get_latest_error(buffer_size, text_buffer);
	assert_true(err_res6);
	assert_int_equal(ec6, SCC_ER_INVALID_INPUT);
	assert_string_equal(text_buffer, "(scclust:dummy1.c:1) Inputted function parameters are invalid.");

	scc_ErrorCode ec7 = iscc_make_error_func(SCC_ER_INVALID_CLUSTERING, "dummy2.c", 2);
	bool err_res7 = scc_get_latest_error(buffer_size, text_buffer);
	assert_true(err_res7);
	assert_int_equal(ec7, SCC_ER_INVALID_CLUSTERING);
	assert_string_equal(text_buffer, "(scclust:dummy2.c:2) Inputted clustering is invalid.");

	scc_ErrorCode ec8 = iscc_make_error_func(SCC_ER_EMPTY_CLUSTERING, "dummy3.c", 3);
	bool err_res8 = scc_get_latest_error(buffer_size, text_buffer);
	assert_true(err_res8);
	assert_int_equal(ec8, SCC_ER_EMPTY_CLUSTERING);
	assert_string_equal(text_buffer, "(scclust:dummy3.c:3) Empty clustering is inputted when non-empty is required.");

	scc_ErrorCode ec9 = iscc_make_error_func(SCC_ER_INVALID_DATA_OBJ, "dummy4.c", 4);
	bool err_res9 = scc_get_latest_error(buffer_size, text_buffer);
	assert_true(err_res9);
	assert_int_equal(ec9, SCC_ER_INVALID_DATA_OBJ);
	assert_string_equal(text_buffer, "(scclust:dummy4.c:4) Inputted data object is invalid.");

	scc_ErrorCode ec10 = iscc_make_error_func(SCC_ER_NO_MEMORY, "dummy5.c", 5);
	bool err_res10 = scc_get_latest_error(buffer_size, text_buffer);
	assert_true(err_res10);
	assert_int_equal(ec10, SCC_ER_NO_MEMORY);
	assert_string_equal(text_buffer, "(scclust:dummy5.c:5) Cannot allocate required memory.");

	scc_ErrorCode ec11 = iscc_make_error_func(SCC_ER_TOO_LARGE_PROBLEM, "dummy6.c", 6);
	bool err_res11 = scc_get_latest_error(100, text_buffer);
	assert_true(err_res11);
	assert_int_equal(ec11, SCC_ER_TOO_LARGE_PROBLEM);
	assert_string_equal(text_buffer, "(scclust:dummy6.c:6) The clustering problem is too large under the current configuration (either to");

	scc_ErrorCode ec12 = iscc_make_error_func(SCC_ER_TOO_LARGE_DIGRAPH, "dummy7.c", 7);
	bool err_res12 = scc_get_latest_error(buffer_size, text_buffer);
	assert_true(err_res12);
	assert_int_equal(ec12, SCC_ER_TOO_LARGE_DIGRAPH);
	assert_string_equal(text_buffer, "(scclust:dummy7.c:7) The clustering problem yields a digraph with too many arcs.");

	scc_ErrorCode ec13 = iscc_make_error_func(SCC_ER_DIST_SEARCH_ERROR, "dummy8.c", 8);
	bool err_res13 = scc_get_latest_error(buffer_size, text_buffer);
	assert_true(err_res13);
	assert_int_equal(ec13, SCC_ER_DIST_SEARCH_ERROR);
	assert_string_equal(text_buffer, "(scclust:dummy8.c:8) Failed to calculate distances.");

	scc_ErrorCode ec14 = iscc_make_error_func(SCC_ER_NO_CLUST_EXIST_CONSTRAINT, "dummy9.c", 9);
	bool err_res14 = scc_get_latest_error(buffer_size, text_buffer);
	assert_true(err_res14);
	assert_int_equal(ec14, SCC_ER_NO_CLUST_EXIST_CONSTRAINT);
	assert_string_equal(text_buffer, "(scclust:dummy9.c:9) No clustering satisfying the specified constraints exists.");

	scc_ErrorCode ec15 = iscc_make_error_func(SCC_ER_NO_CLUST_EXIST_RADIUS, "dummy10.c", 10);
	bool err_res15 = scc_get_latest_error(buffer_size, text_buffer);
	assert_true(err_res15);
	assert_int_equal(ec15, SCC_ER_NO_CLUST_EXIST_RADIUS);
	assert_string_equal(text_buffer, "(scclust:dummy10.c:10) No clustering satisfying the specified radius constraints exists.");

	scc_ErrorCode ec16 = iscc_make_error_func(SCC_ER_NOT_IMPLEMENTED, "dummy11.c", 11);
	bool err_res16 = scc_get_latest_error(buffer_size, text_buffer);
	assert_true(err_res16);
	assert_int_equal(ec16, SCC_ER_NOT_IMPLEMENTED);
	assert_string_equal(text_buffer, "(scclust:dummy11.c:11) Requested functionality is not yet implemented.");
}


int main(void)
{
	const struct CMUnitTest test_cases[] = {
		cmocka_unit_test(scc_ut_get_compiled_version),
		cmocka_unit_test(scc_ut_get_error_message),
	};

	return cmocka_run_group_tests_name("error.c", test_cases, NULL, NULL);
}
