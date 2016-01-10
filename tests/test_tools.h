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
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * ============================================================================== */


#ifndef SCC_UT_TEST_TOOLS_HG
#define SCC_UT_TEST_TOOLS_HG

#include "test_suite.h"

#include <stdio.h>
#include "../include/config.h"

#define scc_ut_print_Vid_array(to_print, length) _scc_ut_print_Vid_array(to_print, length, #to_print, __LINE__)

void _scc_ut_print_Vid_array(const scc_Vid* const to_print,
                             const size_t length,
                             const char* const name,
                             const int line)
{
	printf("%i:%s: ", line, name);

	if (length == 0) {
		printf("[]\n");
	} else {
		printf("[%ju", (uintmax_t)to_print[0]);
		for (size_t i = 1; i < length; ++i) printf(", %ju", (uintmax_t)to_print[i]);
		printf("]\n");
	}
}


#endif
