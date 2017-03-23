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

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <scclust.h>
#include "ann_wrapper.h"


int main(void) {

	// Use ANN library for NN search
	if(!scc_set_ann_dist_search()) return 1;

	// Data: 10 two-dimensional points
	double raw_data[20] = { 0.088316633,  0.807443027,
	                       -1.080004390,  0.969638235,
	                        1.316503268,  1.492648875,
	                        0.140829264, -0.100874408,
	                       -0.832028346,  0.655380698,
	                        1.392129281,  0.885861553,
	                       -2.764608404, -0.449692089,
	                        0.296104717,  0.943822811,
	                       -0.641513488,  0.417153186,
	                        1.509341003, -0.026534633 };

	// Error code variable
	scc_ErrorCode ec;

	// Construct scclust data set object
	scc_DataSet* data_set;
	ec = scc_init_data_set(10,           // Number of data points
	                       2,            // Number of dimensions
	                       20,           // Length of data matrix
	                       raw_data,     // Data matrix
	                       &data_set);   // Data set to initialize
	// Check error code
	if(ec != SCC_ER_OK) return 1;

	// Make empty clustering object
	scc_Clabel cluster_labels[10];
	scc_Clustering* clustering;
	ec = scc_init_empty_clustering(10,               // Number of data points
	                               cluster_labels,   // Clustering labels
	                               &clustering);     // Clustering to initialize
	if(ec != SCC_ER_OK) return 1;

	// Set clustering options (start with defaults)
	scc_ClusterOptions options = scc_default_cluster_options;
	// At least 3 data points in each cluster
	options.size_constraint = 3;

	// Make clustering
	ec = scc_sc_clustering(data_set, &options, clustering);
	if(ec != SCC_ER_OK) return 1;

	// Print clustering
	printf("Cluster assignment:\n");
	for(size_t i = 0; i < 10; ++i) {
		printf("%d ", cluster_labels[i]);
	}
	printf("\n");

	// Free clustering and data set objects
	scc_free_clustering(&clustering);
	scc_free_data_set(&data_set);
}
