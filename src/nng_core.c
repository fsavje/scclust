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


#include "nng_core.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "../include/config.h"
#include "../include/clustering.h"
#include "digraph_core.h"
#include "nng_findseeds.h"


// ==============================================================================
// External function implementations
// ==============================================================================

void scc_free_SeedClustering(scc_SeedClustering* const cl)
{
	if (cl != NULL) {
		free(cl->assigned);
		free(cl->seeds);
		if (!cl->external_labels) free(cl->cluster_label);
		*cl = SCC_NULL_SEED_CLUSTERING;
	}
}


scc_SeedClustering scc_copy_SeedClustering(const scc_SeedClustering* const cl)
{
	if (cl == NULL) return SCC_NULL_SEED_CLUSTERING;

	scc_SeedClustering cl_out = {
		.vertices = cl->vertices,
		.num_clusters = cl->num_clusters,
		.seed_capacity = cl->seed_capacity,
		.external_labels = false,
		.assigned = NULL,
		.seeds = NULL,
		.cluster_label = NULL,
	};

	if (cl->assigned != NULL) {
		cl_out.assigned = malloc(sizeof(bool[cl_out.vertices]));
		for (size_t v = 0; v < cl_out.vertices; ++v) {
			cl_out.assigned[v] = cl->assigned[v];
		}
	}
	if (cl->seeds != NULL) {
		cl_out.seeds = malloc(sizeof(scc_Vid[cl_out.seed_capacity]));
		for (size_t v = 0; v < cl_out.seed_capacity; ++v) {
			cl_out.seeds[v] = cl->seeds[v];
		}
	}
	if (cl->cluster_label != NULL) {
		cl_out.cluster_label = malloc(sizeof(scc_Clabel[cl_out.vertices]));
		for (size_t v = 0; v < cl_out.vertices; ++v) {
			cl_out.cluster_label[v] = cl->cluster_label[v];
		}
	}

	return cl_out;
}


scc_SeedClustering scc_get_seed_clustering(const scc_Digraph* const nng,
                                           const scc_SeedMethod sm,
                                           size_t seed_init_capacity,
                                           scc_Clabel external_cluster_label[const])
{
	if (!scc_digraph_is_initialized(nng)) return SCC_NULL_SEED_CLUSTERING;

	scc_SeedClustering clustering = SCC_NULL_SEED_CLUSTERING;

	switch(sm) {
		case SCC_LEXICAL:
			clustering = iscc_findseeds_lexical(nng, seed_init_capacity);
			break;

		case SCC_INWARDS_ORDER:
			clustering = iscc_findseeds_inwards(nng, seed_init_capacity, false);
			break;

		case SCC_INWARDS_UPDATING:
			clustering = iscc_findseeds_inwards(nng, seed_init_capacity, true);
			break;

		case SCC_EXCLUSION_ORDER:
			clustering = iscc_findseeds_exclusion(nng, seed_init_capacity, false);
			break;

		case SCC_EXCLUSION_UPDATING:
			clustering = iscc_findseeds_exclusion(nng, seed_init_capacity, true);
			break;

		default:
			break;
	}

	if (clustering.seeds == NULL) return SCC_NULL_SEED_CLUSTERING;

	if (external_cluster_label != NULL) {
		clustering.external_labels = true;
		clustering.cluster_label = external_cluster_label;
	} else {
		clustering.external_labels = false;
		clustering.cluster_label = malloc(sizeof(scc_Clabel[nng->vertices]));
	}
	
	clustering.assigned = calloc(nng->vertices, sizeof(bool));

	if ((clustering.assigned == NULL) || (clustering.cluster_label == NULL)) {
		scc_free_SeedClustering(&clustering);
		return SCC_NULL_SEED_CLUSTERING;
	}

	scc_Clabel clabel = 0;
	const scc_Vid* seed_stop = clustering.seeds + clustering.num_clusters;
	for (const scc_Vid* seed = clustering.seeds;
	        seed != seed_stop; ++seed, ++clabel) {

		assert(clabel != SCC_CLABEL_NA);
		assert(clabel < SCC_CLABEL_MAX);
		assert(!clustering.assigned[*seed]);
		clustering.assigned[*seed] = true;
		clustering.cluster_label[*seed] = clabel;

		const scc_Vid* const s_arc_stop = nng->head + nng->tail_ptr[*seed + 1];
		for (const scc_Vid* s_arc = nng->head + nng->tail_ptr[*seed];
		        s_arc != s_arc_stop; ++s_arc) {
			assert(!clustering.assigned[*s_arc] || (*s_arc == *seed));
			clustering.assigned[*s_arc] = true;
			clustering.cluster_label[*s_arc] = clabel;
		}
	}

	return clustering;
}


scc_Clustering scc_ignore_remaining(scc_SeedClustering* cl)
{
	if ((cl == NULL) || (cl->assigned == NULL) || (cl->cluster_label == NULL) || 
	        (cl->vertices >= SCC_VID_MAX) || (cl->num_clusters >= SCC_CLABEL_MAX)) {
		return SCC_NULL_CLUSTERING;
	}

	scc_Clustering out_cl =  {
		.vertices = cl->vertices,
		.num_clusters = cl->num_clusters,
		.external_labels = cl->external_labels,
		.cluster_label = cl->cluster_label,
	};
	// Make labels in input seed clustering external so
	// it's not freed when `scc_free_SeedClustering(cl);` is run
	cl->external_labels = true;

	for (size_t v = 0; v < out_cl.vertices; ++v) {
		if (!cl->assigned[v]) out_cl.cluster_label[v] = SCC_CLABEL_NA;
	}

	return out_cl;
}


scc_Clustering scc_assign_remaining_lexical(scc_SeedClustering* const cl,
                                            const scc_Digraph* const priority_graph)
{
	if ((cl == NULL) || (cl->assigned == NULL) || (cl->cluster_label == NULL) || 
	        (cl->vertices >= SCC_VID_MAX) || (cl->num_clusters >= SCC_CLABEL_MAX) ||
	        !scc_digraph_is_initialized(priority_graph)) {
		return SCC_NULL_CLUSTERING;
	}

	scc_Clustering out_cl =  {
		.vertices = cl->vertices,
		.num_clusters = cl->num_clusters,
		.external_labels = cl->external_labels,
		.cluster_label = cl->cluster_label,
	};
	cl->external_labels = true;
	
	for (size_t v = 0; v < out_cl.vertices; ++v) {
		if (!cl->assigned[v]) {
			out_cl.cluster_label[v] = SCC_CLABEL_NA;

			const scc_Vid* const v_arc_stop = priority_graph->head + priority_graph->tail_ptr[v + 1];
			for (const scc_Vid* v_arc = priority_graph->head + priority_graph->tail_ptr[v];
			        v_arc != v_arc_stop; ++v_arc) {
				if (cl->assigned[*v_arc]) {
					out_cl.cluster_label[v] = out_cl.cluster_label[*v_arc];
					break;
				}
			}
		}
	}

	return out_cl;
}


scc_Clustering scc_assign_remaining_desired_size(scc_SeedClustering* const cl,
                                                 const scc_Digraph* const priority_graph,
                                                 const scc_Vid desired_size)
{
	if ((cl == NULL) || (cl->assigned == NULL) || (cl->cluster_label == NULL) || 
	        (cl->vertices >= SCC_VID_MAX) || (cl->num_clusters >= SCC_CLABEL_MAX) ||
	        !scc_digraph_is_initialized(priority_graph)) {
		return SCC_NULL_CLUSTERING;
	}

	scc_Clustering out_cl =  {
		.vertices = cl->vertices,
		.num_clusters = cl->num_clusters,
		.external_labels = cl->external_labels,
		.cluster_label = cl->cluster_label,
	};
	cl->external_labels = true;

	scc_Vid* cluster_size = calloc(out_cl.num_clusters, sizeof(scc_Vid));
	if (cluster_size == NULL) return SCC_NULL_CLUSTERING;

	for (size_t v = 0; v < out_cl.vertices; ++v) {
		if (cl->assigned[v]) ++cluster_size[out_cl.cluster_label[v]];
	}

	for (size_t v = 0; v < out_cl.vertices; ++v) {
		if (!cl->assigned[v]) {
			scc_Clabel best_cluster = SCC_CLABEL_NA;
			scc_Vid best_size = 0;

			const scc_Vid* const v_arc_stop = priority_graph->head + priority_graph->tail_ptr[v + 1];
			for (const scc_Vid* v_arc = priority_graph->head + priority_graph->tail_ptr[v];
			        v_arc != v_arc_stop; ++v_arc) {
				if (cl->assigned[*v_arc]) {
					if ((best_cluster == SCC_CLABEL_NA) || (best_size < cluster_size[out_cl.cluster_label[*v_arc]])) {
						best_cluster = out_cl.cluster_label[*v_arc];
						best_size = cluster_size[best_cluster];
					}
				}
			}

			out_cl.cluster_label[v] = best_cluster;
			if (best_cluster != SCC_CLABEL_NA) {
				++cluster_size[best_cluster];
				if ((desired_size != 0) && (cluster_size[best_cluster] % desired_size == 0)) {
					cluster_size[best_cluster] = 0;
				} 
			}
		}
	}

	free(cluster_size);

	return out_cl;
}
