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


#include "test_suite.h"
#include "assert_digraph.h"

#include "../example/nng.h"

#include <stdbool.h>
#include "../include/config.h"
#include "../include/digraph.h"
#include "../include/digraph_debug.h"


double coords[20] = { 88.14, 15.83,
                      57.89, 98.98,
                      21.36, 74.42,
                      56.68,  6.05,
                      87.06,  0.71,
                      49.03, 35.05,
                      70.37,  6.43,
                      18.49, 12.50,
                      38.34, 98.15,
                      57.89, 98.98 };

Matrix data_points = {
	.cols = 2, 
	.rows = 10, 
	.elements = coords,
};

/* Distance matrix:

          0         1        2        3         4        5        6        7         8        9
0   0.00000  88.48155 88.83894 32.94511  15.15852 43.57752 20.10306 69.72956  96.21134  88.48155
1  88.48155   0.00000 44.01857 92.93788 102.50796 64.54103 93.38765 95.03237  19.56761   0.00000
2  88.83894  44.01857  0.00000 76.95427  98.74034 48.12095 83.81301 61.98648  29.17933  44.01857
3  32.94511  92.93788 76.95427  0.00000  30.84575 29.99204 13.69527 38.73085  93.90828  92.93788
4  15.15852 102.50796 98.74034 30.84575   0.00000 51.23979 17.64297 69.57621 108.94123 102.50796
5  43.57752  64.54103 48.12095 29.99204  51.23979  0.00000 35.70014 37.96306  63.99911  64.54103
6  20.10306  93.38765 83.81301 13.69527  17.64297 35.70014  0.00000 52.23389  97.15184  93.38765
7  69.72956  95.03237 61.98648 38.73085  69.57621 37.96306 52.23389  0.00000  87.92011  95.03237
8  96.21134  19.56761 29.17933 93.90828 108.94123 63.99911 97.15184 87.92011   0.00000  19.56761
9  88.48155   0.00000 44.01857 92.93788 102.50796 64.54103 93.38765 95.03237  19.56761   0.00000
*/


void scc_ut_get_nng(void** state)
{
	(void) state;

	/* 2-nng in [0,1,2,3,4,5]:

	0: 4 (15.15852)		3 (32.94511)
	2: 1 (44.01857)		5 (48.12095)
	4: 0 (15.15852)		3 (30.84575)
	6: 3 (13.69527)		4 (17.64297)
	8: 1 (19.56761)		2 (29.17933)
	*/

	bool queries[10] = { true, false, true, false, true,
	                     false, true, false, true, false };

	scc_Vid search_indices[6] = { 5, 4, 3, 2, 1, 0 };

	scc_Digraph nng1a = get_nng(&data_points, queries,
	                            6, search_indices,
	                            1, false, 0.0, true);
	scc_Digraph nng1b = get_nng(&data_points, queries,
	                            6, search_indices,
	                            1, false, 0.0, false);
	scc_Digraph ref1 = scc_digraph_from_string("....#...../"
	                                           "........../"
	                                           ".#......../"
	                                           "........../"
	                                           "#........./"
	                                           "........../"
	                                           "...#....../"
	                                           "........../"
	                                           ".#......../"
	                                           "........../");
	assert_equal_digraph(&nng1a, &ref1);
	assert_equal_digraph(&nng1b, &ref1);

	scc_Digraph nng2a = get_nng(&data_points, queries,
	                            6, search_indices,
	                            2, false, 0.0, true);
	scc_Digraph nng2b = get_nng(&data_points, queries,
	                            6, search_indices,
	                            2, false, 0.0, false);
	scc_Digraph ref2 = scc_digraph_from_string("...##...../"
	                                           "........../"
	                                           ".#...#..../"
	                                           "........../"
	                                           "#..#....../"
	                                           "........../"
	                                           "...##...../"
	                                           "........../"
	                                           ".##......./"
	                                           "........../");
	assert_equal_digraph(&nng2a, &ref2);
	assert_equal_digraph(&nng2b, &ref2);

	scc_Digraph nng3 = get_nng(&data_points, queries,
	                           6, search_indices,
	                           2, true, 25.0, true);
	scc_Digraph ref3 = scc_digraph_from_string("....#...../"
	                                           "........../"
	                                           "........../"
	                                           "........../"
	                                           "#........./"
	                                           "........../"
	                                           "...##...../"
	                                           "........../"
	                                           ".#......../"
	                                           "........../");
	assert_equal_digraph(&nng3, &ref3);

	scc_Digraph nng4 = get_nng(&data_points, queries,
	                           6, search_indices,
	                           2, true, 25.0, false);
	scc_Digraph ref4 = scc_digraph_from_string("........../"
	                                           "........../"
	                                           "........../"
	                                           "........../"
	                                           "........../"
	                                           "........../"
	                                           "...##...../"
	                                           "........../"
	                                           "........../"
	                                           "........../");
	assert_equal_digraph(&nng4, &ref4);

	scc_free_digraph(&nng1a);
	scc_free_digraph(&nng1b);
	scc_free_digraph(&nng2a);
	scc_free_digraph(&nng2b);
	scc_free_digraph(&nng3);
	scc_free_digraph(&nng4);
	scc_free_digraph(&ref1);
	scc_free_digraph(&ref2);
	scc_free_digraph(&ref3);
	scc_free_digraph(&ref4);
}



void scc_ut_get_all_nng(void** state)
{
	(void) state;

	/* 2-nng in [0,1,2,3,4,5,6,7,8,9]:

	0: 4 (15.15852)		6 (20.10306)
	1: 9 (0.00000)		8 (19.56761)
	2: 8 (29.17933)		1 (44.01857)
	3: 6 (13.69527)		5 (29.99204)
	4: 0 (15.15852)		6 (17.64297)
	5: 3 (29.99204)		6 (35.70014)
	6: 3 (13.69527)		4 (17.64297)
	7: 5 (37.96306)		3 (38.73085)
	8: 1 (19.56761)		9 (19.56761)
	9: 1 (0.00000)		8 (19.56761)
	*/
	
	scc_Digraph nng1a = get_all_nng(&data_points, 1, false, 0.0, true);
	scc_Digraph nng1b = get_all_nng(&data_points, 1, false, 0.0, false);
	scc_Digraph ref1 = scc_digraph_from_string("....# ...../"
	                                           "..... ....#/"
	                                           "..... ...#./"
	                                           "..... .#.../"
	                                           "#.... ...../"
	                                           "...#. ...../"
	                                           "...#. ...../"
	                                           "..... #..../"
	                                           ".#... ...../"
	                                           ".#... ...../");
	assert_equal_digraph(&nng1a, &ref1);
	assert_equal_digraph(&nng1b, &ref1);

	scc_Digraph nng2a = get_all_nng(&data_points, 2, false, 0.0, true);
	scc_Digraph nng2b = get_all_nng(&data_points, 2, false, 0.0, false);
	scc_Digraph ref2 = scc_digraph_from_string("....# .#.../"
	                                           "..... ...##/"
	                                           ".#... ...#./"
	                                           "..... ##.../"
	                                           "#.... .#.../"
	                                           "...#. .#.../"
	                                           "...## ...../"
	                                           "...#. #..../"
	                                           ".#... ....#/"
	                                           ".#... ...#./");
	assert_equal_digraph(&nng2a, &ref2);
	assert_equal_digraph(&nng2b, &ref2);

	scc_Digraph nng3 = get_all_nng(&data_points, 2, true, 20.0, true);
	scc_Digraph ref3 = scc_digraph_from_string("....# ...../"
	                                           "..... ...##/"
	                                           "..... ...../"
	                                           "..... .#.../"
	                                           "#.... .#.../"
	                                           "..... ...../"
	                                           "...## ...../"
	                                           "..... ...../"
	                                           ".#... ....#/"
	                                           ".#... ...#./");
	assert_equal_digraph(&nng3, &ref3);

	scc_Digraph nng4 = get_all_nng(&data_points, 2, true, 20.0, false);
	scc_Digraph ref4 = scc_digraph_from_string("..... ...../"
	                                           "..... ...##/"
	                                           "..... ...../"
	                                           "..... ...../"
	                                           "#.... .#.../"
	                                           "..... ...../"
	                                           "...## ...../"
	                                           "..... ...../"
	                                           ".#... ....#/"
	                                           ".#... ...#./");
	assert_equal_digraph(&nng4, &ref4);

	scc_free_digraph(&nng1a);
	scc_free_digraph(&nng1b);
	scc_free_digraph(&nng2a);
	scc_free_digraph(&nng2b);
	scc_free_digraph(&nng3);
	scc_free_digraph(&nng4);
	scc_free_digraph(&ref1);
	scc_free_digraph(&ref2);
	scc_free_digraph(&ref3);
	scc_free_digraph(&ref4);
}


int main(void)
{
	const struct CMUnitTest test_nng[] = {
		cmocka_unit_test(scc_ut_get_nng),
		cmocka_unit_test(scc_ut_get_all_nng),
	};

	return cmocka_run_group_tests_name("example nng module", test_nng, NULL, NULL);
}
