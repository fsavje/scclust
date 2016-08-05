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

#define SCC_DOUBLE_ASSERT
#include "test_suite.h"
#include "assert_digraph.h"
#include "../src/nng_core.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "data_object_test.h"
#include "../src/digraph_debug.h"


void scc_ut_get_nng_with_size_constraint(void** state)
{
	(void) state;

	const bool main_data_points[15] = { true,  true,  true,  true,  true,
	                                    false, false, false, false, false,
	                                    true,  true,  true,  true,  true };


	iscc_Digraph out_nng1;
	scc_ErrorCode ec1 = iscc_get_nng_with_size_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 3, NULL, false, 0.0, &out_nng1);
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
	assert_equal_digraph(&out_nng1, &ref_nng1);
	iscc_free_digraph(&out_nng1);
	iscc_free_digraph(&ref_nng1);


	iscc_Digraph out_nng2;
	scc_ErrorCode ec2 = iscc_get_nng_with_size_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 3, NULL, true, 0.2, &out_nng2);
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
	assert_equal_digraph(&out_nng2, &ref_nng2);
	iscc_free_digraph(&out_nng2);
	iscc_free_digraph(&ref_nng2);


	iscc_Digraph out_nng3;
	scc_ErrorCode ec3 = iscc_get_nng_with_size_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 3, main_data_points, false, 0.0, &out_nng3);
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
	assert_equal_digraph(&out_nng3, &ref_nng3);
	iscc_free_digraph(&out_nng3);
	iscc_free_digraph(&ref_nng3);


	iscc_Digraph out_nng4;
	scc_ErrorCode ec4 = iscc_get_nng_with_size_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 3, main_data_points, true, 0.2, &out_nng4);
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
	assert_equal_digraph(&out_nng4, &ref_nng4);
	iscc_free_digraph(&out_nng4);
	iscc_free_digraph(&ref_nng4);


	iscc_Digraph out_nng5;
	scc_ErrorCode ec5 = iscc_get_nng_with_size_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 2, NULL, false, 0.0, &out_nng5);
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
	assert_equal_digraph(&out_nng5, &ref_nng5);
	iscc_free_digraph(&out_nng5);
	iscc_free_digraph(&ref_nng5);


	iscc_Digraph out_nng6;
	scc_ErrorCode ec6 = iscc_get_nng_with_size_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 2, NULL, true, 0.2, &out_nng6);
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
	assert_equal_digraph(&out_nng6, &ref_nng6);
	iscc_free_digraph(&out_nng6);
	iscc_free_digraph(&ref_nng6);


	iscc_Digraph out_nng7;
	scc_ErrorCode ec7 = iscc_get_nng_with_size_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 2, main_data_points, false, 0.0, &out_nng7);
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
	assert_equal_digraph(&out_nng7, &ref_nng7);
	iscc_free_digraph(&out_nng7);
	iscc_free_digraph(&ref_nng7);


	iscc_Digraph out_nng8;
	scc_ErrorCode ec8 = iscc_get_nng_with_size_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 2, main_data_points, true, 0.2, &out_nng8);
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
	assert_equal_digraph(&out_nng8, &ref_nng8);
	iscc_free_digraph(&out_nng8);
	iscc_free_digraph(&ref_nng8);
}


void scc_ut_get_nng_with_type_constraint(void** state)
{
	(void) state;
	const bool main_data_points[15] = { true,  true,  true,  true,  true,
	                                    false, false, false, false, false,
	                                    true,  true,  true,  true,  true };

	const uint32_t type_size_constraints_two[2] = { 1, 1 };
	const scc_TypeLabel type_labels_two[15] = { 0, 1, 0, 0, 1,
	                                            0, 0, 1, 1, 1,
	                                            1, 0, 0, 0, 1 };
	
	const uint32_t type_size_constraints_three[3] = { 2, 0, 1 };
	const scc_TypeLabel type_labels_three[15] = { 0, 1, 2, 2, 2,
	                                              0, 0, 1, 1, 1,
	                                              2, 2, 0, 0, 1 };

	iscc_Digraph out_nng1;
	scc_ErrorCode ec1 = iscc_get_nng_with_type_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 2,
	                                                      2, type_size_constraints_two, type_labels_two,
	                                                      NULL, false, 0.0, &out_nng1);
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
	assert_equal_digraph(&out_nng1, &ref_nng1);
	iscc_free_digraph(&out_nng1);
	iscc_free_digraph(&ref_nng1);


	iscc_Digraph out_nng2;
	scc_ErrorCode ec2 = iscc_get_nng_with_type_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 2,
	                                                      2, type_size_constraints_two, type_labels_two,
	                                                      NULL, true, 0.3, &out_nng2);
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
	assert_equal_digraph(&out_nng2, &ref_nng2);
	iscc_free_digraph(&out_nng2);
	iscc_free_digraph(&ref_nng2);


	iscc_Digraph out_nng3;
	scc_ErrorCode ec3 = iscc_get_nng_with_type_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 2,
	                                                      2, type_size_constraints_two, type_labels_two,
	                                                      main_data_points, false, 0.0, &out_nng3);
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
	assert_equal_digraph(&out_nng3, &ref_nng3);
	iscc_free_digraph(&out_nng3);
	iscc_free_digraph(&ref_nng3);


	iscc_Digraph out_nng4;
	scc_ErrorCode ec4 = iscc_get_nng_with_type_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 2,
	                                                      2, type_size_constraints_two, type_labels_two,
	                                                      main_data_points, true, 0.3, &out_nng4);
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
	assert_equal_digraph(&out_nng4, &ref_nng4);
	iscc_free_digraph(&out_nng4);
	iscc_free_digraph(&ref_nng4);


	iscc_Digraph out_nng5;
	scc_ErrorCode ec5 = iscc_get_nng_with_type_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 3,
	                                                      2, type_size_constraints_two, type_labels_two,
	                                                      NULL, false, 0.0, &out_nng5);
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
	assert_equal_digraph(&out_nng5, &ref_nng5);
	iscc_free_digraph(&out_nng5);
	iscc_free_digraph(&ref_nng5);


	iscc_Digraph out_nng6;
	scc_ErrorCode ec6 = iscc_get_nng_with_type_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 3,
	                                                      2, type_size_constraints_two, type_labels_two,
	                                                      NULL, true, 0.3, &out_nng6);
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
	assert_equal_digraph(&out_nng6, &ref_nng6);
	iscc_free_digraph(&out_nng6);
	iscc_free_digraph(&ref_nng6);


	iscc_Digraph out_nng7;
	scc_ErrorCode ec7 = iscc_get_nng_with_type_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 3,
	                                                      2, type_size_constraints_two, type_labels_two,
	                                                      main_data_points, false, 0.0, &out_nng7);
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
	assert_equal_digraph(&out_nng7, &ref_nng7);
	iscc_free_digraph(&out_nng7);
	iscc_free_digraph(&ref_nng7);


	iscc_Digraph out_nng8;
	scc_ErrorCode ec8 = iscc_get_nng_with_type_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 3,
	                                                      2, type_size_constraints_two, type_labels_two,
	                                                      main_data_points, true, 0.3, &out_nng8);
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
	assert_equal_digraph(&out_nng8, &ref_nng8);
	iscc_free_digraph(&out_nng8);
	iscc_free_digraph(&ref_nng8);


	iscc_Digraph out_nng9;
	scc_ErrorCode ec9 = iscc_get_nng_with_type_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 3,
	                                                      3, type_size_constraints_three, type_labels_three,
	                                                      NULL, false, 0.0, &out_nng9);
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
	assert_equal_digraph(&out_nng9, &ref_nng9);
	iscc_free_digraph(&out_nng9);
	iscc_free_digraph(&ref_nng9);


	iscc_Digraph out_nng10;
	scc_ErrorCode ec10 = iscc_get_nng_with_type_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 3,
	                                                      3, type_size_constraints_three, type_labels_three,
	                                                      NULL, true, 0.5, &out_nng10);
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
	assert_equal_digraph(&out_nng10, &ref_nng10);
	iscc_free_digraph(&out_nng10);
	iscc_free_digraph(&ref_nng10);


	iscc_Digraph out_nng11;
	scc_ErrorCode ec11 = iscc_get_nng_with_type_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 3,
	                                                      3, type_size_constraints_three, type_labels_three,
	                                                      main_data_points, false, 0.0, &out_nng11);
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
	assert_equal_digraph(&out_nng11, &ref_nng11);
	iscc_free_digraph(&out_nng11);
	iscc_free_digraph(&ref_nng11);


	iscc_Digraph out_nng12;
	scc_ErrorCode ec12 = iscc_get_nng_with_type_constraint(&scc_ut_test_data_small_struct,
	                                                      15, 3,
	                                                      3, type_size_constraints_three, type_labels_three,
	                                                      main_data_points, true, 0.5, &out_nng12);
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
	assert_equal_digraph(&out_nng12, &ref_nng12);
	iscc_free_digraph(&out_nng12);
	iscc_free_digraph(&ref_nng12);


	iscc_Digraph out_nng13;
	scc_ErrorCode ec13 = iscc_get_nng_with_type_constraint(&scc_ut_test_data_small_struct,
	                                                       15, 4,
	                                                       3, type_size_constraints_three, type_labels_three,
	                                                       NULL, false, 0.0, &out_nng13);
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
	assert_equal_digraph(&out_nng13, &ref_nng13);
	iscc_free_digraph(&out_nng13);
	iscc_free_digraph(&ref_nng13);


	iscc_Digraph out_nng14;
	scc_ErrorCode ec14 = iscc_get_nng_with_type_constraint(&scc_ut_test_data_small_struct,
	                                                       15, 4,
	                                                       3, type_size_constraints_three, type_labels_three,
	                                                       NULL, true, 0.5, &out_nng14);
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
	assert_equal_digraph(&out_nng14, &ref_nng14);
	iscc_free_digraph(&out_nng14);
	iscc_free_digraph(&ref_nng14);


	iscc_Digraph out_nng15;
	scc_ErrorCode ec15 = iscc_get_nng_with_type_constraint(&scc_ut_test_data_small_struct,
	                                                       15, 4,
	                                                       3, type_size_constraints_three, type_labels_three,
	                                                       main_data_points, false, 0.0, &out_nng15);
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
	assert_equal_digraph(&out_nng15, &ref_nng15);
	iscc_free_digraph(&out_nng15);
	iscc_free_digraph(&ref_nng15);


	iscc_Digraph out_nng16;
	scc_ErrorCode ec16 = iscc_get_nng_with_type_constraint(&scc_ut_test_data_small_struct,
	                                                       15, 4,
	                                                       3, type_size_constraints_three, type_labels_three,
	                                                       main_data_points, true, 0.5, &out_nng16);
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
	assert_equal_digraph(&out_nng16, &ref_nng16);
	iscc_free_digraph(&out_nng16);
	iscc_free_digraph(&ref_nng16);
}


void scc_ut_estimate_avg_seed_dist(void** state)
{
	(void) state;

	iscc_Dpid fp_seeds[4] = {0, 4, 7, 12};
	iscc_SeedResult sr = {
		.capacity = 10,
		.count = 4,
		.seeds = fp_seeds,
	};

	iscc_Digraph nng1;
	iscc_digraph_from_string(".#... #.... ...../"
	                         "..#.. ...#. ...#./"
	                         "..#.. .#... .#.../"
	                         ".#... #..#. ..#.#/"
	                         "..##. ..... ...../"

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
	double out1 = 0.0;
	scc_ErrorCode ec1 = iscc_estimate_avg_seed_dist(&scc_ut_test_data_small_struct,
	                                                &sr,
	                                                &nng1,
	                                                3,
	                                                &out1);
	assert_int_equal(ec1, SCC_ER_OK);
	const double ref1 = 0.99083651;
	assert_double_equal(out1, ref1);
	assert_free_digraph(&nng1);


	iscc_Digraph nng2;
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
	                         "..#.. ###.. ..##./", &nng2);
	double out2 = 0.0;
	scc_ErrorCode ec2 = iscc_estimate_avg_seed_dist(&scc_ut_test_data_small_struct,
	                                                &sr,
	                                                &nng2,
	                                                3,
	                                                &out2);
	assert_int_equal(ec2, SCC_ER_OK);
	const double ref2 = 0.8982535896;
	assert_double_equal(out2, ref2);
	assert_free_digraph(&nng2);
}


void scc_ut_make_nng_clusters_from_seeds(void** state)
{
	(void) state;

	scc_Clabel external_cluster_labels[15];
	const scc_Clabel M = SCC_CLABEL_NA;
	const bool main_data_points[15] = { true, true, true, true, true,
	                                    false, false, false, false, false,
	                                    true, true, true, true, true };

	scc_Clustering* cl1;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl1), SCC_ER_OK);
	iscc_Digraph nng1;
	iscc_digraph_from_string("..... ##... ...../"
	                         "..... ..##. ...../"
	                         "..... ....# #..../"
	                         "..... ..... .##../"
	                         "..... ..... ...##/"

	                         ".##.. ...#. .#.../"
	                         "...#. ..#.. ..#../"
	                         "..... ##... ....#/"
	                         ".#... ..#.. .#.../"
	                         "...#. ..... ..#.#/"

	                         "..#.# .#..# ...#./"
	                         ".#... #.#.. .#.../"
	                         "..#.. .#... .#.../"
	                         "...#. ..#.. ...#./"
	                         ".#... ...#. ..#../", &nng1);
	iscc_Dpid fp_seeds1[5] = {0, 1, 2, 3, 4};
	iscc_SeedResult sr1 = {
		.capacity = 5,
		.count = 5,
		.seeds = fp_seeds1,
	};
	scc_ErrorCode ec1 = iscc_make_nng_clusters_from_seeds(cl1, &scc_ut_test_data_small_struct,
	                                                      &sr1, &nng1, true,
	                                                      SCC_UM_ASSIGN_BY_NNG, false, 0.0,
	                                                      NULL, SCC_UM_IGNORE, false, 0.0);
	const scc_Clabel ref_cluster_label1[15] = { 0, 1, 2, 3, 4, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4 };
	assert_int_equal(ec1, SCC_ER_OK);
	assert_int_equal(cl1->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl1->num_data_points, 15);
	assert_int_equal(cl1->num_clusters, 5);
	assert_non_null(cl1->cluster_label);
	assert_ptr_equal(cl1->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label1, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl1->cluster_label, ref_cluster_label1, 15 * sizeof(scc_Clabel));
	assert_true(cl1->external_labels);
	scc_free_clustering(&cl1);
	iscc_free_digraph(&nng1);


	scc_Clustering* cl2;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl2), SCC_ER_OK);
	iscc_Digraph nng2;
	iscc_digraph_from_string("..... ##... ...../"
	                         "..... ..##. ...../"
	                         "..... ....# #..../"
	                         "..... ..... .##../"
	                         "..... ..... ...##/"

	                         ".##.. ...#. .#.../"
	                         "...#. ..#.. ..#../"
	                         "..... ##... ....#/"
	                         ".#... ..#.. .#.../"
	                         "...#. ..... ..#.#/"

	                         "..#.# .#..# ...#./"
	                         ".#... #.#.. .#.../"
	                         "..#.. .#... .#.../"
	                         "...#. ..#.. ...#./"
	                         ".#... ...#. ..#../", &nng2);
	iscc_Dpid fp_seeds2[4] = {0, 2, 3, 4};
	iscc_SeedResult sr2 = {
		.capacity = 4,
		.count = 4,
		.seeds = fp_seeds2,
	};
	scc_ErrorCode ec2 = iscc_make_nng_clusters_from_seeds(cl2, &scc_ut_test_data_small_struct,
	                                                      &sr2, &nng2, true,
	                                                      SCC_UM_IGNORE, false, 0.0,
	                                                      NULL, SCC_UM_IGNORE, false, 0.0);
	const scc_Clabel ref_cluster_label2[15] = { 0, M, 1, 2, 3, 0, 0, M, M, 1, 1, 2, 2, 3, 3 };
	assert_int_equal(ec2, SCC_ER_OK);
	assert_int_equal(cl2->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl2->num_data_points, 15);
	assert_int_equal(cl2->num_clusters, 4);
	assert_non_null(cl2->cluster_label);
	assert_ptr_equal(cl2->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label2, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl2->cluster_label, ref_cluster_label2, 15 * sizeof(scc_Clabel));
	assert_true(cl2->external_labels);
	scc_free_clustering(&cl2);
	iscc_free_digraph(&nng2);


	scc_Clustering* cl3;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl3), SCC_ER_OK);
	iscc_Digraph nng3;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         ".#... ..... ..#../"
	                         "....# ..#.. ...../"
	                         ".#... ..... #..../"
	                         "..... ....# .#.../"
	                         "...#. ..#.. ...../"

	                         "..#.. ..#.. ...../"
	                         "..... ..... ..#.#/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... .#.../", &nng3);
	iscc_Dpid fp_seeds3[4] = {2, 5, 3, 8};
	iscc_SeedResult sr3 = {
		.capacity = 4,
		.count = 4,
		.seeds = fp_seeds3,
	};
	scc_ErrorCode ec3 = iscc_make_nng_clusters_from_seeds(cl3, &scc_ut_test_data_small_struct,
	                                                      &sr3, &nng3, true,
	                                                      SCC_UM_ASSIGN_BY_NNG, false, 0.0,
	                                                      NULL, SCC_UM_IGNORE, false, 0.0);
	const scc_Clabel ref_cluster_label3[15] = { 0, 1, 0, 2, 3,   1, 0, 1, 3, 3,   2, 3, 1, 2, 1 };
	assert_int_equal(ec3, SCC_ER_OK);
	assert_int_equal(cl3->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl3->num_data_points, 15);
	assert_int_equal(cl3->num_clusters, 4);
	assert_non_null(cl3->cluster_label);
	assert_ptr_equal(cl3->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label3, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl3->cluster_label, ref_cluster_label3, 15 * sizeof(scc_Clabel));
	assert_true(cl3->external_labels);
	scc_free_clustering(&cl3);
	iscc_free_digraph(&nng3);


	scc_Clustering* cl3a;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl3a), SCC_ER_OK);
	iscc_Digraph nng3a;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         ".#... ..... ..#../"
	                         "....# ..#.. ...../"
	                         "..... ..... ...../"
	                         "..... ....# .#.../"
	                         "...#. ..#.. ...../"

	                         "..#.. ..#.. ...../"
	                         "..... ..... ..#.#/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "....# ..#.. ...../", &nng3a);
	iscc_Dpid fp_seeds3a[4] = {2, 5, 3, 8};
	iscc_SeedResult sr3a = {
		.capacity = 4,
		.count = 4,
		.seeds = fp_seeds3a,
	};
	scc_ErrorCode ec3a = iscc_make_nng_clusters_from_seeds(cl3a, &scc_ut_test_data_small_struct,
	                                                      &sr3a, &nng3a, true,
	                                                      SCC_UM_ASSIGN_BY_NNG, false, 0.0,
	                                                      NULL, SCC_UM_IGNORE, false, 0.0);
	const scc_Clabel ref_cluster_label3a[15] = { 0, 1, 0, 2, 3,   1, 0, M, 3, 3,   2, 3, 1, 2, M };
	assert_int_equal(ec3a, SCC_ER_OK);
	assert_int_equal(cl3a->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl3a->num_data_points, 15);
	assert_int_equal(cl3a->num_clusters, 4);
	assert_non_null(cl3a->cluster_label);
	assert_ptr_equal(cl3a->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label3a, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl3a->cluster_label, ref_cluster_label3a, 15 * sizeof(scc_Clabel));
	assert_true(cl3a->external_labels);
	scc_free_clustering(&cl3a);
	iscc_free_digraph(&nng3a);


	scc_Clustering* cl4;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl4), SCC_ER_OK);
	iscc_Digraph nng4;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         ".#... ..... ..#../"
	                         "....# ..#.. ...../"
	                         ".#... ..... #..../"
	                         "..... ....# .#.../"
	                         "...#. ..#.. ...../"

	                         "..#.. ..#.. ...../"
	                         "..... ..... ..#.#/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... .#.../", &nng4);
	iscc_Dpid fp_seeds4[4] = {2, 5, 3, 8};
	iscc_SeedResult sr4 = {
		.capacity = 4,
		.count = 4,
		.seeds = fp_seeds4,
	};
	scc_ErrorCode ec4 = iscc_make_nng_clusters_from_seeds(cl4, &scc_ut_test_data_small_struct,
	                                                      &sr4, &nng4, true,
	                                                      SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                                                      NULL, SCC_UM_IGNORE, false, 0.0);
	const scc_Clabel ref_cluster_label4[15] = { 0, 1, 0, 2, 3,   1, 0, 1, 3, 3,   2, 3, 1, 2, 1 };
	assert_int_equal(ec4, SCC_ER_OK);
	assert_int_equal(cl4->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl4->num_data_points, 15);
	assert_int_equal(cl4->num_clusters, 4);
	assert_non_null(cl4->cluster_label);
	assert_ptr_equal(cl4->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label4, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl4->cluster_label, ref_cluster_label4, 15 * sizeof(scc_Clabel));
	assert_true(cl4->external_labels);
	scc_free_clustering(&cl4);
	iscc_free_digraph(&nng4);


	scc_Clustering* cl4a;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl4a), SCC_ER_OK);
	iscc_Digraph nng4a;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ..... ...../"

	                         ".#... ..... ..#../"
	                         "....# ..#.. ...../"
	                         ".#... ..... #..../"
	                         "..... ....# .#.../"
	                         "...#. ..#.. ...../"

	                         "..#.. ..#.. ...../"
	                         "..... ..... ..#.#/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... .#.../", &nng4a);
	iscc_Dpid fp_seeds4a[4] = {2, 5, 3, 8};
	iscc_SeedResult sr4a = {
		.capacity = 4,
		.count = 4,
		.seeds = fp_seeds4a,
	};
	scc_ErrorCode ec4a = iscc_make_nng_clusters_from_seeds(cl4a, &scc_ut_test_data_small_struct,
	                                                      &sr4a, &nng4a, true,
	                                                      SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                                                      NULL, SCC_UM_IGNORE, false, 0.0);
	const scc_Clabel ref_cluster_label4a[15] = { 0, 1, 0, 2, 2,   1, 0, 1, 3, 3,   2, 3, 1, 2, 1 };
	assert_int_equal(ec4a, SCC_ER_OK);
	assert_int_equal(cl4a->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl4a->num_data_points, 15);
	assert_int_equal(cl4a->num_clusters, 4);
	assert_non_null(cl4a->cluster_label);
	assert_ptr_equal(cl4a->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label4a, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl4a->cluster_label, ref_cluster_label4a, 15 * sizeof(scc_Clabel));
	assert_true(cl4a->external_labels);
	scc_free_clustering(&cl4a);
	iscc_free_digraph(&nng4a);


	scc_Clustering* cl5;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl5), SCC_ER_OK);
	iscc_Digraph nng5;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         ".#... ..... ..#../"
	                         "....# ..#.. ...../"
	                         ".#... ..... #..../"
	                         "..... ....# .#.../"
	                         "...#. ..#.. ...../"

	                         "..#.. ..#.. ...../"
	                         "..... ..... ..#.#/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... .#.../", &nng5);
	iscc_Dpid fp_seeds5[4] = {2, 5, 3, 8};
	iscc_SeedResult sr5 = {
		.capacity = 4,
		.count = 4,
		.seeds = fp_seeds5,
	};
	scc_ErrorCode ec5 = iscc_make_nng_clusters_from_seeds(cl5, &scc_ut_test_data_small_struct,
	                                                      &sr5, &nng5, false,
	                                                      SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                                                      NULL, SCC_UM_IGNORE, false, 0.0);
	const scc_Clabel ref_cluster_label5[15] = { 0, 1, 0, 2, 2,   1, 0, 1, 3, 3,   2, 3, 1, 2, 0 };
	assert_int_equal(ec5, SCC_ER_OK);
	assert_int_equal(cl5->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl5->num_data_points, 15);
	assert_int_equal(cl5->num_clusters, 4);
	assert_non_null(cl5->cluster_label);
	assert_ptr_equal(cl5->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label5, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl5->cluster_label, ref_cluster_label5, 15 * sizeof(scc_Clabel));
	assert_true(cl5->external_labels);
	scc_free_clustering(&cl5);
	iscc_free_digraph(&nng5);


	scc_Clustering* cl5a;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl5a), SCC_ER_OK);
	iscc_Digraph nng5a;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         ".#... ..... ..#../"
	                         "....# ..#.. ...../"
	                         ".#... ..... #..../"
	                         "..... ....# .#.../"
	                         "...#. ..#.. ...../"

	                         "..#.. ..#.. ...../"
	                         "..... ..... ..#.#/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... .#.../", &nng5a);
	iscc_Dpid fp_seeds5a[4] = {2, 5, 3, 8};
	iscc_SeedResult sr5a = {
		.capacity = 4,
		.count = 4,
		.seeds = fp_seeds5a,
	};
	scc_ErrorCode ec5a = iscc_make_nng_clusters_from_seeds(cl5a, &scc_ut_test_data_small_struct,
	                                                      &sr5a, &nng5a, false,
	                                                      SCC_UM_CLOSEST_ASSIGNED, true, 0.1,
	                                                      NULL, SCC_UM_IGNORE, false, 0.0);
	const scc_Clabel ref_cluster_label5a[15] = { 0, 1, 0, 2, 2,   1, 0, M, 3, 3,   2, 3, 1, 2, 0 };
	assert_int_equal(ec5a, SCC_ER_OK);
	assert_int_equal(cl5a->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl5a->num_data_points, 15);
	assert_int_equal(cl5a->num_clusters, 4);
	assert_non_null(cl5a->cluster_label);
	assert_ptr_equal(cl5a->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label5a, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl5a->cluster_label, ref_cluster_label5a, 15 * sizeof(scc_Clabel));
	assert_true(cl5a->external_labels);
	scc_free_clustering(&cl5a);
	iscc_free_digraph(&nng5a);


	scc_Clustering* cl6;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl6), SCC_ER_OK);
	iscc_Digraph nng6;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         ".#... ..... ..#../"
	                         "....# ..#.. ...../"
	                         ".#... ..... #..../"
	                         "..... ....# .#.../"
	                         "...#. ..#.. ...../"

	                         "..#.. ..#.. ...../"
	                         "..... ..... ..#.#/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... .#.../", &nng6);
	iscc_Dpid fp_seeds6[4] = {2, 5, 3, 8};
	iscc_SeedResult sr6 = {
		.capacity = 4,
		.count = 4,
		.seeds = fp_seeds6,
	};
	scc_ErrorCode ec6 = iscc_make_nng_clusters_from_seeds(cl6, &scc_ut_test_data_small_struct,
	                                                      &sr6, &nng6, true,
	                                                      SCC_UM_CLOSEST_SEED, false, 0.0,
	                                                      NULL, SCC_UM_IGNORE, false, 0.0);
	const scc_Clabel ref_cluster_label6[15] = { 0, 1, 0, 2, 0,   1, 0, 3, 3, 3,   2, 3, 1, 2, 0 };
	assert_int_equal(ec6, SCC_ER_OK);
	assert_int_equal(cl6->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl6->num_data_points, 15);
	assert_int_equal(cl6->num_clusters, 4);
	assert_non_null(cl6->cluster_label);
	assert_ptr_equal(cl6->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label6, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl6->cluster_label, ref_cluster_label6, 15 * sizeof(scc_Clabel));
	assert_true(cl6->external_labels);
	scc_free_clustering(&cl6);
	iscc_free_digraph(&nng6);


	scc_Clustering* cl7;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl7), SCC_ER_OK);
	iscc_Digraph nng7;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..#.. ..#.. ...../"
	                         "..... ..... ...##/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... .#.../", &nng7);
	iscc_Dpid fp_seeds7[4] = {11, 0, 4};
	iscc_SeedResult sr7 = {
		.capacity = 3,
		.count = 3,
		.seeds = fp_seeds7,
	};
	scc_ErrorCode ec7 = iscc_make_nng_clusters_from_seeds(cl7, &scc_ut_test_data_small_struct,
	                                                      &sr7, &nng7, true,
	                                                      SCC_UM_IGNORE, false, 0.0,
	                                                      main_data_points, SCC_UM_IGNORE, false, 0.0);
	const scc_Clabel ref_cluster_label7[15] = { 1, 1, M, M, 2,   M, 1, M, 2, M,   M, 0, 2, 0, 0 };
	assert_int_equal(ec7, SCC_ER_OK);
	assert_int_equal(cl7->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl7->num_data_points, 15);
	assert_int_equal(cl7->num_clusters, 3);
	assert_non_null(cl7->cluster_label);
	assert_ptr_equal(cl7->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label7, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl7->cluster_label, ref_cluster_label7, 15 * sizeof(scc_Clabel));
	assert_true(cl7->external_labels);
	scc_free_clustering(&cl7);
	iscc_free_digraph(&nng7);


	scc_Clustering* cl7a;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl7a), SCC_ER_OK);
	iscc_Digraph nng7a;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..#.. ..#.. ...../"
	                         "..... ..... ...##/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... .#.../", &nng7a);
	iscc_Dpid fp_seeds7a[4] = {11, 0, 4};
	iscc_SeedResult sr7a = {
		.capacity = 3,
		.count = 3,
		.seeds = fp_seeds7a,
	};
	scc_ErrorCode ec7a = iscc_make_nng_clusters_from_seeds(cl7a, &scc_ut_test_data_small_struct,
	                                                      &sr7a, &nng7a, true,
	                                                      SCC_UM_IGNORE, false, 0.0,
	                                                      main_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	const scc_Clabel ref_cluster_label7a[15] = { 1, 1, M, M, 2,   0, 1, 1, 2, 2,   M, 0, 2, 0, 0 };
	assert_int_equal(ec7a, SCC_ER_OK);
	assert_int_equal(cl7a->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl7a->num_data_points, 15);
	assert_int_equal(cl7a->num_clusters, 3);
	assert_non_null(cl7a->cluster_label);
	assert_ptr_equal(cl7a->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label7a, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl7a->cluster_label, ref_cluster_label7a, 15 * sizeof(scc_Clabel));
	assert_true(cl7a->external_labels);
	scc_free_clustering(&cl7a);
	iscc_free_digraph(&nng7a);


	scc_Clustering* cl7b;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl7b), SCC_ER_OK);
	iscc_Digraph nng7b;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..#.. ..#.. ...../"
	                         "..... ..... ...##/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... .#.../", &nng7b);
	iscc_Dpid fp_seeds7b[4] = {11, 0, 4};
	iscc_SeedResult sr7b = {
		.capacity = 3,
		.count = 3,
		.seeds = fp_seeds7b,
	};
	scc_ErrorCode ec7b = iscc_make_nng_clusters_from_seeds(cl7b, &scc_ut_test_data_small_struct,
	                                                      &sr7b, &nng7b, true,
	                                                      SCC_UM_IGNORE, false, 0.0,
	                                                      main_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 0.75);
	const scc_Clabel ref_cluster_label7b[15] = { 1, 1, M, M, 2,   M, 1, 1, 2, 2,   M, 0, 2, 0, 0 };
	assert_int_equal(ec7b, SCC_ER_OK);
	assert_int_equal(cl7b->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl7b->num_data_points, 15);
	assert_int_equal(cl7b->num_clusters, 3);
	assert_non_null(cl7b->cluster_label);
	assert_ptr_equal(cl7b->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label7b, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl7b->cluster_label, ref_cluster_label7b, 15 * sizeof(scc_Clabel));
	assert_true(cl7b->external_labels);
	scc_free_clustering(&cl7b);
	iscc_free_digraph(&nng7b);


	scc_Clustering* cl7c;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl7c), SCC_ER_OK);
	iscc_Digraph nng7c;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..#.. ..#.. ...../"
	                         "..... ..... ...##/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... .#.../", &nng7c);
	iscc_Dpid fp_seeds7c[4] = {11, 0, 4};
	iscc_SeedResult sr7c = {
		.capacity = 3,
		.count = 3,
		.seeds = fp_seeds7c,
	};
	scc_ErrorCode ec7c = iscc_make_nng_clusters_from_seeds(cl7c, &scc_ut_test_data_small_struct,
	                                                      &sr7c, &nng7c, true,
	                                                      SCC_UM_IGNORE, false, 0.0,
	                                                      main_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	const scc_Clabel ref_cluster_label7c[15] = { 1, 1, M, M, 2,   0, 1, 1, 2, 2,   M, 0, 2, 0, 0 };
	assert_int_equal(ec7c, SCC_ER_OK);
	assert_int_equal(cl7c->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl7c->num_data_points, 15);
	assert_int_equal(cl7c->num_clusters, 3);
	assert_non_null(cl7c->cluster_label);
	assert_ptr_equal(cl7c->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label7c, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl7c->cluster_label, ref_cluster_label7c, 15 * sizeof(scc_Clabel));
	assert_true(cl7c->external_labels);
	scc_free_clustering(&cl7c);
	iscc_free_digraph(&nng7c);


	scc_Clustering* cl7d;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl7d), SCC_ER_OK);
	iscc_Digraph nng7d;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..#.. ..#.. ...../"
	                         "..... ..... ...##/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... .#.../", &nng7d);
	iscc_Dpid fp_seeds7d[4] = {11, 0, 4};
	iscc_SeedResult sr7d = {
		.capacity = 3,
		.count = 3,
		.seeds = fp_seeds7d,
	};
	scc_ErrorCode ec7d = iscc_make_nng_clusters_from_seeds(cl7d, &scc_ut_test_data_small_struct,
	                                                      &sr7d, &nng7d, true,
	                                                      SCC_UM_IGNORE, false, 0.0,
	                                                      main_data_points, SCC_UM_CLOSEST_SEED, true, 0.75);
	const scc_Clabel ref_cluster_label7d[15] = { 1, 1, M, M, 2,   M, 1, 1, 2, M,   M, 0, 2, 0, 0 };
	assert_int_equal(ec7d, SCC_ER_OK);
	assert_int_equal(cl7d->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl7d->num_data_points, 15);
	assert_int_equal(cl7d->num_clusters, 3);
	assert_non_null(cl7d->cluster_label);
	assert_ptr_equal(cl7d->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label7d, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl7d->cluster_label, ref_cluster_label7d, 15 * sizeof(scc_Clabel));
	assert_true(cl7d->external_labels);
	scc_free_clustering(&cl7d);
	iscc_free_digraph(&nng7d);


	scc_Clustering* cl8;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl8), SCC_ER_OK);
	iscc_Digraph nng8;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..#.. ...#. ...../"
	                         "..... ..... ...##/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... .#.../", &nng8);
	iscc_Dpid fp_seeds8[4] = {11, 0, 4};
	iscc_SeedResult sr8 = {
		.capacity = 3,
		.count = 3,
		.seeds = fp_seeds8,
	};
	scc_ErrorCode ec8 = iscc_make_nng_clusters_from_seeds(cl8, &scc_ut_test_data_small_struct,
	                                                      &sr8, &nng8, true,
	                                                      SCC_UM_ASSIGN_BY_NNG, false, 0.0,
	                                                      main_data_points, SCC_UM_IGNORE, false, 0.0);
	const scc_Clabel ref_cluster_label8[15] = { 1, 1, 1, 0, 2,   M, 1, M, 2, M,   2, 0, 2, 0, 0 };
	assert_int_equal(ec8, SCC_ER_OK);
	assert_int_equal(cl8->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl8->num_data_points, 15);
	assert_int_equal(cl8->num_clusters, 3);
	assert_non_null(cl8->cluster_label);
	assert_ptr_equal(cl8->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label8, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl8->cluster_label, ref_cluster_label8, 15 * sizeof(scc_Clabel));
	assert_true(cl8->external_labels);
	scc_free_clustering(&cl8);
	iscc_free_digraph(&nng8);


	scc_Clustering* cl8a;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl8a), SCC_ER_OK);
	iscc_Digraph nng8a;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..#.. ...#. ...../"
	                         "..... ..... ...##/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... .#.../", &nng8a);
	iscc_Dpid fp_seeds8a[4] = {11, 0, 4};
	iscc_SeedResult sr8a = {
		.capacity = 3,
		.count = 3,
		.seeds = fp_seeds8a,
	};
	scc_ErrorCode ec8a = iscc_make_nng_clusters_from_seeds(cl8a, &scc_ut_test_data_small_struct,
	                                                      &sr8a, &nng8a, true,
	                                                      SCC_UM_ASSIGN_BY_NNG, false, 0.0,
	                                                      main_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	const scc_Clabel ref_cluster_label8a[15] = { 1, 1, 1, 0, 2,   0, 1, 1, 2, 2,   2, 0, 2, 0, 0 };
	assert_int_equal(ec8a, SCC_ER_OK);
	assert_int_equal(cl8a->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl8a->num_data_points, 15);
	assert_int_equal(cl8a->num_clusters, 3);
	assert_non_null(cl8a->cluster_label);
	assert_ptr_equal(cl8a->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label8a, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl8a->cluster_label, ref_cluster_label8a, 15 * sizeof(scc_Clabel));
	assert_true(cl8a->external_labels);
	scc_free_clustering(&cl8a);
	iscc_free_digraph(&nng8a);


	scc_Clustering* cl8b;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl8b), SCC_ER_OK);
	iscc_Digraph nng8b;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..#.. ...#. ...../"
	                         "..... ..... ...##/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... .#.../", &nng8b);
	iscc_Dpid fp_seeds8b[4] = {11, 0, 4};
	iscc_SeedResult sr8b = {
		.capacity = 3,
		.count = 3,
		.seeds = fp_seeds8b,
	};
	scc_ErrorCode ec8b = iscc_make_nng_clusters_from_seeds(cl8b, &scc_ut_test_data_small_struct,
	                                                      &sr8b, &nng8b, true,
	                                                      SCC_UM_ASSIGN_BY_NNG, false, 0.0,
	                                                      main_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 0.75);
	const scc_Clabel ref_cluster_label8b[15] = { 1, 1, 1, 0, 2,   M, 1, 1, 2, 2,   2, 0, 2, 0, 0 };
	assert_int_equal(ec8b, SCC_ER_OK);
	assert_int_equal(cl8b->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl8b->num_data_points, 15);
	assert_int_equal(cl8b->num_clusters, 3);
	assert_non_null(cl8b->cluster_label);
	assert_ptr_equal(cl8b->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label8b, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl8b->cluster_label, ref_cluster_label8b, 15 * sizeof(scc_Clabel));
	assert_true(cl8b->external_labels);
	scc_free_clustering(&cl8b);
	iscc_free_digraph(&nng8b);


	scc_Clustering* cl8c;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl8c), SCC_ER_OK);
	iscc_Digraph nng8c;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..#.. ...#. ...../"
	                         "..... ..... ...##/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... .#.../", &nng8c);
	iscc_Dpid fp_seeds8c[4] = {11, 0, 4};
	iscc_SeedResult sr8c = {
		.capacity = 3,
		.count = 3,
		.seeds = fp_seeds8c,
	};
	scc_ErrorCode ec8c = iscc_make_nng_clusters_from_seeds(cl8c, &scc_ut_test_data_small_struct,
	                                                      &sr8c, &nng8c, true,
	                                                      SCC_UM_ASSIGN_BY_NNG, false, 0.0,
	                                                      main_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	const scc_Clabel ref_cluster_label8c[15] = { 1, 1, 1, 0, 2,   0, 1, 1, 2, 2,   2, 0, 2, 0, 0 };
	assert_int_equal(ec8c, SCC_ER_OK);
	assert_int_equal(cl8c->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl8c->num_data_points, 15);
	assert_int_equal(cl8c->num_clusters, 3);
	assert_non_null(cl8c->cluster_label);
	assert_ptr_equal(cl8c->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label8c, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl8c->cluster_label, ref_cluster_label8c, 15 * sizeof(scc_Clabel));
	assert_true(cl8c->external_labels);
	scc_free_clustering(&cl8c);
	iscc_free_digraph(&nng8c);


	scc_Clustering* cl8d;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl8d), SCC_ER_OK);
	iscc_Digraph nng8d;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..#.. ...#. ...../"
	                         "..... ..... ...##/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... .#.../", &nng8d);
	iscc_Dpid fp_seeds8d[4] = {11, 0, 4};
	iscc_SeedResult sr8d = {
		.capacity = 3,
		.count = 3,
		.seeds = fp_seeds8d,
	};
	scc_ErrorCode ec8d = iscc_make_nng_clusters_from_seeds(cl8d, &scc_ut_test_data_small_struct,
	                                                      &sr8d, &nng8d, true,
	                                                      SCC_UM_ASSIGN_BY_NNG, false, 0.0,
	                                                      main_data_points, SCC_UM_CLOSEST_SEED, true, 0.75);
	const scc_Clabel ref_cluster_label8d[15] = { 1, 1, 1, 0, 2,   M, 1, 1, 2, M,   2, 0, 2, 0, 0 };
	assert_int_equal(ec8d, SCC_ER_OK);
	assert_int_equal(cl8d->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl8d->num_data_points, 15);
	assert_int_equal(cl8d->num_clusters, 3);
	assert_non_null(cl8d->cluster_label);
	assert_ptr_equal(cl8d->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label8d, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl8d->cluster_label, ref_cluster_label8d, 15 * sizeof(scc_Clabel));
	assert_true(cl8d->external_labels);
	scc_free_clustering(&cl8d);
	iscc_free_digraph(&nng8d);


	scc_Clustering* cl9;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl9), SCC_ER_OK);
	iscc_Digraph nng9;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..#.. ...#. ...../"
	                         "..... ..... ...##/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... .#.../", &nng9);
	iscc_Dpid fp_seeds9[4] = {11, 0, 4};
	iscc_SeedResult sr9 = {
		.capacity = 3,
		.count = 3,
		.seeds = fp_seeds9,
	};
	scc_ErrorCode ec9 = iscc_make_nng_clusters_from_seeds(cl9, &scc_ut_test_data_small_struct,
	                                                      &sr9, &nng9, true,
	                                                      SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                                                      main_data_points, SCC_UM_IGNORE, false, 0.0);
	const scc_Clabel ref_cluster_label9[15] = { 1, 1, 1, 0, 2,   M, 1, M, 2, M,   2, 0, 2, 0, 0 };
	assert_int_equal(ec9, SCC_ER_OK);
	assert_int_equal(cl9->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl9->num_data_points, 15);
	assert_int_equal(cl9->num_clusters, 3);
	assert_non_null(cl9->cluster_label);
	assert_ptr_equal(cl9->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label9, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl9->cluster_label, ref_cluster_label9, 15 * sizeof(scc_Clabel));
	assert_true(cl9->external_labels);
	scc_free_clustering(&cl9);
	iscc_free_digraph(&nng9);


	scc_Clustering* cl9a;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl9a), SCC_ER_OK);
	iscc_Digraph nng9a;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..#.. ...#. ...../"
	                         "..... ..... ...##/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... .#.../", &nng9a);
	iscc_Dpid fp_seeds9a[4] = {11, 0, 4};
	iscc_SeedResult sr9a = {
		.capacity = 3,
		.count = 3,
		.seeds = fp_seeds9a,
	};
	scc_ErrorCode ec9a = iscc_make_nng_clusters_from_seeds(cl9a, &scc_ut_test_data_small_struct,
	                                                      &sr9a, &nng9a, true,
	                                                      SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                                                      main_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	const scc_Clabel ref_cluster_label9a[15] = { 1, 1, 1, 0, 2,   0, 1, 1, 2, 2,   2, 0, 2, 0, 0 };
	assert_int_equal(ec9a, SCC_ER_OK);
	assert_int_equal(cl9a->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl9a->num_data_points, 15);
	assert_int_equal(cl9a->num_clusters, 3);
	assert_non_null(cl9a->cluster_label);
	assert_ptr_equal(cl9a->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label9a, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl9a->cluster_label, ref_cluster_label9a, 15 * sizeof(scc_Clabel));
	assert_true(cl9a->external_labels);
	scc_free_clustering(&cl9a);
	iscc_free_digraph(&nng9a);


	scc_Clustering* cl9b;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl9b), SCC_ER_OK);
	iscc_Digraph nng9b;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..#.. ...#. ...../"
	                         "..... ..... ...##/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... .#.../", &nng9b);
	iscc_Dpid fp_seeds9b[4] = {11, 0, 4};
	iscc_SeedResult sr9b = {
		.capacity = 3,
		.count = 3,
		.seeds = fp_seeds9b,
	};
	scc_ErrorCode ec9b = iscc_make_nng_clusters_from_seeds(cl9b, &scc_ut_test_data_small_struct,
	                                                      &sr9b, &nng9b, true,
	                                                      SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                                                      main_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 0.75);
	const scc_Clabel ref_cluster_label9b[15] = { 1, 1, 1, 0, 2,   M, 1, 1, 2, 2,   2, 0, 2, 0, 0 };
	assert_int_equal(ec9b, SCC_ER_OK);
	assert_int_equal(cl9b->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl9b->num_data_points, 15);
	assert_int_equal(cl9b->num_clusters, 3);
	assert_non_null(cl9b->cluster_label);
	assert_ptr_equal(cl9b->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label9b, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl9b->cluster_label, ref_cluster_label9b, 15 * sizeof(scc_Clabel));
	assert_true(cl9b->external_labels);
	scc_free_clustering(&cl9b);
	iscc_free_digraph(&nng9b);


	scc_Clustering* cl9c;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl9c), SCC_ER_OK);
	iscc_Digraph nng9c;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..#.. ...#. ...../"
	                         "..... ..... ...##/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... .#.../", &nng9c);
	iscc_Dpid fp_seeds9c[4] = {11, 0, 4};
	iscc_SeedResult sr9c = {
		.capacity = 3,
		.count = 3,
		.seeds = fp_seeds9c,
	};
	scc_ErrorCode ec9c = iscc_make_nng_clusters_from_seeds(cl9c, &scc_ut_test_data_small_struct,
	                                                      &sr9c, &nng9c, true,
	                                                      SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                                                      main_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	const scc_Clabel ref_cluster_label9c[15] = { 1, 1, 1, 0, 2,   0, 1, 1, 2, 2,   2, 0, 2, 0, 0 };
	assert_int_equal(ec9c, SCC_ER_OK);
	assert_int_equal(cl9c->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl9c->num_data_points, 15);
	assert_int_equal(cl9c->num_clusters, 3);
	assert_non_null(cl9c->cluster_label);
	assert_ptr_equal(cl9c->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label9c, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl9c->cluster_label, ref_cluster_label9c, 15 * sizeof(scc_Clabel));
	assert_true(cl9c->external_labels);
	scc_free_clustering(&cl9c);
	iscc_free_digraph(&nng9c);


	scc_Clustering* cl9d;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl9d), SCC_ER_OK);
	iscc_Digraph nng9d;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..#.. ...#. ...../"
	                         "..... ..... ...##/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... .#.../", &nng9d);
	iscc_Dpid fp_seeds9d[4] = {11, 0, 4};
	iscc_SeedResult sr9d = {
		.capacity = 3,
		.count = 3,
		.seeds = fp_seeds9d,
	};
	scc_ErrorCode ec9d = iscc_make_nng_clusters_from_seeds(cl9d, &scc_ut_test_data_small_struct,
	                                                      &sr9d, &nng9d, true,
	                                                      SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                                                      main_data_points, SCC_UM_CLOSEST_SEED, true, 0.75);
	const scc_Clabel ref_cluster_label9d[15] = { 1, 1, 1, 0, 2,   M, 1, 1, 2, M,   2, 0, 2, 0, 0 };
	assert_int_equal(ec9d, SCC_ER_OK);
	assert_int_equal(cl9d->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl9d->num_data_points, 15);
	assert_int_equal(cl9d->num_clusters, 3);
	assert_non_null(cl9d->cluster_label);
	assert_ptr_equal(cl9d->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label9d, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl9d->cluster_label, ref_cluster_label9d, 15 * sizeof(scc_Clabel));
	assert_true(cl9d->external_labels);
	scc_free_clustering(&cl9d);
	iscc_free_digraph(&nng9d);


	scc_Clustering* cl10;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl10), SCC_ER_OK);
	iscc_Digraph nng10;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..#.. ...#. ...../"
	                         "..... ..... ...##/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... .#.../", &nng10);
	iscc_Dpid fp_seeds10[4] = {11, 0, 4};
	iscc_SeedResult sr10 = {
		.capacity = 3,
		.count = 3,
		.seeds = fp_seeds10,
	};
	scc_ErrorCode ec10 = iscc_make_nng_clusters_from_seeds(cl10, &scc_ut_test_data_small_struct,
	                                                      &sr10, &nng10, false,
	                                                      SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                                                      main_data_points, SCC_UM_IGNORE, false, 0.0);
	const scc_Clabel ref_cluster_label10[15] = { 1, 1, 2, 2, 2,   M, 1, M, 2, M,   2, 0, 2, 0, 0 };
	assert_int_equal(ec10, SCC_ER_OK);
	assert_int_equal(cl10->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl10->num_data_points, 15);
	assert_int_equal(cl10->num_clusters, 3);
	assert_non_null(cl10->cluster_label);
	assert_ptr_equal(cl10->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label10, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl10->cluster_label, ref_cluster_label10, 15 * sizeof(scc_Clabel));
	assert_true(cl10->external_labels);
	scc_free_clustering(&cl10);
	iscc_free_digraph(&nng10);


	scc_Clustering* cl10a;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl10a), SCC_ER_OK);
	iscc_Digraph nng10a;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..#.. ...#. ...../"
	                         "..... ..... ...##/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... .#.../", &nng10a);
	iscc_Dpid fp_seeds10a[4] = {11, 0, 4};
	iscc_SeedResult sr10a = {
		.capacity = 3,
		.count = 3,
		.seeds = fp_seeds10a,
	};
	scc_ErrorCode ec10a = iscc_make_nng_clusters_from_seeds(cl10a, &scc_ut_test_data_small_struct,
	                                                      &sr10a, &nng10a, false,
	                                                      SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                                                      main_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	const scc_Clabel ref_cluster_label10a[15] = { 1, 1, 2, 2, 2,   0, 1, 1, 2, 2,   2, 0, 2, 0, 0 };
	assert_int_equal(ec10a, SCC_ER_OK);
	assert_int_equal(cl10a->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl10a->num_data_points, 15);
	assert_int_equal(cl10a->num_clusters, 3);
	assert_non_null(cl10a->cluster_label);
	assert_ptr_equal(cl10a->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label10a, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl10a->cluster_label, ref_cluster_label10a, 15 * sizeof(scc_Clabel));
	assert_true(cl10a->external_labels);
	scc_free_clustering(&cl10a);
	iscc_free_digraph(&nng10a);


	scc_Clustering* cl10b;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl10b), SCC_ER_OK);
	iscc_Digraph nng10b;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..#.. ...#. ...../"
	                         "..... ..... ...##/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... .#.../", &nng10b);
	iscc_Dpid fp_seeds10b[4] = {11, 0, 4};
	iscc_SeedResult sr10b = {
		.capacity = 3,
		.count = 3,
		.seeds = fp_seeds10b,
	};
	scc_ErrorCode ec10b = iscc_make_nng_clusters_from_seeds(cl10b, &scc_ut_test_data_small_struct,
	                                                      &sr10b, &nng10b, false,
	                                                      SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                                                      main_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 0.75);
	const scc_Clabel ref_cluster_label10b[15] = { 1, 1, 2, 2, 2,   M, 1, 1, 2, 2,   2, 0, 2, 0, 0 };
	assert_int_equal(ec10b, SCC_ER_OK);
	assert_int_equal(cl10b->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl10b->num_data_points, 15);
	assert_int_equal(cl10b->num_clusters, 3);
	assert_non_null(cl10b->cluster_label);
	assert_ptr_equal(cl10b->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label10b, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl10b->cluster_label, ref_cluster_label10b, 15 * sizeof(scc_Clabel));
	assert_true(cl10b->external_labels);
	scc_free_clustering(&cl10b);
	iscc_free_digraph(&nng10b);


	scc_Clustering* cl10c;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl10c), SCC_ER_OK);
	iscc_Digraph nng10c;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..#.. ...#. ...../"
	                         "..... ..... ...##/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... .#.../", &nng10c);
	iscc_Dpid fp_seeds10c[4] = {11, 0, 4};
	iscc_SeedResult sr10c = {
		.capacity = 3,
		.count = 3,
		.seeds = fp_seeds10c,
	};
	scc_ErrorCode ec10c = iscc_make_nng_clusters_from_seeds(cl10c, &scc_ut_test_data_small_struct,
	                                                      &sr10c, &nng10c, false,
	                                                      SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                                                      main_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	const scc_Clabel ref_cluster_label10c[15] = { 1, 1, 2, 2, 2,   0, 1, 1, 2, 2,   2, 0, 2, 0, 0 };
	assert_int_equal(ec10c, SCC_ER_OK);
	assert_int_equal(cl10c->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl10c->num_data_points, 15);
	assert_int_equal(cl10c->num_clusters, 3);
	assert_non_null(cl10c->cluster_label);
	assert_ptr_equal(cl10c->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label10c, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl10c->cluster_label, ref_cluster_label10c, 15 * sizeof(scc_Clabel));
	assert_true(cl10c->external_labels);
	scc_free_clustering(&cl10c);
	iscc_free_digraph(&nng10c);


	scc_Clustering* cl10d;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl10d), SCC_ER_OK);
	iscc_Digraph nng10d;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..#.. ...#. ...../"
	                         "..... ..... ...##/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... .#.../", &nng10d);
	iscc_Dpid fp_seeds10d[4] = {11, 0, 4};
	iscc_SeedResult sr10d = {
		.capacity = 3,
		.count = 3,
		.seeds = fp_seeds10d,
	};
	scc_ErrorCode ec10d = iscc_make_nng_clusters_from_seeds(cl10d, &scc_ut_test_data_small_struct,
	                                                      &sr10d, &nng10d, false,
	                                                      SCC_UM_CLOSEST_ASSIGNED, false, 0.0,
	                                                      main_data_points, SCC_UM_CLOSEST_SEED, true, 0.75);
	const scc_Clabel ref_cluster_label10d[15] = { 1, 1, 2, 2, 2,   M, 1, 1, 2, M,   2, 0, 2, 0, 0 };
	assert_int_equal(ec10d, SCC_ER_OK);
	assert_int_equal(cl10d->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl10d->num_data_points, 15);
	assert_int_equal(cl10d->num_clusters, 3);
	assert_non_null(cl10d->cluster_label);
	assert_ptr_equal(cl10d->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label10d, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl10d->cluster_label, ref_cluster_label10d, 15 * sizeof(scc_Clabel));
	assert_true(cl10d->external_labels);
	scc_free_clustering(&cl10d);
	iscc_free_digraph(&nng10d);


	scc_Clustering* cl11;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl11), SCC_ER_OK);
	iscc_Digraph nng11;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..#.. ...#. ...../"
	                         "..... ..... ...##/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... .#.../", &nng11);
	iscc_Dpid fp_seeds11[4] = {11, 0, 4};
	iscc_SeedResult sr11 = {
		.capacity = 3,
		.count = 3,
		.seeds = fp_seeds11,
	};
	scc_ErrorCode ec11 = iscc_make_nng_clusters_from_seeds(cl11, &scc_ut_test_data_small_struct,
	                                                      &sr11, &nng11, false,
	                                                      SCC_UM_CLOSEST_ASSIGNED, true, 0.5,
	                                                      main_data_points, SCC_UM_IGNORE, false, 0.0);
	const scc_Clabel ref_cluster_label11[15] = { 1, 1, 2, M, 2,   M, 1, M, 2, M,   2, 0, 2, 0, 0 };
	assert_int_equal(ec11, SCC_ER_OK);
	assert_int_equal(cl11->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl11->num_data_points, 15);
	assert_int_equal(cl11->num_clusters, 3);
	assert_non_null(cl11->cluster_label);
	assert_ptr_equal(cl11->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label11, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl11->cluster_label, ref_cluster_label11, 15 * sizeof(scc_Clabel));
	assert_true(cl11->external_labels);
	scc_free_clustering(&cl11);
	iscc_free_digraph(&nng11);


	scc_Clustering* cl11a;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl11a), SCC_ER_OK);
	iscc_Digraph nng11a;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..#.. ...#. ...../"
	                         "..... ..... ...##/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... .#.../", &nng11a);
	iscc_Dpid fp_seeds11a[4] = {11, 0, 4};
	iscc_SeedResult sr11a = {
		.capacity = 3,
		.count = 3,
		.seeds = fp_seeds11a,
	};
	scc_ErrorCode ec11a = iscc_make_nng_clusters_from_seeds(cl11a, &scc_ut_test_data_small_struct,
	                                                      &sr11a, &nng11a, false,
	                                                      SCC_UM_CLOSEST_ASSIGNED, true, 0.5,
	                                                      main_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	const scc_Clabel ref_cluster_label11a[15] = { 1, 1, 2, M, 2,   0, 1, 1, 2, 2,   2, 0, 2, 0, 0 };
	assert_int_equal(ec11a, SCC_ER_OK);
	assert_int_equal(cl11a->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl11a->num_data_points, 15);
	assert_int_equal(cl11a->num_clusters, 3);
	assert_non_null(cl11a->cluster_label);
	assert_ptr_equal(cl11a->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label11a, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl11a->cluster_label, ref_cluster_label11a, 15 * sizeof(scc_Clabel));
	assert_true(cl11a->external_labels);
	scc_free_clustering(&cl11a);
	iscc_free_digraph(&nng11a);


	scc_Clustering* cl11b;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl11b), SCC_ER_OK);
	iscc_Digraph nng11b;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..#.. ...#. ...../"
	                         "..... ..... ...##/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... .#.../", &nng11b);
	iscc_Dpid fp_seeds11b[4] = {11, 0, 4};
	iscc_SeedResult sr11b = {
		.capacity = 3,
		.count = 3,
		.seeds = fp_seeds11b,
	};
	scc_ErrorCode ec11b = iscc_make_nng_clusters_from_seeds(cl11b, &scc_ut_test_data_small_struct,
	                                                      &sr11b, &nng11b, false,
	                                                      SCC_UM_CLOSEST_ASSIGNED, true, 0.5,
	                                                      main_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 0.75);
	const scc_Clabel ref_cluster_label11b[15] = { 1, 1, 2, M, 2,   M, 1, 1, 2, 2,   2, 0, 2, 0, 0 };
	assert_int_equal(ec11b, SCC_ER_OK);
	assert_int_equal(cl11b->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl11b->num_data_points, 15);
	assert_int_equal(cl11b->num_clusters, 3);
	assert_non_null(cl11b->cluster_label);
	assert_ptr_equal(cl11b->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label11b, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl11b->cluster_label, ref_cluster_label11b, 15 * sizeof(scc_Clabel));
	assert_true(cl11b->external_labels);
	scc_free_clustering(&cl11b);
	iscc_free_digraph(&nng11b);


	scc_Clustering* cl11c;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl11c), SCC_ER_OK);
	iscc_Digraph nng11c;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..#.. ...#. ...../"
	                         "..... ..... ...##/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... .#.../", &nng11c);
	iscc_Dpid fp_seeds11c[4] = {11, 0, 4};
	iscc_SeedResult sr11c = {
		.capacity = 3,
		.count = 3,
		.seeds = fp_seeds11c,
	};
	scc_ErrorCode ec11c = iscc_make_nng_clusters_from_seeds(cl11c, &scc_ut_test_data_small_struct,
	                                                      &sr11c, &nng11c, false,
	                                                      SCC_UM_CLOSEST_ASSIGNED, true, 0.5,
	                                                      main_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	const scc_Clabel ref_cluster_label11c[15] = { 1, 1, 2, M, 2,   0, 1, 1, 2, 2,   2, 0, 2, 0, 0 };
	assert_int_equal(ec11c, SCC_ER_OK);
	assert_int_equal(cl11c->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl11c->num_data_points, 15);
	assert_int_equal(cl11c->num_clusters, 3);
	assert_non_null(cl11c->cluster_label);
	assert_ptr_equal(cl11c->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label11c, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl11c->cluster_label, ref_cluster_label11c, 15 * sizeof(scc_Clabel));
	assert_true(cl11c->external_labels);
	scc_free_clustering(&cl11c);
	iscc_free_digraph(&nng11c);


	scc_Clustering* cl11d;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl11d), SCC_ER_OK);
	iscc_Digraph nng11d;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..#.. ...#. ...../"
	                         "..... ..... ...##/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... .#.../", &nng11d);
	iscc_Dpid fp_seeds11d[4] = {11, 0, 4};
	iscc_SeedResult sr11d = {
		.capacity = 3,
		.count = 3,
		.seeds = fp_seeds11d,
	};
	scc_ErrorCode ec11d = iscc_make_nng_clusters_from_seeds(cl11d, &scc_ut_test_data_small_struct,
	                                                      &sr11d, &nng11d, false,
	                                                      SCC_UM_CLOSEST_ASSIGNED, true, 0.5,
	                                                      main_data_points, SCC_UM_CLOSEST_SEED, true, 0.75);
	const scc_Clabel ref_cluster_label11d[15] = { 1, 1, 2, M, 2,   M, 1, 1, 2, M,   2, 0, 2, 0, 0 };
	assert_int_equal(ec11d, SCC_ER_OK);
	assert_int_equal(cl11d->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl11d->num_data_points, 15);
	assert_int_equal(cl11d->num_clusters, 3);
	assert_non_null(cl11d->cluster_label);
	assert_ptr_equal(cl11d->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label11d, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl11d->cluster_label, ref_cluster_label11d, 15 * sizeof(scc_Clabel));
	assert_true(cl11d->external_labels);
	scc_free_clustering(&cl11d);
	iscc_free_digraph(&nng11d);


	scc_Clustering* cl12;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl12), SCC_ER_OK);
	iscc_Digraph nng12;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..#.. ...#. ...../"
	                         "..... ..... ...##/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... .#.../", &nng12);
	iscc_Dpid fp_seeds12[4] = {11, 0, 4};
	iscc_SeedResult sr12 = {
		.capacity = 3,
		.count = 3,
		.seeds = fp_seeds12,
	};
	scc_ErrorCode ec12 = iscc_make_nng_clusters_from_seeds(cl12, &scc_ut_test_data_small_struct,
	                                                      &sr12, &nng12, false,
	                                                      SCC_UM_CLOSEST_SEED, false, 0.0,
	                                                      main_data_points, SCC_UM_IGNORE, false, 0.0);
	const scc_Clabel ref_cluster_label12[15] = { 1, 1, 2, 0, 2,   M, 1, M, 2, M,   2, 0, 2, 0, 0 };
	assert_int_equal(ec12, SCC_ER_OK);
	assert_int_equal(cl12->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl12->num_data_points, 15);
	assert_int_equal(cl12->num_clusters, 3);
	assert_non_null(cl12->cluster_label);
	assert_ptr_equal(cl12->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label12, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl12->cluster_label, ref_cluster_label12, 15 * sizeof(scc_Clabel));
	assert_true(cl12->external_labels);
	scc_free_clustering(&cl12);
	iscc_free_digraph(&nng12);


	scc_Clustering* cl12a;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl12a), SCC_ER_OK);
	iscc_Digraph nng12a;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..#.. ...#. ...../"
	                         "..... ..... ...##/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... .#.../", &nng12a);
	iscc_Dpid fp_seeds12a[4] = {11, 0, 4};
	iscc_SeedResult sr12a = {
		.capacity = 3,
		.count = 3,
		.seeds = fp_seeds12a,
	};
	scc_ErrorCode ec12a = iscc_make_nng_clusters_from_seeds(cl12a, &scc_ut_test_data_small_struct,
	                                                      &sr12a, &nng12a, false,
	                                                      SCC_UM_CLOSEST_SEED, false, 0.0,
	                                                      main_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	const scc_Clabel ref_cluster_label12a[15] = { 1, 1, 2, 0, 2,   0, 1, 1, 2, 2,   2, 0, 2, 0, 0 };
	assert_int_equal(ec12a, SCC_ER_OK);
	assert_int_equal(cl12a->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl12a->num_data_points, 15);
	assert_int_equal(cl12a->num_clusters, 3);
	assert_non_null(cl12a->cluster_label);
	assert_ptr_equal(cl12a->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label12a, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl12a->cluster_label, ref_cluster_label12a, 15 * sizeof(scc_Clabel));
	assert_true(cl12a->external_labels);
	scc_free_clustering(&cl12a);
	iscc_free_digraph(&nng12a);


	scc_Clustering* cl12b;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl12b), SCC_ER_OK);
	iscc_Digraph nng12b;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..#.. ...#. ...../"
	                         "..... ..... ...##/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... .#.../", &nng12b);
	iscc_Dpid fp_seeds12b[4] = {11, 0, 4};
	iscc_SeedResult sr12b = {
		.capacity = 3,
		.count = 3,
		.seeds = fp_seeds12b,
	};
	scc_ErrorCode ec12b = iscc_make_nng_clusters_from_seeds(cl12b, &scc_ut_test_data_small_struct,
	                                                      &sr12b, &nng12b, false,
	                                                      SCC_UM_CLOSEST_SEED, false, 0.0,
	                                                      main_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 0.75);
	const scc_Clabel ref_cluster_label12b[15] = { 1, 1, 2, 0, 2,   M, 1, 1, 2, 2,   2, 0, 2, 0, 0 };
	assert_int_equal(ec12b, SCC_ER_OK);
	assert_int_equal(cl12b->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl12b->num_data_points, 15);
	assert_int_equal(cl12b->num_clusters, 3);
	assert_non_null(cl12b->cluster_label);
	assert_ptr_equal(cl12b->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label12b, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl12b->cluster_label, ref_cluster_label12b, 15 * sizeof(scc_Clabel));
	assert_true(cl12b->external_labels);
	scc_free_clustering(&cl12b);
	iscc_free_digraph(&nng12b);


	scc_Clustering* cl12c;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl12c), SCC_ER_OK);
	iscc_Digraph nng12c;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..#.. ...#. ...../"
	                         "..... ..... ...##/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... .#.../", &nng12c);
	iscc_Dpid fp_seeds12c[4] = {11, 0, 4};
	iscc_SeedResult sr12c = {
		.capacity = 3,
		.count = 3,
		.seeds = fp_seeds12c,
	};
	scc_ErrorCode ec12c = iscc_make_nng_clusters_from_seeds(cl12c, &scc_ut_test_data_small_struct,
	                                                      &sr12c, &nng12c, false,
	                                                      SCC_UM_CLOSEST_SEED, false, 0.0,
	                                                      main_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	const scc_Clabel ref_cluster_label12c[15] = { 1, 1, 2, 0, 2,   0, 1, 1, 2, 2,   2, 0, 2, 0, 0 };
	assert_int_equal(ec12c, SCC_ER_OK);
	assert_int_equal(cl12c->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl12c->num_data_points, 15);
	assert_int_equal(cl12c->num_clusters, 3);
	assert_non_null(cl12c->cluster_label);
	assert_ptr_equal(cl12c->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label12c, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl12c->cluster_label, ref_cluster_label12c, 15 * sizeof(scc_Clabel));
	assert_true(cl12c->external_labels);
	scc_free_clustering(&cl12c);
	iscc_free_digraph(&nng12c);


	scc_Clustering* cl12d;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl12d), SCC_ER_OK);
	iscc_Digraph nng12d;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..#.. ...#. ...../"
	                         "..... ..... ...##/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... .#.../", &nng12d);
	iscc_Dpid fp_seeds12d[4] = {11, 0, 4};
	iscc_SeedResult sr12d = {
		.capacity = 3,
		.count = 3,
		.seeds = fp_seeds12d,
	};
	scc_ErrorCode ec12d = iscc_make_nng_clusters_from_seeds(cl12d, &scc_ut_test_data_small_struct,
	                                                      &sr12d, &nng12d, false,
	                                                      SCC_UM_CLOSEST_SEED, false, 0.0,
	                                                      main_data_points, SCC_UM_CLOSEST_SEED, true, 0.75);
	const scc_Clabel ref_cluster_label12d[15] = { 1, 1, 2, 0, 2,   M, 1, 1, 2, M,   2, 0, 2, 0, 0 };
	assert_int_equal(ec12d, SCC_ER_OK);
	assert_int_equal(cl12d->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl12d->num_data_points, 15);
	assert_int_equal(cl12d->num_clusters, 3);
	assert_non_null(cl12d->cluster_label);
	assert_ptr_equal(cl12d->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label12d, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl12d->cluster_label, ref_cluster_label12d, 15 * sizeof(scc_Clabel));
	assert_true(cl12d->external_labels);
	scc_free_clustering(&cl12d);
	iscc_free_digraph(&nng12d);


	scc_Clustering* cl13;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl13), SCC_ER_OK);
	iscc_Digraph nng13;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..#.. ...#. ...../"
	                         "..... ..... ...##/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... .#.../", &nng13);
	iscc_Dpid fp_seeds13[4] = {11, 0, 4};
	iscc_SeedResult sr13 = {
		.capacity = 3,
		.count = 3,
		.seeds = fp_seeds13,
	};
	scc_ErrorCode ec13 = iscc_make_nng_clusters_from_seeds(cl13, &scc_ut_test_data_small_struct,
	                                                      &sr13, &nng13, false,
	                                                      SCC_UM_CLOSEST_SEED, true, 0.5,
	                                                      main_data_points, SCC_UM_IGNORE, false, 0.0);
	const scc_Clabel ref_cluster_label13[15] = { 1, 1, M, M, 2,   M, 1, M, 2, M,   2, 0, 2, 0, 0 };
	assert_int_equal(ec13, SCC_ER_OK);
	assert_int_equal(cl13->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl13->num_data_points, 15);
	assert_int_equal(cl13->num_clusters, 3);
	assert_non_null(cl13->cluster_label);
	assert_ptr_equal(cl13->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label13, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl13->cluster_label, ref_cluster_label13, 15 * sizeof(scc_Clabel));
	assert_true(cl13->external_labels);
	scc_free_clustering(&cl13);
	iscc_free_digraph(&nng13);


	scc_Clustering* cl13a;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl13a), SCC_ER_OK);
	iscc_Digraph nng13a;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..#.. ...#. ...../"
	                         "..... ..... ...##/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... .#.../", &nng13a);
	iscc_Dpid fp_seeds13a[4] = {11, 0, 4};
	iscc_SeedResult sr13a = {
		.capacity = 3,
		.count = 3,
		.seeds = fp_seeds13a,
	};
	scc_ErrorCode ec13a = iscc_make_nng_clusters_from_seeds(cl13a, &scc_ut_test_data_small_struct,
	                                                      &sr13a, &nng13a, false,
	                                                      SCC_UM_CLOSEST_SEED, true, 0.5,
	                                                      main_data_points, SCC_UM_CLOSEST_ASSIGNED, false, 0.0);
	const scc_Clabel ref_cluster_label13a[15] = { 1, 1, M, M, 2,   0, 1, 1, 2, 2,   2, 0, 2, 0, 0 };
	assert_int_equal(ec13a, SCC_ER_OK);
	assert_int_equal(cl13a->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl13a->num_data_points, 15);
	assert_int_equal(cl13a->num_clusters, 3);
	assert_non_null(cl13a->cluster_label);
	assert_ptr_equal(cl13a->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label13a, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl13a->cluster_label, ref_cluster_label13a, 15 * sizeof(scc_Clabel));
	assert_true(cl13a->external_labels);
	scc_free_clustering(&cl13a);
	iscc_free_digraph(&nng13a);


	scc_Clustering* cl13b;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl13b), SCC_ER_OK);
	iscc_Digraph nng13b;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..#.. ...#. ...../"
	                         "..... ..... ...##/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... .#.../", &nng13b);
	iscc_Dpid fp_seeds13b[4] = {11, 0, 4};
	iscc_SeedResult sr13b = {
		.capacity = 3,
		.count = 3,
		.seeds = fp_seeds13b,
	};
	scc_ErrorCode ec13b = iscc_make_nng_clusters_from_seeds(cl13b, &scc_ut_test_data_small_struct,
	                                                      &sr13b, &nng13b, false,
	                                                      SCC_UM_CLOSEST_SEED, true, 0.5,
	                                                      main_data_points, SCC_UM_CLOSEST_ASSIGNED, true, 0.75);
	const scc_Clabel ref_cluster_label13b[15] = { 1, 1, M, M, 2,   M, 1, 1, 2, 2,   2, 0, 2, 0, 0 };
	assert_int_equal(ec13b, SCC_ER_OK);
	assert_int_equal(cl13b->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl13b->num_data_points, 15);
	assert_int_equal(cl13b->num_clusters, 3);
	assert_non_null(cl13b->cluster_label);
	assert_ptr_equal(cl13b->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label13b, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl13b->cluster_label, ref_cluster_label13b, 15 * sizeof(scc_Clabel));
	assert_true(cl13b->external_labels);
	scc_free_clustering(&cl13b);
	iscc_free_digraph(&nng13b);


	scc_Clustering* cl13c;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl13c), SCC_ER_OK);
	iscc_Digraph nng13c;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..#.. ...#. ...../"
	                         "..... ..... ...##/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... .#.../", &nng13c);
	iscc_Dpid fp_seeds13c[4] = {11, 0, 4};
	iscc_SeedResult sr13c = {
		.capacity = 3,
		.count = 3,
		.seeds = fp_seeds13c,
	};
	scc_ErrorCode ec13c = iscc_make_nng_clusters_from_seeds(cl13c, &scc_ut_test_data_small_struct,
	                                                      &sr13c, &nng13c, false,
	                                                      SCC_UM_CLOSEST_SEED, true, 0.5,
	                                                      main_data_points, SCC_UM_CLOSEST_SEED, false, 0.0);
	const scc_Clabel ref_cluster_label13c[15] = { 1, 1, M, M, 2,   0, 1, 1, 2, 2,   2, 0, 2, 0, 0 };
	assert_int_equal(ec13c, SCC_ER_OK);
	assert_int_equal(cl13c->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl13c->num_data_points, 15);
	assert_int_equal(cl13c->num_clusters, 3);
	assert_non_null(cl13c->cluster_label);
	assert_ptr_equal(cl13c->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label13c, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl13c->cluster_label, ref_cluster_label13c, 15 * sizeof(scc_Clabel));
	assert_true(cl13c->external_labels);
	scc_free_clustering(&cl13c);
	iscc_free_digraph(&nng13c);


	scc_Clustering* cl13d;
	assert_int_equal(scc_init_empty_clustering(15, external_cluster_labels, &cl13d), SCC_ER_OK);
	iscc_Digraph nng13d;
	iscc_digraph_from_string(".#... .#... ...../"
	                         "....# ...#. ...../"
	                         "#.... .#... ...../"
	                         "..... ..... #..#./"
	                         "..... ...#. ..#../"

	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"
	                         "..... ..... ...../"

	                         "..#.. ...#. ...../"
	                         "..... ..... ...##/"
	                         "..... ..#.. #..../"
	                         "....# ..#.. ...../"
	                         "..... #.... .#.../", &nng13d);
	iscc_Dpid fp_seeds13d[4] = {11, 0, 4};
	iscc_SeedResult sr13d = {
		.capacity = 3,
		.count = 3,
		.seeds = fp_seeds13d,
	};
	scc_ErrorCode ec13d = iscc_make_nng_clusters_from_seeds(cl13d, &scc_ut_test_data_small_struct,
	                                                      &sr13d, &nng13d, false,
	                                                      SCC_UM_CLOSEST_SEED, true, 0.5,
	                                                      main_data_points, SCC_UM_CLOSEST_SEED, true, 0.75);
	const scc_Clabel ref_cluster_label13d[15] = { 1, 1, M, M, 2,   M, 1, 1, 2, M,   2, 0, 2, 0, 0 };
	assert_int_equal(ec13d, SCC_ER_OK);
	assert_int_equal(cl13d->clustering_version, ISCC_CURRENT_CLUSTSTRUCT_VERSION);
	assert_int_equal(cl13d->num_data_points, 15);
	assert_int_equal(cl13d->num_clusters, 3);
	assert_non_null(cl13d->cluster_label);
	assert_ptr_equal(cl13d->cluster_label, external_cluster_labels);
	assert_memory_equal(external_cluster_labels, ref_cluster_label13d, 15 * sizeof(scc_Clabel));
	assert_memory_equal(cl13d->cluster_label, ref_cluster_label13d, 15 * sizeof(scc_Clabel));
	assert_true(cl13d->external_labels);
	scc_free_clustering(&cl13d);
	iscc_free_digraph(&nng13d);
}


int main(void)
{
	const struct CMUnitTest test_cases[] = {
		cmocka_unit_test(scc_ut_get_nng_with_size_constraint),
		cmocka_unit_test(scc_ut_get_nng_with_type_constraint),
		cmocka_unit_test(scc_ut_estimate_avg_seed_dist),
		cmocka_unit_test(scc_ut_make_nng_clusters_from_seeds),
	};
	
	return cmocka_run_group_tests_name("nng_core.c", test_cases, NULL, NULL);
}
