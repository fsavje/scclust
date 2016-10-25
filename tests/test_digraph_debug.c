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
#include "assert_digraph.h"
#include "../src/digraph_debug.h"

#include <stddef.h>
#include "../src/digraph_core.h"
#include "../src/scclust_int.h"


void scc_ut_is_balanced_digraph(void** state)
{
	(void) state;

	iscc_Dpid head1[4] = {0, 1, 2, 3};
	iscc_Arci tail_ptr1[5] = {0, 1, 2, 3, 4};
	iscc_Digraph dg1 = {
		.vertices = 4,
		.max_arcs = 4,
		.head = head1,
		.tail_ptr = tail_ptr1,
	};

	iscc_Dpid head2[4] = {0, 1, 2, 3};
	iscc_Arci tail_ptr2[5] = {0, 2, 2, 3, 4};
	iscc_Digraph dg2 = {
		.vertices = 4,
		.max_arcs = 4,
		.head = head2,
		.tail_ptr = tail_ptr2,
	};

	iscc_Dpid head3[4] = {123, 124, 125, 126};
	iscc_Arci tail_ptr3[5] = {0, 0, 0, 0, 0};
	iscc_Digraph dg3 = {
		.vertices = 4,
		.max_arcs = 4,
		.head = head3,
		.tail_ptr = tail_ptr3,
	};

	assert_true(iscc_is_balanced_digraph(&dg1, 1));
	assert_false(iscc_is_balanced_digraph(&dg1, 2));
	assert_false(iscc_is_balanced_digraph(&dg2, 1));
	assert_false(iscc_is_balanced_digraph(&dg2, 2));
	assert_true(iscc_is_balanced_digraph(&dg3, 0));
	assert_false(iscc_is_balanced_digraph(&dg3, 1));
}


void scc_ut_digraphs_equal(void** state)
{
	(void) state;

	iscc_Dpid head1[4] = {1, 0, 2, 2};
	iscc_Arci tail_ptr1[5] = {0, 2, 2, 4, 4};
	iscc_Digraph dg1 = {
		.vertices = 4,
		.max_arcs = 4,
		.head = head1,
		.tail_ptr = tail_ptr1,
	};

	iscc_Dpid head2[3] = {0, 1, 2};
	iscc_Arci tail_ptr2[5] = {0, 2, 2, 3, 3};
	iscc_Digraph dg2 = {
		.vertices = 4,
		.max_arcs = 3,
		.head = head2,
		.tail_ptr = tail_ptr2,
	};

	iscc_Dpid head3[3] = {0, 3, 2};
	iscc_Arci tail_ptr3[5] = {0, 2, 2, 3, 3};
	iscc_Digraph dg3 = {
		.vertices = 4,
		.max_arcs = 3,
		.head = head3,
		.tail_ptr = tail_ptr3,
	};

	iscc_Dpid head4[4] = {1, 0, 2, 3};
	iscc_Arci tail_ptr4[5] = {0, 2, 2, 4, 4};
	iscc_Digraph dg4 = {
		.vertices = 4,
		.max_arcs = 4,
		.head = head4,
		.tail_ptr = tail_ptr4,
	};

	iscc_Dpid head5[3] = {0, 1, 2};
	iscc_Arci tail_ptr5[4] = {0, 2, 2, 3};
	iscc_Digraph dg5 = {
		.vertices = 3,
		.max_arcs = 3,
		.head = head5,
		.tail_ptr = tail_ptr5,
	};

	iscc_Arci tail_ptr6[5] = {0, 0, 0, 0, 0};
	iscc_Digraph dg6 = {
		.vertices = 4,
		.max_arcs = 0,
		.head = NULL,
		.tail_ptr = tail_ptr6,
	};

	iscc_Dpid head7[1] = {0};
	iscc_Arci tail_ptr7[5] = {0, 0, 0, 0, 0};
	iscc_Digraph dg7 = {
		.vertices = 4,
		.max_arcs = 1,
		.head = head7,
		.tail_ptr = tail_ptr7,
	};

	assert_true(iscc_digraphs_equal(&dg1, &dg1));

	assert_true(iscc_digraphs_equal(&dg1, &dg2));
	assert_true(iscc_digraphs_equal(&dg2, &dg1));

	assert_true(iscc_digraphs_equal(&dg6, &dg7));
	assert_true(iscc_digraphs_equal(&dg7, &dg6));

	assert_false(iscc_digraphs_equal(&dg1, &dg3));
	assert_false(iscc_digraphs_equal(&dg3, &dg1));

	assert_false(iscc_digraphs_equal(&dg1, &dg4));
	assert_false(iscc_digraphs_equal(&dg4, &dg1));

	assert_false(iscc_digraphs_equal(&dg1, &dg5));
	assert_false(iscc_digraphs_equal(&dg5, &dg1));

	assert_false(iscc_digraphs_equal(&dg1, &dg6));
	assert_false(iscc_digraphs_equal(&dg6, &dg1));

	assert_false(iscc_digraphs_equal(&dg1, &dg7));
	assert_false(iscc_digraphs_equal(&dg7, &dg1));
}


void scc_ut_digraph_from_pieces(void** state)
{
	(void) state;

	const iscc_Dpid head[6] = { 1, 0, 2, 4, 6, 2 };
	const iscc_Arci tail_ptr[5] = { 0, 2, 2, 3, 3 };

	iscc_Digraph dg;
	scc_ErrorCode ec = iscc_digraph_from_pieces(4, 3, tail_ptr, head, &dg);
	assert_int_equal(dg.vertices, 4);
	assert_int_equal(dg.max_arcs, 3);
	assert_non_null(dg.head);
	assert_ptr_not_equal(dg.head, head);
	assert_memory_equal(dg.head, head, 3 * sizeof(iscc_Dpid));
	assert_non_null(dg.tail_ptr);
	assert_ptr_not_equal(dg.tail_ptr, tail_ptr);
	assert_memory_equal(dg.tail_ptr, tail_ptr, 5 * sizeof(iscc_Arci));
	assert_int_equal(ec, SCC_ER_OK);

	iscc_free_digraph(&dg);
}


void scc_ut_digraph_from_string(void** state)
{
	(void) state;

	iscc_Dpid headA[7] = {0,1,1,1,3,2,3};
	iscc_Arci tail_ptrA[5] = {0,2,3,5,7};

	iscc_Digraph ut_dg1;
	scc_ErrorCode ec1 = iscc_digraph_from_pieces(4, 7, tail_ptrA, headA, &ut_dg1);
	iscc_Digraph ut_dg2;
	scc_ErrorCode ec2 = iscc_digraph_from_string("##../.#../.#.#/..##/", &ut_dg2);
	iscc_Digraph ut_dg3;
	scc_ErrorCode ec3 = iscc_digraph_from_string("##../.#../.#.#/..##", &ut_dg3);

	iscc_Arci tail_ptrB[5] = {0,0,0,0,0};
	iscc_Digraph ut_dg4;
	scc_ErrorCode ec4 = iscc_digraph_from_pieces(4, 7, tail_ptrB, headA, &ut_dg4);
	iscc_Digraph ut_dg5;
	scc_ErrorCode ec5 = iscc_digraph_from_string("..../..../..../....", &ut_dg5);
	iscc_Digraph empty_dg1;
	iscc_empty_digraph(4, 0, &empty_dg1);

	iscc_Dpid headC[4] = {0,1,2,3};
	iscc_Arci tail_ptrC[5] = {0,1,2,3,4};
	iscc_Digraph ut_dg6;
	scc_ErrorCode ec6 = iscc_digraph_from_pieces(4, 4, tail_ptrC, headC, &ut_dg6);
	iscc_Digraph ut_dg7;
	scc_ErrorCode ec7 = iscc_digraph_from_string("#.../.#../..#./...#", &ut_dg7);
	iscc_Digraph ut_dg8;
	scc_ErrorCode ec8 = iscc_digraph_from_string("a#.../e fdgtthhh.#..t/2..#.5/7..   .#", &ut_dg8);

	assert_int_equal(ec1, SCC_ER_OK);
	assert_int_equal(ec2, SCC_ER_OK);
	assert_int_equal(ec3, SCC_ER_OK);
	assert_int_equal(ec4, SCC_ER_OK);
	assert_int_equal(ec5, SCC_ER_OK);
	assert_int_equal(ec6, SCC_ER_OK);
	assert_int_equal(ec7, SCC_ER_OK);
	assert_int_equal(ec8, SCC_ER_OK);

	assert_valid_digraph(&ut_dg1, 4);
	assert_valid_digraph(&ut_dg2, 4);
	assert_valid_digraph(&ut_dg3, 4);
	assert_valid_digraph(&ut_dg4, 4);
	assert_valid_digraph(&ut_dg5, 4);
	assert_valid_digraph(&ut_dg6, 4);
	assert_valid_digraph(&ut_dg7, 4);
	assert_valid_digraph(&ut_dg8, 4);

	assert_int_equal(ut_dg1.max_arcs, 7);
	assert_int_equal(ut_dg2.max_arcs, 7);
	assert_int_equal(ut_dg3.max_arcs, 7);
	assert_int_equal(ut_dg4.max_arcs, 7);
	assert_int_equal(ut_dg5.max_arcs, 0);
	assert_int_equal(ut_dg6.max_arcs, 4);
	assert_int_equal(ut_dg7.max_arcs, 4);
	assert_int_equal(ut_dg8.max_arcs, 4);

	assert_memory_equal(ut_dg1.tail_ptr, &tail_ptrA, 5 * sizeof(iscc_Arci));
	assert_memory_equal(ut_dg2.tail_ptr, &tail_ptrA, 5 * sizeof(iscc_Arci));
	assert_memory_equal(ut_dg3.tail_ptr, &tail_ptrA, 5 * sizeof(iscc_Arci));
	assert_memory_equal(ut_dg4.tail_ptr, &tail_ptrB, 5 * sizeof(iscc_Arci));
	assert_memory_equal(ut_dg5.tail_ptr, &tail_ptrB, 5 * sizeof(iscc_Arci));
	assert_memory_equal(ut_dg6.tail_ptr, &tail_ptrC, 5 * sizeof(iscc_Arci));
	assert_memory_equal(ut_dg7.tail_ptr, &tail_ptrC, 5 * sizeof(iscc_Arci));
	assert_memory_equal(ut_dg8.tail_ptr, &tail_ptrC, 5 * sizeof(iscc_Arci));

	assert_memory_equal(ut_dg1.head, &headA, ut_dg1.max_arcs * sizeof(iscc_Dpid));
	assert_memory_equal(ut_dg2.head, &headA, ut_dg2.max_arcs * sizeof(iscc_Dpid));
	assert_memory_equal(ut_dg3.head, &headA, ut_dg3.max_arcs * sizeof(iscc_Dpid));
	assert_memory_equal(ut_dg4.head, &headA, ut_dg4.max_arcs * sizeof(iscc_Dpid));
	assert_null(ut_dg5.head);
	assert_memory_equal(ut_dg6.head, &headC, ut_dg6.max_arcs * sizeof(iscc_Dpid));
	assert_memory_equal(ut_dg7.head, &headC, ut_dg7.max_arcs * sizeof(iscc_Dpid));
	assert_memory_equal(ut_dg8.head, &headC, ut_dg8.max_arcs * sizeof(iscc_Dpid));

	assert_true(iscc_digraphs_equal(&ut_dg1, &ut_dg2));
	assert_true(iscc_digraphs_equal(&ut_dg1, &ut_dg3));
	assert_true(iscc_digraphs_equal(&ut_dg4, &ut_dg5));
	assert_true(iscc_digraphs_equal(&ut_dg4, &empty_dg1));
	assert_true(iscc_digraphs_equal(&ut_dg6, &ut_dg7));
	assert_true(iscc_digraphs_equal(&ut_dg6, &ut_dg8));

	assert_false(iscc_digraphs_equal(&ut_dg1, &ut_dg5));

	assert_identical_digraph(&ut_dg1, &ut_dg2);
	assert_identical_digraph(&ut_dg1, &ut_dg3);
	assert_identical_digraph(&ut_dg6, &ut_dg7);
	assert_identical_digraph(&ut_dg6, &ut_dg8);

	iscc_free_digraph(&ut_dg1);
	iscc_free_digraph(&ut_dg2);
	iscc_free_digraph(&ut_dg3);
	iscc_free_digraph(&ut_dg4);
	iscc_free_digraph(&ut_dg5);
	iscc_free_digraph(&ut_dg6);
	iscc_free_digraph(&ut_dg7);
	iscc_free_digraph(&ut_dg8);
	iscc_free_digraph(&empty_dg1);
}


void scc_ut_copy_digraph(void** state)
{
	(void) state;

	iscc_Digraph dg1;
	iscc_digraph_from_string("####/..#./####/#.../", &dg1);
	iscc_Digraph dg2;
	iscc_empty_digraph(2, 0, &dg2);

	iscc_Digraph res1;
	scc_ErrorCode ec1 = iscc_copy_digraph(&dg1, &res1);
	iscc_Digraph res2;
	scc_ErrorCode ec2 = iscc_copy_digraph(&dg2, &res2);

	assert_int_equal(ec1, SCC_ER_OK);
	assert_int_equal(ec2, SCC_ER_OK);

	assert_valid_digraph(&res1, 4);
	assert_valid_digraph(&res2, 2);

	assert_equal_digraph(&res1, &dg1);
	assert_equal_digraph(&res2, &dg2);

	assert_free_digraph(&dg1);
	assert_free_digraph(&dg2);
	assert_free_digraph(&res1);
	assert_free_digraph(&res2);
}


int main(void)
{
	const struct CMUnitTest test_cases[] = {
		cmocka_unit_test(scc_ut_is_balanced_digraph),
		cmocka_unit_test(scc_ut_digraphs_equal),
		cmocka_unit_test(scc_ut_digraph_from_pieces),
		cmocka_unit_test(scc_ut_digraph_from_string),
		cmocka_unit_test(scc_ut_copy_digraph),
	};

	return cmocka_run_group_tests_name("digraph_debug.c", test_cases, NULL, NULL);
}
