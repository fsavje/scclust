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
#include <stdint.h>
#include <include/scclust.h>
#include <src/error.h>


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
	assert_string_equal(text_buffer, "(scclust) No error.");

	scc_ErrorCode ec4 = iscc_make_error(SCC_ER_INVALID_INPUT);
	bool err_res4 = scc_get_latest_error(buffer_size, text_buffer);
	assert_true(err_res4);
	assert_int_equal(ec4, SCC_ER_INVALID_INPUT);
	assert_string_equal(text_buffer, "(scclust:test_error.c:48) Function parameters are invalid.");

	scc_ErrorCode ec4b = iscc_make_error_msg(SCC_ER_INVALID_INPUT, "Test message 12345.");
	bool err_res4b = scc_get_latest_error(buffer_size, text_buffer);
	assert_true(err_res4b);
	assert_int_equal(ec4b, SCC_ER_INVALID_INPUT);
	assert_string_equal(text_buffer, "(scclust:test_error.c:54) Test message 12345.");

	iscc_reset_error();
	bool err_res5 = scc_get_latest_error(buffer_size, text_buffer);
	assert_true(err_res5);
	assert_string_equal(text_buffer, "(scclust) No error.");

	scc_ErrorCode ec6 = iscc_make_error__(SCC_ER_UNKNOWN_ERROR, NULL, "dummy1.c", 1);
	bool err_res6 = scc_get_latest_error(buffer_size, text_buffer);
	assert_true(err_res6);
	assert_int_equal(ec6, SCC_ER_UNKNOWN_ERROR);
	assert_string_equal(text_buffer, "(scclust:dummy1.c:1) Unkonwn error.");

	scc_ErrorCode ec7 = iscc_make_error__(SCC_ER_INVALID_INPUT, NULL, "dummy2.c", 2);
	bool err_res7 = scc_get_latest_error(buffer_size, text_buffer);
	assert_true(err_res7);
	assert_int_equal(ec7, SCC_ER_INVALID_INPUT);
	assert_string_equal(text_buffer, "(scclust:dummy2.c:2) Function parameters are invalid.");

	scc_ErrorCode ec8 = iscc_make_error__(SCC_ER_NO_MEMORY, NULL, "dummy3.c", 3);
	bool err_res8 = scc_get_latest_error(buffer_size, text_buffer);
	assert_true(err_res8);
	assert_int_equal(ec8, SCC_ER_NO_MEMORY);
	assert_string_equal(text_buffer, "(scclust:dummy3.c:3) Cannot allocate required memory.");

	scc_ErrorCode ec9 = iscc_make_error__(SCC_ER_NO_SOLUTION, NULL, "dummy4.c", 4);
	bool err_res9 = scc_get_latest_error(buffer_size, text_buffer);
	assert_true(err_res9);
	assert_int_equal(ec9, SCC_ER_NO_SOLUTION);
	assert_string_equal(text_buffer, "(scclust:dummy4.c:4) Clustering problem has no solution.");

	scc_ErrorCode ec10 = iscc_make_error__(SCC_ER_TOO_LARGE_PROBLEM, NULL, "dummy5.c", 5);
	bool err_res10 = scc_get_latest_error(buffer_size, text_buffer);
	assert_true(err_res10);
	assert_int_equal(ec10, SCC_ER_TOO_LARGE_PROBLEM);
	assert_string_equal(text_buffer, "(scclust:dummy5.c:5) Clustering problem is too large.");

	scc_ErrorCode ec11 = iscc_make_error__(SCC_ER_DIST_SEARCH_ERROR, NULL, "dummy6.c", 6);
	bool err_res11 = scc_get_latest_error(buffer_size, text_buffer);
	assert_true(err_res11);
	assert_int_equal(ec11, SCC_ER_DIST_SEARCH_ERROR);
	assert_string_equal(text_buffer, "(scclust:dummy6.c:6) Failed to calculate distances.");

	scc_ErrorCode ec12 = iscc_make_error__(SCC_ER_NOT_IMPLEMENTED, NULL, "dummy7.c", 7);
	bool err_res12 = scc_get_latest_error(buffer_size, text_buffer);
	assert_true(err_res12);
	assert_int_equal(ec12, SCC_ER_NOT_IMPLEMENTED);
	assert_string_equal(text_buffer, "(scclust:dummy7.c:7) Functionality not yet implemented.");

	scc_ErrorCode ec13 = iscc_make_error__(SCC_ER_INVALID_INPUT, "Another test message 67890.", "dummy8.c", 8);
	bool err_res13 = scc_get_latest_error(buffer_size, text_buffer);
	assert_true(err_res13);
	assert_int_equal(ec13, SCC_ER_INVALID_INPUT);
	assert_string_equal(text_buffer, "(scclust:dummy8.c:8) Another test message 67890.");
}


int main(void)
{
	scc_ut_init_tests();

	const struct CMUnitTest test_cases[] = {
		cmocka_unit_test(scc_ut_get_error_message),
	};

	return cmocka_run_group_tests_name("error.c", test_cases, NULL, NULL);
}
