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

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "../src/findseeds.c"
#include "../include/config.h"
#include "../include/digraph.h"
#include "../include/digraph_debug.h"


#ifdef SCC_STABLE_CLUSTERING
    #error Please run this test without the SCC_STABLE_CLUSTERING flag
#endif


void scc_ut_exclusion_graph(void** state)
{
	(void) state;

	scc_Digraph nng = scc_digraph_from_string(".#..#............./"
	                                          "#...#............./"
	                                          "....#..#........../"
	                                          "#...#............./"
	                                          ".#.#............../"
	                                          "..#.....#........./"
	                                          "...#.....#......../"
	                                          "......#.#........./"
	                                          ".....#.....#....../"
	                                          "..........#.....#./"
	                                          ".......#.....#..../"
	                                          "........#.#......./"
	                                          "...............##./"
	                                          "..............#..#/"
	                                          ".............#...#/"
	                                          ".........#..#...../"
	                                          ".............##.../"
	                                          "..............#.#./");

	scc_Digraph exg = scc_digraph_from_string(".####............./"
	                                          "#.###............./"
	                                          "##.###.#..#......./"
	                                          "###.#.#.........../"
	                                          "####..#.........../"
	                                          "..#....##..#....../"
	                                          "...##..#.#.....#../"
	                                          "..#..##.#.##....../"
	                                          ".....#.#...#....../"
	                                          "......#...###..###/"
	                                          "..#....#.#.#.##.#./"
	                                          ".....#.####......./"
	                                          ".........#.....###/"
	                                          "..........#...#.##/"
	                                          "..........#..#..##/"
	                                          "......#..#..#...../"
	                                          ".........##.###..#/"
	                                          ".........#..###.#./");

	scc_Digraph exclusion_graph = iscc_exclusion_graph(&nng);
	assert_equal_digraph(&exg, &exclusion_graph);


	scc_Digraph nng2 = scc_digraph_from_string(".#..# ...../"
	                                           "..... ...../"
	                                           ".#... ..#../"
	                                           "..#.. #..../"
	                                           "..... ...../"
	                                           "#.... ...../"
	                                           "..#.. ...../"
	                                           "....# ...../"
	                                           "..... ..#../"
	                                           "..... ..##./");

	scc_Digraph exg2 = scc_digraph_from_string(".##.# #.#../"
	                                           "#.#.. ...../"
	                                           "##.#. .####/"
	                                           "..#.. ##.../"
	                                           "#.... ..#../"
	                                           "#..#. ...../"
	                                           "..##. ...../"
	                                           "#.#.# ...##/"
	                                           "..#.. ..#.#/"
	                                           "..#.. ..##./");

	scc_Digraph exclusion_graph2 = iscc_exclusion_graph(&nng2);
	assert_equal_digraph(&exg2, &exclusion_graph2);

	scc_free_digraph(&nng);
	scc_free_digraph(&exg);
	scc_free_digraph(&exclusion_graph);
	scc_free_digraph(&nng2);
	scc_free_digraph(&exg2);
	scc_free_digraph(&exclusion_graph2);
}


void scc_ut_exclusion_graph_withdiag(void** state)
{
	(void) state;

	scc_Digraph nng = scc_digraph_from_string("##..#............./"
	                                          "##..#............./"
	                                          "..#.#..#........../"
	                                          "#..##............./"
	                                          ".#.##............./"
	                                          "..#..#..#........./"
	                                          "...#..#..#......../"
	                                          "......###........./"
	                                          ".....#..#..#....../"
	                                          ".........##.....#./"
	                                          ".......#..#..#..../"
	                                          "........#.##....../"
	                                          "............#..##./"
	                                          ".............##..#/"
	                                          ".............##..#/"
	                                          ".........#..#..#../"
	                                          ".............##.#./"
	                                          "..............#.##/");

	scc_Digraph exg = scc_digraph_from_string(".####............./"
	                                          "#.###............./"
	                                          "##.###.#..#......./"
	                                          "###.#.#.........../"
	                                          "####..#.........../"
	                                          "..#....##..#....../"
	                                          "...##..#.#.....#../"
	                                          "..#..##.#.##....../"
	                                          ".....#.#...#....../"
	                                          "......#...###..###/"
	                                          "..#....#.#.#.##.#./"
	                                          ".....#.####......./"
	                                          ".........#.....###/"
	                                          "..........#...#.##/"
	                                          "..........#..#..##/"
	                                          "......#..#..#...../"
	                                          ".........##.###..#/"
	                                          ".........#..###.#./");

	scc_Digraph exclusion_graph = iscc_exclusion_graph(&nng);
	assert_equal_digraph(&exg, &exclusion_graph);


	scc_Digraph nng2 = scc_digraph_from_string("##..# ...../"
	                                           "..... ...../"
	                                           ".##.. ..#../"
	                                           "..##. #..../"
	                                           "..... ...../"
	                                           "#.... #..../"
	                                           "..#.. .#.../"
	                                           "....# ..#../"
	                                           "..... ..##./"
	                                           "..... ..###/");

	scc_Digraph exg2 = scc_digraph_from_string(".##.# #.#../"
	                                           "#.#.. ...../"
	                                           "##.#. .####/"
	                                           "..#.. ##.../"
	                                           "#.... ..#../"
	                                           "#..#. ...../"
	                                           "..##. ...../"
	                                           "#.#.# ...##/"
	                                           "..#.. ..#.#/"
	                                           "..#.. ..##./");

	scc_Digraph exclusion_graph2 = iscc_exclusion_graph(&nng2);
	assert_equal_digraph(&exg2, &exclusion_graph2);

	scc_free_digraph(&nng);
	scc_free_digraph(&exg);
	scc_free_digraph(&exclusion_graph);
	scc_free_digraph(&nng2);
	scc_free_digraph(&exg2);
	scc_free_digraph(&exclusion_graph2);
}


void scc_ut_fs_init_clustering(void** state)
{
	(void) state;

	scc_SeedClustering cl1 = iscc_fs_init_clustering(100, 500);

	assert_int_equal(cl1.vertices, 100);
	assert_int_equal(cl1.num_clusters, 0);
	assert_int_equal(cl1.seed_capacity, 500);
	assert_null(cl1.assigned);
	assert_non_null(cl1.seeds);
	assert_null(cl1.cluster_label);
	cl1.seeds[499] = 123;

	scc_SeedClustering cl2 = iscc_fs_init_clustering(10, 10);

	assert_int_equal(cl2.vertices, 10);
	assert_int_equal(cl2.num_clusters, 0);
	assert_int_equal(cl2.seed_capacity, 128);
	assert_null(cl2.assigned);
	assert_non_null(cl2.seeds);
	assert_null(cl2.cluster_label);
	cl2.seeds[127] = 123;

	scc_free_SeedClustering(&cl1);
	scc_free_SeedClustering(&cl2);
}


void scc_ut_fs_shrink_seed_array(void** state)
{
	(void) state;

	scc_SeedClustering cl = iscc_fs_init_clustering(100, 500);
	assert_int_equal(cl.vertices, 100);
	assert_int_equal(cl.num_clusters, 0);
	assert_int_equal(cl.seed_capacity, 500);
	assert_null(cl.assigned);
	assert_non_null(cl.seeds);
	assert_null(cl.cluster_label);

	iscc_fs_shrink_seed_array(&cl);
	assert_int_equal(cl.vertices, 100);
	assert_int_equal(cl.num_clusters, 0);
	assert_int_equal(cl.seed_capacity, 500);
	assert_null(cl.assigned);
	assert_non_null(cl.seeds);
	assert_null(cl.cluster_label);

	cl.num_clusters = 10;
	iscc_fs_shrink_seed_array(&cl);
	assert_int_equal(cl.vertices, 100);
	assert_int_equal(cl.num_clusters, 10);
	assert_int_equal(cl.seed_capacity, 10);
	assert_null(cl.assigned);
	assert_non_null(cl.seeds);
	assert_null(cl.cluster_label);

	scc_free_SeedClustering(&cl);
}


void scc_ut_fs_add_seed(void** state)
{
	(void) state;

	scc_SeedClustering cl = {
		.vertices = 10,
		.num_clusters = 0,
		.seed_capacity = 1,
		.external_labels = false,
		.assigned = NULL,
		.seeds = malloc(sizeof(scc_Vid[1])),
		.cluster_label = NULL,
	};

	scc_Vid ref_seeds[6] = {4, 1, 6, 3, 5, 7};

	assert_true(iscc_fs_add_seed(4, &cl));
	assert_true(iscc_fs_add_seed(1, &cl));
	assert_true(iscc_fs_add_seed(6, &cl));
	assert_true(iscc_fs_add_seed(3, &cl));
	assert_true(iscc_fs_add_seed(5, &cl));
	assert_true(iscc_fs_add_seed(7, &cl));

	assert_int_equal(cl.vertices, 10);
	assert_int_equal(cl.num_clusters, 6);
	assert_int_equal(cl.seed_capacity, 1025);
	assert_null(cl.assigned);
	assert_non_null(cl.seeds);
	assert_memory_equal(cl.seeds, ref_seeds, 6 * sizeof(scc_Vid));
	assert_null(cl.cluster_label);

	iscc_fs_shrink_seed_array(&cl);

	assert_int_equal(cl.vertices, 10);
	assert_int_equal(cl.num_clusters, 6);
	assert_int_equal(cl.seed_capacity, 6);
	assert_null(cl.assigned);
	assert_non_null(cl.seeds);
	assert_memory_equal(cl.seeds, ref_seeds, 6 * sizeof(scc_Vid));
	assert_null(cl.cluster_label);

	iscc_fs_shrink_seed_array(&cl);

	assert_int_equal(cl.vertices, 10);
	assert_int_equal(cl.num_clusters, 6);
	assert_int_equal(cl.seed_capacity, 6);
	assert_null(cl.assigned);
	assert_non_null(cl.seeds);
	assert_memory_equal(cl.seeds, ref_seeds, 6 * sizeof(scc_Vid));
	assert_null(cl.cluster_label);


	scc_SeedClustering cl2 = iscc_fs_init_clustering(99999, 1);

	for (scc_Vid s = 0; s < 10000; ++s) {
		assert_true(iscc_fs_add_seed(10000 - s, &cl2));
	}

	assert_int_equal(cl2.vertices, 99999);
	assert_int_equal(cl2.num_clusters, 10000);
	assert_int_equal(cl2.seed_capacity, 10780);
	assert_null(cl2.assigned);
	assert_non_null(cl2.seeds);
	for (scc_Vid s = 0; s < 10000; ++s) {
		assert_int_equal(cl2.seeds[s], 10000 - s);
	}
	assert_null(cl2.cluster_label);

	iscc_fs_shrink_seed_array(&cl2);

	assert_int_equal(cl2.vertices, 99999);
	assert_int_equal(cl2.num_clusters, 10000);
	assert_int_equal(cl2.seed_capacity, 10000);
	assert_null(cl2.assigned);
	assert_non_null(cl2.seeds);
	for (scc_Vid s = 0; s < 10000; ++s) {
		assert_int_equal(cl2.seeds[s], 10000 - s);
	}
	assert_null(cl2.cluster_label);
	
	iscc_fs_shrink_seed_array(&cl2);

	assert_int_equal(cl2.vertices, 99999);
	assert_int_equal(cl2.num_clusters, 10000);
	assert_int_equal(cl2.seed_capacity, 10000);
	assert_null(cl2.assigned);
	assert_non_null(cl2.seeds);
	for (scc_Vid s = 0; s < 10000; ++s) {
		assert_int_equal(cl2.seeds[s], 10000 - s);
	}
	assert_null(cl2.cluster_label);

	scc_free_SeedClustering(&cl);
	scc_free_SeedClustering(&cl2);
}


void scc_ut_fs_check_neighbors_marks(void** state)
{
	(void) state;

	scc_Digraph nng = scc_digraph_from_string("..##..#/"
	                                          "..#.#.#/"
	                                          "#...#.#/"
	                                          "..#..##/"
	                                          "......./"
	                                          "..#..../"
	                                          "......./");

	bool marks[7] = {true, false, false, false, true, false, false};

	assert_false(iscc_fs_check_neighbors_marks(0, &nng, marks));
	assert_false(iscc_fs_check_neighbors_marks(1, &nng, marks));
	assert_false(iscc_fs_check_neighbors_marks(2, &nng, marks));
	assert_true(iscc_fs_check_neighbors_marks(3, &nng, marks));
	assert_false(iscc_fs_check_neighbors_marks(4, &nng, marks));
	assert_true(iscc_fs_check_neighbors_marks(5, &nng, marks));
	assert_false(iscc_fs_check_neighbors_marks(6, &nng, marks));

	scc_free_digraph(&nng);
}


void scc_ut_fs_check_neighbors_marks_withdiag(void** state)
{
	(void) state;

	scc_Digraph nng = scc_digraph_from_string("#.##..#/"
	                                          ".##.#.#/"
	                                          "#.#.#.#/"
	                                          "..##.##/"
	                                          "....#../"
	                                          "..#..#./"
	                                          "......./");

	bool marks[7] = {true, false, false, false, true, false, false};

	assert_false(iscc_fs_check_neighbors_marks(0, &nng, marks));
	assert_false(iscc_fs_check_neighbors_marks(1, &nng, marks));
	assert_false(iscc_fs_check_neighbors_marks(2, &nng, marks));
	assert_true(iscc_fs_check_neighbors_marks(3, &nng, marks));
	assert_false(iscc_fs_check_neighbors_marks(4, &nng, marks));
	assert_true(iscc_fs_check_neighbors_marks(5, &nng, marks));
	assert_false(iscc_fs_check_neighbors_marks(6, &nng, marks));

	scc_free_digraph(&nng);
}


void scc_ut_fs_mark_seed_neighbors(void** state)
{
	(void) state;

	scc_Digraph nng = scc_digraph_from_string("..##..#/"
	                                          "..#.#.#/"
	                                          "#...#.#/"
	                                          "..#..##/"
	                                          "......./"
	                                          "..#..../"
	                                          "......./");

	bool stc_marks[7] = {false, false, false, false, false, false, false};

	iscc_fs_mark_seed_neighbors(0, &nng, stc_marks);
	bool ref_marks0[7] = {true, false, true, true, false, false, true};
	assert_memory_equal(stc_marks, ref_marks0, 7 * sizeof(bool));

	stc_marks[0] = stc_marks[2] = stc_marks[3] = stc_marks[6] = false;

	iscc_fs_mark_seed_neighbors(1, &nng, stc_marks);
	bool ref_marks1[7] = {false, true, true, false, true, false, true};
	assert_memory_equal(stc_marks, ref_marks1, 7 * sizeof(bool));

	stc_marks[2] = stc_marks[4] = stc_marks[6] = false;

	iscc_fs_mark_seed_neighbors(2, &nng, stc_marks);
	bool ref_marks2[7] = {true, true, true, false, true, false, true};
	assert_memory_equal(stc_marks, ref_marks2, 7 * sizeof(bool));

	stc_marks[0] = stc_marks[1] = stc_marks[2] = stc_marks[4] = stc_marks[6] = false;

	iscc_fs_mark_seed_neighbors(5, &nng, stc_marks);
	bool ref_marks5[7] = {false, false, true, false, false, true, false};
	assert_memory_equal(stc_marks, ref_marks5, 7 * sizeof(bool));

	stc_marks[2] = stc_marks[5] = false;

	iscc_fs_mark_seed_neighbors(3, &nng, stc_marks);
	bool ref_marks3[7] = {false, false, true, true, false, true, true};
	assert_memory_equal(stc_marks, ref_marks3, 7 * sizeof(bool));

	stc_marks[6] = false;

	iscc_fs_mark_seed_neighbors(6, &nng, stc_marks);
	bool ref_marks6[7] = {false, false, true, true, false, true, true};
	assert_memory_equal(stc_marks, ref_marks6, 7 * sizeof(bool));

	scc_free_digraph(&nng);
}


void scc_ut_fs_mark_seed_neighbors_withdiag(void** state)
{
	(void) state;

	scc_Digraph nng = scc_digraph_from_string("#.##..#/"
	                                          ".##.#.#/"
	                                          "#.#.#.#/"
	                                          "..##.##/"
	                                          "....#../"
	                                          "..#..#./"
	                                          "......#/");

	bool stc_marks[7] = {false, false, false, false, false, false, false};

	iscc_fs_mark_seed_neighbors(0, &nng, stc_marks);
	bool ref_marks0[7] = {true, false, true, true, false, false, true};
	assert_memory_equal(stc_marks, ref_marks0, 7 * sizeof(bool));

	stc_marks[0] = stc_marks[2] = stc_marks[3] = stc_marks[6] = false;

	iscc_fs_mark_seed_neighbors(1, &nng, stc_marks);
	bool ref_marks1[7] = {false, true, true, false, true, false, true};
	assert_memory_equal(stc_marks, ref_marks1, 7 * sizeof(bool));

	stc_marks[2] = stc_marks[4] = stc_marks[6] = false;

	iscc_fs_mark_seed_neighbors(2, &nng, stc_marks);
	bool ref_marks2[7] = {true, true, true, false, true, false, true};
	assert_memory_equal(stc_marks, ref_marks2, 7 * sizeof(bool));

	stc_marks[0] = stc_marks[1] = stc_marks[2] = stc_marks[4] = stc_marks[6] = false;

	iscc_fs_mark_seed_neighbors(5, &nng, stc_marks);
	bool ref_marks5[7] = {false, false, true, false, false, true, false};
	assert_memory_equal(stc_marks, ref_marks5, 7 * sizeof(bool));

	stc_marks[2] = stc_marks[5] = false;

	iscc_fs_mark_seed_neighbors(3, &nng, stc_marks);
	bool ref_marks3[7] = {false, false, true, true, false, true, true};
	assert_memory_equal(stc_marks, ref_marks3, 7 * sizeof(bool));

	stc_marks[6] = false;

	iscc_fs_mark_seed_neighbors(6, &nng, stc_marks);
	bool ref_marks6[7] = {false, false, true, true, false, true, true};
	assert_memory_equal(stc_marks, ref_marks6, 7 * sizeof(bool));

	scc_free_digraph(&nng);
}


void scc_ut_fs_sort_by_inwards(void** state)
{
	(void) state;

	scc_Digraph nng1 = scc_digraph_from_string("##...#/"
	                                           "###.../"
	                                           "##.##./"
	                                           "#.###./"
	                                           "####../"
	                                           "###.../");
	scc_Vid ref_sorted_vertices[6] = {5, 4, 3, 2, 1, 0};
	scc_Vid ref_inwards_count[6] = {6, 5, 4, 3, 2, 1};
	ptrdiff_t ref_vertex_index[6] = {5, 4, 3, 2, 1, 0};
	ptrdiff_t ref_bucket_index[7] = {0, 0, 1, 2, 3, 4, 5};

	iscc_fs_SortResult sort_n = iscc_fs_sort_by_inwards(&nng1, false);
	assert_memory_equal(sort_n.sorted_vertices, ref_sorted_vertices, 6 * sizeof(scc_Vid));
	assert_null(sort_n.inwards_count);
	assert_null(sort_n.vertex_index);
	assert_null(sort_n.bucket_index);

	iscc_fs_SortResult sort_i = iscc_fs_sort_by_inwards(&nng1, true);
	ptrdiff_t check_vertex_index[6];
	ptrdiff_t check_bucket_index[7];
	for (size_t i = 0; i < 6; ++i) check_vertex_index[i] = sort_i.vertex_index[i] - sort_i.sorted_vertices;
	for (size_t i = 0; i < 7; ++i) check_bucket_index[i] = sort_i.bucket_index[i] - sort_i.sorted_vertices;

	assert_memory_equal(sort_i.sorted_vertices, ref_sorted_vertices, 6 * sizeof(scc_Vid));
	assert_memory_equal(sort_i.inwards_count, ref_inwards_count, 6 * sizeof(scc_Vid));
	assert_memory_equal(check_vertex_index, ref_vertex_index, 6 * sizeof(ptrdiff_t));
	assert_memory_equal(check_bucket_index, ref_bucket_index, 7 * sizeof(ptrdiff_t));

	assert_memory_equal(sort_i.sorted_vertices, sort_n.sorted_vertices, 6 * sizeof(scc_Vid));

	scc_Digraph nng2 = scc_digraph_from_string(".###.#./"
	                                           ".#....#/"
	                                           "##.#.#./"
	                                           "#..#..#/"
	                                           ".###.#./"
	                                           ".....##/"
	                                           "###..../");
	scc_Vid ref_sorted_vertices2[7] = {4, 0, 2, 6, 3, 5, 1};
	scc_Vid ref_inwards_count2[7] = {3, 5, 3, 4, 0, 4, 3};
	ptrdiff_t ref_vertex_index2[7] = {1, 6, 2, 4, 0, 5, 3};
	ptrdiff_t ref_bucket_index2[6] = {0, 1, 1, 1, 4, 6};

	iscc_fs_SortResult sort_n2 = iscc_fs_sort_by_inwards(&nng2, false);
	assert_memory_equal(sort_n2.sorted_vertices, ref_sorted_vertices2, 7 * sizeof(scc_Vid));
	assert_null(sort_n2.inwards_count);
	assert_null(sort_n2.vertex_index);
	assert_null(sort_n2.bucket_index);

	iscc_fs_SortResult sort_i2 = iscc_fs_sort_by_inwards(&nng2, true);
	ptrdiff_t check_vertex_index2[7];
	ptrdiff_t check_bucket_index2[6];
	for (size_t i = 0; i < 7; ++i) check_vertex_index2[i] = sort_i2.vertex_index[i] - sort_i2.sorted_vertices;
	for (size_t i = 0; i < 6; ++i) check_bucket_index2[i] = sort_i2.bucket_index[i] - sort_i2.sorted_vertices;

	assert_memory_equal(sort_i2.sorted_vertices, ref_sorted_vertices2, 7 * sizeof(scc_Vid));
	assert_memory_equal(sort_i2.inwards_count, ref_inwards_count2, 7 * sizeof(scc_Vid));
	assert_memory_equal(check_vertex_index2, ref_vertex_index2, 7 * sizeof(ptrdiff_t));
	assert_memory_equal(check_bucket_index2, ref_bucket_index2, 6 * sizeof(ptrdiff_t));

	assert_memory_equal(sort_i2.sorted_vertices, sort_n2.sorted_vertices, 7 * sizeof(scc_Vid));


	scc_Digraph nng3 = scc_digraph_from_string("#..#...##./"
	                                           ".#.##..###/"
	                                           "#.##.#.##./"
	                                           ".####.####/"
	                                           ".###.#####/"
	                                           ".###.#####/"
	                                           "..#..#####/"
	                                           "..#...####/"
	                                           "..#...####/"
	                                           "..#....###/");
	scc_Vid ref_sorted_vertices3[10] = {0, 4, 1, 5, 3, 6, 2, 9, 7, 8};
	scc_Vid ref_inwards_count3[10] = {2, 4, 8, 6, 2, 4, 6, 10, 10, 8};
	ptrdiff_t ref_vertex_index3[10] = {0, 2, 6, 4, 1, 3, 5, 8, 9, 7};
	ptrdiff_t ref_bucket_index3[11] = {0, 0, 0, 2, 2, 4, 4, 6, 6, 8, 8};

	iscc_fs_SortResult sort_n3 = iscc_fs_sort_by_inwards(&nng3, false);
	assert_memory_equal(sort_n3.sorted_vertices, ref_sorted_vertices3, 10 * sizeof(scc_Vid));
	assert_null(sort_n3.inwards_count);
	assert_null(sort_n3.vertex_index);
	assert_null(sort_n3.bucket_index);

	iscc_fs_SortResult sort_i3 = iscc_fs_sort_by_inwards(&nng3, true);
	ptrdiff_t check_vertex_index3[10];
	ptrdiff_t check_bucket_index3[11];
	for (size_t i = 0; i < 10; ++i) check_vertex_index3[i] = sort_i3.vertex_index[i] - sort_i3.sorted_vertices;
	for (size_t i = 0; i < 11; ++i) check_bucket_index3[i] = sort_i3.bucket_index[i] - sort_i3.sorted_vertices;

	assert_memory_equal(sort_i3.sorted_vertices, ref_sorted_vertices3, 10 * sizeof(scc_Vid));
	assert_memory_equal(sort_i3.inwards_count, ref_inwards_count3, 10 * sizeof(scc_Vid));
	assert_memory_equal(check_vertex_index3, ref_vertex_index3, 10 * sizeof(ptrdiff_t));
	assert_memory_equal(check_bucket_index3, ref_bucket_index3, 11 * sizeof(ptrdiff_t));

	assert_memory_equal(sort_i3.sorted_vertices, sort_n3.sorted_vertices, 10 * sizeof(scc_Vid));

	iscc_fs_free_SortResult(&sort_n);
	iscc_fs_free_SortResult(&sort_i);
	iscc_fs_free_SortResult(&sort_n2);
	iscc_fs_free_SortResult(&sort_i2);
	iscc_fs_free_SortResult(&sort_n3);
	iscc_fs_free_SortResult(&sort_i3);
	scc_free_digraph(&nng1);
	scc_free_digraph(&nng2);
	scc_free_digraph(&nng3);
}


void scc_ut_fs_free_SortResult(void** state)
{
	(void) state;

	scc_Digraph nng = scc_digraph_from_string("##...#/"
	                                           "###.../"
	                                           "##.##./"
	                                           "#.###./"
	                                           "####../"
	                                           "###.../");

	iscc_fs_SortResult sort = iscc_fs_sort_by_inwards(&nng, false);

	iscc_fs_free_SortResult(&sort);

	assert_null(sort.inwards_count);
	assert_null(sort.sorted_vertices);
	assert_null(sort.vertex_index);
	assert_null(sort.bucket_index);

	scc_free_digraph(&nng);
}


void scc_ut_fs_decrease_v_in_sort(void** state)
{
	(void) state;

	scc_Digraph nng = scc_digraph_from_string("...#...###/"
	                                          "..###..###/"
	                                          "##.#.####./"
	                                          ".##.#.####/"
	                                          "####.#####/"
	                                          ".###..####/"
	                                          "..##.#.###/"
	                                          "..#..#####/"
	                                          "..#...####/"
	                                          "..#....##./");
	scc_Vid ref_sorted_vertices[10] = {0, 4, 1, 5, 3, 6, 2, 9, 7, 8};
	scc_Vid ref_inwards_count[10] = {2, 4, 8, 6, 2, 4, 6, 10, 10, 8};
	ptrdiff_t ref_vertex_index[10] = {0, 2, 6, 4, 1, 3, 5, 8, 9, 7};
	ptrdiff_t ref_bucket_index[11] = {0, 0, 0, 2, 2, 4, 4, 6, 6, 8, 8};

	iscc_fs_SortResult sort = iscc_fs_sort_by_inwards(&nng, true);
	ptrdiff_t check_vertex_index[10];
	ptrdiff_t check_bucket_index[11];
	for (size_t i = 0; i < 10; ++i) check_vertex_index[i] = sort.vertex_index[i] - sort.sorted_vertices;
	for (size_t i = 0; i < 11; ++i) check_bucket_index[i] = sort.bucket_index[i] - sort.sorted_vertices;
	assert_memory_equal(sort.sorted_vertices, ref_sorted_vertices, 10 * sizeof(scc_Vid));
	assert_memory_equal(sort.inwards_count, ref_inwards_count, 10 * sizeof(scc_Vid));
	assert_memory_equal(check_vertex_index, ref_vertex_index, 10 * sizeof(ptrdiff_t));
	assert_memory_equal(check_bucket_index, ref_bucket_index, 11 * sizeof(ptrdiff_t));

	iscc_fs_decrease_v_in_sort(8, sort.inwards_count, sort.vertex_index, sort.bucket_index, sort.sorted_vertices);
	scc_Vid ref_sorted_vertices2[10] = {0, 4, 1, 5, 3, 6, 2, 9, 8, 7};
	scc_Vid ref_inwards_count2[10] = {2, 4, 8, 6, 2, 4, 6, 10, 9, 8};
	ptrdiff_t ref_vertex_index2[10] = {0, 2, 6, 4, 1, 3, 5, 9, 8, 7};
	ptrdiff_t ref_bucket_index2[11] = {0, 0, 0, 2, 2, 4, 4, 6, 6, 8, 9};
	for (size_t i = 0; i < 10; ++i) check_vertex_index[i] = sort.vertex_index[i] - sort.sorted_vertices;
	for (size_t i = 0; i < 11; ++i) check_bucket_index[i] = sort.bucket_index[i] - sort.sorted_vertices;
	assert_memory_equal(sort.sorted_vertices, ref_sorted_vertices2, 10 * sizeof(scc_Vid));
	assert_memory_equal(sort.inwards_count, ref_inwards_count2, 10 * sizeof(scc_Vid));
	assert_memory_equal(check_vertex_index, ref_vertex_index2, 10 * sizeof(ptrdiff_t));
	assert_memory_equal(check_bucket_index, ref_bucket_index2, 11 * sizeof(ptrdiff_t));

	iscc_fs_decrease_v_in_sort(8, sort.inwards_count, sort.vertex_index, sort.bucket_index, sort.sorted_vertices);
	scc_Vid ref_sorted_vertices3[10] = {0, 4, 1, 5, 3, 6, 2, 9, 8, 7};
	scc_Vid ref_inwards_count3[10] = {2, 4, 8, 6, 2, 4, 6, 10, 8, 8};
	ptrdiff_t ref_vertex_index3[10] = {0, 2, 6, 4, 1, 3, 5, 9, 8, 7};
	ptrdiff_t ref_bucket_index3[11] = {0, 0, 0, 2, 2, 4, 4, 6, 6, 9, 9};
	for (size_t i = 0; i < 10; ++i) check_vertex_index[i] = sort.vertex_index[i] - sort.sorted_vertices;
	for (size_t i = 0; i < 11; ++i) check_bucket_index[i] = sort.bucket_index[i] - sort.sorted_vertices;
	assert_memory_equal(sort.sorted_vertices, ref_sorted_vertices3, 10 * sizeof(scc_Vid));
	assert_memory_equal(sort.inwards_count, ref_inwards_count3, 10 * sizeof(scc_Vid));
	assert_memory_equal(check_vertex_index, ref_vertex_index3, 10 * sizeof(ptrdiff_t));
	assert_memory_equal(check_bucket_index, ref_bucket_index3, 11 * sizeof(ptrdiff_t));

	iscc_fs_decrease_v_in_sort(8, sort.inwards_count, sort.vertex_index, sort.bucket_index, sort.sorted_vertices + 6);
	scc_Vid ref_sorted_vertices4[10] = {0, 4, 1, 5, 3, 6, 2, 8, 9, 7};
	scc_Vid ref_inwards_count4[10] = {2, 4, 8, 6, 2, 4, 6, 10, 7, 8};
	ptrdiff_t ref_vertex_index4[10] = {0, 2, 6, 4, 1, 3, 5, 9, 7, 8};
	ptrdiff_t ref_bucket_index4[11] = {0, 0, 0, 2, 2, 4, 4, 7, 8, 9, 9};
	for (size_t i = 0; i < 10; ++i) check_vertex_index[i] = sort.vertex_index[i] - sort.sorted_vertices;
	for (size_t i = 0; i < 11; ++i) check_bucket_index[i] = sort.bucket_index[i] - sort.sorted_vertices;
	assert_memory_equal(sort.sorted_vertices, ref_sorted_vertices4, 10 * sizeof(scc_Vid));
	assert_memory_equal(sort.inwards_count, ref_inwards_count4, 10 * sizeof(scc_Vid));
	assert_memory_equal(check_vertex_index, ref_vertex_index4, 10 * sizeof(ptrdiff_t));
	assert_memory_equal(check_bucket_index, ref_bucket_index4, 11 * sizeof(ptrdiff_t));


	scc_Digraph nng2 = scc_digraph_from_string(".......##./"
	                                           "..###..###/"
	                                           "##.#.#####/"
	                                           ".##.#.####/"
	                                           "####.#####/"
	                                           ".###..####/"
	                                           "..##.#.###/"
	                                           "..##.##.##/"
	                                           "..#...##.#/"
	                                           "..#....##./");
	scc_Vid ref2_sorted_vertices[10] = {0, 4, 1, 5, 3, 6, 2, 9, 7, 8};
	scc_Vid ref2_inwards_count[10] = {2, 4, 8, 6, 2, 4, 6, 9, 9, 8};
	ptrdiff_t ref2_vertex_index[10] = {0, 2, 6, 4, 1, 3, 5, 8, 9, 7};
	ptrdiff_t ref2_bucket_index[10] = {0, 0, 0, 2, 2, 4, 4, 6, 6, 8};

	iscc_fs_SortResult sort2 = iscc_fs_sort_by_inwards(&nng2, true);
	ptrdiff_t check2_vertex_index[10];
	ptrdiff_t check2_bucket_index[10];
	for (size_t i = 0; i < 10; ++i) check2_vertex_index[i] = sort2.vertex_index[i] - sort2.sorted_vertices;
	for (size_t i = 0; i < 10; ++i) check2_bucket_index[i] = sort2.bucket_index[i] - sort2.sorted_vertices;
	assert_memory_equal(sort2.sorted_vertices, ref2_sorted_vertices, 10 * sizeof(scc_Vid));
	assert_memory_equal(sort2.inwards_count, ref2_inwards_count, 10 * sizeof(scc_Vid));
	assert_memory_equal(check2_vertex_index, ref2_vertex_index, 10 * sizeof(ptrdiff_t));
	assert_memory_equal(check2_bucket_index, ref2_bucket_index, 10 * sizeof(ptrdiff_t));

	iscc_fs_decrease_v_in_sort(2, sort2.inwards_count, sort2.vertex_index, sort2.bucket_index, sort2.sorted_vertices + 0);
	iscc_fs_decrease_v_in_sort(3, sort2.inwards_count, sort2.vertex_index, sort2.bucket_index, sort2.sorted_vertices + 0);
	iscc_fs_decrease_v_in_sort(5, sort2.inwards_count, sort2.vertex_index, sort2.bucket_index, sort2.sorted_vertices + 0);
	iscc_fs_decrease_v_in_sort(6, sort2.inwards_count, sort2.vertex_index, sort2.bucket_index, sort2.sorted_vertices + 0);
	iscc_fs_decrease_v_in_sort(9, sort2.inwards_count, sort2.vertex_index, sort2.bucket_index, sort2.sorted_vertices + 0);
	iscc_fs_decrease_v_in_sort(2, sort2.inwards_count, sort2.vertex_index, sort2.bucket_index, sort2.sorted_vertices + 0);
	iscc_fs_decrease_v_in_sort(6, sort2.inwards_count, sort2.vertex_index, sort2.bucket_index, sort2.sorted_vertices + 0);
	iscc_fs_decrease_v_in_sort(9, sort2.inwards_count, sort2.vertex_index, sort2.bucket_index, sort2.sorted_vertices + 0);

	scc_Vid ref2_sorted_vertices2[10] = {0, 4, 5, 1, 6, 3, 2, 9, 7, 8};
	scc_Vid ref2_inwards_count2[10] = {2, 4, 6, 5, 2, 3, 4, 9, 9, 6};
	ptrdiff_t ref2_vertex_index2[10] = {0, 3, 6, 5, 1, 2, 4, 8, 9, 7};
	ptrdiff_t ref2_bucket_index2[10] = {0, 0, 0, 2, 3, 5, 6, 8, 8, 8};
	for (size_t i = 0; i < 10; ++i) check2_vertex_index[i] = sort2.vertex_index[i] - sort2.sorted_vertices;
	for (size_t i = 0; i < 10; ++i) check2_bucket_index[i] = sort2.bucket_index[i] - sort2.sorted_vertices;
	assert_memory_equal(sort2.sorted_vertices, ref2_sorted_vertices2, 10 * sizeof(scc_Vid));
	assert_memory_equal(sort2.inwards_count, ref2_inwards_count2, 10 * sizeof(scc_Vid));
	assert_memory_equal(check2_vertex_index, ref2_vertex_index2, 10 * sizeof(ptrdiff_t));
	assert_memory_equal(check2_bucket_index, ref2_bucket_index2, 10 * sizeof(ptrdiff_t));

	iscc_fs_free_SortResult(&sort);
	iscc_fs_free_SortResult(&sort2);
	scc_free_digraph(&nng);
	scc_free_digraph(&nng2);
}


int main(void)
{
	const struct CMUnitTest test_findseeds_internals[] = {
		cmocka_unit_test(scc_ut_exclusion_graph),
		cmocka_unit_test(scc_ut_exclusion_graph_withdiag),
		cmocka_unit_test(scc_ut_fs_init_clustering),
		cmocka_unit_test(scc_ut_fs_shrink_seed_array),
		cmocka_unit_test(scc_ut_fs_add_seed),
		cmocka_unit_test(scc_ut_fs_check_neighbors_marks),
		cmocka_unit_test(scc_ut_fs_check_neighbors_marks_withdiag),
		cmocka_unit_test(scc_ut_fs_mark_seed_neighbors),
		cmocka_unit_test(scc_ut_fs_mark_seed_neighbors_withdiag),
		cmocka_unit_test(scc_ut_fs_sort_by_inwards),
		cmocka_unit_test(scc_ut_fs_free_SortResult),
		cmocka_unit_test(scc_ut_fs_decrease_v_in_sort),
	};

	return cmocka_run_group_tests_name("internal find seeds module", test_findseeds_internals, NULL, NULL);
}
