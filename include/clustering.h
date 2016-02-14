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


/** @file
 *
 *  Generic size constrained clustering declarations.
 */


#ifndef SCC_CLUSTERING_HG
#define SCC_CLUSTERING_HG

#include <stdbool.h>
#include <stddef.h>
#include "config.h"


/// Typedef for the scc_Clustering struct
typedef struct scc_Clustering scc_Clustering;

/** Clustering struct.
 *
 *  This struct describes clusterings by enumerating a cluster label for each vertex.
 */
struct scc_Clustering {

	/// Number of vertices in the clustering problem.
	size_t vertices;

	/// Number of clusters.
	size_t num_clusters;

	/** Indicator whether the #cluster_label array was assigned by the user (rather than the library). If #external_labels is \c true, 
	 *  #cluster_label will not be freed when the instance of #scc_Clustering is destroyed.
	 */
	bool external_labels;

	/** Array of length #vertices with cluster labels of the assigned vertices. 
	 *  Unassigned vertices have the value #SCC_CLABEL_NA.
	 */
	scc_Clabel* cluster_label;
};

/** The null clustering.
 *
 *  The null clustering is an easily detectable invalid clustering. It is mainly used as return
 *  value when functions encounter errors.
 */
static const scc_Clustering SCC_NULL_CLUSTERING = { 0, 0, false, NULL };

/// Typedef for the scc_ClusteringStatistics struct
typedef struct scc_ClusteringStatistics scc_ClusteringStatistics;

/** Clustering statistics struct.
 */
struct scc_ClusteringStatistics {
	size_t num_populated_clusters;
	size_t num_assigned;
	size_t min_cluster_size;
	size_t max_cluster_size;
	double avg_cluster_size;
	scc_Distance sum_dists;
	scc_Distance min_dist;
	scc_Distance max_dist;
	scc_Distance cl_avg_min_dist;
	scc_Distance cl_avg_max_dist;
	scc_Distance cl_avg_dist_weighted;
	scc_Distance cl_avg_dist_unweighted;
};

/** The null clustering statistics struct.
 *
 *  This is an easily detectable invalid struct used as return value on errors.
 */
static const scc_ClusteringStatistics SCC_NULL_CLUSTERING_STATS = { 0, 0, 0, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

/** Destructor for clusterings.
 *
 *  Frees the memory allocated by the input and writes the null clustering to it.
 *  Assumes the memory was allocated by the standard `malloc`, `calloc` or `realloc` functions.
 *
 *  \param[in,out] cl clustering to destroy. When #scc_free_Clustering returns, \p cl is set to #SCC_NULL_CLUSTERING.
 *
 *  \note If `cl->external_labels` is true, `cl->cluster_label` will *not* be freed by #scc_free_Clustering.
 */
void scc_free_Clustering(scc_Clustering* cl);

bool scc_is_valid_clustering(const scc_Clustering* cl);

scc_ClusteringStatistics scc_get_clustering_stats(const scc_Clustering* cl,
                                                  scc_DataSetObject* data_set_object);


#endif
