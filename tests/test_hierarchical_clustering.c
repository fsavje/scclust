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
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <include/scclust.h>
#include <src/clustering_struct.h>
#include <src/scclust_types.h>
#include "data_object_test.h"

/*
R code:
break_cluster <- function(search, k, dist, batch) {
   visited <- NULL
   best <- c(lookup = NA, index = NA, dist = NA)
   centers_lookup <- search[seq(1, length(search), 2)]
   while (length(centers_lookup) > 0) {
      visited <- c(centers_lookup, visited)
      centers_index <- search[as.vector(apply(dist[centers_lookup, search, drop = FALSE], 1, order, decreasing = TRUE)[1, ])]
      centers_dist <- as.vector(apply(dist[centers_lookup, search, drop = FALSE], 1, sort, decreasing = TRUE)[1, ])
      if (is.na(best["dist"]) || (best["dist"] < centers_dist[order(centers_dist, decreasing = TRUE)[1]])) {
         best <- c(lookup = centers_lookup[order(centers_dist, decreasing = TRUE)[1]],
                   index = centers_index[order(centers_dist, decreasing = TRUE)[1]],
                   dist = centers_dist[order(centers_dist, decreasing = TRUE)[1]])
      }
      centers_lookup <- unique(centers_index[!(centers_index %in% visited)])
   }

   center1 <- as.vector(best["lookup"])
   clust1 <- center1
   center2 <- as.vector(best["index"])
   clust2 <- center2
   search <- setdiff(search, c(center1, center2))

   if (as.vector(sort(dist[center1, search]))[k - 1] >= as.vector(sort(dist[center2, search]))[k - 1]) {
      clust1 <- c(clust1, search[order(dist[center1, search])[1:(k - 1)]])
      search <- setdiff(search, clust1)
      clust2 <- c(clust2, search[order(dist[center2, search])[1:(k - 1)]])
      search <- setdiff(search, clust2)
   } else {
      clust2 <- c(clust2, search[order(dist[center2, search])[1:(k - 1)]])
      search <- setdiff(search, clust2)
      clust1 <- c(clust1, search[order(dist[center1, search])[1:(k - 1)]])
      search <- setdiff(search, clust1)
   }

   remaining_k <- 1
   if (batch) remaining_k <- k
   while (length(search) > 0) {
      if (length(search) < remaining_k) remaining_k <- length(search)
      if (as.vector(sort(dist[center1, search]))[remaining_k] <= as.vector(sort(dist[center2, search]))[remaining_k]) {
         clust1 <- c(clust1, search[order(dist[center1, search])[1:remaining_k]])
         search <- setdiff(search, clust1)
      } else {
         clust2 <- c(clust2, search[order(dist[center2, search])[1:remaining_k]])
         search <- setdiff(search, clust2)
      }
   }

   list(clust1, rev(clust2))
}

hierarchical_clustering <- function(in_clustering, k, dist, batch) {
   cl_stack <- list()
   if (length(unique(in_clustering)) == 1) {
      cl_stack <- list(1:length(in_clustering))
   } else {
      for (i in 0:max(in_clustering)) {
         cl_stack <- c(cl_stack, list(rev(which(in_clustering == i))))
      }
   }

   out_cl <- rep(NA, length(in_clustering))
   cl_label <- 0
   while (length(cl_stack) > 0) {
      to_run <- cl_stack[[length(cl_stack)]]
      cl_stack <- cl_stack[-length(cl_stack)]
      if (length(to_run) < 2 * k) {
         out_cl[to_run] <- cl_label
         cl_label <- cl_label + 1
      } else {
         cl_stack <- c(cl_stack, break_cluster(to_run, k, dist, batch))
      }
   }
   out_cl
}
*/

void scc_ut_hierarchical_clustering(void** state)
{
	(void) state;

	scc_Clabel ref_label1[100] = { 2, 3, 3, 2, 2, 3, 0, 0, 4, 3, 2, 1, 1, 0, 4, 3, 0, 2, 0, 4, 3, 1, 3,
	                               0, 0, 0, 4, 0, 4, 0, 3, 4, 3, 1, 0, 0, 3, 4, 1, 0, 3, 2, 1, 2, 2, 2,
	                               0, 2, 1, 4, 3, 4, 4, 4, 1, 0, 3, 1, 3, 1, 1, 1, 4, 4, 3, 4, 4, 1, 2,
	                               3, 1, 4, 2, 3, 4, 0, 0, 4, 1, 1, 3, 2, 1, 0, 2, 1, 0, 2, 2, 2, 4, 2,
	                               1, 2, 2, 1, 3, 4, 3, 0 };

	scc_Clustering* cl1_a;
	scc_init_empty_clustering(100, NULL, &cl1_a);
	scc_ErrorCode ec1_a = scc_hierarchical_clustering(cl1_a, scc_ut_test_data_large, 20, true);
	assert_int_equal(ec1_a, SCC_ER_OK);
	assert_int_equal(cl1_a->num_data_points, 100);
	assert_int_equal(cl1_a->num_clusters, 5);
	assert_memory_equal(cl1_a->cluster_label, ref_label1, 100 * sizeof(scc_Clabel));
	scc_free_clustering(&cl1_a);

	scc_Clabel* ext_cluster_label1 = malloc(sizeof(scc_Clabel[100]));
	scc_Clustering* cl1_b;
	scc_init_empty_clustering(100, ext_cluster_label1, &cl1_b);
	scc_ErrorCode ec1_b = scc_hierarchical_clustering(cl1_b, scc_ut_test_data_large, 20, true);
	assert_int_equal(ec1_b, SCC_ER_OK);
	assert_int_equal(cl1_b->num_data_points, 100);
	assert_int_equal(cl1_b->num_clusters, 5);
	assert_memory_equal(cl1_b->cluster_label, ref_label1, 100 * sizeof(scc_Clabel));
	assert_memory_equal(ext_cluster_label1, ref_label1, 100 * sizeof(scc_Clabel));
	scc_free_clustering(&cl1_b);
	free(ext_cluster_label1);


	scc_Clabel ref_label2[100] = { 3, 0, 2, 3, 3, 2, 1, 1, 3, 2, 3, 0, 1, 0, 2, 2, 1, 3, 0, 2, 2, 0, 1, 1, 0, 1, 2, 1, 2, 0,
	                               2, 2, 2, 0, 1, 1, 2, 2, 0, 0, 3, 3, 0, 3, 3, 0, 1, 3, 0, 2, 0, 2, 2, 2, 0, 0, 2, 0, 2, 1,
	                               0, 0, 3, 3, 3, 3, 1, 1, 3, 2, 0, 1, 3, 2, 2, 1, 1, 3, 0, 0, 2, 3, 0, 1, 3, 0, 1, 3, 3, 3,
	                               3, 2, 0, 3, 3, 1, 0, 3, 2, 0 };

	scc_Clustering* cl2_a;
	scc_init_empty_clustering(100, NULL, &cl2_a);
	scc_ErrorCode ec2_a = scc_hierarchical_clustering(cl2_a, scc_ut_test_data_large, 20, false);
	assert_int_equal(ec2_a, SCC_ER_OK);
	assert_int_equal(cl2_a->num_data_points, 100);
	assert_int_equal(cl2_a->num_clusters, 4);
	assert_memory_equal(cl2_a->cluster_label, ref_label2, 100 * sizeof(scc_Clabel));
	scc_free_clustering(&cl2_a);

	scc_Clabel* ext_cluster_label2 = malloc(sizeof(scc_Clabel[100]));
	scc_Clustering* cl2_b;
	scc_init_empty_clustering(100, ext_cluster_label2, &cl2_b);
	scc_ErrorCode ec2_b = scc_hierarchical_clustering(cl2_b, scc_ut_test_data_large, 20, false);
	assert_int_equal(ec2_b, SCC_ER_OK);
	assert_int_equal(cl2_b->num_data_points, 100);
	assert_int_equal(cl2_b->num_clusters, 4);
	assert_memory_equal(cl2_b->cluster_label, ref_label2, 100 * sizeof(scc_Clabel));
	assert_memory_equal(ext_cluster_label2, ref_label2, 100 * sizeof(scc_Clabel));
	scc_free_clustering(&cl2_b);
	free(ext_cluster_label2);


	scc_Clabel cluster_label3[100] = { 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0,
	                                   0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0,
	                                   1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 0,
	                                   0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0,
	                                   0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1,
	                                   0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0,
	                                   0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0 };
	scc_Clustering* cl3;
	scc_init_existing_clustering(100, 2, cluster_label3, true, &cl3);
	scc_ErrorCode ec3 = scc_hierarchical_clustering(cl3, scc_ut_test_data_large, 20, true);
	assert_int_equal(ec3, SCC_ER_OK);
	scc_Clabel ref_label3[100] = { 1, 1, 3, 3, 3, 0, 0, 2, 0, 0, 3, 1, 2, 2, 3, 1, 0, 3, 1, 0, 0, 2, 1, 1, 1,
	                               0, 1, 2, 0, 2, 3, 0, 0, 1, 2, 0, 3, 2, 1, 1, 1, 3, 2, 3, 1, 2, 2, 2, 1, 0,
	                               2, 2, 3, 3, 2, 2, 3, 1, 0, 2, 1, 2, 1, 0, 3, 3, 2, 2, 1, 0, 2, 0, 3, 1, 3,
	                               0, 2, 1, 1, 1, 2, 1, 1, 2, 3, 1, 2, 2, 1, 1, 0, 1, 2, 3, 2, 2, 1, 3, 0, 2 };
	assert_int_equal(cl3->num_data_points, 100);
	assert_int_equal(cl3->num_clusters, 4);
	assert_memory_equal(cl3->cluster_label, ref_label3, 100 * sizeof(scc_Clabel));
	scc_free_clustering(&cl3);


	scc_Clabel cluster_label4[100] = { 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0,
	                                   0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0,
	                                   1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 0,
	                                   0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0,
	                                   0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1,
	                                   0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0,
	                                   0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0 };
	scc_Clustering* cl4;
	scc_init_existing_clustering(100, 2, cluster_label4, false, &cl4);
	scc_ErrorCode ec4 = scc_hierarchical_clustering(cl4, scc_ut_test_data_large, 20, false);
	assert_int_equal(ec4, SCC_ER_OK);
	scc_Clabel ref_label4[100] = { 1, 0, 3, 3, 3, 0, 0, 2, 0, 0, 3, 0, 2, 2, 3, 1, 0, 3, 1, 0, 0, 2, 1, 1, 1,
	                               0, 0, 2, 0, 2, 3, 0, 0, 1, 2, 0, 3, 3, 0, 0, 1, 3, 2, 3, 1, 2, 2, 2, 0, 0,
	                               2, 3, 3, 3, 2, 2, 3, 1, 0, 2, 1, 2, 1, 0, 3, 3, 2, 2, 1, 0, 2, 0, 3, 0, 3,
	                               0, 2, 1, 1, 1, 3, 1, 1, 2, 3, 1, 2, 3, 1, 1, 0, 1, 2, 3, 3, 2, 1, 3, 0, 2 };
	assert_int_equal(cl4->num_data_points, 100);
	assert_int_equal(cl4->num_clusters, 4);
	assert_memory_equal(cl4->cluster_label, ref_label4, 100 * sizeof(scc_Clabel));
	assert_memory_equal(cluster_label4, ref_label4, 100 * sizeof(scc_Clabel));
	scc_free_clustering(&cl4);
}


int main(void)
{
	if(!scc_ut_init_tests()) return 1;

	const struct CMUnitTest test_cases[] = {
		cmocka_unit_test(scc_ut_hierarchical_clustering),
	};

	return cmocka_run_group_tests_name("hierarchical_clustering.c", test_cases, NULL, NULL);
}
