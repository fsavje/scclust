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


#ifndef SCC_UT_TEST_SUITE_HG
#define SCC_UT_TEST_SUITE_HG

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <stdlib.h>
#include <assert.h>

#define malloc(size) _test_malloc(size, __FILE__, __LINE__)
#define calloc(num, size) _test_calloc(num, size, __FILE__, __LINE__)
#define realloc(ptr, size) _test_realloc(ptr, size, __FILE__, __LINE__)
#define free(ptr) _test_free(ptr, __FILE__, __LINE__)

#undef assert
#define assert(expression) mock_assert((int)(expression), #expression, __LINE__)

#ifdef SCC_DOUBLE_ASSERT
	#include <math.h>

	#define SCC_DOUBLE_EPSILON 0.000001

	#define assert_double_equal(a, b) _assert_double_equal(a, b, #a, #b, __FILE__, __LINE__)

	void _assert_double_equal(const double a,
	                          const double b,
	                          const char* const name_a,
	                          const char* const name_b,
	                          const char* const file,
	                          const int line)
	{
		if (fabs(a - b) >= SCC_DOUBLE_EPSILON) {
			print_error("%s and %s are not equal: %f != %f\n", name_a, name_b, a, b);
			_fail(file, line);
		}
	}
#endif


#endif
