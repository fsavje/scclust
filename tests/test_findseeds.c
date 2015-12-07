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

#include "../src/findseeds.h"
#include "../include/config.h"
#include "../include/digraph.h"
#include "../include/digraph_debug.h"
#include "../include/nng_clustering.h"


#ifdef SCC_STABLE_CLUSTERING
    #error Please run this test without the SCC_STABLE_CLUSTERING flag
#endif


void scc_ut_findseeds_lexical(void** state) {
	(void) state;

	scc_Digraph nng = scc_digraph_from_string(".*......../"
	                                          "...*....../"
	                                          "*........./"
	                                          ".*......../"
	                                          ".....*..../"
	                                          "...*....../"
	                                          "....*...../"
	                                          "......*.../"
	                                          "......*.../"
	                                          "......*.../");

	bool fp_assigned[10] = {true, true, false, false, true, true, true, true, false, false};
	bool fp_seed[10] = {true, false, false, false, true, false, false, true, false, false};
	scc_Clulab fp_cluster_label[10] = {0, 0, SCC_CLULAB_MAX, SCC_CLULAB_MAX, 1, 1, 2, 2, SCC_CLULAB_MAX, SCC_CLULAB_MAX};

	scc_Clustering cl_from_low = {
		.vertices = 10,
		.num_clusters = 0,
		.assigned = calloc(10, sizeof(bool)),
		.seed = calloc(10, sizeof(bool)),
		.cluster_label = malloc(sizeof(scc_Clulab[10])),
	};

	iscc_findseeds_lexical(&nng, &cl_from_low);
	assert_int_equal(cl_from_low.vertices, 10);
	assert_int_equal(cl_from_low.num_clusters, 3);
	assert_memory_equal(cl_from_low.assigned, fp_assigned, 10 * sizeof(bool));
	assert_memory_equal(cl_from_low.seed, fp_seed, 10 * sizeof(bool));
	assert_int_equal(cl_from_low.cluster_label[0], fp_cluster_label[0]);
	assert_int_equal(cl_from_low.cluster_label[1], fp_cluster_label[1]);
	assert_int_equal(cl_from_low.cluster_label[4], fp_cluster_label[4]);
	assert_int_equal(cl_from_low.cluster_label[5], fp_cluster_label[5]);
	assert_int_equal(cl_from_low.cluster_label[6], fp_cluster_label[6]);
	assert_int_equal(cl_from_low.cluster_label[7], fp_cluster_label[7]);


	scc_Clustering cl_from_high = scc_base_clustering(&nng, lexical);
	assert_int_equal(cl_from_high.vertices, 10);
	assert_int_equal(cl_from_high.num_clusters, 3);
	assert_memory_equal(cl_from_high.assigned, fp_assigned, 10 * sizeof(bool));
	assert_memory_equal(cl_from_high.seed, fp_seed, 10 * sizeof(bool));
	assert_int_equal(cl_from_high.cluster_label[0], fp_cluster_label[0]);
	assert_int_equal(cl_from_high.cluster_label[1], fp_cluster_label[1]);
	assert_int_equal(cl_from_high.cluster_label[4], fp_cluster_label[4]);
	assert_int_equal(cl_from_high.cluster_label[5], fp_cluster_label[5]);
	assert_int_equal(cl_from_high.cluster_label[6], fp_cluster_label[6]);
	assert_int_equal(cl_from_high.cluster_label[7], fp_cluster_label[7]);

	scc_free_Clustering(&cl_from_low);
	scc_free_Clustering(&cl_from_high);

	scc_free_digraph(&nng);
}

void scc_ut_findseeds_inwards(void** state) {
	(void) state;

	scc_Digraph nng = scc_digraph_from_string("...*....../"
	                                          "...*....../"
	                                          "*........./"
	                                          ".*......../"
	                                          ".....*..../"
	                                          ".*......../"
	                                          "....*...../"
	                                          "......*.../"
	                                          "......*.../"
	                                          "......*.../");

	bool fp_assigned[10] = {true, true, true, true, true, true, true, true, false, false};
	bool fp_seed[10] = {false, true, true, false, true, false, false, true, false, false};
	scc_Clulab fp_cluster_label[10] = {0, 3, 0, 3, 2, 2, 1, 1, SCC_CLULAB_MAX, SCC_CLULAB_MAX};

	scc_Clustering cl_from_low = {
		.vertices = 10,
		.num_clusters = 0,
		.assigned = calloc(10, sizeof(bool)),
		.seed = calloc(10, sizeof(bool)),
		.cluster_label = malloc(sizeof(scc_Clulab[10])),
	};

	iscc_findseeds_inwards(&nng, &cl_from_low, false);
	assert_int_equal(cl_from_low.vertices, 10);
	assert_int_equal(cl_from_low.num_clusters, 4);
	assert_memory_equal(cl_from_low.assigned, fp_assigned, 10 * sizeof(bool));
	assert_memory_equal(cl_from_low.seed, fp_seed, 10 * sizeof(bool));
	assert_memory_equal(cl_from_low.cluster_label, fp_cluster_label, 8 * sizeof(bool));


	scc_Clustering cl_from_high = scc_base_clustering(&nng, inwards_order);
	assert_int_equal(cl_from_high.vertices, 10);
	assert_int_equal(cl_from_high.num_clusters, 4);
	assert_memory_equal(cl_from_high.assigned, fp_assigned, 10 * sizeof(bool));
	assert_memory_equal(cl_from_high.seed, fp_seed, 10 * sizeof(bool));
	assert_memory_equal(cl_from_high.cluster_label, fp_cluster_label, 8 * sizeof(bool));

	scc_free_Clustering(&cl_from_low);
	scc_free_Clustering(&cl_from_high);

	scc_free_digraph(&nng);
}

void scc_ut_findseeds_inwards_updating(void** state) {
	(void) state;

	scc_Digraph nng = scc_digraph_from_string("...*....../"
	                                          "...*....../"
	                                          "*........./"
	                                          ".*......../"
	                                          ".....*..../"
	                                          ".*......../"
	                                          "....*...../"
	                                          "......*.../"
	                                          "......*.../"
	                                          "......*.../");

	bool fp_assigned[10] = {true, true, true, true, true, true, true, true, false, false};
	bool fp_seed[10] = {false, false, true, true, true, false, false, true, false, false};
	scc_Clulab fp_cluster_label[10] = {0, 3, 0, 3, 2, 2, 1, 1, SCC_CLULAB_MAX, SCC_CLULAB_MAX};

	scc_Clustering cl_from_low = {
		.vertices = 10,
		.num_clusters = 0,
		.assigned = calloc(10, sizeof(bool)),
		.seed = calloc(10, sizeof(bool)),
		.cluster_label = malloc(sizeof(scc_Clulab[10])),
	};

	iscc_findseeds_inwards(&nng, &cl_from_low, true);
	assert_int_equal(cl_from_low.vertices, 10);
	assert_int_equal(cl_from_low.num_clusters, 4);
	assert_memory_equal(cl_from_low.assigned, fp_assigned, 10 * sizeof(bool));
	assert_memory_equal(cl_from_low.seed, fp_seed, 10 * sizeof(bool));
	assert_memory_equal(cl_from_low.cluster_label, fp_cluster_label, 8 * sizeof(bool));


	scc_Clustering cl_from_high = scc_base_clustering(&nng, inwards_updating);
	assert_int_equal(cl_from_high.vertices, 10);
	assert_int_equal(cl_from_high.num_clusters, 4);
	assert_memory_equal(cl_from_high.assigned, fp_assigned, 10 * sizeof(bool));
	assert_memory_equal(cl_from_high.seed, fp_seed, 10 * sizeof(bool));
	assert_memory_equal(cl_from_high.cluster_label, fp_cluster_label, 8 * sizeof(bool));

	scc_free_Clustering(&cl_from_low);
	scc_free_Clustering(&cl_from_high);

	scc_free_digraph(&nng);
}

void scc_ut_findseeds_exclusion(void** state) {
	(void) state;

	scc_Digraph nng = scc_digraph_from_string("...*....../"
	                                          "...*....../"
	                                          ".*......../"
	                                          ".*......../"
	                                          ".....*..../"
	                                          ".*......../"
	                                          "....*...../"
	                                          "......*.../"
	                                          "......*.../"
	                                          "......*.../");

	bool fp_assigned[10] = {true, true, true, true, true, true, true, true, false, false};
	bool fp_seed[10] = {true, false, true, false, true, false, false, true, false, false};
	scc_Clulab fp_cluster_label[10] = {0, 2, 2, 0, 1, 1, 3, 3, SCC_CLULAB_MAX, SCC_CLULAB_MAX};

	scc_Clustering cl_from_low = {
		.vertices = 10,
		.num_clusters = 0,
		.assigned = calloc(10, sizeof(bool)),
		.seed = calloc(10, sizeof(bool)),
		.cluster_label = malloc(sizeof(scc_Clulab[10])),
	};

	iscc_findseeds_exclusion(&nng, &cl_from_low, false);
	assert_int_equal(cl_from_low.vertices, 10);
	assert_int_equal(cl_from_low.num_clusters, 4);
	assert_memory_equal(cl_from_low.assigned, fp_assigned, 10 * sizeof(bool));
	assert_memory_equal(cl_from_low.seed, fp_seed, 10 * sizeof(bool));
	assert_memory_equal(cl_from_low.cluster_label, fp_cluster_label, 8 * sizeof(bool));


	scc_Clustering cl_from_high = scc_base_clustering(&nng, exclusion_order);
	assert_int_equal(cl_from_high.vertices, 10);
	assert_int_equal(cl_from_high.num_clusters, 4);
	assert_memory_equal(cl_from_high.assigned, fp_assigned, 10 * sizeof(bool));
	assert_memory_equal(cl_from_high.seed, fp_seed, 10 * sizeof(bool));
	assert_memory_equal(cl_from_high.cluster_label, fp_cluster_label, 8 * sizeof(bool));

	scc_free_Clustering(&cl_from_low);
	scc_free_Clustering(&cl_from_high);

	scc_free_digraph(&nng);
}

void scc_ut_findseeds_exclusion_updating(void** state) {
	(void) state;

	scc_Digraph nng = scc_digraph_from_string("...*....../"
	                                          "...*....../"
	                                          ".*......../"
	                                          ".*......../"
	                                          ".....*..../"
	                                          ".*......../"
	                                          "....*...../"
	                                          "......*.../"
	                                          "......*.../"
	                                          "......*.../");

	bool fp_assigned[10] = {true, true, true, true, true, true, true, true, false, false};
	bool fp_seed[10] = {true, false, true, false, true, false, false, true, false, false};
	scc_Clulab fp_cluster_label[10] = {0, 1, 1, 0, 2, 2, 3, 3, SCC_CLULAB_MAX, SCC_CLULAB_MAX};

	scc_Clustering cl_from_low = {
		.vertices = 10,
		.num_clusters = 0,
		.assigned = calloc(10, sizeof(bool)),
		.seed = calloc(10, sizeof(bool)),
		.cluster_label = malloc(sizeof(scc_Clulab[10])),
	};

	iscc_findseeds_exclusion(&nng, &cl_from_low, true);
	assert_int_equal(cl_from_low.vertices, 10);
	assert_int_equal(cl_from_low.num_clusters, 4);
	assert_memory_equal(cl_from_low.assigned, fp_assigned, 10 * sizeof(bool));
	assert_memory_equal(cl_from_low.seed, fp_seed, 10 * sizeof(bool));
	assert_memory_equal(cl_from_low.cluster_label, fp_cluster_label, 8 * sizeof(bool));


	scc_Clustering cl_from_high = scc_base_clustering(&nng, exclusion_updating);
	assert_int_equal(cl_from_high.vertices, 10);
	assert_int_equal(cl_from_high.num_clusters, 4);
	assert_memory_equal(cl_from_high.assigned, fp_assigned, 10 * sizeof(bool));
	assert_memory_equal(cl_from_high.seed, fp_seed, 10 * sizeof(bool));
	assert_memory_equal(cl_from_high.cluster_label, fp_cluster_label, 8 * sizeof(bool));

	scc_free_Clustering(&cl_from_low);
	scc_free_Clustering(&cl_from_high);

	scc_free_digraph(&nng);
}

void scc_ut_findseeds_checkseeds18(void** state) {
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

	scc_Clustering cl_seed_lexical = scc_base_clustering(&nng, lexical);
	bool ref_seed_lexical[18] = {true, false, false, false, false, true,
	                             true, false, false, false, true, false,
	                             true, false, false, false, false, false};
	assert_memory_equal(cl_seed_lexical.seed, ref_seed_lexical, 18 * sizeof(bool));

	scc_Clustering cl_seed_inwards = scc_base_clustering(&nng, inwards_order);
	bool ref_seed_inwards[18] = {false, false, true, false, false, false,
	                             true, false, false, false, false, true,
	                             true, true, false, false, false, false};
	assert_memory_equal(cl_seed_inwards.seed, ref_seed_inwards, 18 * sizeof(bool));

	scc_Clustering cl_seed_inupdat = scc_base_clustering(&nng, inwards_updating);
	bool ref_seed_inupdat[18] = {false, false, true, false, false, false,
	                             true, false, false, false, false, true,
	                             true, true, false, false, false, false};
	assert_memory_equal(cl_seed_inupdat.seed, ref_seed_inupdat, 18 * sizeof(bool));

	scc_Clustering cl_seed_exclusi = scc_base_clustering(&nng, exclusion_order);
	bool ref_seed_exclusi[18] = {true, false, false, false, false, false,
	                             false, false, true, false, false, false,
	                             false, true, false, true, false, false};
	assert_memory_equal(cl_seed_exclusi.seed, ref_seed_exclusi, 18 * sizeof(bool));

	scc_Clustering cl_seed_exupdat = scc_base_clustering(&nng, exclusion_updating);
	bool ref_seed_exupdat[18] = {false, false, false, false, true, false,
	                             false, false, true, false, true, false,
	                             false, false, false, true, false, true};
	assert_memory_equal(cl_seed_exupdat.seed, ref_seed_exupdat, 18 * sizeof(bool));

	scc_free_Clustering(&cl_seed_lexical);
	scc_free_Clustering(&cl_seed_inwards);
	scc_free_Clustering(&cl_seed_inupdat);
	scc_free_Clustering(&cl_seed_exclusi);
	scc_free_Clustering(&cl_seed_exupdat);
	scc_free_digraph(&nng);
}

int main(void) {
	const struct CMUnitTest test_findseeds[] = {
		cmocka_unit_test(scc_ut_findseeds_lexical),
		cmocka_unit_test(scc_ut_findseeds_inwards),
		cmocka_unit_test(scc_ut_findseeds_inwards_updating),
		cmocka_unit_test(scc_ut_findseeds_exclusion),
		cmocka_unit_test(scc_ut_findseeds_exclusion_updating),
		cmocka_unit_test(scc_ut_findseeds_checkseeds18),
	};

	return cmocka_run_group_tests_name("find seeds module", test_findseeds, NULL, NULL);
}

