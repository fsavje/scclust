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
 * Size constrained clustering using nearest neighbor graphs (NNGs).
 */


#ifndef SCC_NNG_CLUSTERING_HG
#define SCC_NNG_CLUSTERING_HG

#include <stdbool.h>
#include <stddef.h>
#include "config.h"
#include "digraph.h"
#include "clustering.h"


enum scc_SeedMethod {
	SCC_LEXICAL,
	SCC_INWARDS_ORDER,
	SCC_INWARDS_UPDATING,
	SCC_EXCLUSION_ORDER,
	SCC_EXCLUSION_UPDATING,
	//SCC_SIMULATED_ANNEALING,
};
typedef enum scc_SeedMethod scc_SeedMethod;

/// Typedef for scc_SeedClustering struct
typedef struct scc_SeedClustering scc_SeedClustering;

/** Clustering struct with seed information.
 *
 *  The NNG-based clustering functions declared in this header form clusters by growing them from "seeds".
 *  After clusters have been grown, the size constraint is satisfied, but all vertices need not be assigned to a cluster.
 *  See, e.g., \cite Higgins2016 for more details.
 *
 *  This struct describes such partial clusterings. In particular, apart from the fields in #scc_Clustering, it contain
 *  a list of seeds and an array of indicators of whether each vertex is assigned to a cluster.
 */
struct scc_SeedClustering {

	/// Number of vertices in the clustering problem.
	size_t vertices;

	/// Number of clusters produced by the used NNG-based algorithm.
	size_t num_clusters;

	/// The length of the #seeds array.
	size_t seed_capacity;

	/** Indicator whether the #cluster_label array was assigned by the user (rather than the library). If #external_labels is \c true, 
     *  #cluster_label will not be freed when the instance of #scc_SeedClustering is destroyed.
     */
	bool external_labels;

	/** Array of length #vertices of indicators whether the vertices are assigned to a clusters. 
     *  If vertex \c i is assigned, `assigned[i]` will be \c true.
     */
	bool* assigned;

	/// Array of length #seed_capacity where the first #num_clusters elements contain the IDs of the seeds of each cluster.
	scc_Vid* seeds;

	/** Array of length #vertices with cluster labels of the assigned vertices. 
     *  If `assigned[i]` is false, the correspoding `cluster_label[i]` has no meaning.
     */
	scc_Clabel* cluster_label;
};

/** The null seed clustering.
 *
 *  The null seed clustering is an easily detectable invalid clustering. It is mainly used as return
 *  value when functions encounter errors.
 */
static const scc_SeedClustering SCC_NULL_SEED_CLUSTERING = { 0, 0, 0, false, NULL, NULL, NULL };

/** Destructor for seed clusterings.
 *
 *  Frees the memory allocated by the input and writes the null seed clustering to it.
 *  Assumes the memory was allocated by the standard `malloc`, `calloc` or `realloc` functions.
 *
 *  \param[in,out] cl clustering to destroy. When #scc_free_SeedClustering returns, \p cl is set to #SCC_NULL_SEED_CLUSTERING.
 *
 *  \note If `cl->external_labels` is true, `cl->cluster_label` will *not* be freed by #scc_free_SeedClustering.
 */
void scc_free_SeedClustering(scc_SeedClustering* cl);

/** Deep copy of a seed clustering.
 *
 *  This function produces a deep copy of the inputted seed clustering.
 *
 *  \param[in] cl seed clustering to copy.
 *
 *  \return a copy of \p cl that does not share memory space with it.
 *
 *  \note This function takes a deep copy of `scc_SeedClustering::cluster_label` even if
 *  `scc_SeedClustering::external_labels` is \c true.
 */
scc_SeedClustering scc_copy_SeedClustering(const scc_SeedClustering* cl);

/** Construct seed clustering using NNG algorithm.
 *
 *  This function derives a partial clustering based on the provided nearest neighbor graph. This is done in two steps:
 *  -# Find a set of *seeds*. These set will satisfy four conditions. First, each seed must have at least one outwards pointing arc.
 *     Second, no seed may have a arc pointing to another seed. Third, no two seeds may have arcs pointing to the same vertex. Fourth,
 *     Adding any vertex not in the set of seeds will violate at least one of these conditions. The exact method used to derive the seeds
 *     is decided by \p sm. See #scc_SeedMethod for further details.
 *  -# Each seed forms a cluster and assigns all vertices it is pointing to to its cluster.
 *
 *  In the constructed partial clustering, the maximum distance between any two vertices in a common cluster is bounded by twice the maximum
 *  distance in the NNG.
 *
 *  By constructing the appropriate NNG, many situations can be accommodated by #scc_copy_SeedClustering. In general, if clusters are required
 *  to contain at least \f$N\f$ vertices, construct the NNG by drawing an arc from each vertex to its \f$N-1\f$ closest neighbors. If vertices have
 *  different types and at least \f$(X_1, X_2, ...)\f$ of each type is required in each cluster, draw an arc from each vertex to its \f$X_i\f$ closest
 *  neighbors of each type except for the vertices own type


 and , construct the NNG so that each vertex is matched as
 *
 *  See \cite Higgins2016 for more details.
 *
 *  \param[in] nng a digraph describing a directed NNG to base the clustering on.
 *  \param sm the method to find a maximal independent set in \p nng (i.e., the seeds).
 *  \param seed_init_capacity the initial size of the seed array. Usually the user has a fairly tight upper bound on the amount of seeds, by providing it, one avoids costly reallocations. 
 *  \param[out] external_cluster_label an optional array of cluster labels. If non-null, the vertices' labels will be written to this array. If `NULL`, a new array will be allocated. 
 *                                     If provided, the array must be of size `nng->vertices`. Existing values will be overwritten.
 *
 *  \return a #scc_SeedClustering corresponding to the partial clustering produced by the selected NNG algorithm.
 *
 *  \note If some vertex cannot be a seed (e.g., because not enough other vertices are sufficiently close to it to satisfy the size constraint), delete its outwards pointing arcs.
 *
 *  \note Arcs pointing to 
 */
scc_SeedClustering scc_get_seed_clustering(const scc_Digraph* nng,
                                           scc_SeedMethod sm,
                                           size_t seed_init_capacity,
                                           scc_Clabel external_cluster_label[]);

scc_Clustering scc_ignore_remaining(scc_SeedClustering* cl);

scc_Clustering scc_assign_remaining_lexical(scc_SeedClustering* cl,
                                            const scc_Digraph* priority_graph);

scc_Clustering scc_assign_remaining_desired_size(scc_SeedClustering* cl,
                                                 const scc_Digraph* priority_graph,
                                                 scc_Vid desired_size);


#endif
