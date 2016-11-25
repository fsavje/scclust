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


#ifndef SCC_UT_INIT_TEST_HG
#define SCC_UT_INIT_TEST_HG

#include <src/cmocka_headers.h>

#ifdef SCC_UT_ANN
	#include <ann_wrapper.h>

	static bool scc_ut_init_tests() {
		return scc_set_ann_dist_search();
	}

#else

	#define scc_ut_init_tests() (true)

#endif

#endif // SCC_UT_INIT_TEST_HG
