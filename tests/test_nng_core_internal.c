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
#include <src/nng_core.c>
#include <src/digraph_debug.h>
#include <src/scclust_types.h>
#include "assert_digraph.h"
#include "data_object_test.h"


void scc_ut_make_nng(void** state)
{
	(void) state;

	iscc_Dpid search1[10] = { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18 };

	// 3, 6, 9, 15, 19, 20, 23, 33, 88, 90
	const bool query1b[100] = { false, false, false, true, false, false, true, false, false, true, false, false, false, false, false, true, false,
	                            false, false, true, true, false, false, true, false, false, false, false, false, false, false, false, false, true,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, true, false, true, false, false, false, false, false, false, false, false, false };
	const iscc_Arci ref_nn_ref1b[101] = { 0, 0, 0, 0, 3, 3, 3, 6, 6, 6, 9, 9, 9, 9, 9, 9, 12, 12, 12, 12, 15, 18, 18, 18, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
	                                      24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	                                      24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 27, 27, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30 };
	const iscc_Dpid ref_nn_indices1b[30] = { 4, 12, 0, 6, 12, 16, 2, 4, 14, 4, 12, 2, 14, 2, 16, 14, 2, 4, 8, 10, 16, 4, 12, 6, 0, 14, 10, 8, 10, 0 };
	iscc_Digraph ref_nng1b;
	iscc_digraph_from_pieces(100, 30, ref_nn_ref1b, ref_nn_indices1b, &ref_nng1b);
	iscc_Digraph out_nng1b;
	scc_ErrorCode ec1b = iscc_make_nng(scc_ut_test_data_large, 10, search1,
                                      100, query1b, NULL,
                                      3, false, 0.0, 30, &out_nng1b);
	assert_int_equal(ec1b, SCC_ER_OK);
	assert_equal_digraph(&out_nng1b, &ref_nng1b);
	iscc_free_digraph(&out_nng1b);
	iscc_free_digraph(&ref_nng1b);


	// 43, 99
	const bool query1c[100] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true };
	const iscc_Arci ref_nn_ref1c[101] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	                                      2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	                                      2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4 };
	const iscc_Dpid ref_nn_indices1c[4] = { 0, 10, 18, 10 };
	iscc_Digraph ref_nng1c;
	iscc_digraph_from_pieces(100, 4, ref_nn_ref1c, ref_nn_indices1c, &ref_nng1c);
	iscc_Digraph out_nng1c;
	scc_ErrorCode ec1c = iscc_make_nng(scc_ut_test_data_large, 10, search1,
                                      100, query1c, NULL,
                                      2, false, 0.0, 20, &out_nng1c);
	assert_int_equal(ec1c, SCC_ER_OK);
	assert_equal_digraph(&out_nng1c, &ref_nng1c);
	iscc_free_digraph(&out_nng1c);
	iscc_free_digraph(&ref_nng1c);


	iscc_Dpid search4b[2] = { 76, 33 };
	const iscc_Arci ref_nn_ref4b[101] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33,
	                                      34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65,
	                                      66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100 };
	const iscc_Dpid ref_nn_indices4b[100] = { 76, 33, 33, 33, 33, 33, 76, 76, 76, 33, 76, 76, 76, 76, 76, 33, 76, 76, 76, 76, 76, 33, 76, 76, 76, 76, 33, 76, 76, 33, 33, 76, 33, 33, 76,
	                                          76, 76, 76, 76, 76, 76, 33, 33, 33, 76, 33, 76, 33, 33, 76, 33, 76, 33, 76, 76, 76, 33, 33, 33, 76, 33, 33, 76, 76, 76, 76, 76, 76, 33, 76,
	                                          33, 76, 33, 33, 33, 76, 76, 33, 33, 33, 76, 33, 33, 76, 33, 33, 76, 33, 33, 33, 76, 33, 33, 33, 33, 76, 33, 76, 33, 33 };
	iscc_Digraph ref_nng4b;
	iscc_digraph_from_pieces(100, 100, ref_nn_ref4b, ref_nn_indices4b, &ref_nng4b);
	iscc_Digraph out_nng4b;
	scc_ErrorCode ec4b = iscc_make_nng(scc_ut_test_data_large, 2, search4b,
                                      100, NULL, NULL,
                                      1, false, 0.0, 100, &out_nng4b);
	assert_int_equal(ec4b, SCC_ER_OK);
	assert_equal_digraph(&out_nng4b, &ref_nng4b);
	iscc_free_digraph(&out_nng4b);
	iscc_free_digraph(&ref_nng4b);


	// 15
	const bool query5a[100] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };
	const iscc_Arci ref_nn_ref5a[101] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	                                      5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 };
	const iscc_Dpid ref_nn_indices5a[5] = { 15, 50, 96, 1, 73 };
	iscc_Digraph ref_nng5a;
	iscc_digraph_from_pieces(100, 5, ref_nn_ref5a, ref_nn_indices5a, &ref_nng5a);
	iscc_Digraph out_nng5a;
	scc_ErrorCode ec5a = iscc_make_nng(scc_ut_test_data_large, 100, NULL,
                                      100, query5a, NULL,
                                      5, false, 0.0, 5, &out_nng5a);
	assert_int_equal(ec5a, SCC_ER_OK);
	assert_equal_digraph(&out_nng5a, &ref_nng5a);
	iscc_free_digraph(&out_nng5a);
	iscc_free_digraph(&ref_nng5a);


	// 15, 65
	const bool query5b[100] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false };
	const iscc_Arci ref_nn_ref5b[101] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	                                      4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 };
	const iscc_Dpid ref_nn_indices5b[8] = { 15, 50, 96, 1, 65, 8, 97, 63 };
	iscc_Digraph ref_nng5b;
	iscc_digraph_from_pieces(100, 8, ref_nn_ref5b, ref_nn_indices5b, &ref_nng5b);
	iscc_Digraph out_nng5b;
	scc_ErrorCode ec5b = iscc_make_nng(scc_ut_test_data_large, 100, NULL,
                                      100, query5b, NULL,
                                      4, false, 0.0, 50, &out_nng5b);
	assert_int_equal(ec5b, SCC_ER_OK);
	assert_equal_digraph(&out_nng5b, &ref_nng5b);
	iscc_free_digraph(&out_nng5b);
	iscc_free_digraph(&ref_nng5b);


	// 15, 65
	const bool query5c[100] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false };
	const iscc_Arci ref_nn_ref5c[101] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	                                      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6 };
	const iscc_Dpid ref_nn_indices5c[6] = { 15, 1, 42, 8, 10, 27 };
	iscc_Digraph ref_nng5c;
	iscc_digraph_from_pieces(100, 6, ref_nn_ref5c, ref_nn_indices5c, &ref_nng5c);
	iscc_Digraph out_nng5c;
	scc_ErrorCode ec5c = iscc_make_nng(scc_ut_test_data_large, 50, NULL,
                                      100, query5c, NULL,
                                      3, false, 0.0, 6, &out_nng5c);
	assert_int_equal(ec5c, SCC_ER_OK);
	assert_equal_digraph(&out_nng5c, &ref_nng5c);
	iscc_free_digraph(&out_nng5c);
	iscc_free_digraph(&ref_nng5c);


	const iscc_Arci ref_nn_ref6a[16] = { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30 };
	const iscc_Dpid ref_nn_indices6a[30] = { 0, 8, 1, 7, 2, 10, 3, 5, 4, 13, 5, 3, 6, 12, 7, 1, 8, 0, 9, 3, 10, 12, 11, 5, 12, 6, 13, 4, 14, 6 };
	iscc_Digraph ref_nng6a;
	iscc_digraph_from_pieces(15, 30, ref_nn_ref6a, ref_nn_indices6a, &ref_nng6a);
	iscc_Digraph out_nng6a;
	scc_ErrorCode ec6a = iscc_make_nng(scc_ut_test_data_small, 15, NULL,
                                      15, NULL, NULL,
                                      2, false, 0.0, 30, &out_nng6a);
	assert_int_equal(ec6a, SCC_ER_OK);
	assert_equal_digraph(&out_nng6a, &ref_nng6a);
	iscc_free_digraph(&out_nng6a);
	iscc_free_digraph(&ref_nng6a);


	const iscc_Arci ref_nn_ref6c[16] = { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30 };
	const iscc_Dpid ref_nn_indices6c[30] = { 0, 8, 1, 7, 2, 9, 3, 5, 4, 6, 5, 3, 6, 4, 7, 1, 8, 0, 9, 3, 6, 2, 5, 3, 6, 4, 4, 6, 6, 4 };
	iscc_Digraph ref_nng6c;
	iscc_digraph_from_pieces(15, 30, ref_nn_ref6c, ref_nn_indices6c, &ref_nng6c);
	iscc_Digraph out_nng6c;
	scc_ErrorCode ec6c = iscc_make_nng(scc_ut_test_data_small, 10, NULL,
                                      15, NULL, NULL,
                                      2, false, 0.0, 100, &out_nng6c);
	assert_int_equal(ec6c, SCC_ER_OK);
	assert_equal_digraph(&out_nng6c, &ref_nng6c);
	iscc_free_digraph(&out_nng6c);
	iscc_free_digraph(&ref_nng6c);


	const iscc_Arci ref_nn_ref6d[11] = { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20 };
	const iscc_Dpid ref_nn_indices6d[20] = { 0, 8, 1, 7, 2, 9, 3, 5, 4, 6, 5, 3, 6, 4, 7, 1, 8, 0, 9, 3 };
	iscc_Digraph ref_nng6d;
	iscc_digraph_from_pieces(10, 20, ref_nn_ref6d, ref_nn_indices6d, &ref_nng6d);
	iscc_Digraph out_nng6d;
	scc_ErrorCode ec6d = iscc_make_nng(scc_ut_test_data_small, 10, NULL,
                                      10, NULL, NULL,
                                      2, false, 0.0, 20, &out_nng6d);
	assert_int_equal(ec6d, SCC_ER_OK);
	assert_equal_digraph(&out_nng6d, &ref_nng6d);
	iscc_free_digraph(&out_nng6d);
	iscc_free_digraph(&ref_nng6d);
}


void scc_ut_make_nng_radius(void** state)
{
	(void) state;

	iscc_Dpid search1[10] = { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18 };

	// 3, 6, 9, 15, 19, 20, 23, 33, 88, 90
	const bool query1b[100] = { false, false, false, true, false, false, true, false, false, true, false, false, false, false, false, true, false,
	                            false, false, true, true, false, false, true, false, false, false, false, false, false, false, false, false, true,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, true, false, true, false, false, false, false, false, false, false, false, false };
	const iscc_Arci ref_nn_ref1b[101] = { 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 9, 12, 12, 12, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
	                                      15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
	                                      15, 15, 18, 18, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21 };
	const iscc_Dpid ref_nn_indices1b[21] = { 6, 12, 16, 2, 4, 14, 14, 2, 16, 14, 2, 4, 8, 10, 16, 0, 14, 10, 8, 10, 0 };
	iscc_Digraph ref_nng1b;
	iscc_digraph_from_pieces(100, 21, ref_nn_ref1b, ref_nn_indices1b, &ref_nng1b);
	iscc_Digraph out_nng1b;
	scc_ErrorCode ec1b = iscc_make_nng(scc_ut_test_data_large, 10, search1,
                                      100, query1b, NULL,
                                      3, true, 50.0, 21, &out_nng1b);
	assert_int_equal(ec1b, SCC_ER_OK);
	assert_equal_digraph(&out_nng1b, &ref_nng1b);
	iscc_free_digraph(&out_nng1b);
	iscc_free_digraph(&ref_nng1b);


	// 43, 99
	const bool query1c[100] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true };
	bool out_indicators1c[100] = { true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                               true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                               true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                               true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                               true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                               true, true, true, true, true };
	const bool ref_indicators1c[100] = { true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                                     true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                                     true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                                     true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true };
	const iscc_Arci ref_nn_ref1c[101] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	                                      2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4 };
	const iscc_Dpid ref_nn_indices1c[4] = { 0, 10, 18, 10 };
	iscc_Digraph ref_nng1c;
	iscc_digraph_from_pieces(100, 4, ref_nn_ref1c, ref_nn_indices1c, &ref_nng1c);
	iscc_Digraph out_nng1c;
	scc_ErrorCode ec1c = iscc_make_nng(scc_ut_test_data_large, 10, search1,
                                      100, query1c, out_indicators1c,
                                      2, true, 40.0, 10, &out_nng1c);
	assert_int_equal(ec1c, SCC_ER_OK);
	assert_equal_digraph(&out_nng1c, &ref_nng1c);
	assert_memory_equal(out_indicators1c, ref_indicators1c, 100 * sizeof(bool));
	iscc_free_digraph(&out_nng1c);
	iscc_free_digraph(&ref_nng1c);


	iscc_Dpid search4b[2] = { 76, 33 };
	bool out_indicators4b[100] = { true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                               true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                               true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                               true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                               true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                               true, true, true, true, true };
	const bool ref_indicators4b[100] = { false, false, false, false, false, false, true, true, false, false, false, false, false, false, false, false,
		                                  false, false, false, false, false, true, false, false, false, false, false, false, false, false, false, false,
		                                  false, true, false, false, false, false, false, false, false, false, false, false, false, false, true, false,
		                                  false, false, false, false, false, false, false, false, false, true, false, true, false, true, false, false,
		                                  false, false, false, false, false, false, true, false, false, false, false, true, true, false, false, true,
		                                  false, false, false, false, false, true, true, false, false, false, false, false, false, false, false, false,
		                                  false, false, false, false };
	const iscc_Arci ref_nn_ref4b[101] = { 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5,
	                                      5, 5, 5, 5, 5, 5, 6, 6, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 10, 11, 11, 11, 12, 12, 12, 12, 12, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 };
	const iscc_Dpid ref_nn_indices4b[14] = { 76, 76, 33, 33, 76, 33, 76, 33, 33, 76, 76, 33, 33, 76 };
	iscc_Digraph ref_nng4b;
	iscc_digraph_from_pieces(100, 14, ref_nn_ref4b, ref_nn_indices4b, &ref_nng4b);
	iscc_Digraph out_nng4b;
	scc_ErrorCode ec4b = iscc_make_nng(scc_ut_test_data_large, 2, search4b,
                                      100, NULL, out_indicators4b,
                                      1, true, 20.0, 14, &out_nng4b);
	assert_int_equal(ec4b, SCC_ER_OK);
	assert_equal_digraph(&out_nng4b, &ref_nng4b);
	assert_memory_equal(out_indicators4b, ref_indicators4b, 100 * sizeof(bool));
	iscc_free_digraph(&out_nng4b);
	iscc_free_digraph(&ref_nng4b);


	// 15
	bool query5a[100] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };
	const bool ref_indicators5a[100] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                                     false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                                     false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                                     false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                                     false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };
	iscc_Digraph ref_nng5a;
	iscc_empty_digraph(100, 0, &ref_nng5a);
	iscc_Digraph out_nng5a;
	scc_ErrorCode ec5a = iscc_make_nng(scc_ut_test_data_large, 100, NULL,
                                      100, query5a, query5a,
                                      5, true, 20.0, 5, &out_nng5a);
	assert_int_equal(ec5a, SCC_ER_OK);
	assert_equal_digraph(&out_nng5a, &ref_nng5a);
	assert_memory_equal(query5a, ref_indicators5a, 100 * sizeof(bool));
	iscc_free_digraph(&out_nng5a);
	iscc_free_digraph(&ref_nng5a);


	// 15, 65
	bool query5b[100] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, false, false, false, false, false, false, false, false,
	                      false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                      false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, false, false, false, false, false, false,
	                      false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                      false, false, false, false };
	const bool ref_indicators5b[100] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, false, false, false, false, false, false, false, false,
	                                     false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                                     false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                                     false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                                     false, false, false, false };
	const iscc_Arci ref_nn_ref5b[101] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	                                      4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };
	const iscc_Dpid ref_nn_indices5b[4] = { 15, 50, 96, 1 };
	iscc_Digraph ref_nng5b;
	iscc_digraph_from_pieces(100, 4, ref_nn_ref5b, ref_nn_indices5b, &ref_nng5b);
	iscc_Digraph out_nng5b;
	scc_ErrorCode ec5b = iscc_make_nng(scc_ut_test_data_large, 100, NULL,
                                      100, query5b, query5b,
                                      4, true, 20.5, 10, &out_nng5b);
	assert_int_equal(ec5b, SCC_ER_OK);
	assert_equal_digraph(&out_nng5b, &ref_nng5b);
	assert_memory_equal(query5b, ref_indicators5b, 100 * sizeof(bool));
	iscc_free_digraph(&out_nng5b);
	iscc_free_digraph(&ref_nng5b);


	// 15, 65
	const bool query5c[100] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false };
	const iscc_Arci ref_nn_ref5c[101] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	                                      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 };
	const iscc_Dpid ref_nn_indices5c[3] = { 15, 1, 42 };
	iscc_Digraph ref_nng5c;
	iscc_digraph_from_pieces(100, 3, ref_nn_ref5c, ref_nn_indices5c, &ref_nng5c);
	iscc_Digraph out_nng5c;
	scc_ErrorCode ec5c = iscc_make_nng(scc_ut_test_data_large, 50, NULL,
                                      100, query5c, NULL,
                                      3, true, 30.0, 8, &out_nng5c);
	assert_int_equal(ec5c, SCC_ER_OK);
	assert_equal_digraph(&out_nng5c, &ref_nng5c);
	iscc_free_digraph(&out_nng5c);
	iscc_free_digraph(&ref_nng5c);


	const bool query5d[100] = { true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                            true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                            true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                            true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                            true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                            true, true, true, true, true };
	const iscc_Arci ref_nn_ref5d[101] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	iscc_Digraph out_nng5d;
	scc_ErrorCode ec5d = iscc_make_nng(scc_ut_test_data_large, 100, NULL,
                                      100, query5d, NULL,
                                      3, true, 0.1, 10, &out_nng5d);
	assert_int_equal(ec5d, SCC_ER_OK);
	assert_int_equal(out_nng5d.vertices, 100);
	assert_int_equal(out_nng5d.max_arcs, 0);
	assert_null(out_nng5d.head);
	assert_non_null(out_nng5d.tail_ptr);
	assert_memory_equal(out_nng5d.tail_ptr, ref_nn_ref5d, 101 * sizeof(iscc_Arci));
	iscc_free_digraph(&out_nng5d);


	bool query5e[100] = { true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                      true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                      true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                      true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                      true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                      true, true, true, true, true };
	const iscc_Arci ref_nn_ref5e[101] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	const bool ref_query5e[100] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                                false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                                false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                                false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                                false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                                false, false, false, false, false };
	iscc_Digraph out_nng5e;
	scc_ErrorCode ec5e = iscc_make_nng(scc_ut_test_data_large, 100, NULL,
                                      100, query5e, query5e,
                                      3, true, 0.1, 10, &out_nng5e);
	assert_int_equal(ec5e, SCC_ER_OK);
	assert_int_equal(out_nng5e.vertices, 100);
	assert_int_equal(out_nng5e.max_arcs, 0);
	assert_null(out_nng5e.head);
	assert_non_null(out_nng5e.tail_ptr);
	assert_memory_equal(out_nng5e.tail_ptr, ref_nn_ref5e, 101 * sizeof(iscc_Arci));
	iscc_free_digraph(&out_nng5e);
	assert_memory_equal(query5e, ref_query5e, 100 * sizeof(bool));


	bool out_indicators6a[15] = { true, true, true, true, true, true, true, true, true, true, true, true, true, true, true };
	const bool ref_indicators6a[15] = { true, true, false, false, true, false, true, true, true, false, true, false, true, true, true };
	const iscc_Arci ref_nn_ref6a[16] = { 0, 2, 4, 4, 4, 6, 6, 8, 10, 12, 12, 14, 14, 16, 18, 20 };
	const iscc_Dpid ref_nn_indices6a[20] = { 0, 8, 1, 7, 4, 13, 6, 12, 7, 1, 8, 0, 10, 12, 12, 6, 13, 4, 14, 6 };
	iscc_Digraph ref_nng6a;
	iscc_digraph_from_pieces(15, 20, ref_nn_ref6a, ref_nn_indices6a, &ref_nng6a);
	iscc_Digraph out_nng6a;
	scc_ErrorCode ec6a = iscc_make_nng(scc_ut_test_data_small, 15, NULL,
                                      15, NULL, out_indicators6a,
                                      2, true, 0.2, 20, &out_nng6a);
	assert_int_equal(ec6a, SCC_ER_OK);
	assert_equal_digraph(&out_nng6a, &ref_nng6a);
	assert_memory_equal(out_indicators6a, ref_indicators6a, 15 * sizeof(bool));
	iscc_free_digraph(&out_nng6a);
	iscc_free_digraph(&ref_nng6a);


	bool out_indicators6c[15] = { true, true, true, true, true, true, true, true, true, true, true, true, true, true, true };
	const bool ref_indicators6c[15] = { true, true, false, false, false, false, false, true, true, false, false, false, false, true, true };
	const iscc_Arci ref_nn_ref6c[16] = { 0, 2, 4, 4, 4, 4, 4, 4, 6, 8, 8, 8, 8, 8, 10, 12 };
	const iscc_Dpid ref_nn_indices6c[12] = { 0, 8, 1, 7, 7, 1, 8, 0, 4, 6, 6, 4 };
	iscc_Digraph ref_nng6c;
	iscc_digraph_from_pieces(15, 12, ref_nn_ref6c, ref_nn_indices6c, &ref_nng6c);
	iscc_Digraph out_nng6c;
	scc_ErrorCode ec6c = iscc_make_nng(scc_ut_test_data_small, 10, NULL,
                                      15, NULL, out_indicators6c,
                                      2, true, 0.3, 14, &out_nng6c);
	assert_int_equal(ec6c, SCC_ER_OK);
	assert_equal_digraph(&out_nng6c, &ref_nng6c);
	assert_memory_equal(out_indicators6c, ref_indicators6c, 15 * sizeof(bool));
	iscc_free_digraph(&out_nng6c);
	iscc_free_digraph(&ref_nng6c);


	bool out_indicators6d[10] = { true, true, true, true, true, true, true, true, true, true };
	const bool ref_indicators6d[10] = { true, true, false, false, false, false, false, true, true, false };
	const iscc_Arci ref_nn_ref6d[11] = { 0, 2, 4, 4, 4, 4, 4, 4, 6, 8, 8 };
	const iscc_Dpid ref_nn_indices6d[8] = { 0, 8, 1, 7, 7, 1, 8, 0 };
	iscc_Digraph ref_nng6d;
	iscc_digraph_from_pieces(10, 8, ref_nn_ref6d, ref_nn_indices6d, &ref_nng6d);
	iscc_Digraph out_nng6d;
	scc_ErrorCode ec6d = iscc_make_nng(scc_ut_test_data_small, 10, NULL,
                                      10, NULL, out_indicators6d,
                                      2, true, 0.2, 16, &out_nng6d);
	assert_int_equal(ec6d, SCC_ER_OK);
	assert_equal_digraph(&out_nng6d, &ref_nng6d);
	assert_memory_equal(out_indicators6d, ref_indicators6d, 10 * sizeof(bool));
	iscc_free_digraph(&out_nng6d);
	iscc_free_digraph(&ref_nng6d);
}


void scc_ut_make_nng_from_search_object(void** state)
{
	(void) state;

	iscc_Dpid search1[10] = { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18 };
	iscc_NNSearchObject* nn_search_object1;
	iscc_init_nn_search_object(scc_ut_test_data_large, 10, search1, &nn_search_object1);

	// 3, 6, 9, 15, 19, 20, 23, 33, 88, 90
	const bool query1b[100] = { false, false, false, true, false, false, true, false, false, true, false, false, false, false, false, true, false,
	                            false, false, true, true, false, false, true, false, false, false, false, false, false, false, false, false, true,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, true, false, true, false, false, false, false, false, false, false, false, false };
	const iscc_Arci ref_nn_ref1b[101] = { 0, 0, 0, 0, 3, 3, 3, 6, 6, 6, 9, 9, 9, 9, 9, 9, 12, 12, 12, 12, 15, 18, 18, 18, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
	                                      24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	                                      24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 27, 27, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30 };
	const iscc_Dpid ref_nn_indices1b[30] = { 4, 12, 0, 6, 12, 16, 2, 4, 14, 4, 12, 2, 14, 2, 16, 14, 2, 4, 8, 10, 16, 4, 12, 6, 0, 14, 10, 8, 10, 0 };
	iscc_Digraph ref_nng1b;
	iscc_digraph_from_pieces(100, 30, ref_nn_ref1b, ref_nn_indices1b, &ref_nng1b);
	iscc_Digraph out_nng1b;
	scc_ErrorCode ec1b = iscc_make_nng_from_search_object(nn_search_object1,
	                                                    100, query1b, NULL,
                                                        3, false, 0.0, 30, &out_nng1b);
	assert_int_equal(ec1b, SCC_ER_OK);
	assert_equal_digraph(&out_nng1b, &ref_nng1b);
	iscc_free_digraph(&out_nng1b);
	iscc_free_digraph(&ref_nng1b);


	// 43, 99
	const bool query1c[100] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true };
	const iscc_Arci ref_nn_ref1c[101] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	                                      2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	                                      2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4 };
	const iscc_Dpid ref_nn_indices1c[4] = { 0, 10, 18, 10 };
	iscc_Digraph ref_nng1c;
	iscc_digraph_from_pieces(100, 4, ref_nn_ref1c, ref_nn_indices1c, &ref_nng1c);
	iscc_Digraph out_nng1c;
	scc_ErrorCode ec1c = iscc_make_nng_from_search_object(nn_search_object1,
	                                                    100, query1c, NULL,
                                                        2, false, 0.0, 20, &out_nng1c);
	assert_int_equal(ec1c, SCC_ER_OK);
	assert_equal_digraph(&out_nng1c, &ref_nng1c);
	iscc_free_digraph(&out_nng1c);
	iscc_free_digraph(&ref_nng1c);

	iscc_close_nn_search_object(&nn_search_object1);


	iscc_Dpid search4b[2] = { 76, 33 };
	const iscc_Arci ref_nn_ref4b[101] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33,
	                                      34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65,
	                                      66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100 };
	const iscc_Dpid ref_nn_indices4b[100] = { 76, 33, 33, 33, 33, 33, 76, 76, 76, 33, 76, 76, 76, 76, 76, 33, 76, 76, 76, 76, 76, 33, 76, 76, 76, 76, 33, 76, 76, 33, 33, 76, 33, 33, 76,
	                                          76, 76, 76, 76, 76, 76, 33, 33, 33, 76, 33, 76, 33, 33, 76, 33, 76, 33, 76, 76, 76, 33, 33, 33, 76, 33, 33, 76, 76, 76, 76, 76, 76, 33, 76,
	                                          33, 76, 33, 33, 33, 76, 76, 33, 33, 33, 76, 33, 33, 76, 33, 33, 76, 33, 33, 33, 76, 33, 33, 33, 33, 76, 33, 76, 33, 33 };
	iscc_Digraph ref_nng4b;
	iscc_digraph_from_pieces(100, 100, ref_nn_ref4b, ref_nn_indices4b, &ref_nng4b);
	iscc_Digraph out_nng4b;
	iscc_NNSearchObject* nn_search_object4b;
	iscc_init_nn_search_object(scc_ut_test_data_large, 2, search4b, &nn_search_object4b);
	scc_ErrorCode ec4b = iscc_make_nng_from_search_object(nn_search_object4b,
	                                                    100, NULL, NULL,
                                                        1, false, 0.0, 100, &out_nng4b);
	iscc_close_nn_search_object(&nn_search_object4b);
	assert_int_equal(ec4b, SCC_ER_OK);
	assert_equal_digraph(&out_nng4b, &ref_nng4b);
	iscc_free_digraph(&out_nng4b);
	iscc_free_digraph(&ref_nng4b);


	// 15
	const bool query5a[100] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };
	const iscc_Arci ref_nn_ref5a[101] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	                                      5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5 };
	const iscc_Dpid ref_nn_indices5a[5] = { 15, 50, 96, 1, 73 };
	iscc_Digraph ref_nng5a;
	iscc_digraph_from_pieces(100, 5, ref_nn_ref5a, ref_nn_indices5a, &ref_nng5a);
	iscc_Digraph out_nng5a;
	iscc_NNSearchObject* nn_search_object5a;
	iscc_init_nn_search_object(scc_ut_test_data_large, 100, NULL, &nn_search_object5a);
	scc_ErrorCode ec5a = iscc_make_nng_from_search_object(nn_search_object5a,
	                                                    100, query5a, NULL,
                                                        5, false, 0.0, 5, &out_nng5a);
	iscc_close_nn_search_object(&nn_search_object5a);
	assert_int_equal(ec5a, SCC_ER_OK);
	assert_equal_digraph(&out_nng5a, &ref_nng5a);
	iscc_free_digraph(&out_nng5a);
	iscc_free_digraph(&ref_nng5a);


	// 15, 65
	const bool query5b[100] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false };
	const iscc_Arci ref_nn_ref5b[101] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	                                      4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 };
	const iscc_Dpid ref_nn_indices5b[8] = { 15, 50, 96, 1, 65, 8, 97, 63 };
	iscc_Digraph ref_nng5b;
	iscc_digraph_from_pieces(100, 8, ref_nn_ref5b, ref_nn_indices5b, &ref_nng5b);
	iscc_Digraph out_nng5b;
	iscc_NNSearchObject* nn_search_object5b;
	iscc_init_nn_search_object(scc_ut_test_data_large, 100, NULL, &nn_search_object5b);
	scc_ErrorCode ec5b = iscc_make_nng_from_search_object(nn_search_object5b,
	                                                    100, query5b, NULL,
                                                        4, false, 0.0, 50, &out_nng5b);
	iscc_close_nn_search_object(&nn_search_object5b);
	assert_int_equal(ec5b, SCC_ER_OK);
	assert_equal_digraph(&out_nng5b, &ref_nng5b);
	iscc_free_digraph(&out_nng5b);
	iscc_free_digraph(&ref_nng5b);


	// 15, 65
	const bool query5c[100] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false };
	const iscc_Arci ref_nn_ref5c[101] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	                                      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6 };
	const iscc_Dpid ref_nn_indices5c[6] = { 15, 1, 42, 8, 10, 27 };
	iscc_Digraph ref_nng5c;
	iscc_digraph_from_pieces(100, 6, ref_nn_ref5c, ref_nn_indices5c, &ref_nng5c);
	iscc_Digraph out_nng5c;
	iscc_NNSearchObject* nn_search_object5c;
	iscc_init_nn_search_object(scc_ut_test_data_large, 50, NULL, &nn_search_object5c);
	scc_ErrorCode ec5c = iscc_make_nng_from_search_object(nn_search_object5c,
	                                                    100, query5c, NULL,
                                                        3, false, 0.0, 6, &out_nng5c);
	iscc_close_nn_search_object(&nn_search_object5c);
	assert_int_equal(ec5c, SCC_ER_OK);
	assert_equal_digraph(&out_nng5c, &ref_nng5c);
	iscc_free_digraph(&out_nng5c);
	iscc_free_digraph(&ref_nng5c);


	const iscc_Arci ref_nn_ref6a[16] = { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30 };
	const iscc_Dpid ref_nn_indices6a[30] = { 0, 8, 1, 7, 2, 10, 3, 5, 4, 13, 5, 3, 6, 12, 7, 1, 8, 0, 9, 3, 10, 12, 11, 5, 12, 6, 13, 4, 14, 6 };
	iscc_Digraph ref_nng6a;
	iscc_digraph_from_pieces(15, 30, ref_nn_ref6a, ref_nn_indices6a, &ref_nng6a);
	iscc_Digraph out_nng6a;
	iscc_NNSearchObject* nn_search_object6a;
	iscc_init_nn_search_object(scc_ut_test_data_small, 15, NULL, &nn_search_object6a);
	scc_ErrorCode ec6a = iscc_make_nng_from_search_object(nn_search_object6a,
	                                                    15, NULL, NULL,
                                                        2, false, 0.0, 30, &out_nng6a);
	iscc_close_nn_search_object(&nn_search_object6a);
	assert_int_equal(ec6a, SCC_ER_OK);
	assert_equal_digraph(&out_nng6a, &ref_nng6a);
	iscc_free_digraph(&out_nng6a);
	iscc_free_digraph(&ref_nng6a);


	const iscc_Arci ref_nn_ref6c[16] = { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30 };
	const iscc_Dpid ref_nn_indices6c[30] = { 0, 8, 1, 7, 2, 9, 3, 5, 4, 6, 5, 3, 6, 4, 7, 1, 8, 0, 9, 3, 6, 2, 5, 3, 6, 4, 4, 6, 6, 4 };
	iscc_Digraph ref_nng6c;
	iscc_digraph_from_pieces(15, 30, ref_nn_ref6c, ref_nn_indices6c, &ref_nng6c);
	iscc_Digraph out_nng6c;
	iscc_NNSearchObject* nn_search_object6c;
	iscc_init_nn_search_object(scc_ut_test_data_small, 10, NULL, &nn_search_object6c);
	scc_ErrorCode ec6c = iscc_make_nng_from_search_object(nn_search_object6c,
	                                                    15, NULL, NULL,
                                                        2, false, 0.0, 100, &out_nng6c);
	iscc_close_nn_search_object(&nn_search_object6c);
	assert_int_equal(ec6c, SCC_ER_OK);
	assert_equal_digraph(&out_nng6c, &ref_nng6c);
	iscc_free_digraph(&out_nng6c);
	iscc_free_digraph(&ref_nng6c);


	const iscc_Arci ref_nn_ref6d[11] = { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20 };
	const iscc_Dpid ref_nn_indices6d[20] = { 0, 8, 1, 7, 2, 9, 3, 5, 4, 6, 5, 3, 6, 4, 7, 1, 8, 0, 9, 3 };
	iscc_Digraph ref_nng6d;
	iscc_digraph_from_pieces(10, 20, ref_nn_ref6d, ref_nn_indices6d, &ref_nng6d);
	iscc_Digraph out_nng6d;
	iscc_NNSearchObject* nn_search_object6d;
	iscc_init_nn_search_object(scc_ut_test_data_small, 10, NULL, &nn_search_object6d);
	scc_ErrorCode ec6d = iscc_make_nng_from_search_object(nn_search_object6d,
	                                                    10, NULL, NULL,
                                                        2, false, 0.0, 20, &out_nng6d);
	iscc_close_nn_search_object(&nn_search_object6d);
	assert_int_equal(ec6d, SCC_ER_OK);
	assert_equal_digraph(&out_nng6d, &ref_nng6d);
	iscc_free_digraph(&out_nng6d);
	iscc_free_digraph(&ref_nng6d);
}


void scc_ut_make_nng_from_search_object_radius(void** state)
{
	(void) state;

	iscc_Dpid search1[10] = { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18 };
	iscc_NNSearchObject* nn_search_object1;
	iscc_init_nn_search_object(scc_ut_test_data_large, 10, search1, &nn_search_object1);

	// 3, 6, 9, 15, 19, 20, 23, 33, 88, 90
	const bool query1b[100] = { false, false, false, true, false, false, true, false, false, true, false, false, false, false, false, true, false,
	                            false, false, true, true, false, false, true, false, false, false, false, false, false, false, false, false, true,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, true, false, true, false, false, false, false, false, false, false, false, false };
	const iscc_Arci ref_nn_ref1b[101] = { 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 9, 12, 12, 12, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
	                                      15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
	                                      15, 15, 18, 18, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21 };
	const iscc_Dpid ref_nn_indices1b[21] = { 6, 12, 16, 2, 4, 14, 14, 2, 16, 14, 2, 4, 8, 10, 16, 0, 14, 10, 8, 10, 0 };
	iscc_Digraph ref_nng1b;
	iscc_digraph_from_pieces(100, 21, ref_nn_ref1b, ref_nn_indices1b, &ref_nng1b);
	iscc_Digraph out_nng1b;
	scc_ErrorCode ec1b = iscc_make_nng_from_search_object(nn_search_object1,
	                                                    100, query1b, NULL,
                                                        3, true, 50.0, 21, &out_nng1b);
	assert_int_equal(ec1b, SCC_ER_OK);
	assert_equal_digraph(&out_nng1b, &ref_nng1b);
	iscc_free_digraph(&out_nng1b);
	iscc_free_digraph(&ref_nng1b);


	// 43, 99
	const bool query1c[100] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true };
	bool out_indicators1c[100] = { true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                               true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                               true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                               true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                               true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                               true, true, true, true, true };
	const bool ref_indicators1c[100] = { true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                                     true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                                     true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                                     true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true };
	const iscc_Arci ref_nn_ref1c[101] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	                                      2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4 };
	const iscc_Dpid ref_nn_indices1c[4] = { 0, 10, 18, 10 };
	iscc_Digraph ref_nng1c;
	iscc_digraph_from_pieces(100, 4, ref_nn_ref1c, ref_nn_indices1c, &ref_nng1c);
	iscc_Digraph out_nng1c;
	scc_ErrorCode ec1c = iscc_make_nng_from_search_object(nn_search_object1,
	                                                    100, query1c, out_indicators1c,
                                                        2, true, 40.0, 10, &out_nng1c);
	assert_int_equal(ec1c, SCC_ER_OK);
	assert_equal_digraph(&out_nng1c, &ref_nng1c);
	assert_memory_equal(out_indicators1c, ref_indicators1c, 100 * sizeof(bool));
	iscc_free_digraph(&out_nng1c);
	iscc_free_digraph(&ref_nng1c);

	iscc_close_nn_search_object(&nn_search_object1);


	iscc_Dpid search4b[2] = { 76, 33 };
	bool out_indicators4b[100] = { true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                               true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                               true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                               true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                               true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                               true, true, true, true, true };
	const bool ref_indicators4b[100] = { false, false, false, false, false, false, true, true, false, false, false, false, false, false, false, false, false, false, false,
	                                     false, false, true, false, false, false, false, false, false, false, false, false, false, false, true, false, false, false, false,
	                                     false, false, false, false, false, false, false, false, true, false, false, false, false, false, false, false, false, false, false,
	                                     true, false, true, false, true, false, false, false, false, false, false, false, false, true, false, false, false, false, true, true,
	                                     false, false, true, false, false, false, false, false, true, true, false, false, false, false, false, false, false, false, false,
	                                     false, false, false, false };
	const iscc_Arci ref_nn_ref4b[101] = { 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5,
	                                      5, 5, 5, 5, 5, 5, 6, 6, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 10, 11, 11, 11, 12, 12, 12, 12, 12, 12, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14 };
	const iscc_Dpid ref_nn_indices4b[14] = { 76, 76, 33, 33, 76, 33, 76, 33, 33, 76, 76, 33, 33, 76 };
	iscc_Digraph ref_nng4b;
	iscc_digraph_from_pieces(100, 14, ref_nn_ref4b, ref_nn_indices4b, &ref_nng4b);
	iscc_Digraph out_nng4b;
	iscc_NNSearchObject* nn_search_object4b;
	iscc_init_nn_search_object(scc_ut_test_data_large, 2, search4b, &nn_search_object4b);
	scc_ErrorCode ec4b = iscc_make_nng_from_search_object(nn_search_object4b,
	                                                    100, NULL, out_indicators4b,
                                                        1, true, 20.0, 14, &out_nng4b);
	iscc_close_nn_search_object(&nn_search_object4b);
	assert_int_equal(ec4b, SCC_ER_OK);
	assert_equal_digraph(&out_nng4b, &ref_nng4b);
	assert_memory_equal(out_indicators4b, ref_indicators4b, 100 * sizeof(bool));
	iscc_free_digraph(&out_nng4b);
	iscc_free_digraph(&ref_nng4b);


	// 15
	bool query5a[100] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };
	const bool ref_indicators5a[100] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                                     false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                                     false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                                     false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                                     false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false };
	iscc_Digraph ref_nng5a;
	iscc_empty_digraph(100, 0, &ref_nng5a);
	iscc_Digraph out_nng5a;
	iscc_NNSearchObject* nn_search_object5a;
	iscc_init_nn_search_object(scc_ut_test_data_large, 100, NULL, &nn_search_object5a);
	scc_ErrorCode ec5a = iscc_make_nng_from_search_object(nn_search_object5a,
	                                                    100, query5a, query5a,
                                                        5, true, 20.0, 5, &out_nng5a);
	iscc_close_nn_search_object(&nn_search_object5a);
	assert_int_equal(ec5a, SCC_ER_OK);
	assert_equal_digraph(&out_nng5a, &ref_nng5a);
	assert_memory_equal(query5a, ref_indicators5a, 100 * sizeof(bool));
	iscc_free_digraph(&out_nng5a);
	iscc_free_digraph(&ref_nng5a);


	// 15, 65
	bool query5b[100] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, false, false, false, false, false, false, false, false,
	                      false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                      false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, false, false, false, false, false, false,
	                      false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                      false, false, false, false };
	const bool ref_indicators5b[100] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, false, false, false, false, false, false, false, false,
	                                     false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                                     false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                                     false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                                     false, false, false, false };
	const iscc_Arci ref_nn_ref5b[101] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	                                      4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };
	const iscc_Dpid ref_nn_indices5b[4] = { 15, 50, 96, 1 };
	iscc_Digraph ref_nng5b;
	iscc_digraph_from_pieces(100, 4, ref_nn_ref5b, ref_nn_indices5b, &ref_nng5b);
	iscc_Digraph out_nng5b;
	iscc_NNSearchObject* nn_search_object5b;
	iscc_init_nn_search_object(scc_ut_test_data_large, 100, NULL, &nn_search_object5b);
	scc_ErrorCode ec5b = iscc_make_nng_from_search_object(nn_search_object5b,
	                                                    100, query5b, query5b,
                                                        4, true, 20.5, 10, &out_nng5b);
	iscc_close_nn_search_object(&nn_search_object5b);
	assert_int_equal(ec5b, SCC_ER_OK);
	assert_equal_digraph(&out_nng5b, &ref_nng5b);
	assert_memory_equal(query5b, ref_indicators5b, 100 * sizeof(bool));
	iscc_free_digraph(&out_nng5b);
	iscc_free_digraph(&ref_nng5b);


	// 15, 65
	const bool query5c[100] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, false, false, false, false, false, false,
	                            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                            false, false, false, false };
	const iscc_Arci ref_nn_ref5c[101] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	                                      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 };
	const iscc_Dpid ref_nn_indices5c[3] = { 15, 1, 42 };
	iscc_Digraph ref_nng5c;
	iscc_digraph_from_pieces(100, 3, ref_nn_ref5c, ref_nn_indices5c, &ref_nng5c);
	iscc_Digraph out_nng5c;
	iscc_NNSearchObject* nn_search_object5c;
	iscc_init_nn_search_object(scc_ut_test_data_large, 50, NULL, &nn_search_object5c);
	scc_ErrorCode ec5c = iscc_make_nng_from_search_object(nn_search_object5c,
	                                                    100, query5c, NULL,
                                                        3, true, 30.0, 8, &out_nng5c);
	iscc_close_nn_search_object(&nn_search_object5c);
	assert_int_equal(ec5c, SCC_ER_OK);
	assert_equal_digraph(&out_nng5c, &ref_nng5c);
	iscc_free_digraph(&out_nng5c);
	iscc_free_digraph(&ref_nng5c);


	const bool query5d[100] = { true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                            true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                            true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                            true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                            true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                            true, true, true, true, true };
	const iscc_Arci ref_nn_ref5d[101] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	iscc_Digraph out_nng5d;
	iscc_NNSearchObject* nn_search_object5d;
	iscc_init_nn_search_object(scc_ut_test_data_large, 100, NULL, &nn_search_object5d);
	scc_ErrorCode ec5d = iscc_make_nng_from_search_object(nn_search_object5d,
	                                                    100, query5d, NULL,
                                                        3, true, 0.1, 10, &out_nng5d);
	iscc_close_nn_search_object(&nn_search_object5d);
	assert_int_equal(ec5d, SCC_ER_OK);
	assert_int_equal(out_nng5d.vertices, 100);
	assert_int_equal(out_nng5d.max_arcs, 0);
	assert_null(out_nng5d.head);
	assert_non_null(out_nng5d.tail_ptr);
	assert_memory_equal(out_nng5d.tail_ptr, ref_nn_ref5d, 101 * sizeof(iscc_Arci));
	iscc_free_digraph(&out_nng5d);


	bool query5e[100] = { true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                      true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                      true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                      true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                      true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
	                      true, true, true, true, true };
	const iscc_Arci ref_nn_ref5e[101] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	const bool ref_query5e[100] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                                false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                                false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                                false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                                false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
	                                false, false, false, false, false };
	iscc_Digraph out_nng5e;
	iscc_NNSearchObject* nn_search_object5e;
	iscc_init_nn_search_object(scc_ut_test_data_large, 100, NULL, &nn_search_object5e);
	scc_ErrorCode ec5e = iscc_make_nng_from_search_object(nn_search_object5e,
	                                                    100, query5e, query5e,
                                                        3, true, 0.1, 10, &out_nng5e);
	iscc_close_nn_search_object(&nn_search_object5e);
	assert_int_equal(ec5e, SCC_ER_OK);
	assert_int_equal(out_nng5e.vertices, 100);
	assert_int_equal(out_nng5e.max_arcs, 0);
	assert_null(out_nng5e.head);
	assert_non_null(out_nng5e.tail_ptr);
	assert_memory_equal(out_nng5e.tail_ptr, ref_nn_ref5e, 101 * sizeof(iscc_Arci));
	iscc_free_digraph(&out_nng5e);
	assert_memory_equal(query5e, ref_query5e, 100 * sizeof(bool));


	bool out_indicators6a[15] = { true, true, true, true, true, true, true, true, true, true, true, true, true, true, true };
	const bool ref_indicators6a[15] = { true, true, false, false, true, false, true, true, true, false, true, false, true, true, true };
	const iscc_Arci ref_nn_ref6a[16] = { 0, 2, 4, 4, 4, 6, 6, 8, 10, 12, 12, 14, 14, 16, 18, 20 };
	const iscc_Dpid ref_nn_indices6a[20] = { 0, 8, 1, 7, 4, 13, 6, 12, 7, 1, 8, 0, 10, 12, 12, 6, 13, 4, 14, 6 };
	iscc_Digraph ref_nng6a;
	iscc_digraph_from_pieces(15, 20, ref_nn_ref6a, ref_nn_indices6a, &ref_nng6a);
	iscc_Digraph out_nng6a;
	iscc_NNSearchObject* nn_search_object6a;
	iscc_init_nn_search_object(scc_ut_test_data_small, 15, NULL, &nn_search_object6a);
	scc_ErrorCode ec6a = iscc_make_nng_from_search_object(nn_search_object6a,
	                                                    15, NULL, out_indicators6a,
                                                        2, true, 0.2, 20, &out_nng6a);
	iscc_close_nn_search_object(&nn_search_object6a);
	assert_int_equal(ec6a, SCC_ER_OK);
	assert_equal_digraph(&out_nng6a, &ref_nng6a);
	assert_memory_equal(out_indicators6a, ref_indicators6a, 15 * sizeof(bool));
	iscc_free_digraph(&out_nng6a);
	iscc_free_digraph(&ref_nng6a);


	bool out_indicators6c[15] = { true, true, true, true, true, true, true, true, true, true, true, true, true, true, true };
	const bool ref_indicators6c[15] = { true, true, false, false, false, false, false, true, true, false, false, false, false, true, true };
	const iscc_Arci ref_nn_ref6c[16] = { 0, 2, 4, 4, 4, 4, 4, 4, 6, 8, 8, 8, 8, 8, 10, 12 };
	const iscc_Dpid ref_nn_indices6c[12] = { 0, 8, 1, 7, 7, 1, 8, 0, 4, 6, 6, 4 };
	iscc_Digraph ref_nng6c;
	iscc_digraph_from_pieces(15, 12, ref_nn_ref6c, ref_nn_indices6c, &ref_nng6c);
	iscc_Digraph out_nng6c;
	iscc_NNSearchObject* nn_search_object6c;
	iscc_init_nn_search_object(scc_ut_test_data_small, 10, NULL, &nn_search_object6c);
	scc_ErrorCode ec6c = iscc_make_nng_from_search_object(nn_search_object6c,
	                                                    15, NULL, out_indicators6c,
                                                        2, true, 0.3, 14, &out_nng6c);
	iscc_close_nn_search_object(&nn_search_object6c);
	assert_int_equal(ec6c, SCC_ER_OK);
	assert_equal_digraph(&out_nng6c, &ref_nng6c);
	assert_memory_equal(out_indicators6c, ref_indicators6c, 15 * sizeof(bool));
	iscc_free_digraph(&out_nng6c);
	iscc_free_digraph(&ref_nng6c);


	bool out_indicators6d[10] = { true, true, true, true, true, true, true, true, true, true };
	const bool ref_indicators6d[10] = { true, true, false, false, false, false, false, true, true, false };
	const iscc_Arci ref_nn_ref6d[11] = { 0, 2, 4, 4, 4, 4, 4, 4, 6, 8, 8 };
	const iscc_Dpid ref_nn_indices6d[8] = { 0, 8, 1, 7, 7, 1, 8, 0 };
	iscc_Digraph ref_nng6d;
	iscc_digraph_from_pieces(10, 8, ref_nn_ref6d, ref_nn_indices6d, &ref_nng6d);
	iscc_Digraph out_nng6d;
	iscc_NNSearchObject* nn_search_object6d;
	iscc_init_nn_search_object(scc_ut_test_data_small, 10, NULL, &nn_search_object6d);
	scc_ErrorCode ec6d = iscc_make_nng_from_search_object(nn_search_object6d,
	                                                    10, NULL, out_indicators6d,
                                                        2, true, 0.2, 16, &out_nng6d);
	iscc_close_nn_search_object(&nn_search_object6d);
	assert_int_equal(ec6d, SCC_ER_OK);
	assert_equal_digraph(&out_nng6d, &ref_nng6d);
	assert_memory_equal(out_indicators6d, ref_indicators6d, 10 * sizeof(bool));
	iscc_free_digraph(&out_nng6d);
	iscc_free_digraph(&ref_nng6d);
}


void scc_ut_ensure_self_match(void** state)
{
	(void) state;

	const iscc_Arci out_nn_ref1[11] = { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20 };
	const iscc_Dpid out_nn_indices1[20] = { 0, 1, 3, 2, 4, 1, 3, 8, 5, 2,
	                                        1, 0, 3, 6, 3, 2, 9, 7, 9, 3 };
	iscc_Digraph out_nng1;
	iscc_digraph_from_pieces(10, 20, out_nn_ref1, out_nn_indices1, &out_nng1);
	const iscc_Arci ref_nn_ref1[11] = { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20 };
	const iscc_Dpid ref_nn_indices1[20] = { 0, 1, 3, 1, 4, 2, 3, 8, 5, 4,
	                                        1, 5, 3, 6, 3, 7, 9, 8, 9, 3 };
	iscc_Digraph ref_nng1;
	iscc_digraph_from_pieces(10, 20, ref_nn_ref1, ref_nn_indices1, &ref_nng1);

	iscc_ensure_self_match(&out_nng1, 10, NULL);

	assert_equal_digraph(&out_nng1, &ref_nng1);
	iscc_free_digraph(&out_nng1);
	iscc_free_digraph(&ref_nng1);


	const iscc_Arci out_nn_ref2[11] = { 0, 2, 4, 6, 8, 10, 10, 12, 12, 14, 16 };
	const iscc_Dpid out_nn_indices2[16] = { 0, 1,  1, 6,  2, 3,  3, 1,  0, 1,
	                                        3, 5,  4, 2,  0, 9 };
	iscc_Digraph out_nng2;
	iscc_digraph_from_pieces(10, 16, out_nn_ref2, out_nn_indices2, &out_nng2);
	const iscc_Arci ref_nn_ref2[11] = { 0, 2, 4, 6, 8, 10, 10, 12, 12, 14, 16 };
	const iscc_Dpid ref_nn_indices2[16] = { 0, 1,  1, 6,  2, 3,  3, 1,  0, 4,
	                                        3, 6,  4, 8,  0, 9 };
	iscc_Digraph ref_nng2;
	iscc_digraph_from_pieces(10, 16, ref_nn_ref2, ref_nn_indices2, &ref_nng2);

	iscc_ensure_self_match(&out_nng2, 10, NULL);

	assert_equal_digraph(&out_nng2, &ref_nng2);
	iscc_free_digraph(&out_nng2);
	iscc_free_digraph(&ref_nng2);


	const iscc_Dpid search_indices3[5] = { 0, 1, 2, 3, 4 };

	const iscc_Arci out_nn_ref3[11] = { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20 };
	const iscc_Dpid out_nn_indices3[20] = { 4, 3,  2, 1,  0, 1,  2, 3,  4, 0,
	                                        4, 3,  4, 3,  2, 1,  3, 2,  4, 1 };
	iscc_Digraph out_nng3;
	iscc_digraph_from_pieces(10, 20, out_nn_ref3, out_nn_indices3, &out_nng3);
	const iscc_Arci ref_nn_ref3[11] = { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20 };
	const iscc_Dpid ref_nn_indices3[20] = { 4, 0,  2, 1,  0, 2,  2, 3,  4, 0,
	                                        4, 3,  4, 3,  2, 1,  3, 2,  4, 1 };
	iscc_Digraph ref_nng3;
	iscc_digraph_from_pieces(10, 20, ref_nn_ref3, ref_nn_indices3, &ref_nng3);

	iscc_ensure_self_match(&out_nng3, 5, search_indices3);

	assert_equal_digraph(&out_nng3, &ref_nng3);
	iscc_free_digraph(&out_nng3);
	iscc_free_digraph(&ref_nng3);


	const iscc_Dpid search_indices4[5] = { 0, 1, 2, 3, 4 };

	const iscc_Arci out_nn_ref4[11] = { 0, 2, 2, 4, 4, 6, 8, 10, 12, 12, 14 };
	const iscc_Dpid out_nn_indices4[14] = { 4, 3,  0, 1,  4, 0,  4, 3,  4, 3,  2, 1,  4, 1 };
	iscc_Digraph out_nng4;
	iscc_digraph_from_pieces(10, 14, out_nn_ref4, out_nn_indices4, &out_nng4);
	const iscc_Arci ref_nn_ref4[11] = { 0, 2, 2, 4, 4, 6, 8, 10, 12, 12, 14 };
	const iscc_Dpid ref_nn_indices4[14] = { 4, 0,  0, 2,  4, 0,  4, 3,  4, 3,  2, 1,  4, 1 };
	iscc_Digraph ref_nng4;
	iscc_digraph_from_pieces(10, 14, ref_nn_ref4, ref_nn_indices4, &ref_nng4);

	iscc_ensure_self_match(&out_nng4, 5, search_indices4);

	assert_equal_digraph(&out_nng4, &ref_nng4);
	iscc_free_digraph(&out_nng4);
	iscc_free_digraph(&ref_nng4);
}


void scc_ut_type_count(void** state)
{
	(void) state;

	const size_t num_data_points1 = 20;
	const uint32_t size_constraint1 = 8;
	const uint_fast16_t num_types1 = 4;
	const uint32_t type_constraints1[4] = { 2, 3, 1, 0 };
	const scc_TypeLabel type_labels1[20] = { 0, 2, 2, 3, 1,
	                                         2, 1, 0, 0, 1,
	                                         3, 3, 2, 1, 0,
	                                         2, 2, 3, 0, 1 };

	iscc_TypeCount tc1;
	scc_ErrorCode ec1 = iscc_type_count(num_data_points1,
	                                    size_constraint1,
	                                    num_types1,
	                                    type_constraints1,
	                                    type_labels1,
	                                    &tc1);
	assert_int_equal(ec1, SCC_ER_OK);

	const size_t ref_type_group_size1[4] = { 5, 5, 6, 4 };
	const iscc_Dpid ref_point_store1[20] = { 18, 14, 8, 7, 0,
	                                         19, 13, 9, 6, 4,
	                                         16, 15, 12, 5, 2, 1,
	                                         17, 11, 10, 3 };
	const iscc_Dpid* ref_type_groups1[4] = { tc1.point_store,
	                                         tc1.point_store + 5,
	                                         tc1.point_store + 10,
	                                         tc1.point_store + 16 };
	const iscc_Dpid ref_content1_t0[5] = { 18, 14, 8, 7, 0 };
	const iscc_Dpid ref_content1_t1[5] = { 19, 13, 9, 6, 4 };
	const iscc_Dpid ref_content1_t2[6] = { 16, 15, 12, 5, 2, 1 };
	const iscc_Dpid ref_content1_t3[4] = { 17, 11, 10, 3 };

	assert_int_equal(tc1.sum_type_constraints, 6);
	assert_memory_equal(tc1.type_group_size, ref_type_group_size1, 4 * sizeof(size_t));
	assert_memory_equal(tc1.point_store, ref_point_store1, 20 * sizeof(iscc_Dpid));
	assert_memory_equal(tc1.type_groups, ref_type_groups1, 4 * sizeof(iscc_Dpid*));
	assert_memory_equal(tc1.type_groups[0], ref_content1_t0, 5 * sizeof(iscc_Dpid));
	assert_memory_equal(tc1.type_groups[1], ref_content1_t1, 5 * sizeof(iscc_Dpid));
	assert_memory_equal(tc1.type_groups[2], ref_content1_t2, 6 * sizeof(iscc_Dpid));
	assert_memory_equal(tc1.type_groups[3], ref_content1_t3, 4 * sizeof(iscc_Dpid));

	free(tc1.type_group_size);
	free(tc1.point_store);
	free(tc1.type_groups);


	const uint32_t type_constraints2[4] = { 2, 10, 1, 0 };
	iscc_TypeCount tc2;
	scc_ErrorCode ec2 = iscc_type_count(num_data_points1,
	                                    size_constraint1,
	                                    num_types1,
	                                    type_constraints2,
	                                    type_labels1,
	                                    &tc2);
	assert_int_equal(ec2, SCC_ER_NO_SOLUTION);


	const uint32_t size_constraint3 = 4;
	iscc_TypeCount tc3;
	scc_ErrorCode ec3 = iscc_type_count(num_data_points1,
	                                    size_constraint3,
	                                    num_types1,
	                                    type_constraints1,
	                                    type_labels1,
	                                    &tc3);
	assert_int_equal(ec3, SCC_ER_INVALID_INPUT);
}


void scc_ut_assign_seeds_and_neighbors(void** state)
{
	(void) state;

	scc_Clabel external_cluster_labels[10];
	const scc_Clabel M = SCC_CLABEL_NA;

	iscc_Digraph nng1;
	iscc_digraph_from_string(".#... #..../"
	                         "..#.. ...#./"
	                         "..#.. .#.../"
	                         ".#... #..#./"
	                         "..##. ...../"

	                         "...#. .#.../"
	                         "....# ...#./"
	                         "..... ..###/"
	                         "....# ...#./"
	                         "...#. ....#/", &nng1);
	iscc_Dpid fp_seeds1[3] = {0, 4, 7};
	iscc_SeedResult sr1 = {
		.capacity = 10,
		.count = 3,
		.seeds = fp_seeds1,
	};
	scc_Clustering* cl1;
	assert_int_equal(scc_init_empty_clustering(10, external_cluster_labels, &cl1), SCC_ER_OK);
	const scc_Clabel ref_cluster_label1[10] = { 0, 0, 1, 1, 1, 0, M, 2, 2, 2 };
	size_t num_assigned1 = iscc_assign_seeds_and_neighbors(cl1, &sr1, &nng1);
	assert_int_equal(num_assigned1, 9);
	assert_int_equal(cl1->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl1->num_data_points, 10);
	assert_int_equal(cl1->num_clusters, 3);
	assert_non_null(cl1->cluster_label);
	assert_ptr_equal(cl1->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label1, 10 * sizeof(scc_Clabel));
	assert_memory_equal(cl1->cluster_label, ref_cluster_label1, 10 * sizeof(scc_Clabel));
	assert_true(cl1->external_labels);
	scc_free_clustering(&cl1);
	assert_free_digraph(&nng1);

	iscc_Digraph nng2;
	iscc_digraph_from_string(".#.#. #..../"
	                         "..#.. ...#./"
	                         "..#.. .#.../"
	                         ".#... #..#./"
	                         "..##. ...../"

	                         "...#. .#.../"
	                         "....# ...#./"
	                         "..... ..###/"
	                         "....# ...#./"
	                         "...#. ....#/", &nng2);
	iscc_Dpid fp_seeds2[3] = {0, 7};
	iscc_SeedResult sr2 = {
		.capacity = 10,
		.count = 2,
		.seeds = fp_seeds2,
	};
	scc_Clustering* cl2;
	assert_int_equal(scc_init_empty_clustering(10, external_cluster_labels, &cl2), SCC_ER_OK);
	const scc_Clabel ref_cluster_label2[10] = { 0, 0, M, 0, M, 0, M, 1, 1, 1 };
	size_t num_assigned2 = iscc_assign_seeds_and_neighbors(cl2, &sr2, &nng2);
	assert_int_equal(num_assigned2, 7);
	assert_int_equal(cl2->clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(cl2->num_data_points, 10);
	assert_int_equal(cl2->num_clusters, 2);
	assert_non_null(cl2->cluster_label);
	assert_ptr_equal(cl2->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label2, 10 * sizeof(scc_Clabel));
	assert_memory_equal(cl2->cluster_label, ref_cluster_label2, 10 * sizeof(scc_Clabel));
	assert_true(cl2->external_labels);
	scc_free_clustering(&cl2);
	assert_free_digraph(&nng2);
}


void scc_ut_assign_by_nng(void** state)
{
	(void) state;

	const scc_Clabel M = SCC_CLABEL_NA;
	bool scratch[15];

	scc_Clabel cl_labels1[15] = { 0, 1, 2, 3, 4,
	                              M, 4, M, 3, M,
	                              2, M, 1, M, 0 };
	scc_Clustering clust1 = {
		.clustering_version = ISCC_CLUSTERING_STRUCT_VERSION,
		.num_data_points = 15,
		.num_clusters = 5,
		.cluster_label = cl_labels1,
		.external_labels = true,
	};
	iscc_Digraph nng1;
	iscc_digraph_from_string(".#... ##... ...../"
	                         "..#.. ...#. ...#./"
	                         "..#.. .#... .#.../"
	                         ".#... #..#. ..#.#/"
	                         "..##. ..... ....#/"

	                         "...#. .#... ....#/"
	                         "....# ...#. ..#../"
	                         "..... ..### ...../"
	                         "....# ...#. .#.../"
	                         "...#. ....# ...#./"

	                         "...## ...#. #..#./"
	                         ".##.. .#... .#.../"
	                         "..... ..... #.##./"
	                         ".#.#. #..#. ..#../"
	                         "..#.. ###.. ..##./", &nng1);
	size_t num_assigned1 = iscc_assign_by_nng(&clust1, &nng1, scratch);
	assert_int_equal(num_assigned1, 5);
	assert_int_equal(clust1.clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(clust1.num_data_points, 15);
	assert_int_equal(clust1.num_clusters, 5);
	const scc_Clabel ref_cl_labels1[15] = { 0, 1, 2, 3, 4, 3, 4, 3, 3, 3, 2, 1, 1, 1, 0 };
	assert_memory_equal(clust1.cluster_label, ref_cl_labels1, 15 * sizeof(scc_Clabel));
	assert_true(clust1.external_labels);
	assert_free_digraph(&nng1);


	scc_Clabel cl_labels2[15] = { 0, 1, 2, 3, 4,
	                              M, 4, M, 3, M,
	                              2, M, 1, M, 0 };
	scc_Clustering clust2 = {
		.clustering_version = ISCC_CLUSTERING_STRUCT_VERSION,
		.num_data_points = 15,
		.num_clusters = 5,
		.cluster_label = cl_labels2,
		.external_labels = true,
	};
	iscc_Digraph nng2;
	iscc_digraph_from_string(".#... ##... ...../"
	                         "..#.. ...#. ...#./"
	                         "..#.. .#... .#.../"
	                         ".#... #..#. ..#.#/"
	                         "..##. ..... ....#/"

	                         "..... ##... ....#/"
	                         "....# ...#. ..#../"
	                         "..#.. #.#.. ...../"
	                         "....# ...#. .#.../"
	                         "..... #.... .#.#./"

	                         "...## ...#. #..#./"
	                         "..... #...# #..../"
	                         "..... ..... #.##./"
	                         "..... ..#.# ...#./"
	                         "..#.. ###.. ..##./", &nng2);
	size_t num_assigned2 = iscc_assign_by_nng(&clust2, &nng2, scratch);
	assert_int_equal(num_assigned2, 3);
	assert_int_equal(clust2.clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(clust2.num_data_points, 15);
	assert_int_equal(clust2.num_clusters, 5);
	const scc_Clabel ref_cl_labels2[15] = { 0, 1, 2, 3, 4, 4, 4, 2, 3, M, 2, 2, 1, M, 0 };
	assert_memory_equal(clust2.cluster_label, ref_cl_labels2, 15 * sizeof(scc_Clabel));
	assert_true(clust2.external_labels);
	assert_free_digraph(&nng2);
}


void scc_ut_assign_by_nn_search(void** state)
{
	(void) state;

	const scc_Clabel M = SCC_CLABEL_NA;

	scc_Clabel cl_labels1[15] = { 0, 1, 2, 3, 4,
	                              M, 4, M, 3, M,
	                              2, M, 1, M, 0 };
	scc_Clustering clust1 = {
		.clustering_version = ISCC_CLUSTERING_STRUCT_VERSION,
		.num_data_points = 15,
		.num_clusters = 5,
		.cluster_label = cl_labels1,
		.external_labels = true,
	};
	iscc_NNSearchObject* nn_search_object1;
	const iscc_Dpid search_indices1[5] = { 0, 1, 2, 3, 4 };
	assert_true(iscc_init_nn_search_object(&scc_ut_test_data_small_struct, 5, search_indices1, &nn_search_object1));
	bool to_assign1[15] = { false, false, false, false, false,
	                        true,  false, true,  false, true,
	                        false, false, false, false, false };
	scc_ErrorCode ec1 = iscc_assign_by_nn_search(&clust1,
	                                             nn_search_object1,
	                                             3,
	                                             to_assign1,
	                                             false,
	                                             0.0);
	assert_int_equal(ec1, SCC_ER_OK);
	assert_int_equal(clust1.clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(clust1.num_data_points, 15);
	assert_int_equal(clust1.num_clusters, 5);
	const scc_Clabel ref_cl_labels1[15] = { 0, 1, 2, 3, 4, 3, 4, 1, 3, 3, 2, M, 1, M, 0 };
	assert_memory_equal(clust1.cluster_label, ref_cl_labels1, 15 * sizeof(scc_Clabel));
	assert_true(clust1.external_labels);
	assert_true(iscc_close_nn_search_object(&nn_search_object1));


	scc_Clabel cl_labels2[15] = { 0, 1, 2, 3, 4,
	                              M, 4, M, 3, M,
	                              2, M, 1, M, 0 };
	scc_Clustering clust2 = {
		.clustering_version = ISCC_CLUSTERING_STRUCT_VERSION,
		.num_data_points = 15,
		.num_clusters = 5,
		.cluster_label = cl_labels2,
		.external_labels = true,
	};
	iscc_NNSearchObject* nn_search_object2;
	const iscc_Dpid search_indices2[5] = { 0, 1, 2, 3, 4 };
	assert_true(iscc_init_nn_search_object(&scc_ut_test_data_small_struct, 5, search_indices2, &nn_search_object2));
	bool to_assign2[15] = { false, false, false, false, false,
	                        true,  false, true,  false, true,
	                        false, false, false, false, false };
	scc_ErrorCode ec2 = iscc_assign_by_nn_search(&clust2,
	                                             nn_search_object2,
	                                             3,
	                                             to_assign2,
	                                             true,
	                                             0.3);
	assert_int_equal(ec2, SCC_ER_OK);
	assert_int_equal(clust2.clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(clust2.num_data_points, 15);
	assert_int_equal(clust2.num_clusters, 5);
	const scc_Clabel ref_cl_labels2[15] = { 0, 1, 2, 3, 4, M, 4, 1, 3, M, 2, M, 1, M, 0 };
	assert_memory_equal(clust2.cluster_label, ref_cl_labels2, 15 * sizeof(scc_Clabel));
	assert_true(clust2.external_labels);
	assert_true(iscc_close_nn_search_object(&nn_search_object2));


	scc_Clabel cl_labels3[15] = { 4, 1, 3, 2, 0,
	                              M, M, M, M, M,
	                              2, 1, 4, 3, 0 };
	scc_Clustering clust3 = {
		.clustering_version = ISCC_CLUSTERING_STRUCT_VERSION,
		.num_data_points = 15,
		.num_clusters = 5,
		.cluster_label = cl_labels3,
		.external_labels = true,
	};
	iscc_NNSearchObject* nn_search_object3;
	const iscc_Dpid search_indices3[10] = { 0, 1, 2, 3, 4, 10, 11, 12, 13, 14 };
	assert_true(iscc_init_nn_search_object(&scc_ut_test_data_small_struct, 10, search_indices3, &nn_search_object3));
	bool to_assign3[15] = { false, false, false, false, false,
	                        true,  true,  true,  true,  true,
	                        false, false, false, false, false };
	scc_ErrorCode ec3 = iscc_assign_by_nn_search(&clust3,
	                                             nn_search_object3,
	                                             5,
	                                             to_assign3,
	                                             false,
	                                             0.0);
	assert_int_equal(ec3, SCC_ER_OK);
	assert_int_equal(clust3.clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(clust3.num_data_points, 15);
	assert_int_equal(clust3.num_clusters, 5);
	const scc_Clabel ref_cl_labels3[15] = { 4, 1, 3, 2, 0, 2, 4, 1, 4, 2, 2, 1, 4, 3, 0 };
	assert_memory_equal(clust3.cluster_label, ref_cl_labels3, 15 * sizeof(scc_Clabel));
	assert_true(clust3.external_labels);
	assert_true(iscc_close_nn_search_object(&nn_search_object3));


	scc_Clabel cl_labels4[15] = { 4, 1, 3, 2, 0,
	                              M, M, M, M, M,
	                              2, 1, 4, 3, 0 };
	scc_Clustering clust4 = {
		.clustering_version = ISCC_CLUSTERING_STRUCT_VERSION,
		.num_data_points = 15,
		.num_clusters = 5,
		.cluster_label = cl_labels4,
		.external_labels = true,
	};
	iscc_NNSearchObject* nn_search_object4;
	const iscc_Dpid search_indices4[10] = { 0, 1, 2, 3, 4, 10, 11, 12, 13, 14 };
	assert_true(iscc_init_nn_search_object(&scc_ut_test_data_small_struct, 10, search_indices4, &nn_search_object4));
	bool to_assign4[15] = { false, false, false, false, false,
	                        true,  true,  true,  true,  true,
	                        false, false, false, false, false };
	scc_ErrorCode ec4 = iscc_assign_by_nn_search(&clust4,
	                                             nn_search_object4,
	                                             5,
	                                             to_assign4,
	                                             true,
	                                             0.2);
	assert_int_equal(ec4, SCC_ER_OK);
	assert_int_equal(clust4.clustering_version, ISCC_CLUSTERING_STRUCT_VERSION);
	assert_int_equal(clust4.num_data_points, 15);
	assert_int_equal(clust4.num_clusters, 5);
	const scc_Clabel ref_cl_labels4[15] = { 4, 1, 3, 2, 0, M, 4, 1, 4, M, 2, 1, 4, 3, 0 };
	assert_memory_equal(clust4.cluster_label, ref_cl_labels4, 15 * sizeof(scc_Clabel));
	assert_true(clust4.external_labels);
	assert_true(iscc_close_nn_search_object(&nn_search_object4));
}


int main(void)
{
	if(!scc_ut_init_tests()) return 1;

	const struct CMUnitTest test_cases[] = {
		cmocka_unit_test(scc_ut_make_nng),
		cmocka_unit_test(scc_ut_make_nng_radius),
		cmocka_unit_test(scc_ut_make_nng_from_search_object),
		cmocka_unit_test(scc_ut_make_nng_from_search_object_radius),
		cmocka_unit_test(scc_ut_ensure_self_match),
		cmocka_unit_test(scc_ut_type_count),
		cmocka_unit_test(scc_ut_assign_seeds_and_neighbors),
		cmocka_unit_test(scc_ut_assign_by_nng),
		cmocka_unit_test(scc_ut_assign_by_nn_search),
	};

	return cmocka_run_group_tests_name("internal nng_core.c", test_cases, NULL, NULL);
}
