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
#include <stdlib.h>
#include <time.h>
#include "../include/scclust.h"

const size_t SAMPLE_SIZE = 10000000; // 10^7
const size_t DATA_DIMENSION = 3;

int main(void) {
	// Variable for error reporting
	scc_ErrorCode ec;

	printf("Generating data: ");

	const size_t DATA_SIZE = SAMPLE_SIZE * DATA_DIMENSION;

	double* const data_set = malloc(sizeof(double[DATA_SIZE]));
	scc_Clabel* const cluster_labels = malloc(sizeof(scc_Clabel[SAMPLE_SIZE]));
	if ((data_set == NULL) || (cluster_labels == NULL)) {
		free(data_set);
		free(cluster_labels);
		return 1;
	}

	for (size_t i = 0; i < DATA_SIZE; ++i) {
		data_set[i] = ((double) rand()) / ((double) RAND_MAX);
	}

	printf("DONE.\n");
	printf("Deriving clustering: \n");

	const clock_t begin = clock();

	// Construct scclust data set object
	scc_DataSetObject* data_set_object;
	ec = scc_get_data_set_object(SAMPLE_SIZE,
	                             DATA_DIMENSION,
	                             DATA_SIZE,
	                             data_set,
	                             false,
	                             &data_set_object);
	if(ec != SCC_ER_OK) {
		free(data_set);
		free(cluster_labels);
		return 1;
	}

	// Make empty clustering to populate
	scc_Clustering* cl;
	ec = scc_init_empty_clustering(SAMPLE_SIZE, cluster_labels, &cl);
	if(ec != SCC_ER_OK) {
		free(data_set);
		free(cluster_labels);
		return 1;
	}

	ec = scc_nng_clustering_batches(cl,
	                                data_set_object,
	                                1000,
	                                SCC_UM_ASSIGN_BY_NNG,
	                                false,
	                                0.0,
	                                0,
	                                NULL,
	                                100);
	if(ec != SCC_ER_OK) {
		free(data_set);
		free(cluster_labels);
		return 1;
	}

	const clock_t end = clock();
	const double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

	printf("DONE.\n");
	printf("Time spent on clustering: %f seconds.\n", time_spent);

	printf("Cluster assignment (first 1000):\n");
	for(size_t i = 0; i < 1000; ++i) {
		printf("%d ", cluster_labels[i]);
	}
	printf("\n");

	// Free cluster and data set objects
	scc_free_clustering(&cl);
	scc_free_data_set_object(&data_set_object);
	free(data_set);
	free(cluster_labels);
}
