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
#include "../include/config.h"
#include "../include/clustering.h"
#include "digraph_core.h"


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
	SCC_FINDSEEDS_LEXICAL,

	/** Find seeds ordered by inwards pointing arcs. 
	 *
	 *  This method counts vertices' inwards pointing arcs and finds seeds in ascending order by the arc count. Vertices pointing to a seed cannot
	 *  themselves be seeds, thus a vertex with many inwards pointing arcs will exclude many vertices from being seeds. Heuristically, picking such
	 *  a vertex as seed will lead to fewer clusters. 
	 */
	SCC_FINDSEEDS_INWARDS_ORDER,

	/** Find seeds ordered by inwards pointing arcs from unassigned vertices. 
	 *
	 *  This method counts vertices' inwards pointing arcs from *unassigned* vertices and finds seeds by in ascending order by the arc count. Unlike
	 *  the #SCC_INWARDS_ORDER, this method updates the arc count after finding a seed so that only arcs where the tails are unassigned are counted.
	 *  Vertices already assigned to a cluster cannot be a seed, thus it is inconsequential if they are pointing to a seed.
	 *
	 *  If the desired size is two, this method ensures that the maximum distance between any two vertices in a common cluster in the
	 *  final clustering is bounded by twice the maximum distance in the NNG.
	 */
	SCC_FINDSEEDS_INWARDS_UPDATING,

	/** Find seeds ordered by edge count in the exclusion graph.
	 *
	 *  The exclusion graph is the undirected graph where an edge is drawn between two vertices if they cannot both be seeds.
	 *  Any maximal independent set in this graph is a valid set of seeds. This method counts the edges of each vertex in this
	 *  graph and find seeds in ascending order.
	 */
	SCC_FINDSEEDS_EXCLUSION_ORDER,

	/** Find seeds ordered by edge count in the exclusion graph from non-excluded vertices.
	 *
	 *  The exclusion graph is the undirected graph where an edge is draw between two vertices if they cannot both be seeds.
	 *  Any maximal independent set in this graph is a valid set of seeds. This method counts the edges of each that vertex is not already excluded
	 *  and find seeds in ascending order by this count. Unlike the #SCC_EXCLUSION_ORDER, this method updates the edge count after finding a
	 *  seed so that only edges where the tails that still can become seeds are counted.
	 */
	SCC_FINDSEEDS_EXCLUSION_UPDATING,
};

/// Typedef for the scc_SeedMethod enum
typedef enum scc_SeedMethod scc_SeedMethod;

/// Typedef for the scc_SeedClustering struct
typedef struct scc_SeedClustering scc_SeedClustering;

/** Clustering struct with seed information.
 *
 *  The NNG-based clustering functions declared in this header construct clusters by growing them from "seeds".
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

	/** Array of length #vertices of indicators whether the vertices are assigned to clusters. 
     *  If vertex \c i is assigned to a cluster, `assigned[i]` will be \c true.
     */
	bool* assigned;

	/// Array of length #seed_capacity where the first #num_clusters elements contain the IDs of the seeds of each cluster.
	scc_Vid* seeds;

	/** Array of length #vertices with cluster labels for the assigned vertices. 
     *  
     *  \note If `assigned[i]` is false, the correspoding `cluster_label[i]` has no meaning.
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
 *  \note This function takes a deep copy of `cl->cluster_label` even if
 *  `cl->external_labels` is \c true.
 */
scc_SeedClustering scc_copy_SeedClustering(const scc_SeedClustering* cl);

/** Construct seed clustering using NNG-based algorithm.
 *
 *  This function derives a partial clustering based on the provided nearest neighbor graph. This is done in two steps:
 *  -# Find a set of *seeds*. This set will satisfy four conditions. First, each seed must have at least one outwards pointing arc.
 *     Second, no seed may have an arc pointing to another seed. Third, no two seeds may have arcs pointing to the same vertex. Fourth,
 *     adding any vertex not in the set of seeds will violate at least one of these conditions. The exact method used to derive the seeds
 *     is decided by the \p sm parameter. See #scc_SeedMethod for further details.
 *  -# Each seed forms a cluster and assigns all vertices it is pointing to to its cluster.
 *
 *  In the constructed partial clustering, the maximum distance between any two vertices in a common cluster is bounded by twice the maximum
 *  distance in the NNG.
 *
 *  By constructing the appropriate NNG, many situations can be accommodated by #scc_copy_SeedClustering. If clusters are required
 *  to contain at least \f$N\f$ vertices, construct the NNG by drawing an arc from each vertex to its \f$N-1\f$ closest neighbors. If vertices have
 *  different types and at least \f$(X_1, X_2, ...)\f$ of each type is required in each cluster, draw an arc from each vertex to its \f$X_i\f$ closest
 *  neighbors of each type (including itself). If some vertex cannot be a seed (e.g., because not enough other vertices are sufficiently close to it
 *  to satisfy the size constraint), delete its outwards pointing arcs.
 *
 *  See \cite Higgins2016 for more details.
 *
 *  \param[in] nng a digraph describing a directed NNG to base the clustering on.
 *  \param sm the method to find a maximal independent set in \p nng (i.e., the seeds).
 *  \param seed_init_capacity the initial size of the seed array. Usually the user has a fairly tight upper bound on the amount of seeds. By providing it, one avoids costly reallocations. 
 *  \param[out] external_cluster_label an optional array of cluster labels. If non-null, the vertices' labels will be written to this array. If `NULL`, a new array will be allocated. 
 *                                     If provided, the array must be of size `nng->vertices`. Existing values will be overwritten.
 *
 *  \return a #scc_SeedClustering corresponding to the partial clustering produced by the selected NNG algorithm.
 *
 *  \note Loops in the NNG are inconsequential to the clustering. However, arc count will be wrong with the #SCC_INWARDS_ORDER and #SCC_INWARDS_UPDATING methods when
 *        some vertices have self-loops and other do not. To improve performance, calling #scc_copy_SeedClustering without loops is recommended unless clusters of size one
 *        are allowed.
 */
scc_SeedClustering scc_get_seed_clustering(const scc_Digraph* nng,
                                           scc_SeedMethod sm,
                                           size_t seed_init_capacity,
                                           scc_Clabel external_cluster_label[]);

/** Construct final clustering by ignoring unassigned vertices.
 *  
 *  This function creates a #scc_Clustering struct from a #scc_SeedClustering by ignoring unassigned vertices.
 *  All vertices not assigned to a cluster in \p cl will have their cluster labels set to #SCC_CLABEL_NA.
 *
 *  A shallow copy of `cl->cluster_label` is made by the function so that the cluster labels in the two structs
 *  point to the same memory area.
 *
 *  \param[in,out] cl seed clustering to build final clustering from.
 *
 *  \return the final clustering in the form of a #scc_Clustering struct. 
 *
 *  \note The `cl->external_labels` flag is set to true so that calling #scc_free_SeedClustering with \p cl does
 *        not free the cluster labels as they are used by the output struct as well.
 */
scc_Clustering scc_ignore_remaining(scc_SeedClustering* cl);

/** Construct final clustering by assigning unassigned vertices lexically.
 *  
 *  This function creates a #scc_Clustering struct from a #scc_SeedClustering by assigning vertices to clusters lexically
 *  given the digraph \p priority_graph. In particular, in the order of the arcs in \p priority_graph, each unassigned vertex
 *  is assigned to the same cluster as the first vertex that was assign in the original seed clustering it is pointing to. Vertices
 *  not pointing to at least one assigned vertex have their cluster labels set to #SCC_CLABEL_NA.
 *
 *  A shallow copy of `cl->cluster_label` is made by the function so that the cluster labels in the two structs
 *  point to the same memory area.
 *
 *  If \p priority_graph is the NNG inputted to #scc_get_seed_clustering, all vertices will be assigned to a cluster and the
 *  the maximum distance between any two vertices in a common cluster is bounded by four times the maximum
 *  distance in the NNG.
 *
 *  \param[in,out] cl seed clustering to build final clustering from.
 *  \param[in] priority_graph digraph to search for assigned vertices in.
 *
 *  \return the final clustering in the form of a #scc_Clustering struct. 
 *
 *  \note The `cl->external_labels` flag is set to true so that calling #scc_free_SeedClustering with \p cl does
 *        not free the cluster labels as they are used by the output struct as well.
 *  \note `cl->assigned` is not updated by this function and refer to whether vertices were assigned to a cluster in the
 *        original partial seed clustering.
 */
scc_Clustering scc_assign_remaining_lexical(scc_SeedClustering* cl,
                                            const scc_Digraph* priority_graph);

/** Construct final clustering by assigning unassigned vertices to cluster by size.
 *  
 *  This function creates a #scc_Clustering struct from a #scc_SeedClustering by assigning vertices to clusters
 *  given the digraph \p priority_graph while trying to keep the size of the clusters to an even multiple of \p desired_size.
 *  In particular, among clusters that neighboring vertices in \p priority_graph are assigned to in the original seed clustering,
 *  an unassigned vertex is assigned to the cluster where the remainder of the cluster size divided by \p desired_size is the highest.
 *  In the case of ties, vertices are assigned lexically as in #scc_assign_remaining_lexical. Vertices
 *  not pointing to at least one assigned vertex have their cluster labels set to #SCC_CLABEL_NA.
 *
 *  In some applications, odd-sized clusters are detrimental to performance (e.g., in experimental design when deriving blocks). This
 *  function tries to avoid such blocks by concentrating the assignment of unassigned vertices to a few clusters.
 *
 *  A shallow copy of `cl->cluster_label` is made by the function so that the cluster labels in the two structs
 *  point to the same memory area.
 *
 *  If \p priority_graph is the NNG inputted to #scc_get_seed_clustering, all vertices will be assigned to a cluster, and the
 *  the maximum distance between any two vertices in a common cluster is bounded by four times the maximum
 *  distance in the NNG.
 *
 *  \param[in,out] cl seed clustering to build final clustering from.
 *  \param[in] priority_graph digraph to search for assigned vertices in.
 *  \param desired_size the desired size of the clusters.
 *
 *  \return the final clustering in the form of a #scc_Clustering struct. 
 *
 *  \note The `cl->external_labels` flag is set to true so that calling #scc_free_SeedClustering with \p cl does
 *        not free the cluster labels as they are used by the output struct as well.
 *  \note `cl->assigned` is not updated by this function and refer to whether vertices were assigned to a cluster in the
 *        original partial seed clustering.
 *  \note If \p desired_size is set to `0`, vertices will be assigned to the largest possible cluster.
 */
scc_Clustering scc_assign_remaining_desired_size(scc_SeedClustering* cl,
                                                 const scc_Digraph* priority_graph,
                                                 scc_Vid desired_size);


#endif
