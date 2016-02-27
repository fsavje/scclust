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
#include "../include/scclust.h"
#include "digraph_core.h"
#include "nng_findseeds.h"


scc_ErrorCode iscc_get_nng_with_size_constraint(scc_DataSetObject* data_set_object,
                                                size_t num_data_points,
                                                size_t size_constraint,
                                                const bool must_be_assigned[],
                                                bool radius_constraint,
                                                double radius,
                                                iscc_Digraph* out_nng);

scc_ErrorCode iscc_get_nng_with_type_constraint(scc_DataSetObject* data_set_object,
                                                size_t num_data_points,
                                                size_t num_types,
                                                const scc_TypeLabel type_labels[],
                                                const size_t type_constraints[],
                                                size_t size_constraint,
                                                const bool must_be_assigned[],
                                                bool radius_constraint,
                                                double radius,
                                                iscc_Digraph* out_nng);

scc_ErrorCode iscc_assign_seed_neighbors(scc_Clustering* clustering,
                                         const iscc_Digraph* nng,
                                         const iscc_SeedResult* sr,
                                         bool* out_unassigned);

scc_ErrorCode iscc_remaining_ignore(scc_Clustering* clustering,
                                    const bool* unassigned);

scc_ErrorCode iscc_remaining_to_nearest_assigned(scc_DataSetObject* data_set_object,
                                                 scc_Clustering* clustering,
                                                 iscc_Digraph* nng,
                                                 size_t num_assigned, // sr->count * size_constraint
                                                 bool* unassigned,
                                                 const bool main_data_points[],
                                                 bool assign_secondary_points,
                                                 bool secondary_radius_constraint,
                                                 double secondary_radius);

scc_ErrorCode iscc_remaining_to_nearest_seed(scc_DataSetObject* data_set_object,
                                             scc_Clustering* clustering,
                                             const iscc_SeedResult* sr,
                                             size_t num_assigned, // sr->count * size_constraint
                                             bool* unassigned,
                                             const bool main_data_points[],
                                             bool assign_secondary_points,
                                             bool main_radius_constraint,
                                             double main_radius,
                                             bool secondary_radius_constraint,
                                             double secondary_radius);


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


#endif
