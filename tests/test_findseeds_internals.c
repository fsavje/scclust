/* scclust -- A C library for size constrained clustering
 * https://github.com/fsavje/scclust
 *
 * Copyright (C) 2015  Fredrik Savje -- http://fredriksavje.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
==============================================================================*/


#include "test_suite.h"
#include "test_tools.h"

#include <stdbool.h>
#include <stdlib.h>

#include "../src/findseeds.c"
#include "../include/config.h"
#include "../include/digraph.h"
#include "../include/digraph_debug.h"


void scc_ut_fs_check_input(void** state) {
	(void) state;

	scc_Digraph empty0 = scc_empty_digraph(0, 0);
	scc_Digraph empty10 = scc_empty_digraph(10, 10);

	scc_Clustering clustering = {
		.vertices = 10,
		.num_clusters = 0,
		.assigned = calloc(10, sizeof(bool)),
		.seed = calloc(10, sizeof(bool)),
		.cluster_label = malloc(sizeof(scc_Clulab[10])),
	};

	assert_true(iscc_fs_check_input(&empty10, &clustering));

	assert_false(iscc_fs_check_input(NULL, &clustering));
	assert_false(iscc_fs_check_input(&empty10, NULL));

	scc_Digraph nng_tmp = scc_digraph_from_pieces(10, 10, NULL, NULL);
	assert_false(iscc_fs_check_input(&nng_tmp, &clustering));

	scc_Clustering cl_tmp = clustering;
	cl_tmp.assigned = NULL;
	assert_false(iscc_fs_check_input(&empty10, &cl_tmp));
	cl_tmp = clustering;
	cl_tmp.seed = NULL;
	assert_false(iscc_fs_check_input(&empty10, &cl_tmp));
	cl_tmp = clustering;
	cl_tmp.cluster_label = NULL;
	assert_false(iscc_fs_check_input(&empty10, &cl_tmp));

	clustering.vertices = 5;
	assert_false(iscc_fs_check_input(&empty10, &clustering));
	clustering.vertices = 10;
	assert_true(iscc_fs_check_input(&empty10, &clustering));

	assert_false(iscc_fs_check_input(&empty0, &clustering));

	scc_free_Clustering(&clustering);
	scc_free_digraph(&empty0);
	scc_free_digraph(&empty10);
}

/*
typedef struct iscc_fs_SortResult iscc_fs_SortResult;
struct iscc_fs_SortResult {
	scc_Vid* inwards_count;
	scc_Vid* sorted_vertices;
	scc_Vid** vertex_index;
	scc_Vid** bucket_index;
};
*/

void scc_ut_fs_sort_by_inwards(void** state) {
	(void) state;

	scc_Digraph nng1 = scc_digraph_from_string("**...*/"
	                                           "***.../"
	                                           "**.**./"
	                                           "*.***./"
	                                           "****../"
	                                           "***.../");
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

	scc_Digraph nng2 = scc_digraph_from_string(".***.*./"
	                                           ".*....*/"
	                                           "**.*.*./"
	                                           "*..*..*/"
	                                           ".***.*./"
	                                           ".....**/"
	                                           "***..../");
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


	scc_Digraph nng3 = scc_digraph_from_string("*..*...**./"
	                                           ".*.**..***/"
	                                           "*.**.*.**./"
	                                           ".****.****/"
	                                           ".***.*****/"
	                                           ".***.*****/"
	                                           "..*..*****/"
	                                           "..*...****/"
	                                           "..*...****/"
	                                           "..*....***/");
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

void scc_ut_fs_check_candidate_vertex(void** state) {
	(void) state;

	scc_Digraph nng = scc_digraph_from_string("..**..*/"
	                                          "..*.*.*/"
	                                          "*...*.*/"
	                                          "..*..**/"
	                                          "......./"
	                                          "..*..../"
	                                          "......./");

	bool assigned[7] = {true, false, false, false, true, false, false};

	assert_false(iscc_fs_check_candidate_vertex(0, &nng, assigned));
	assert_false(iscc_fs_check_candidate_vertex(1, &nng, assigned));
	assert_false(iscc_fs_check_candidate_vertex(2, &nng, assigned));
	assert_true(iscc_fs_check_candidate_vertex(3, &nng, assigned));
	assert_false(iscc_fs_check_candidate_vertex(4, &nng, assigned));
	assert_true(iscc_fs_check_candidate_vertex(5, &nng, assigned));
	assert_false(iscc_fs_check_candidate_vertex(6, &nng, assigned));

	scc_free_digraph(&nng);
}


void scc_ut_fs_set_seed(void** state) {
	(void) state;

	scc_Digraph nng = scc_digraph_from_string("..**..*/"
	                                          "..*.*.*/"
	                                          "*...*.*/"
	                                          "..*..**/"
	                                          "......./"
	                                          "..*..../"
	                                          "......./");

	bool stc_assigned[7] = {false, false, false, false, false, false, false};
	bool stc_seed[7] = {false, false, false, false, false, false, false};
	scc_Clulab stc_cluster_label[7] = {SCC_CLULAB_MAX, SCC_CLULAB_MAX, SCC_CLULAB_MAX, SCC_CLULAB_MAX, SCC_CLULAB_MAX, SCC_CLULAB_MAX, SCC_CLULAB_MAX};

	scc_Clustering clustering = {
		.vertices = 7,
		.num_clusters = 0,
		.assigned = stc_assigned,
		.seed = stc_seed,
		.cluster_label = stc_cluster_label,
	};

	iscc_fs_set_seed(0, &nng, &clustering, clustering.assigned);
	bool ref_assigned0[7] = {true, false, true, true, false, false, true};
	bool ref_seed0[7] = {true, false, false, false, false, false, false};
	scc_Clulab ref_cluster_label0[7] = {0, SCC_CLULAB_MAX, 0, 0, SCC_CLULAB_MAX, SCC_CLULAB_MAX, 0};
	assert_memory_equal(clustering.assigned, ref_assigned0, 7 * sizeof(bool));
	assert_memory_equal(clustering.seed, ref_seed0, 7 * sizeof(bool));
	assert_memory_equal(clustering.cluster_label, ref_cluster_label0, 7 * sizeof(scc_Clulab));
	assert_int_equal(clustering.vertices, 7);
	assert_int_equal(clustering.num_clusters, 1);

	stc_assigned[0] = stc_assigned[2] = stc_assigned[3] = stc_assigned[6] = false;
	stc_seed[0] = false;

	iscc_fs_set_seed(1, &nng, &clustering, clustering.assigned);
	bool ref_assigned1[7] = {false, true, true, false, true, false, true};
	bool ref_seed1[7] = {false, true, false, false, false, false, false};
	scc_Clulab ref_cluster_label1[7] = {0, 1, 1, 0, 1, SCC_CLULAB_MAX, 1};
	assert_memory_equal(clustering.assigned, ref_assigned1, 7 * sizeof(bool));
	assert_memory_equal(clustering.seed, ref_seed1, 7 * sizeof(bool));
	assert_memory_equal(clustering.cluster_label, ref_cluster_label1, 7 * sizeof(scc_Clulab));
	assert_int_equal(clustering.vertices, 7);
	assert_int_equal(clustering.num_clusters, 2);

	iscc_fs_set_seed(2, &nng, &clustering, clustering.assigned);
	bool ref_assigned2[7] = {true, true, true, false, true, false, true};
	bool ref_seed2[7] = {false, true, true, false, false, false, false};
	scc_Clulab ref_cluster_label2[7] = {2, 1, 2, 0, 2, SCC_CLULAB_MAX, 2};
	assert_memory_equal(clustering.assigned, ref_assigned2, 7 * sizeof(bool));
	assert_memory_equal(clustering.seed, ref_seed2, 7 * sizeof(bool));
	assert_memory_equal(clustering.cluster_label, ref_cluster_label2, 7 * sizeof(scc_Clulab));
	assert_int_equal(clustering.vertices, 7);
	assert_int_equal(clustering.num_clusters, 3);

	stc_assigned[0] = stc_assigned[1] = stc_assigned[2] = stc_assigned[4] = stc_assigned[6] = false;
	stc_seed[1] = stc_seed[2] = false;

	iscc_fs_set_seed(5, &nng, &clustering, clustering.assigned);
	bool ref_assigned5[7] = {false, false, true, false, false, true, false};
	bool ref_seed5[7] = {false, false, false, false, false, true, false};
	scc_Clulab ref_cluster_label5[7] = {2, 1, 3, 0, 2, 3, 2};
	assert_memory_equal(clustering.assigned, ref_assigned5, 7 * sizeof(bool));
	assert_memory_equal(clustering.seed, ref_seed5, 7 * sizeof(bool));
	assert_memory_equal(clustering.cluster_label, ref_cluster_label5, 7 * sizeof(scc_Clulab));
	assert_int_equal(clustering.vertices, 7);
	assert_int_equal(clustering.num_clusters, 4);

	iscc_fs_set_seed(3, &nng, &clustering, clustering.assigned);
	bool ref_assigned3[7] = {false, false, true, true, false, true, true};
	bool ref_seed3[7] = {false, false, false, true, false, true, false};
	scc_Clulab ref_cluster_label3[7] = {2, 1, 4, 4, 2, 4, 4};
	assert_memory_equal(clustering.assigned, ref_assigned3, 7 * sizeof(bool));
	assert_memory_equal(clustering.seed, ref_seed3, 7 * sizeof(bool));
	assert_memory_equal(clustering.cluster_label, ref_cluster_label3, 7 * sizeof(scc_Clulab));
	assert_int_equal(clustering.vertices, 7);
	assert_int_equal(clustering.num_clusters, 5);

	iscc_fs_set_seed(6, &nng, &clustering, clustering.assigned);
	bool ref_assigned6[7] = {false, false, true, true, false, true, true};
	bool ref_seed6[7] = {false, false, false, true, false, true, true};
	scc_Clulab ref_cluster_label6[7] = {2, 1, 4, 4, 2, 4, 5};
	assert_memory_equal(clustering.assigned, ref_assigned6, 7 * sizeof(bool));
	assert_memory_equal(clustering.seed, ref_seed6, 7 * sizeof(bool));
	assert_memory_equal(clustering.cluster_label, ref_cluster_label6, 7 * sizeof(scc_Clulab));
	assert_int_equal(clustering.vertices, 7);
	assert_int_equal(clustering.num_clusters, 6);

	scc_free_digraph(&nng);
}


void scc_ut_fs_decrease_v_in_sort(void** state) {
	(void) state;

	scc_Digraph nng = scc_digraph_from_string("...*...***/"
	                                          "..***..***/"
	                                          "**.*.****./"
	                                          ".**.*.****/"
	                                          "****.*****/"
	                                          ".***..****/"
	                                          "..**.*.***/"
	                                          "..*..*****/"
	                                          "..*...****/"
	                                          "..*....**./");
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


	scc_Digraph nng2 = scc_digraph_from_string(".......**./"
	                                           "..***..***/"
	                                           "**.*.*****/"
	                                           ".**.*.****/"
	                                           "****.*****/"
	                                           ".***..****/"
	                                           "..**.*.***/"
	                                           "..**.**.**/"
	                                           "..*...**.*/"
	                                           "..*....**./");
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

int main(void) {
	const struct CMUnitTest test_findseeds_internals[] = {
		cmocka_unit_test(scc_ut_fs_check_input),
		cmocka_unit_test(scc_ut_fs_sort_by_inwards),
		cmocka_unit_test(scc_ut_fs_check_candidate_vertex),
		cmocka_unit_test(scc_ut_fs_set_seed),
		cmocka_unit_test(scc_ut_fs_decrease_v_in_sort),
	};

	return cmocka_run_group_tests_name("internal find seeds module", test_findseeds_internals, NULL, NULL);
}

