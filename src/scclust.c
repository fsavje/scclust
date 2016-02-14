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
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "clustering.h"
#include "dist_search.h"



 /** The null clustering statistics struct.
 *
 *  This is an easily detectable invalid struct used as return value on errors.
 */
static const scc_ClusteringStats SCC_NULL_CLUSTERING_STATS = { 0, 0, 0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };


// ==============================================================================
// External function implementations
// ==============================================================================

void scc_free_clustering(scc_Clustering* const cl)
{
	if (cl != NULL) {
		if (!cl->external_labels) free(cl->cluster_label);
		*cl = SCC_NULL_CLUSTERING;
	}
}

scc_Clustering scc_init_empty_clustering(const size_t num_data_points,
                                         scc_Clabel external_cluster_labels[const])
{
	if (num_data_points < 2) return SCC_NULL_CLUSTERING;

	return (scc_Clustering) {
		.vertices = num_data_points,
		.num_clusters = 0,
		.external_labels = (external_cluster_labels != NULL),
		.cluster_label = external_cluster_labels,
	};
}

scc_Clustering scc_init_existing_clustering(const size_t num_data_points,
                                            const size_t num_clusters,
                                            scc_Clabel current_cluster_labels[const],
                                            const bool deep_label_copy)
{
	if (num_data_points < 2) return SCC_NULL_CLUSTERING;
	if (num_clusters == 0) return SCC_NULL_CLUSTERING;
	if (current_cluster_labels == NULL) return SCC_NULL_CLUSTERING;

	scc_Clustering out_cl = {
		.vertices = num_data_points,
		.num_clusters = num_clusters,
		.external_labels = !deep_label_copy,
		.cluster_label = NULL,
	};

	if (deep_label_copy) {
		out_cl.cluster_label = malloc(sizeof(scc_Clabel[num_data_points]));
		if (out_cl.cluster_label == NULL) return SCC_NULL_CLUSTERING;
		memcpy(out_cl.cluster_label, current_cluster_labels, num_data_points * sizeof(scc_Clabel));
	} else {
		out_cl.cluster_label = current_cluster_labels;
	}

	if (!iscc_check_input_clustering(&out_cl)) {
		scc_free_clustering(&out_cl);
		return SCC_NULL_CLUSTERING;
	}

	return out_cl;
}


bool scc_check_clustering(const scc_Clustering* const cl,
                          const bool extensive_check)
{
	if (cl == NULL) return false;
	if (cl->vertices < 2) return false;
	if (cl->vertices >= SCC_VID_MAX) return false;
	if (cl->num_clusters >= SCC_CLABEL_MAX) return false;
	
	if (cl->num_clusters != 0) {
		if (cl->cluster_label == NULL) return false;

		if (extensive_check) {
			for (size_t i = 0; i < cl->vertices; ++i) {
				if (cl->cluster_label[i] > 0 && cl->cluster_label[i] < cl->num_clusters) continue;
				if (cl->cluster_label[i] == 0) continue; // Since `scc_Clabel` can be unsigned
				if (cl->cluster_label[i] == SCC_CLABEL_NA) continue;
				return false;
			}
		}
	}

	return true;
}

size_t scc_count_data_points(const scc_Clustering* cl);

size_t scc_count_clusters(const scc_Clustering* cl);

scc_Clabel* scc_get_labels(const scc_Clustering* cl);

void scc_make_labels_external(const scc_Clustering* cl);


scc_ClusteringStats scc_get_clustering_stats(const scc_Clustering* const cl,
                                             scc_DataSetObject* const data_set_object)
{
	if (!iscc_check_input_clustering(cl)) return SCC_NULL_CLUSTERING_STATS;
	if (!scc_is_valid_data_set_object(data_set_object)) return SCC_NULL_CLUSTERING_STATS;
	if (cl->vertices > scc_get_data_point_count(data_set_object)) return SCC_NULL_CLUSTERING_STATS;
	
	size_t* const cluster_size = calloc(cl->num_clusters, sizeof(size_t));
	if (cluster_size == NULL) return SCC_NULL_CLUSTERING_STATS;

	for (size_t v = 0; v < cl->vertices; ++v) {
		if (cl->cluster_label[v] != SCC_CLABEL_NA) {
			++cluster_size[cl->cluster_label[v]];
		}
	}

	scc_ClusteringStats out_stat = {
		.num_populated_clusters = 0,
		.num_assigned = 0,
		.min_cluster_size = cluster_size[0],
		.max_cluster_size = cluster_size[0],
		.avg_cluster_size = -1.0,
		.sum_dists = 0.0,
		.min_dist = DBL_MAX,
		.max_dist = 0.0,
		.cl_avg_min_dist = 0.0,
		.cl_avg_max_dist = 0.0,
		.cl_avg_dist_weighted = 0.0,
		.cl_avg_dist_unweighted = 0.0,
	};

	for (size_t c = 0; c < cl->num_clusters; ++c) {
		if (cluster_size[c] == 0) continue;
		++out_stat.num_populated_clusters;
		out_stat.num_assigned += cluster_size[c];
		if (out_stat.min_cluster_size > cluster_size[c]) {
			out_stat.min_cluster_size = cluster_size[c];
		}
		if (out_stat.max_cluster_size < cluster_size[c]) {
			out_stat.max_cluster_size = cluster_size[c];
		}
	}

	if (out_stat.num_populated_clusters == 0) {
		free(cluster_size);
		return SCC_NULL_CLUSTERING_STATS;
	}

	scc_Vid* const vertex_store = malloc(sizeof(scc_Vid[out_stat.num_assigned]));
	scc_Vid** const cl_members = malloc(sizeof(scc_Vid*[cl->num_clusters]));
	const size_t largest_dist_matrix = (out_stat.max_cluster_size * (out_stat.max_cluster_size - 1)) / 2;
	double* const dist_scratch = malloc(sizeof(double[largest_dist_matrix]));
	if ((vertex_store == NULL) || (cl_members == NULL) || (dist_scratch == NULL)) {
		free(cluster_size);
		free(vertex_store);
		free(cl_members);
		free(dist_scratch);
		return SCC_NULL_CLUSTERING_STATS;
	}

	cl_members[0] = vertex_store + cluster_size[0];
	for (size_t c = 1; c < cl->num_clusters; ++c) {
		cl_members[c] = cl_members[c - 1] + cluster_size[c];
	}

	assert(cl->vertices < SCC_VID_MAX);
	const scc_Vid vertices = (scc_Vid) cl->vertices; // If `scc_Vid` is signed
	for (scc_Vid v = 0; v < vertices; ++v) {
		if (cl->cluster_label[v] != SCC_CLABEL_NA) {
			--cl_members[cl->cluster_label[v]];
			*cl_members[cl->cluster_label[v]] = v;
		}
	}

	for (size_t c = 0; c < cl->num_clusters; ++c) {
		if (cluster_size[c] < 2) {
			if (cluster_size[c] == 1) out_stat.min_dist = 0.0;
			continue;
		}
		const size_t size_dist_matrix = (cluster_size[c] * (cluster_size[c] - 1)) / 2;
		if (!scc_get_dist_matrix(data_set_object, cluster_size[c], cl_members[c], dist_scratch)) {
			free(cluster_size);
			free(vertex_store);
			free(cl_members);
			free(dist_scratch);
			return SCC_NULL_CLUSTERING_STATS;
		}

		double cluster_average = dist_scratch[0];
		double cluster_min = dist_scratch[0];
		double cluster_max = dist_scratch[0];

		for (size_t d = 1; d < size_dist_matrix; ++d) {
			cluster_average += dist_scratch[d];
			if (cluster_min > dist_scratch[d]) {
				cluster_min = dist_scratch[d];
			}
			if (cluster_max < dist_scratch[d]) {
				cluster_max = dist_scratch[d];
			}
		}

		out_stat.sum_dists += cluster_average;

		if (out_stat.min_dist > cluster_min) {
			out_stat.min_dist = cluster_min;
		}
		if (out_stat.max_dist < cluster_max) {
			out_stat.max_dist = cluster_max;
		}
		out_stat.cl_avg_min_dist += cluster_min;
		out_stat.cl_avg_max_dist += cluster_max;

		cluster_average /= (double) size_dist_matrix;

		out_stat.cl_avg_dist_weighted += ((double) cluster_size[c]) * cluster_average;
		out_stat.cl_avg_dist_unweighted += cluster_average;
	}

	out_stat.avg_cluster_size = ((double) out_stat.num_assigned) / ((double) out_stat.num_populated_clusters);
	out_stat.cl_avg_min_dist = out_stat.cl_avg_min_dist / ((double) out_stat.num_populated_clusters);
	out_stat.cl_avg_max_dist = out_stat.cl_avg_max_dist / ((double) out_stat.num_populated_clusters);
	out_stat.cl_avg_dist_weighted = out_stat.cl_avg_dist_weighted / ((double) out_stat.num_assigned);
	out_stat.cl_avg_dist_unweighted = out_stat.cl_avg_dist_unweighted / ((double) out_stat.num_populated_clusters);

	free(cluster_size);
	free(vertex_store);
	free(cl_members);
	free(dist_scratch);

	return out_stat;
}
