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

#ifndef SCC_CLUSTERING_HG
#define SCC_CLUSTERING_HG

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "../include/scclust.h"


// ==============================================================================
// Macros
// ==============================================================================

/// Macro for internal clustering checks.
#ifndef NDEBUG
	#define iscc_check_input_clustering(cl) scc_check_clustering(cl, true)
#else
	#define iscc_check_input_clustering(cl) scc_check_clustering(cl, false)
#endif


// ==============================================================================
// Structs, types and variables
// ==============================================================================

/** Clustering struct.
 *
 *  This struct describes clusterings by enumerating a cluster label for each vertex.
 */
struct scc_Clustering {
	/// Number of data points in the clustering problem.
	size_t num_data_points;

	/// Number of clusters.
	size_t num_clusters;

	/** Array of length #num_data_points with cluster labels of the assigned data points. 
	 *  Unassigned vertices have the value #SCC_CLABEL_NA.
	 */
	scc_Clabel* cluster_label;

	/** Indicator whether the #cluster_label array was assigned by the user (rather than the library). If #external_labels is \c true, 
	 *  #cluster_label will not be freed when the instance of #scc_Clustering is destroyed.
	 */
	bool external_labels;

	/// Version of the struct.
	int32_t clustering_version;
};

/// Current version of the clustering struct.
static const int32_t ISCC_CURRENT_CLUSTSTRUCT_VERSION = 1;

/** The null clustering.
 *
 *  The null clustering is an easily detectable invalid clustering. It is mainly used as return
 *  value when functions encounter errors.
 */
static const scc_Clustering ISCC_NULL_CLUSTERING = { 0, 0, NULL, false, 0 };


#endif // ifndef SCC_CLUSTERING_HG
