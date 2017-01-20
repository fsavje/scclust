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

#define SCC_STABLE_NNG

#include "init_test.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <src/digraph_debug.h>
#include <src/nng_core.c>
#include "assert_digraph.h"
#include "data_object_test.h"


void scc_ut_sort_nng(void** state)
{
	(void) state;

	scc_PointIndex head[7] = { 0, 1, 4, 2, 5, 3, 4 };
	iscc_ArcIndex tail_ptr[5] = { 0, 2, 6, 7, 7 };

	iscc_Digraph nng = {
		.vertices = 4,
		.max_arcs = 7,
		.head = head,
		.tail_ptr = tail_ptr,
	};

	iscc_sort_nng(&nng);

	const scc_PointIndex ref_head[7] = { 0, 1, 2, 3, 4, 5, 4 };
	const iscc_ArcIndex ref_tail_ptr[5] = { 0, 2, 6, 7, 7 };

	assert_int_equal(nng.vertices, 4);
	assert_int_equal(nng.max_arcs, 7);
	assert_non_null(nng.head);
	assert_non_null(nng.tail_ptr);
	assert_memory_equal(nng.head, ref_head, 7 * sizeof(scc_PointIndex));
	assert_memory_equal(nng.tail_ptr, ref_tail_ptr, 5 * sizeof(iscc_ArcIndex));
}


void scc_ut_get_nng_with_size_constraint_stable(void** state)
{
	(void) state;

	const scc_PointIndex primary_data_points[10] = { 0, 1, 2, 3, 4, 10, 11, 12, 13, 14 };

	iscc_Digraph out_nng1;
	scc_ErrorCode ec1 = iscc_get_nng_with_size_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 3, 0, NULL, false, 0.0, &out_nng1);
	iscc_Digraph ref_nng1;
	iscc_digraph_from_string("..... ..##. ...../"
	                         "....# ..#.. ...../"
	                         "..... ..... #.#../"
	                         "..... #...# ...../"
	                         "..... ..... ...##/"

	                         "...#. ....# ...../"
	                         "..... ..... ..#.#/"
	                         "##... ..... ...../"
	                         "#.... ..#.. ...../"
	                         "..##. ..... ...../"

	                         "..... .#... ..#../"
	                         "...#. #.... ...../"
	                         "..... .#... ....#/"
	                         "....# ..... ....#/"
	                         "..... .#... ..#../", &ref_nng1);
	assert_int_equal(ec1, SCC_ER_OK);
	assert_identical_digraph(&out_nng1, &ref_nng1);
	iscc_free_digraph(&out_nng1);
	iscc_free_digraph(&ref_nng1);


	iscc_Digraph out_nng2;
	scc_ErrorCode ec2 = iscc_get_nng_with_size_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 3, 0, NULL, true, 0.2, &out_nng2);
	iscc_Digraph ref_nng2;
	iscc_digraph_from_string("..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..... ..... ...../"
	                         "..... ..... ..#.#/"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..... .#... ..#../"
	                         "..... ..... ...../"
	                         "..... .#... ....#/"
	                         "..... ..... ...../"
	                         "..... .#... ..#../", &ref_nng2);
	assert_int_equal(ec2, SCC_ER_OK);
	assert_identical_digraph(&out_nng2, &ref_nng2);
	iscc_free_digraph(&out_nng2);
	iscc_free_digraph(&ref_nng2);


	iscc_Digraph out_nng3;
	scc_ErrorCode ec3 = iscc_get_nng_with_size_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 3, 10, primary_data_points, false, 0.0, &out_nng3);
	iscc_Digraph ref_nng3;
	iscc_digraph_from_string("..... ..##. ...../"
	                         "....# ..#.. ...../"
	                         "..... ..... #.#../"
	                         "..... #...# ...../"
	                         "..... ..... ...##/"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..... .#... ..#../"
	                         "...#. #.... ...../"
	                         "..... .#... ....#/"
	                         "....# ..... ....#/"
	                         "..... .#... ..#../", &ref_nng3);
	assert_int_equal(ec3, SCC_ER_OK);
	assert_identical_digraph(&out_nng3, &ref_nng3);
	iscc_free_digraph(&out_nng3);
	iscc_free_digraph(&ref_nng3);


	iscc_Digraph out_nng4;
	scc_ErrorCode ec4 = iscc_get_nng_with_size_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 3, 10, primary_data_points, true, 0.2, &out_nng4);
	iscc_Digraph ref_nng4;
	iscc_digraph_from_string("..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..... .#... ..#../"
	                         "..... ..... ...../"
	                         "..... .#... ....#/"
	                         "..... ..... ...../"
	                         "..... .#... ..#../", &ref_nng4);
	assert_int_equal(ec4, SCC_ER_OK);
	assert_identical_digraph(&out_nng4, &ref_nng4);
	iscc_free_digraph(&out_nng4);
	iscc_free_digraph(&ref_nng4);


	iscc_Digraph out_nng5;
	scc_ErrorCode ec5 = iscc_get_nng_with_size_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 2, 0, NULL, false, 0.0, &out_nng5);
	iscc_Digraph ref_nng5;
	iscc_digraph_from_string("..... ...#. ...../"
	                         "..... ..#.. ...../"
	                         "..... ..... #..../"
	                         "..... #.... ...../"
	                         "..... ..... ...#./"

	                         "...#. ..... ...../"
	                         "..... ..... ..#../"
	                         ".#... ..... ...../"
	                         "#.... ..... ...../"
	                         "...#. ..... ...../"

	                         "..... ..... ..#../"
	                         "..... #.... ...../"
	                         "..... .#... ...../"
	                         "....# ..... ...../"
	                         "..... .#... ...../", &ref_nng5);
	assert_int_equal(ec5, SCC_ER_OK);
	assert_identical_digraph(&out_nng5, &ref_nng5);
	iscc_free_digraph(&out_nng5);
	iscc_free_digraph(&ref_nng5);


	iscc_Digraph out_nng6;
	scc_ErrorCode ec6 = iscc_get_nng_with_size_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 2, 0, NULL, true, 0.2, &out_nng6);
	iscc_Digraph ref_nng6;
	iscc_digraph_from_string("..... ...#. ...../"
	                         "..... ..#.. ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...#./"

	                         "..... ..... ...../"
	                         "..... ..... ..#../"
	                         ".#... ..... ...../"
	                         "#.... ..... ...../"
	                         "..... ..... ...../"

	                         "..... ..... ..#../"
	                         "..... ..... ...../"
	                         "..... .#... ...../"
	                         "....# ..... ...../"
	                         "..... .#... ...../", &ref_nng6);
	assert_int_equal(ec6, SCC_ER_OK);
	assert_identical_digraph(&out_nng6, &ref_nng6);
	iscc_free_digraph(&out_nng6);
	iscc_free_digraph(&ref_nng6);


	iscc_Digraph out_nng7;
	scc_ErrorCode ec7 = iscc_get_nng_with_size_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 2, 10, primary_data_points, false, 0.0, &out_nng7);
	iscc_Digraph ref_nng7;
	iscc_digraph_from_string("..... ...#. ...../"
	                         "..... ..#.. ...../"
	                         "..... ..... #..../"
	                         "..... #.... ...../"
	                         "..... ..... ...#./"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..... ..... ..#../"
	                         "..... #.... ...../"
	                         "..... .#... ...../"
	                         "....# ..... ...../"
	                         "..... .#... ...../", &ref_nng7);
	assert_int_equal(ec7, SCC_ER_OK);
	assert_identical_digraph(&out_nng7, &ref_nng7);
	iscc_free_digraph(&out_nng7);
	iscc_free_digraph(&ref_nng7);


	iscc_Digraph out_nng8;
	scc_ErrorCode ec8 = iscc_get_nng_with_size_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 2, 10, primary_data_points, true, 0.2, &out_nng8);
	iscc_Digraph ref_nng8;
	iscc_digraph_from_string("..... ...#. ...../"
	                         "..... ..#.. ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...#./"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..... ..... ..#../"
	                         "..... ..... ...../"
	                         "..... .#... ...../"
	                         "....# ..... ...../"
	                         "..... .#... ...../", &ref_nng8);
	assert_int_equal(ec8, SCC_ER_OK);
	assert_identical_digraph(&out_nng8, &ref_nng8);
	iscc_free_digraph(&out_nng8);
	iscc_free_digraph(&ref_nng8);

	iscc_Digraph out_nng9;
	scc_ErrorCode ec9 = iscc_get_nng_with_size_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 3, 0, NULL, true, 0.01, &out_nng9);
	assert_int_equal(ec9, SCC_ER_NO_SOLUTION);
}


void scc_ut_get_nng_with_type_constraint_stable(void** state)
{
	(void) state;
	const scc_PointIndex primary_data_points[10] = { 0, 1, 2, 3, 4, 10, 11, 12, 13, 14 };

	const uint32_t type_constraints_two[2] = { 1, 1 };
	const scc_TypeLabel type_labels_two[15] = { 0, 1, 0, 0, 1,
	                                            0, 0, 1, 1, 1,
	                                            1, 0, 0, 0, 1 };

	const uint32_t type_constraints_three[3] = { 2, 0, 1 };
	const scc_TypeLabel type_labels_three[15] = { 0, 1, 2, 2, 2,
	                                              0, 0, 1, 1, 1,
	                                              2, 2, 0, 0, 1 };

	iscc_Digraph out_nng1;
	scc_ErrorCode ec1 = iscc_get_nng_with_type_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 2,
	                                                      2, type_constraints_two, type_labels_two,
	                                                      0, NULL, false, 0.0, &out_nng1);
	iscc_Digraph ref_nng1;
	iscc_digraph_from_string("..... ...#. ...../"
	                         "..... ..... ...#./"
	                         "..... ..... #..../"
	                         "..... ....# ...../"
	                         "..... ..... ...#./"

	                         "..... ....# ...../"
	                         "..... ..... ....#/"
	                         "#.... ..... ...../"
	                         "#.... ..... ...../"
	                         "...#. ..... ...../"

	                         "..... ..... ..#../"
	                         "..... ....# ...../"
	                         "..... ..... ....#/"
	                         "....# ..... ...../"
	                         "..... .#... ...../", &ref_nng1);
	assert_int_equal(ec1, SCC_ER_OK);
	assert_identical_digraph(&out_nng1, &ref_nng1);
	iscc_free_digraph(&out_nng1);
	iscc_free_digraph(&ref_nng1);


	iscc_Digraph out_nng2;
	scc_ErrorCode ec2 = iscc_get_nng_with_type_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 2,
	                                                      2, type_constraints_two, type_labels_two,
	                                                      0, NULL, true, 0.3, &out_nng2);
	iscc_Digraph ref_nng2;
	iscc_digraph_from_string("..... ...#. ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...#./"

	                         "..... ..... ...../"
	                         "..... ..... ....#/"
	                         "..... ..... ...../"
	                         "#.... ..... ...../"
	                         "..... ..... ...../"

	                         "..... ..... ..#../"
	                         "..... ..... ...../"
	                         "..... ..... ....#/"
	                         "....# ..... ...../"
	                         "..... .#... ...../", &ref_nng2);
	assert_int_equal(ec2, SCC_ER_OK);
	assert_identical_digraph(&out_nng2, &ref_nng2);
	iscc_free_digraph(&out_nng2);
	iscc_free_digraph(&ref_nng2);


	iscc_Digraph out_nng3;
	scc_ErrorCode ec3 = iscc_get_nng_with_type_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 2,
	                                                      2, type_constraints_two, type_labels_two,
	                                                      10, primary_data_points, false, 0.0, &out_nng3);
	iscc_Digraph ref_nng3;
	iscc_digraph_from_string("..... ...#. ...../"
	                         "..... ..... ...#./"
	                         "..... ..... #..../"
	                         "..... ....# ...../"
	                         "..... ..... ...#./"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..... ..... ..#../"
	                         "..... ....# ...../"
	                         "..... ..... ....#/"
	                         "....# ..... ...../"
	                         "..... .#... ...../", &ref_nng3);
	assert_int_equal(ec3, SCC_ER_OK);
	assert_identical_digraph(&out_nng3, &ref_nng3);
	iscc_free_digraph(&out_nng3);
	iscc_free_digraph(&ref_nng3);


	iscc_Digraph out_nng4;
	scc_ErrorCode ec4 = iscc_get_nng_with_type_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 2,
	                                                      2, type_constraints_two, type_labels_two,
	                                                      10, primary_data_points, true, 0.3, &out_nng4);
	iscc_Digraph ref_nng4;
	iscc_digraph_from_string("..... ...#. ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...#./"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..... ..... ..#../"
	                         "..... ..... ...../"
	                         "..... ..... ....#/"
	                         "....# ..... ...../"
	                         "..... .#... ...../", &ref_nng4);
	assert_int_equal(ec4, SCC_ER_OK);
	assert_identical_digraph(&out_nng4, &ref_nng4);
	iscc_free_digraph(&out_nng4);
	iscc_free_digraph(&ref_nng4);


	iscc_Digraph out_nng5;
	scc_ErrorCode ec5 = iscc_get_nng_with_type_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 3,
	                                                      2, type_constraints_two, type_labels_two,
	                                                      0, NULL, false, 0.0, &out_nng5);
	iscc_Digraph ref_nng5;
	iscc_digraph_from_string("..... ..##. ...../"
	                         "..... ..#.. ...#./"
	                         "..... ..... #.#../"
	                         "..... #...# ...../"
	                         "..... ..... ...##/"

	                         "...#. ....# ...../"
	                         "..... ..... ..#.#/"
	                         "##... ..... ...../"
	                         "#.... ..#.. ...../"
	                         "..##. ..... ...../"

	                         "..... .#... ..#../"
	                         "..... #...# ...../"
	                         "..... .#... ....#/"
	                         "....# ..... ....#/"
	                         "..... .#... ..#../", &ref_nng5);
	assert_int_equal(ec5, SCC_ER_OK);
	assert_identical_digraph(&out_nng5, &ref_nng5);
	iscc_free_digraph(&out_nng5);
	iscc_free_digraph(&ref_nng5);


	iscc_Digraph out_nng6;
	scc_ErrorCode ec6 = iscc_get_nng_with_type_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 3,
	                                                      2, type_constraints_two, type_labels_two,
	                                                      0, NULL, true, 0.3, &out_nng6);
	iscc_Digraph ref_nng6;
	iscc_digraph_from_string("..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...##/"

	                         "..... ..... ...../"
	                         "..... ..... ..#.#/"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..... .#... ..#../"
	                         "..... ..... ...../"
	                         "..... .#... ....#/"
	                         "....# ..... ....#/"
	                         "..... .#... ..#../", &ref_nng6);
	assert_int_equal(ec6, SCC_ER_OK);
	assert_identical_digraph(&out_nng6, &ref_nng6);
	iscc_free_digraph(&out_nng6);
	iscc_free_digraph(&ref_nng6);


	iscc_Digraph out_nng7;
	scc_ErrorCode ec7 = iscc_get_nng_with_type_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 3,
	                                                      2, type_constraints_two, type_labels_two,
	                                                      10, primary_data_points, false, 0.0, &out_nng7);
	iscc_Digraph ref_nng7;
	iscc_digraph_from_string("..... ..##. ...../"
	                         "..... ..#.. ...#./"
	                         "..... ..... #.#../"
	                         "..... #...# ...../"
	                         "..... ..... ...##/"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..... .#... ..#../"
	                         "..... #...# ...../"
	                         "..... .#... ....#/"
	                         "....# ..... ....#/"
	                         "..... .#... ..#../", &ref_nng7);
	assert_int_equal(ec7, SCC_ER_OK);
	assert_identical_digraph(&out_nng7, &ref_nng7);
	iscc_free_digraph(&out_nng7);
	iscc_free_digraph(&ref_nng7);


	iscc_Digraph out_nng8;
	scc_ErrorCode ec8 = iscc_get_nng_with_type_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 3,
	                                                      2, type_constraints_two, type_labels_two,
	                                                      10, primary_data_points, true, 0.3, &out_nng8);
	iscc_Digraph ref_nng8;
	iscc_digraph_from_string("..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...##/"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..... .#... ..#../"
	                         "..... ..... ...../"
	                         "..... .#... ....#/"
	                         "....# ..... ....#/"
	                         "..... .#... ..#../", &ref_nng8);
	assert_int_equal(ec8, SCC_ER_OK);
	assert_identical_digraph(&out_nng8, &ref_nng8);
	iscc_free_digraph(&out_nng8);
	iscc_free_digraph(&ref_nng8);


	iscc_Digraph out_nng9;
	scc_ErrorCode ec9 = iscc_get_nng_with_type_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 3,
	                                                      3, type_constraints_three, type_labels_three,
	                                                      0, NULL, false, 0.0, &out_nng9);
	iscc_Digraph ref_nng9;
	iscc_digraph_from_string("....# ..... ...#./"
	                         "#...# ..... ...#./"
	                         "..... .#... ..#../"
	                         "..... #.... ..#../"
	                         "..... .#... ...#./"

	                         "...#. ..... ..#../"
	                         "..... ..... #.#../"
	                         "#...# ..... ...#./"
	                         "#...# ..... ...#./"
	                         "...#. #.... ..#../"

	                         "..... .#... ..#../"
	                         "..... #.... ..#../"
	                         "..... .#... #..../"
	                         "....# .#... ...../"
	                         "..... .#... #.#../", &ref_nng9);
	assert_int_equal(ec9, SCC_ER_OK);
	assert_identical_digraph(&out_nng9, &ref_nng9);
	iscc_free_digraph(&out_nng9);
	iscc_free_digraph(&ref_nng9);


	iscc_Digraph out_nng10;
	scc_ErrorCode ec10 = iscc_get_nng_with_type_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 3,
	                                                      3, type_constraints_three, type_labels_three,
	                                                      0, NULL, true, 0.5, &out_nng10);
	iscc_Digraph ref_nng10;
	iscc_digraph_from_string("..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... .#... ..#../"
	                         "..... ..... ...../"
	                         "..... .#... ...#./"

	                         "..... ..... ...../"
	                         "..... ..... #.#../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..... .#... ..#../"
	                         "..... ..... ...../"
	                         "..... .#... #..../"
	                         "....# .#... ...../"
	                         "..... .#... #.#../", &ref_nng10);
	assert_int_equal(ec10, SCC_ER_OK);
	assert_identical_digraph(&out_nng10, &ref_nng10);
	iscc_free_digraph(&out_nng10);
	iscc_free_digraph(&ref_nng10);


	iscc_Digraph out_nng11;
	scc_ErrorCode ec11 = iscc_get_nng_with_type_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 3,
	                                                      3, type_constraints_three, type_labels_three,
	                                                      10, primary_data_points, false, 0.0, &out_nng11);
	iscc_Digraph ref_nng11;
	iscc_digraph_from_string("....# ..... ...#./"
	                         "#...# ..... ...#./"
	                         "..... .#... ..#../"
	                         "..... #.... ..#../"
	                         "..... .#... ...#./"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..... .#... ..#../"
	                         "..... #.... ..#../"
	                         "..... .#... #..../"
	                         "....# .#... ...../"
	                         "..... .#... #.#../", &ref_nng11);
	assert_int_equal(ec11, SCC_ER_OK);
	assert_identical_digraph(&out_nng11, &ref_nng11);
	iscc_free_digraph(&out_nng11);
	iscc_free_digraph(&ref_nng11);


	iscc_Digraph out_nng12;
	scc_ErrorCode ec12 = iscc_get_nng_with_type_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 3,
	                                                      3, type_constraints_three, type_labels_three,
	                                                      10, primary_data_points, true, 0.5, &out_nng12);
	iscc_Digraph ref_nng12;
	iscc_digraph_from_string("..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... .#... ..#../"
	                         "..... ..... ...../"
	                         "..... .#... ...#./"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..... .#... ..#../"
	                         "..... ..... ...../"
	                         "..... .#... #..../"
	                         "....# .#... ...../"
	                         "..... .#... #.#../", &ref_nng12);
	assert_int_equal(ec12, SCC_ER_OK);
	assert_identical_digraph(&out_nng12, &ref_nng12);
	iscc_free_digraph(&out_nng12);
	iscc_free_digraph(&ref_nng12);


	iscc_Digraph out_nng13;
	scc_ErrorCode ec13 = iscc_get_nng_with_type_constraint(&scc_ut_test_data_small_struct,
	                                                       15, 4,
	                                                       3, type_constraints_three, type_labels_three,
	                                                       0, NULL, false, 0.0, &out_nng13);
	iscc_Digraph ref_nng13;
	iscc_digraph_from_string("....# ...#. ...#./"
	                         "#...# ..... ...#./"
	                         "..... .#... #.#../"
	                         "..... #...# ..#../"
	                         "..... .#... ...##/"

	                         "...#. ....# ..#../"
	                         "..... ..... #.#.#/"
	                         "#...# ..... ...#./"
	                         "#...# ..... ...#./"
	                         "...#. #.... ..#../"

	                         "..... .#... ..#.#/"
	                         "...#. #.... ..#../"
	                         "..... .#... #...#/"
	                         "....# .#... ....#/"
	                         "..... .#... #.#../", &ref_nng13);
	assert_int_equal(ec13, SCC_ER_OK);
	assert_identical_digraph(&out_nng13, &ref_nng13);
	iscc_free_digraph(&out_nng13);
	iscc_free_digraph(&ref_nng13);


	iscc_Digraph out_nng14;
	scc_ErrorCode ec14 = iscc_get_nng_with_type_constraint(&scc_ut_test_data_small_struct,
	                                                       15, 4,
	                                                       3, type_constraints_three, type_labels_three,
	                                                       0, NULL, true, 0.5, &out_nng14);
	iscc_Digraph ref_nng14;
	iscc_digraph_from_string("..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... .#... #.#../"
	                         "..... ..... ...../"
	                         "..... .#... ...##/"

	                         "..... ..... ...../"
	                         "..... ..... #.#.#/"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..... .#... ..#.#/"
	                         "..... ..... ...../"
	                         "..... .#... #...#/"
	                         "....# .#... ....#/"
	                         "..... .#... #.#../", &ref_nng14);
	assert_int_equal(ec14, SCC_ER_OK);
	assert_identical_digraph(&out_nng14, &ref_nng14);
	iscc_free_digraph(&out_nng14);
	iscc_free_digraph(&ref_nng14);


	iscc_Digraph out_nng15;
	scc_ErrorCode ec15 = iscc_get_nng_with_type_constraint(&scc_ut_test_data_small_struct,
	                                                       15, 4,
	                                                       3, type_constraints_three, type_labels_three,
	                                                       10, primary_data_points, false, 0.0, &out_nng15);
	iscc_Digraph ref_nng15;
	iscc_digraph_from_string("....# ...#. ...#./"
	                         "#...# ..... ...#./"
	                         "..... .#... #.#../"
	                         "..... #...# ..#../"
	                         "..... .#... ...##/"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..... .#... ..#.#/"
	                         "...#. #.... ..#../"
	                         "..... .#... #...#/"
	                         "....# .#... ....#/"
	                         "..... .#... #.#../", &ref_nng15);
	assert_int_equal(ec15, SCC_ER_OK);
	assert_identical_digraph(&out_nng15, &ref_nng15);
	iscc_free_digraph(&out_nng15);
	iscc_free_digraph(&ref_nng15);


	iscc_Digraph out_nng16;
	scc_ErrorCode ec16 = iscc_get_nng_with_type_constraint(&scc_ut_test_data_small_struct,
	                                                       15, 4,
	                                                       3, type_constraints_three, type_labels_three,
	                                                       10, primary_data_points, true, 0.5, &out_nng16);
	iscc_Digraph ref_nng16;
	iscc_digraph_from_string("..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... .#... #.#../"
	                         "..... ..... ...../"
	                         "..... .#... ...##/"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..... .#... ..#.#/"
	                         "..... ..... ...../"
	                         "..... .#... #...#/"
	                         "....# .#... ....#/"
	                         "..... .#... #.#../", &ref_nng16);
	assert_int_equal(ec16, SCC_ER_OK);
	assert_identical_digraph(&out_nng16, &ref_nng16);
	iscc_free_digraph(&out_nng16);
	iscc_free_digraph(&ref_nng16);


	iscc_Digraph out_nng17;
	scc_ErrorCode ec17 = iscc_get_nng_with_type_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 2,
	                                                      2, type_constraints_two, type_labels_two,
	                                                      0, NULL, true, 1.0, &out_nng17);
	assert_int_equal(ec17, SCC_ER_OK);
	iscc_free_digraph(&out_nng17);

	iscc_Digraph out_nng18;
	scc_ErrorCode ec18 = iscc_get_nng_with_type_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 2,
	                                                      2, type_constraints_two, type_labels_two,
	                                                      0, NULL, true, 0.01, &out_nng18);
	assert_int_equal(ec18, SCC_ER_NO_SOLUTION);


	const uint32_t type_constraints_two_mod[2] = { 1, 0 };
	const scc_PointIndex primary_data_points_mod[7] = { 1, 4, 7, 8, 9, 10, 14 };

	iscc_Digraph out_nng19;
	scc_ErrorCode ec19 = iscc_get_nng_with_type_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 2,
	                                                      2, type_constraints_two_mod, type_labels_two,
	                                                      7, primary_data_points_mod, true, 0.04, &out_nng19);
	assert_int_equal(ec19, SCC_ER_OK);
	iscc_free_digraph(&out_nng19);

	iscc_Digraph out_nng20;
	scc_ErrorCode ec20 = iscc_get_nng_with_type_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 2,
	                                                      2, type_constraints_two_mod, type_labels_two,
	                                                      7, primary_data_points_mod, true, 0.03, &out_nng20);
	assert_int_equal(ec20, SCC_ER_NO_SOLUTION);

	iscc_Digraph out_nng21;
	scc_ErrorCode ec21 = iscc_get_nng_with_type_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 3,
	                                                      2, type_constraints_two_mod, type_labels_two,
	                                                      7, primary_data_points_mod, true, 0.04, &out_nng21);
	assert_int_equal(ec21, SCC_ER_NO_SOLUTION);

	iscc_Digraph out_nng22;
	scc_ErrorCode ec22 = iscc_get_nng_with_type_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 3,
	                                                      2, type_constraints_two_mod, type_labels_two,
	                                                      7, primary_data_points_mod, true, 0.06, &out_nng22);
	assert_int_equal(ec22, SCC_ER_OK);
	iscc_free_digraph(&out_nng22);
}


int main(void)
{
	if(!scc_ut_init_tests()) return 1;

	const struct CMUnitTest test_cases[] = {
		cmocka_unit_test(scc_ut_sort_nng),
		cmocka_unit_test(scc_ut_get_nng_with_size_constraint_stable),
		cmocka_unit_test(scc_ut_get_nng_with_type_constraint_stable),
	};

	return cmocka_run_group_tests_name("nng_core.c stable", test_cases, NULL, NULL);
}
