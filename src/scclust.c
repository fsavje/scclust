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

#include "../include/scclust.h"

#include <assert.h>
#include <float.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "clustering.h"
#include "config.h"
#include "dist_search.h"
#include "error.h"


// ==============================================================================
// Internal variables
// ==============================================================================

/** The null clustering statistics struct.
 *
 *  This is an easily detectable invalid struct used as return value on errors.
 */
static const scc_ClusteringStats ISCC_NULL_CLUSTERING_STATS = { 0, 0, 0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };


// ==============================================================================
// External function implementations
// ==============================================================================

void scc_free_clustering(scc_Clustering** const clustering)
{
	if ((clustering != NULL) && (*clustering != NULL)) {
		if (!((*clustering)->external_labels)) free((*clustering)->cluster_label);
		free(*clustering);
		*clustering = NULL;
	}
}


scc_ErrorCode scc_init_empty_clustering(const uintmax_t num_data_points,
                                        scc_Clabel external_cluster_labels[const],
                                        scc_Clustering** const out_clustering)
{
	if (out_clustering == NULL) return iscc_make_error(SCC_ER_NULL_INPUT);
	// Initialize to null, so subsequent functions detect invalid clustering
	// if user doesn't check for errors.
	*out_clustering = NULL;

	if (num_data_points < 2) return iscc_make_error(SCC_ER_INVALID_INPUT);
	if (num_data_points > SCC_DPID_MAX) return iscc_make_error(SCC_ER_TOO_LARGE_PROBLEM);

	scc_Clustering* tmp_cl = malloc(sizeof(scc_Clustering));
	if (tmp_cl == NULL) return iscc_make_error(SCC_ER_NO_MEMORY);

	*tmp_cl = (scc_Clustering) {
		.num_data_points = (size_t) num_data_points,
		.num_clusters = 0,
		.cluster_label = external_cluster_labels,
		.external_labels = (external_cluster_labels != NULL),
		.clustering_version = ISCC_CURRENT_CLUSTSTRUCT_VERSION,
	};

	assert(iscc_check_input_clustering(tmp_cl));

	*out_clustering = tmp_cl;

	return iscc_no_error();
}


scc_ErrorCode scc_init_existing_clustering(const uintmax_t num_clusters,
                                           const uintmax_t num_data_points,
                                           scc_Clabel current_cluster_labels[const],
                                           const bool deep_label_copy,
                                           scc_Clustering** const out_clustering)
{
	if (out_clustering == NULL) return iscc_make_error(SCC_ER_NULL_INPUT);
	// Initialize to null, so subsequent functions detect invalid clustering
	// if user doesn't check for errors.
	*out_clustering = NULL;

	if (num_clusters == 0) return iscc_make_error(SCC_ER_INVALID_INPUT);
	if (num_clusters > SCC_CLABEL_MAX) return iscc_make_error(SCC_ER_TOO_LARGE_PROBLEM);
	if (num_data_points < 2) return iscc_make_error(SCC_ER_INVALID_INPUT);
	if (num_data_points > SCC_DPID_MAX) return iscc_make_error(SCC_ER_TOO_LARGE_PROBLEM);
	if (current_cluster_labels == NULL) return iscc_make_error(SCC_ER_NULL_INPUT);

	const size_t num_data_points_st = (size_t) num_data_points;

	scc_Clustering* tmp_cl = malloc(sizeof(scc_Clustering));
	if (tmp_cl == NULL) return iscc_make_error(SCC_ER_NO_MEMORY);

	*tmp_cl = (scc_Clustering) {
		.num_data_points = num_data_points_st,
		.num_clusters = (size_t) num_clusters,
		.cluster_label = NULL,
		.external_labels = !deep_label_copy,
		.clustering_version = ISCC_CURRENT_CLUSTSTRUCT_VERSION,
	};

	if (deep_label_copy) {
		tmp_cl->cluster_label = malloc(sizeof(scc_Clabel[num_data_points_st]));
		if (tmp_cl->cluster_label == NULL) {
			free(tmp_cl);
			return iscc_make_error(SCC_ER_NO_MEMORY);
		}
		memcpy(tmp_cl->cluster_label, current_cluster_labels, num_data_points_st * sizeof(scc_Clabel));
	} else {
		tmp_cl->cluster_label = current_cluster_labels;
	}

	// Checks if elements of `cluster_label` is well-behaved
	if (!scc_check_clustering(tmp_cl, true)) {
		scc_free_clustering(&tmp_cl);
		return iscc_make_error(SCC_ER_INVALID_CLUSTERING);
	}

	*out_clustering = tmp_cl;

	return iscc_no_error();
}


scc_ErrorCode scc_copy_clustering(const scc_Clustering* const in_clustering,
                                  scc_Clustering** const out_clustering)
{
	if (out_clustering == NULL) return iscc_make_error(SCC_ER_NULL_INPUT);
	// Initialize to null, so subsequent functions detect invalid clustering
	// if user doesn't check for errors.
	*out_clustering = NULL;

	if (!iscc_check_input_clustering(in_clustering)) return iscc_make_error(SCC_ER_INVALID_CLUSTERING);

	scc_Clustering* tmp_cl = malloc(sizeof(scc_Clustering));
	if (tmp_cl == NULL) return iscc_make_error(SCC_ER_NO_MEMORY);

	*tmp_cl = (scc_Clustering) {
		.num_data_points = in_clustering->num_data_points,
		.num_clusters = in_clustering->num_clusters,
		.cluster_label = malloc(sizeof(scc_Clabel[in_clustering->num_data_points])),
		.external_labels = false,
		.clustering_version = ISCC_CURRENT_CLUSTSTRUCT_VERSION,
	};

	if (tmp_cl->cluster_label == NULL) {
		free(&tmp_cl);
		return iscc_make_error(SCC_ER_NO_MEMORY);
	}

	memcpy(tmp_cl->cluster_label, in_clustering->cluster_label, in_clustering->num_data_points * sizeof(scc_Clabel));

	assert(iscc_check_input_clustering(tmp_cl));

	*out_clustering = tmp_cl;

	return iscc_no_error();
}


bool scc_check_clustering(const scc_Clustering* const cl,
                          const bool extensive_check)
{
	if (cl == NULL) return false;
	if (cl->num_data_points < 2) return false;
	if (cl->num_data_points > SCC_DPID_MAX) return false;
	if (cl->num_clusters > SCC_CLABEL_MAX) return false;
	if (cl->clustering_version != ISCC_CURRENT_CLUSTSTRUCT_VERSION) return false;
	
	if (cl->num_clusters != 0) {
		if (cl->cluster_label == NULL) return false;

		if (extensive_check) {
			for (size_t i = 0; i < cl->num_data_points; ++i) {
				if ((cl->cluster_label[i] > 0) && (cl->cluster_label[i] < cl->num_clusters)) continue;
				if (cl->cluster_label[i] == 0) continue; // Since `scc_Clabel` can be unsigned
				if (cl->cluster_label[i] == SCC_CLABEL_NA) continue;
				return false;
			}
		}
	}

	return true;
}


scc_ErrorCode scc_get_clustering_info(const scc_Clustering* const clustering,
                                      uintmax_t* const out_num_data_points,
                                      uintmax_t* const out_num_clusters)
{
	if (!iscc_check_input_clustering(clustering)) return iscc_make_error(SCC_ER_INVALID_CLUSTERING);
	if (out_num_data_points == NULL) return iscc_make_error(SCC_ER_NULL_INPUT);
	if (out_num_clusters == NULL) return iscc_make_error(SCC_ER_NULL_INPUT);

	*out_num_data_points = clustering->num_data_points;
	*out_num_clusters = clustering->num_clusters;

	return iscc_no_error();
}


scc_ErrorCode scc_get_cluster_labels(const scc_Clustering* const clustering,
                                     const size_t buffer_size,
                                     scc_Clabel out_label_buffer[const])
{
	if (!iscc_check_input_clustering(clustering)) return iscc_make_error(SCC_ER_INVALID_CLUSTERING);
	if (clustering->num_clusters == 0) return iscc_make_error(SCC_ER_EMPTY_CLUSTERING);
	if (buffer_size == 0) return iscc_make_error(SCC_ER_INVALID_INPUT);
	if (out_label_buffer == NULL) return iscc_make_error(SCC_ER_NULL_INPUT);

	size_t write = 0;
	for (; (write < buffer_size) && (write < clustering->num_data_points); ++write) {
		out_label_buffer[write] = clustering->cluster_label[write];
	}
	for (; write < buffer_size; ++write) {
		out_label_buffer[write] = SCC_CLABEL_NA;
	}

	return iscc_no_error();
}


scc_ErrorCode scc_get_clustering_stats(const scc_Clustering* const clustering,
                                       scc_DataSetObject* const data_set_object,
                                       scc_ClusteringStats* const out_stats2)
{
	if (out_stats2 == NULL) return iscc_make_error(SCC_ER_NULL_INPUT);
	*out_stats2 = ISCC_NULL_CLUSTERING_STATS;
	if (!iscc_check_input_clustering(clustering)) return iscc_make_error(SCC_ER_INVALID_CLUSTERING);
	if (clustering->num_clusters == 0) return iscc_make_error(SCC_ER_EMPTY_CLUSTERING);
	if (!iscc_check_data_set_object(data_set_object, clustering->num_data_points)) return iscc_make_error(SCC_ER_INVALID_DATA_OBJ);

	size_t* const cluster_size = calloc(clustering->num_clusters, sizeof(size_t));
	if (cluster_size == NULL) return iscc_make_error(SCC_ER_NO_MEMORY);

	for (size_t v = 0; v < clustering->num_data_points; ++v) {
		if (clustering->cluster_label[v] != SCC_CLABEL_NA) {
			++cluster_size[clustering->cluster_label[v]];
		}
	}

	scc_ClusteringStats tmp_stats = {
		.num_populated_clusters = 0,
		.num_assigned = 0,
		.min_cluster_size = UINT64_MAX,
		.max_cluster_size = 0,
		.avg_cluster_size = 0.0,
		.sum_dists = 0.0,
		.min_dist = DBL_MAX,
		.max_dist = 0.0,
		.cl_avg_min_dist = 0.0,
		.cl_avg_max_dist = 0.0,
		.cl_avg_dist_weighted = 0.0,
		.cl_avg_dist_unweighted = 0.0,
	};

	for (size_t c = 0; c < clustering->num_clusters; ++c) {
		if (cluster_size[c] == 0) continue;
		++tmp_stats.num_populated_clusters;
		tmp_stats.num_assigned += cluster_size[c];
		if (tmp_stats.min_cluster_size > cluster_size[c]) {
			tmp_stats.min_cluster_size = cluster_size[c];
		}
		if (tmp_stats.max_cluster_size < cluster_size[c]) {
			tmp_stats.max_cluster_size = cluster_size[c];
		}
	}

	if (tmp_stats.num_populated_clusters == 0) {
		free(cluster_size);
		return iscc_make_error(SCC_ER_EMPTY_CLUSTERING);
	}

	const size_t largest_dist_matrix = (tmp_stats.max_cluster_size * (tmp_stats.max_cluster_size - 1)) / 2;
	scc_Dpid* const id_store = malloc(sizeof(scc_Dpid[tmp_stats.num_assigned]));
	scc_Dpid** const cl_members = malloc(sizeof(scc_Dpid*[clustering->num_clusters]));
	double* const dist_scratch = malloc(sizeof(double[largest_dist_matrix]));
	if ((id_store == NULL) || (cl_members == NULL) || (dist_scratch == NULL)) {
		free(cluster_size);
		free(id_store);
		free(cl_members);
		free(dist_scratch);
		return iscc_make_error(SCC_ER_NO_MEMORY);
	}

	cl_members[0] = id_store + cluster_size[0];
	for (size_t c = 1; c < clustering->num_clusters; ++c) {
		cl_members[c] = cl_members[c - 1] + cluster_size[c];
	}

	assert(clustering->num_data_points < SCC_DPID_MAX);
	const scc_Dpid num_data_points = (scc_Dpid) clustering->num_data_points; // If `scc_Dpid` is signed
	for (scc_Dpid i = 0; i < num_data_points; ++i) {
		if (clustering->cluster_label[i] != SCC_CLABEL_NA) {
			--cl_members[clustering->cluster_label[i]];
			*(cl_members[clustering->cluster_label[i]]) = i;
		}
	}

	for (size_t c = 0; c < clustering->num_clusters; ++c) {
		if (cluster_size[c] < 2) {
			if (cluster_size[c] == 1) tmp_stats.min_dist = 0.0;
			continue;
		}
		
		const size_t size_dist_matrix = (cluster_size[c] * (cluster_size[c] - 1)) / 2;
		if (!iscc_get_dist_matrix(data_set_object, cluster_size[c], cl_members[c], dist_scratch)) {
			free(cluster_size);
			free(id_store);
			free(cl_members);
			free(dist_scratch);
			return iscc_make_error(SCC_ER_DIST_SEARCH_ERROR);
		}

		double cluster_sum_dists = dist_scratch[0];
		double cluster_min = dist_scratch[0];
		double cluster_max = dist_scratch[0];

		for (size_t d = 1; d < size_dist_matrix; ++d) {
			cluster_sum_dists += dist_scratch[d];
			if (cluster_min > dist_scratch[d]) {
				cluster_min = dist_scratch[d];
			}
			if (cluster_max < dist_scratch[d]) {
				cluster_max = dist_scratch[d];
			}
		}

		tmp_stats.sum_dists += cluster_sum_dists;

		if (tmp_stats.min_dist > cluster_min) {
			tmp_stats.min_dist = cluster_min;
		}
		if (tmp_stats.max_dist < cluster_max) {
			tmp_stats.max_dist = cluster_max;
		}
		tmp_stats.cl_avg_min_dist += cluster_min;
		tmp_stats.cl_avg_max_dist += cluster_max;

		tmp_stats.cl_avg_dist_weighted += ((double) cluster_size[c]) * cluster_sum_dists / ((double) size_dist_matrix);
		tmp_stats.cl_avg_dist_unweighted += cluster_sum_dists / ((double) size_dist_matrix);
	}

	tmp_stats.avg_cluster_size = ((double) tmp_stats.num_assigned) / ((double) tmp_stats.num_populated_clusters);
	tmp_stats.cl_avg_min_dist = tmp_stats.cl_avg_min_dist / ((double) tmp_stats.num_populated_clusters);
	tmp_stats.cl_avg_max_dist = tmp_stats.cl_avg_max_dist / ((double) tmp_stats.num_populated_clusters);
	tmp_stats.cl_avg_dist_weighted = tmp_stats.cl_avg_dist_weighted / ((double) tmp_stats.num_assigned);
	tmp_stats.cl_avg_dist_unweighted = tmp_stats.cl_avg_dist_unweighted / ((double) tmp_stats.num_populated_clusters);

	free(cluster_size);
	free(id_store);
	free(cl_members);
	free(dist_scratch);

	*out_stats2 = tmp_stats;

	return iscc_no_error();
}