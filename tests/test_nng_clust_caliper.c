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

#include <stddef.h>
#include <stdbool.h>
#include "../include/scclust.h"
#include "../src/clustering.h"
#include "../src/digraph_core.h"
#include "../src/digraph_debug.h"
#include "../src/nng_core.h"
#include "../src/nng_findseeds.h"


#ifdef SCC_STABLE_CLUSTERING
    #error Please run this test without the SCC_STABLE_CLUSTERING flag
#endif


void scc_ut_findseeds_caliper(void** state)
{
	(void) state;

	iscc_Digraph nng = iscc_digraph_from_string("..... ...../"
	                                          "..... #.#../"
	                                          "..... #..../"
	                                          "..... .##../"
	                                          "..... ...../"

	                                          "..... ...../"
	                                          ".#... ..#../"
	                                          "..#.# ...../"
	                                          "...## ....#/"
	                                          "...#. .#.../");

	scc_Dpid ref_seeds_lexical[2] = {1, 8};
	iscc_SeedClustering clustering_lexical = iscc_findseeds_lexical(&nng, 1);
	assert_int_equal(clustering_lexical.num_data_points, 10);
	assert_int_equal(clustering_lexical.num_clusters, 2);
	assert_int_equal(clustering_lexical.seed_capacity, 2);
	assert_null(clustering_lexical.assigned);
	assert_non_null(clustering_lexical.seeds);
	assert_memory_equal(clustering_lexical.seeds, ref_seeds_lexical, 2 * sizeof(scc_Dpid));
	assert_null(clustering_lexical.cluster_label);

	scc_Dpid ref_seeds_inwards[2] = {8, 1};
	iscc_SeedClustering clustering_inwards = iscc_findseeds_inwards(&nng, 1, false);
	assert_int_equal(clustering_inwards.num_data_points, 10);
	assert_int_equal(clustering_inwards.num_clusters, 2);
	assert_int_equal(clustering_inwards.seed_capacity, 2);
	assert_null(clustering_inwards.assigned);
	assert_non_null(clustering_inwards.seeds);
	assert_memory_equal(clustering_inwards.seeds, ref_seeds_inwards, 2 * sizeof(scc_Dpid));
	assert_null(clustering_inwards.cluster_label);

	scc_Dpid ref_seeds_inupdat[3] = {8, 6, 2};
	iscc_SeedClustering clustering_inupdat = iscc_findseeds_inwards(&nng, 1, true);
	assert_int_equal(clustering_inupdat.num_data_points, 10);
	assert_int_equal(clustering_inupdat.num_clusters, 3);
	assert_int_equal(clustering_inupdat.seed_capacity, 3);
	assert_null(clustering_inupdat.assigned);
	assert_non_null(clustering_inupdat.seeds);
	assert_memory_equal(clustering_inupdat.seeds, ref_seeds_inupdat, 3 * sizeof(scc_Dpid));
	assert_null(clustering_inupdat.cluster_label);

	scc_Dpid ref_seeds_exclusi[3] = {2, 8, 6};
	iscc_SeedClustering clustering_exclusi = iscc_findseeds_exclusion(&nng, 1, false);
	assert_int_equal(clustering_exclusi.num_data_points, 10);
	assert_int_equal(clustering_exclusi.num_clusters, 3);
	assert_int_equal(clustering_exclusi.seed_capacity, 3);
	assert_null(clustering_exclusi.assigned);
	assert_non_null(clustering_exclusi.seeds);
	assert_memory_equal(clustering_exclusi.seeds, ref_seeds_exclusi, 3 * sizeof(scc_Dpid));
	assert_null(clustering_exclusi.cluster_label);

	scc_Dpid ref_seeds_exupdat[3] = {2, 8, 6};
	iscc_SeedClustering clustering_exupdat = iscc_findseeds_exclusion(&nng, 1, true);
	assert_int_equal(clustering_exupdat.num_data_points, 10);
	assert_int_equal(clustering_exupdat.num_clusters, 3);
	assert_int_equal(clustering_exupdat.seed_capacity, 3);
	assert_null(clustering_exupdat.assigned);
	assert_non_null(clustering_exupdat.seeds);
	assert_memory_equal(clustering_exupdat.seeds, ref_seeds_exupdat, 3 * sizeof(scc_Dpid));
	assert_null(clustering_exupdat.cluster_label);

	iscc_free_digraph(&nng);
	iscc_free_seed_clustering(&clustering_lexical);
	iscc_free_seed_clustering(&clustering_inwards);
	iscc_free_seed_clustering(&clustering_inupdat);
	iscc_free_seed_clustering(&clustering_exclusi);
	iscc_free_seed_clustering(&clustering_exupdat);
}


void scc_ut_nng_caliper_ignore(void** state)
{
	(void) state;

	iscc_Digraph nng = iscc_digraph_from_string("..... ...../"
	                                          "..... #.#../"
	                                          "..... #..../"
	                                          "..... .##../"
	                                          "..... ...../"

	                                          "..... ...../"
	                                          ".#... ..#../"
	                                          "..#.# ...../"
	                                          "...## ....#/"
	                                          "...#. .#.../");

	scc_Clabel ref_lexical_cluster_label[10] = {SCC_CLABEL_NA, 0, SCC_CLABEL_NA, 1, 1, 0, SCC_CLABEL_NA, 0, 1, 1};
	iscc_SeedClustering lexical_tmp_clustering = iscc_get_seed_clustering(&nng, SCC_FINDSEEDS_LEXICAL, 1, NULL);
	scc_Clustering lexical_clustering = iscc_ignore_remaining(&lexical_tmp_clustering);
	assert_int_equal(lexical_clustering.num_data_points, 10);
	assert_int_equal(lexical_clustering.num_clusters, 2);
	assert_false(lexical_clustering.external_labels);
	assert_memory_equal(lexical_clustering.cluster_label, ref_lexical_cluster_label, 10 * sizeof(scc_Clabel));

	scc_Clabel ref_inwards_cluster_label[10] = {SCC_CLABEL_NA, 1, SCC_CLABEL_NA, 0, 0, 1, SCC_CLABEL_NA, 1, 0, 0};
	iscc_SeedClustering inwards_tmp_clustering = iscc_get_seed_clustering(&nng, SCC_FINDSEEDS_INWARDS_ORDER, 1, NULL);
	scc_Clustering inwards_clustering = iscc_ignore_remaining(&inwards_tmp_clustering);
	assert_int_equal(inwards_clustering.num_data_points, 10);
	assert_int_equal(inwards_clustering.num_clusters, 2);
	assert_false(inwards_clustering.external_labels);
	assert_memory_equal(inwards_clustering.cluster_label, ref_inwards_cluster_label, 10 * sizeof(scc_Clabel));

	scc_Clabel ref_inupdat_cluster_label[10] = {SCC_CLABEL_NA, 1, 2, 0, 0, 2, 1, 1, 0, 0};
	iscc_SeedClustering inupdat_tmp_clustering = iscc_get_seed_clustering(&nng, SCC_FINDSEEDS_INWARDS_UPDATING, 1, NULL);
	scc_Clustering inupdat_clustering = iscc_ignore_remaining(&inupdat_tmp_clustering);
	assert_int_equal(inupdat_clustering.num_data_points, 10);
	assert_int_equal(inupdat_clustering.num_clusters, 3);
	assert_false(inupdat_clustering.external_labels);
	assert_memory_equal(inupdat_clustering.cluster_label, ref_inupdat_cluster_label, 10 * sizeof(scc_Clabel));

	scc_Clabel ref_exclusi_cluster_label[10] = {SCC_CLABEL_NA, 2, 0, 1, 1, 0, 2, 2, 1, 1};
	iscc_SeedClustering exclusi_tmp_clustering = iscc_get_seed_clustering(&nng, SCC_FINDSEEDS_EXCLUSION_ORDER, 1, NULL);
	scc_Clustering exclusi_clustering = iscc_ignore_remaining(&exclusi_tmp_clustering);
	assert_int_equal(exclusi_clustering.num_data_points, 10);
	assert_int_equal(exclusi_clustering.num_clusters, 3);
	assert_false(exclusi_clustering.external_labels);
	assert_memory_equal(exclusi_clustering.cluster_label, ref_exclusi_cluster_label, 10 * sizeof(scc_Clabel));

	scc_Clabel ref_exupdat_cluster_label[10] = {SCC_CLABEL_NA, 2, 0, 1, 1, 0, 2, 2, 1, 1};
	iscc_SeedClustering exupdat_tmp_clustering = iscc_get_seed_clustering(&nng, SCC_FINDSEEDS_EXCLUSION_UPDATING, 1, NULL);
	scc_Clustering exupdat_clustering = iscc_ignore_remaining(&exupdat_tmp_clustering);
	assert_int_equal(exupdat_clustering.num_data_points, 10);
	assert_int_equal(exupdat_clustering.num_clusters, 3);
	assert_false(exupdat_clustering.external_labels);
	assert_memory_equal(exupdat_clustering.cluster_label, ref_exupdat_cluster_label, 10 * sizeof(scc_Clabel));

	iscc_free_digraph(&nng);
	iscc_free_seed_clustering(&lexical_tmp_clustering);
	scc_free_clustering(&lexical_clustering);
	iscc_free_seed_clustering(&inwards_tmp_clustering);
	scc_free_clustering(&inwards_clustering);
	iscc_free_seed_clustering(&inupdat_tmp_clustering);
	scc_free_clustering(&inupdat_clustering);
	iscc_free_seed_clustering(&exclusi_tmp_clustering);
	scc_free_clustering(&exclusi_clustering);
	iscc_free_seed_clustering(&exupdat_tmp_clustering);
	scc_free_clustering(&exupdat_clustering);
}


void scc_ut_nng_caliper_assign(void** state)
{
	(void) state;

	iscc_Digraph nng = iscc_digraph_from_string("..... ...../"
	                                          "..... #.#../"
	                                          "..... #..../"
	                                          "..... .##../"
	                                          "..... ...../"

	                                          "..... ...../"
	                                          ".#... ..#../"
	                                          "..#.# ...../"
	                                          "...## ....#/"
	                                          "...#. .#.../");

	scc_Clabel ref_lexical_cluster_label[10] = {SCC_CLABEL_NA, 0, 0, 1, 1, 0, 0, 0, 1, 1};
	iscc_SeedClustering lexical_tmp_clustering = iscc_get_seed_clustering(&nng, SCC_FINDSEEDS_LEXICAL, 1, NULL);
	scc_Clustering lexical_clustering = iscc_assign_remaining_lexical(&lexical_tmp_clustering, &nng);
	assert_int_equal(lexical_clustering.num_data_points, 10);
	assert_int_equal(lexical_clustering.num_clusters, 2);
	assert_false(lexical_clustering.external_labels);
	assert_memory_equal(lexical_clustering.cluster_label, ref_lexical_cluster_label, 10 * sizeof(scc_Clabel));

	scc_Clabel ref_inwards_cluster_label[10] = {SCC_CLABEL_NA, 1, 1, 0, 0, 1, 1, 1, 0, 0};
	iscc_SeedClustering inwards_tmp_clustering = iscc_get_seed_clustering(&nng, SCC_FINDSEEDS_INWARDS_ORDER, 1, NULL);
	scc_Clustering inwards_clustering = iscc_assign_remaining_lexical(&inwards_tmp_clustering, &nng);
	assert_int_equal(inwards_clustering.num_data_points, 10);
	assert_int_equal(inwards_clustering.num_clusters, 2);
	assert_false(inwards_clustering.external_labels);
	assert_memory_equal(inwards_clustering.cluster_label, ref_inwards_cluster_label, 10 * sizeof(scc_Clabel));

	scc_Clabel ref_inupdat_cluster_label[10] = {SCC_CLABEL_NA, 1, 2, 0, 0, 2, 1, 1, 0, 0};
	iscc_SeedClustering inupdat_tmp_clustering = iscc_get_seed_clustering(&nng, SCC_FINDSEEDS_INWARDS_UPDATING, 1, NULL);
	scc_Clustering inupdat_clustering = iscc_assign_remaining_lexical(&inupdat_tmp_clustering, &nng);
	assert_int_equal(inupdat_clustering.num_data_points, 10);
	assert_int_equal(inupdat_clustering.num_clusters, 3);
	assert_false(inupdat_clustering.external_labels);
	assert_memory_equal(inupdat_clustering.cluster_label, ref_inupdat_cluster_label, 10 * sizeof(scc_Clabel));

	scc_Clabel ref_exclusi_cluster_label[10] = {SCC_CLABEL_NA, 2, 0, 1, 1, 0, 2, 2, 1, 1};
	iscc_SeedClustering exclusi_tmp_clustering = iscc_get_seed_clustering(&nng, SCC_FINDSEEDS_EXCLUSION_ORDER, 1, NULL);
	scc_Clustering exclusi_clustering = iscc_assign_remaining_lexical(&exclusi_tmp_clustering, &nng);
	assert_int_equal(exclusi_clustering.num_data_points, 10);
	assert_int_equal(exclusi_clustering.num_clusters, 3);
	assert_false(exclusi_clustering.external_labels);
	assert_memory_equal(exclusi_clustering.cluster_label, ref_exclusi_cluster_label, 10 * sizeof(scc_Clabel));

	scc_Clabel ref_exupdat_cluster_label[10] = {SCC_CLABEL_NA, 2, 0, 1, 1, 0, 2, 2, 1, 1};
	iscc_SeedClustering exupdat_tmp_clustering = iscc_get_seed_clustering(&nng, SCC_FINDSEEDS_EXCLUSION_UPDATING, 1, NULL);
	scc_Clustering exupdat_clustering = iscc_assign_remaining_lexical(&exupdat_tmp_clustering, &nng);
	assert_int_equal(exupdat_clustering.num_data_points, 10);
	assert_int_equal(exupdat_clustering.num_clusters, 3);
	assert_false(exupdat_clustering.external_labels);
	assert_memory_equal(exupdat_clustering.cluster_label, ref_exupdat_cluster_label, 10 * sizeof(scc_Clabel));

	iscc_free_digraph(&nng);
	iscc_free_seed_clustering(&lexical_tmp_clustering);
	scc_free_clustering(&lexical_clustering);
	iscc_free_seed_clustering(&inwards_tmp_clustering);
	scc_free_clustering(&inwards_clustering);
	iscc_free_seed_clustering(&inupdat_tmp_clustering);
	scc_free_clustering(&inupdat_clustering);
	iscc_free_seed_clustering(&exclusi_tmp_clustering);
	scc_free_clustering(&exclusi_clustering);
	iscc_free_seed_clustering(&exupdat_tmp_clustering);
	scc_free_clustering(&exupdat_clustering);
}


int main(void)
{
	const struct CMUnitTest test_caliper[] = {
		cmocka_unit_test(scc_ut_findseeds_caliper),
		cmocka_unit_test(scc_ut_nng_caliper_ignore),
		cmocka_unit_test(scc_ut_nng_caliper_assign),
	};

	return cmocka_run_group_tests_name("caliper module", test_caliper, NULL, NULL);
}
