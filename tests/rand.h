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


#ifndef SCC_UT_RAND_HG
#define SCC_UT_RAND_HG

#include <src/cmocka_headers.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>


void scc_rand_bool_array(const size_t len_out_array,
                         bool out_array[const static len_out_array])
{
	assert(len_out_array > 0);
	assert(out_array != NULL);

	for (size_t i = 0; i < len_out_array; ++i) {
		out_array[i] = (bool) (rand() % 2);
	}
}


uint16_t scc_rand_uint(const uint16_t min,
                       const uint16_t max)
{
	return (uint16_t) ((((uint16_t) rand()) % (max - min + 1)) + min);
}


void scc_rand_uint16_t_array(const uint16_t min,
                             const uint16_t max,
                             const size_t len_out_array,
                             uint16_t out_array[const static len_out_array])
{
	assert(len_out_array > 0);
	assert(out_array != NULL);

	for (size_t i = 0; i < len_out_array; ++i) {
		out_array[i] = (uint16_t) ((((uint16_t) rand()) % (max - min + 1)) + min);
	}
}


void scc_rand_uint32_t_array(const uint32_t min,
                             const uint32_t max,
                             const size_t len_out_array,
                             uint32_t out_array[const static len_out_array])
{
	assert(len_out_array > 0);
	assert(out_array != NULL);

	for (size_t i = 0; i < len_out_array; ++i) {
		out_array[i] = (((uint32_t) rand()) % (max - min + 1)) + min;
	}
}


double scc_rand_double(const double min,
                       const double max)
{
	return (max - min) * ((double) rand()) / ((double) RAND_MAX) + min;
}


void scc_rand_double_array(const double min,
                           const double max,
                           const size_t len_out_array,
                           double out_array[const static len_out_array])
{
	assert(len_out_array > 0);
	assert(out_array != NULL);

	const double scaling = (max - min) / ((double) RAND_MAX);
	for (size_t i = 0; i < len_out_array; ++i) {
		out_array[i] = scaling * ((double) rand()) + min;
	}
}

#endif
