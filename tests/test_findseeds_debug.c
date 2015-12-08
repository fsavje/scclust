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

#define SCC_STABLE_CLUSTERING

#include "test_suite.h"

#include "../src/findseeds.c"
#include "../include/config.h"
#include "../include/digraph.h"
#include "../include/digraph_debug.h"
#include "../include/nng_clustering.h"


void scc_ut_fs_debug_vid_sort(void** state) {
	(void) state;

	scc_Vid buckets1[7] = {1, 4, 3, 5, 6, 2, 0};
	scc_Vid inwards_count1[7] = {3, 1, 2, 2, 1, 2, 2};
	scc_Vid* vertex_index1[7];
	for (scc_Vid* b = buckets1; b != buckets1 + 7; ++b) vertex_index1[*b] = b;
	iscc_fs_debug_vid_sort(buckets1 + 2, buckets1 + 5, inwards_count1, vertex_index1);

	scc_Vid ref_buckets1[7] = {1, 4, 2, 3, 5, 6, 0};
	scc_Vid ref_inwards_count1[7] = {3, 1, 2, 2, 1, 2, 2};
	scc_Vid* ref_vertex_index1[7];
	for (scc_Vid* b = ref_buckets1; b != ref_buckets1 + 7; ++b) ref_vertex_index1[*b] = b - ref_buckets1 + buckets1;
	assert_memory_equal(buckets1, ref_buckets1, 7 * sizeof(scc_Vid));
	assert_memory_equal(inwards_count1, ref_inwards_count1, 7 * sizeof(scc_Vid));
	assert_memory_equal(vertex_index1, ref_vertex_index1, 7 * sizeof(scc_Vid*));


	scc_Vid buckets2[7] = {1, 4, 2, 5, 6, 3, 0};
	scc_Vid inwards_count2[7] = {3, 1, 2, 2, 1, 2, 2};
	scc_Vid* vertex_index2[7];
	for (scc_Vid* b = buckets2; b != buckets2 + 7; ++b) vertex_index2[*b] = b;
	iscc_fs_debug_vid_sort(buckets2 + 2, buckets2 + 5, inwards_count2, vertex_index2);

	scc_Vid ref_buckets2[7] = {1, 4, 2, 3, 5, 6, 0};
	scc_Vid ref_inwards_count2[7] = {3, 1, 2, 2, 1, 2, 2};
	scc_Vid* ref_vertex_index2[7];
	for (scc_Vid* b = ref_buckets2; b != ref_buckets2 + 7; ++b) ref_vertex_index2[*b] = b - ref_buckets2 + buckets2;
	assert_memory_equal(buckets2, ref_buckets2, 7 * sizeof(scc_Vid));
	assert_memory_equal(inwards_count2, ref_inwards_count2, 7 * sizeof(scc_Vid));
	assert_memory_equal(vertex_index2, ref_vertex_index2, 7 * sizeof(scc_Vid*));


	scc_Vid buckets3[7] = {1, 4, 2, 3, 5, 6, 0};
	scc_Vid inwards_count3[7] = {3, 1, 2, 2, 1, 2, 2};
	scc_Vid* vertex_index3[7];
	for (scc_Vid* b = buckets3; b != buckets3 + 7; ++b) vertex_index3[*b] = b;
	iscc_fs_debug_vid_sort(buckets3 + 6, buckets3 + 6, inwards_count3, vertex_index3);

	scc_Vid ref_buckets3[7] = {1, 4, 2, 3, 5, 6, 0};
	scc_Vid ref_inwards_count3[7] = {3, 1, 2, 2, 1, 2, 2};
	scc_Vid* ref_vertex_index3[7];
	for (scc_Vid* b = ref_buckets3; b != ref_buckets3 + 7; ++b) ref_vertex_index3[*b] = b - ref_buckets3 + buckets3;
	assert_memory_equal(buckets3, ref_buckets3, 7 * sizeof(scc_Vid));
	assert_memory_equal(inwards_count3, ref_inwards_count3, 7 * sizeof(scc_Vid));
	assert_memory_equal(vertex_index3, ref_vertex_index3, 7 * sizeof(scc_Vid*));
}

void scc_ut_findseeds_checkseeds18_debug(void** state) {
	(void) state;

	scc_Digraph nng = scc_digraph_from_string(".*..*............./"
	                                          "*...*............./"
	                                          "....*..*........../"
	                                          "*...*............./"
	                                          ".*.*............../"
	                                          "..*.....*........./"
	                                          "...*.....*......../"
	                                          "......*.*........./"
	                                          ".....*.....*....../"
	                                          "..........*.....*./"
	                                          ".......*.....*..../"
	                                          "........*.*......./"
	                                          "...............**./"
	                                          "..............*..*/"
	                                          ".............*...*/"
	                                          ".........*..*...../"
	                                          ".............**.../"
	                                          "..............*.*./");

	scc_Clustering cl_seed_inupdat = scc_base_clustering(&nng, inwards_updating, 7);
	scc_Vid ref_seeds_inupdat[5] = {2, 6, 11, 12, 13};
	assert_memory_equal(cl_seed_inupdat.seeds, ref_seeds_inupdat, 5 * sizeof(bool));
	assert_int_equal(cl_seed_inupdat.num_clusters, 5);

	scc_Clustering cl_seed_exupdat = scc_base_clustering(&nng, exclusion_updating, 7);
	scc_Vid ref_seeds_exupdat[5] = {8, 15, 17, 10, 0};
	assert_memory_equal(cl_seed_exupdat.seeds, ref_seeds_exupdat, 5 * sizeof(bool));
	assert_int_equal(cl_seed_exupdat.num_clusters, 5);

	scc_free_digraph(&nng);
	scc_free_Clustering(&cl_seed_inupdat);
	scc_free_Clustering(&cl_seed_exupdat);
}

int main(void) {
	const struct CMUnitTest test_findseeds_debug[] = {
		cmocka_unit_test(scc_ut_fs_debug_vid_sort),
		cmocka_unit_test(scc_ut_findseeds_checkseeds18_debug),
	};

	return cmocka_run_group_tests_name("debug find seeds module", test_findseeds_debug, NULL, NULL);
}

