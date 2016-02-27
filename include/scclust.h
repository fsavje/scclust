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
 *  Public header for the scclust library.
 */


#ifndef SCC_SCCLUST_HG
#define SCC_SCCLUST_HG

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// ==============================================================================
// Library specific types, user-serviceable
// ==============================================================================

/** Type used for cluster labels. May be unsigned or signed.
 *  
 *  \note
 *  Possible cluster labels are the sequence `[0, 1, ..., SCC_CLABEL_MAX - 1]`. 
 *  `SCC_CLABEL_NA` may not be in this sequence.
 *  \note
 *  Number of clusters in any clustering problem must be strictly less
 *  than the maximum number that can be stored in #scc_Clabel (i.e., less
 *  than #SCC_CLABEL_MAX).
 */
typedef uint32_t scc_Clabel;

/// Maximum number that can be stored in #scc_Clabel.
static const scc_Clabel SCC_CLABEL_MAX = UINT32_MAX;

/// Label given to unassigned vertices.
static const scc_Clabel SCC_CLABEL_NA = UINT32_MAX;

/** Type used for data point IDs. May be unsigned or signed.
 *
 *  \note
 *  Possible data point IDs are the sequence `[0, 1, ..., SCC_DPID_MAX - 1]`. 
 *  `SCC_DPID_NA` may not be in this sequence.
 *  \note
 *  Number of data points in any clustering problem must be strictly less
 *  than the maximum number that can be stored in #scc_Dpid (i.e., less
 *  than #SCC_DPID_MAX).
 */
typedef uint32_t scc_Dpid;

/// Maximum number that can be stored in #scc_Dpid.
static const scc_Dpid SCC_DPID_MAX = UINT32_MAX;

/// Value to indicate invalid data_points.
static const scc_Dpid SCC_DPID_NA = UINT32_MAX;

/** Type used for arc indices. Must be unsigned.
 *  
 *  \note
 *  Number of arcs in any digraph must be less or equal to 
 *  the maximum number that can be stored in #scc_Arci.
 */
typedef uint32_t scc_Arci;

/// Maximum number that can be stored in #scc_Arci.
static const scc_Arci SCC_ARCI_MAX = UINT32_MAX;


typedef uint32_t scc_TypeLabel;


// ==============================================================================
// Library specific types, non-serviceable
// ==============================================================================

/// Type used for data set objects. This struct is defined in `src/dist_search.c` (or by user). 
typedef struct scc_DataSetObject scc_DataSetObject;

/// Type used for clusterings
typedef struct scc_Clustering scc_Clustering;

/// Type used for clustering statistics
typedef struct scc_ClusteringStats scc_ClusteringStats;

/// Struct to store clustering statistics
struct scc_ClusteringStats {
	size_t num_populated_clusters;
	size_t num_assigned;
	size_t min_cluster_size;
	size_t max_cluster_size;
	double avg_cluster_size;
	double sum_dists;
	double min_dist;
	double max_dist;
	double cl_avg_min_dist;
	double cl_avg_max_dist;
	double cl_avg_dist_weighted;
	double cl_avg_dist_unweighted;
};


// ==============================================================================
// Error handling
// ==============================================================================

enum scc_ErrorCode {
	SCC_ER_OK,
	SCC_ER_NULL_INPUT,
	SCC_ER_INVALID_INPUT,
	SCC_ER_INVALID_CLUSTERING,
	SCC_ER_EMPTY_CLUSTERING,
	SCC_ER_INVALID_DATA_OBJ,
	SCC_ER_NO_MEMORY,
	SCC_ER_TOO_LARGE_PROBLEM,
	SCC_ER_TOO_LARGE_DIGRAPH,
	SCC_ER_DIST_SEARCH_ERROR,
	SCC_ER_NOT_IMPLEMENTED,
};

/// Typedef for the scc_ErrorCode enum
typedef enum scc_ErrorCode scc_ErrorCode;

void scc_get_error_message(scc_ErrorCode ec,
                           char error_message_buffer[],
                           size_t buffer_size);


// ==============================================================================
// Utility functions
// ==============================================================================

/** Destructor for clusterings.
 *
 *  Frees the memory allocated by the input and writes the null clustering to it.
 *  Assumes the memory was allocated by the standard `malloc`, `calloc` or `realloc` functions.
 *
 *  \param[in,out] cl clustering to destroy. When #scc_free_Clustering returns, \p cl is set to #SCC_NULL_CLUSTERING.
 *
 *  \note If `cl->external_labels` is true, `cl->cluster_label` will *not* be freed by #scc_free_Clustering.
 */
void scc_free_clustering(scc_Clustering** clustering);


scc_ErrorCode scc_init_empty_clustering(size_t num_data_points,
                                        scc_Clabel external_cluster_labels[],
                                        scc_Clustering** out_clustering);


scc_ErrorCode scc_init_existing_clustering(size_t num_clusters,
                                           size_t num_data_points,
                                           scc_Clabel current_cluster_labels[],
                                           bool deep_label_copy,
                                           scc_Clustering** out_clustering);


scc_ErrorCode scc_copy_clustering(const scc_Clustering* in_clustering,
                                  scc_Clustering** out_clustering);


bool scc_check_clustering(const scc_Clustering* cl,
                          bool extensive_check);


scc_ErrorCode scc_get_clustering_info(const scc_Clustering* clustering,
                                      size_t* out_num_data_points,
                                      size_t* out_num_clusters);


scc_ErrorCode scc_get_cluster_labels(const scc_Clustering* clustering,
                                     size_t buffer_size,
                                     scc_Clabel out_label_buffer[]);


scc_ErrorCode scc_get_clustering_stats(const scc_Clustering* clustering,
                                       scc_DataSetObject* data_set_object,
                                       scc_ClusteringStats* out_stats);


// ==============================================================================
// Greedy clustering function
// ==============================================================================

scc_ErrorCode scc_top_down_greedy_clustering(scc_Clustering* clustering,
                                             scc_DataSetObject* data_set_object,
                                             size_t size_constraint,
                                             bool batch_assign);

scc_ErrorCode scc_bottom_up_greedy_clustering(scc_Clustering* clustering,
                                              scc_DataSetObject* data_set_object,
                                              size_t size_constraint);


// ==============================================================================
// NNG-based clustering
// ==============================================================================

/** Enum to specify seed finding methods.
 *
 *  The NNG-based clustering algorithms find seeds to build the clustering on. This enum specifies which method is used to find the seeds.
 *
 *  In most settings, it is desired to find as many clusters (i.e., as many seeds) as possible. The choice between the methods is
 *  largely one between performance in this aspect and resource requirements.
 *
 *  See #scc_get_seed_clustering for more details. See also the appendix of \cite Higgins2016.
 */
enum scc_SeedMethod {

	/** Find seeds lexically by vertex ID. 
	 *
	 *  This method finds seed sequentially by checking whether adding the next seed satisfy the four conditions described in #scc_get_seed_clustering.
	 */
	SCC_SM_LEXICAL,

	/** Find seeds ordered by inwards pointing arcs. 
	 *
	 *  This method counts vertices' inwards pointing arcs and finds seeds in ascending order by the arc count. Vertices pointing to a seed cannot
	 *  themselves be seeds, thus a vertex with many inwards pointing arcs will exclude many vertices from being seeds. Heuristically, picking such
	 *  a vertex as seed will lead to fewer clusters. 
	 */
	SCC_SM_INWARDS_ORDER,

	/** Find seeds ordered by inwards pointing arcs from unassigned vertices. 
	 *
	 *  This method counts vertices' inwards pointing arcs from *unassigned* vertices and finds seeds by in ascending order by the arc count. Unlike
	 *  the #SCC_INWARDS_ORDER, this method updates the arc count after finding a seed so that only arcs where the tails are unassigned are counted.
	 *  Vertices already assigned to a cluster cannot be a seed, thus it is inconsequential if they are pointing to a seed.
	 *
	 *  If the desired size is two, this method ensures that the maximum distance between any two vertices in a common cluster in the
	 *  final clustering is bounded by twice the maximum distance in the NNG.
	 */
	SCC_SM_INWARDS_UPDATING,

	/** Find seeds ordered by edge count in the exclusion graph.
	 *
	 *  The exclusion graph is the undirected graph where an edge is drawn between two vertices if they cannot both be seeds.
	 *  Any maximal independent set in this graph is a valid set of seeds. This method counts the edges of each vertex in this
	 *  graph and find seeds in ascending order.
	 */
	SCC_SM_EXCLUSION_ORDER,

	/** Find seeds ordered by edge count in the exclusion graph from non-excluded vertices.
	 *
	 *  The exclusion graph is the undirected graph where an edge is draw between two vertices if they cannot both be seeds.
	 *  Any maximal independent set in this graph is a valid set of seeds. This method counts the edges of each that vertex is not already excluded
	 *  and find seeds in ascending order by this count. Unlike the #SCC_EXCLUSION_ORDER, this method updates the edge count after finding a
	 *  seed so that only edges where the tails that still can become seeds are counted.
	 */
	SCC_SM_EXCLUSION_UPDATING,
};

/// Typedef for the scc_NNGMethod enum
typedef enum scc_SeedMethod scc_SeedMethod;

enum scc_AssignMethod {
	SCC_AM_IGNORE,
	SCC_AM_CLOSEST_ASSIGNED,
	SCC_AM_CLOSEST_SEED,
};
typedef enum scc_AssignMethod scc_AssignMethod;


scc_ErrorCode scc_nng_clusterng(scc_Clustering* clustering,
                                scc_DataSetObject* data_set_object,
                                size_t num_data_points,
                                size_t size_constraint,
                                scc_SeedMethod seed_method,
                                scc_AssignMethod assign_method,
                                const bool main_data_points[],
                                bool assign_secondary_points,
                                bool main_radius_constraint,
                                double main_radius,
                                bool secondary_radius_constraint,
                                double secondary_radius);

scc_ErrorCode scc_nng_clusterng_with_types(scc_Clustering* clustering,
                                           scc_DataSetObject* data_set_object,
                                           size_t num_data_points,
                                           size_t num_types,
                                           const scc_TypeLabel type_labels[],
                                           const size_t type_constraints[],
                                           size_t size_constraint,
                                           scc_SeedMethod seed_method,
                                           scc_AssignMethod assign_method,
                                           const bool main_data_points[],
                                           bool assign_secondary_points,
                                           bool main_radius_constraint,
                                           double main_radius,
                                           bool secondary_radius_constraint,
                                           double secondary_radius);


// ==============================================================================
// One-dimensional clustering
// ==============================================================================


#endif
