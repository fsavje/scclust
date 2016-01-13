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


#include "../include/nng_clustering.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../include/config.h"
#include "../include/digraph.h"
#include "../include/clustering.h"
#include "findseeds.h"


// ==============================================================================
// External function implementations
// ==============================================================================

void scc_free_TempSeedClustering(scc_TempSeedClustering* const cl)
{
	if (cl) {
		free(cl->assigned);
		free(cl->seeds);
		free(cl->cluster_label);
		*cl = SCC_NULL_TEMP_SEED_CLUSTERING;
	}
}


scc_TempSeedClustering scc_copy_TempSeedClustering(const scc_TempSeedClustering* const cl)
{
	if (!cl) return SCC_NULL_TEMP_SEED_CLUSTERING;

	scc_TempSeedClustering cl_out = {
		.vertices = cl->vertices,
		.num_clusters = cl->num_clusters,
		.seed_capacity = cl->seed_capacity,
		.assigned = NULL,
		.seeds = NULL,
		.cluster_label = NULL,
	};

	if (cl->assigned) {
		cl_out.assigned = malloc(sizeof(bool[cl_out.vertices]));
		for (scc_Vid v = 0; v < cl_out.vertices; ++v) cl_out.assigned[v] = cl->assigned[v];
	}
	if (cl->seeds) {
		cl_out.seeds = malloc(sizeof(scc_Vid[cl_out.seed_capacity]));
		for (scc_Vid v = 0; v < cl_out.seed_capacity; ++v) cl_out.seeds[v] = cl->seeds[v];
	}
	if (cl->cluster_label) {
		cl_out.cluster_label = malloc(sizeof(scc_Clabel[cl_out.vertices]));
		for (scc_Vid v = 0; v < cl_out.vertices; ++v) cl_out.cluster_label[v] = cl->cluster_label[v];
	}

	return cl_out;
}


scc_TempSeedClustering scc_get_seed_clustering(const scc_Digraph* const nng,
                                               const scc_SeedMethod sm,
                                               scc_Vid seed_init_capacity)
                                               scc_Clabel external_cluster_label[const])
{
	if (!scc_digraph_is_initialized(nng)) return SCC_NULL_TEMP_SEED_CLUSTERING;

	scc_TempSeedClustering clustering = SCC_NULL_TEMP_SEED_CLUSTERING;

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

	if (!clustering.seeds) {
		scc_free_TempSeedClustering(&clustering);
		return SCC_NULL_TEMP_SEED_CLUSTERING;
	}

	clustering.assigned = calloc(nng->vertices, sizeof(bool));
	if (!clustering.assigned) {
		scc_free_TempSeedClustering(&clustering);
		return SCC_NULL_TEMP_SEED_CLUSTERING;
	}

	if (external_cluster_label) {
		clustering.cluster_label = external_cluster_label;
	} else {
		clustering.cluster_label = malloc(sizeof(scc_Clabel[nng->vertices]));
		if (!clustering.cluster_label) {
			scc_free_TempSeedClustering(&clustering);
			return SCC_NULL_TEMP_SEED_CLUSTERING;
		}
	}

	scc_Clabel cluster_label = 0;
	const scc_Vid* seed_stop = clustering.seeds + clustering.num_clusters;
	for (const scc_Vid* seed = clustering.seeds;
	        seed != seed_stop; ++seed, ++cluster_label) {

		assert(cluster_label != SCC_CLABEL_NA);
		assert(cluster_label < SCC_CLABEL_MAX_NEW);
		assert(!clustering.assigned[*seed]);
		clustering.assigned[*seed] = true;
		clustering.cluster_label[*seed] = cluster_label;

		const scc_Vid* const s_arc_stop = nng->head + nng->tail_ptr[*seed + 1];
		for (const scc_Vid* s_arc = nng->head + nng->tail_ptr[*seed];
		        s_arc != s_arc_stop; ++s_arc) {
			assert(!clustering.assigned[*s_arc]);
			clustering.assigned[*s_arc] = true;
			clustering.cluster_label[*s_arc] = cluster_label;
		}
	}

	return clustering;
}


scc_Clustering scc_ignore_remaining(scc_TempSeedClustering* cl)
{
	if (!cl || !cl->assigned || !cl->cluster_label) return SCC_NULL_CLUSTERING;

	free(cl->seeds);
	bool* const assigned = cl->assigned;
	scc_Clustering out_cl =  {
		.vertices = cl->vertices,
		.num_clusters = cl->num_clusters,
		.cluster_label = cl->cluster_label,
	};
	*cl = SCC_NULL_TEMP_SEED_CLUSTERING;

	for (scc_Vid v = 0; v < out_cl.vertices; ++v) {
		if (!assigned[v]) out_cl.cluster_label[v] = SCC_CLABEL_MAX;
	}

	free(assigned);

	return out_cl;
}


scc_Clustering scc_assign_remaining_lexical(scc_TempSeedClustering* const cl,
                                            const scc_Digraph* const priority_graph)
{
	if (!cl || !cl->assigned || !cl->cluster_label || !scc_digraph_is_initialized(priority_graph)) {
		return SCC_NULL_CLUSTERING;
	}

	free(cl->seeds);
	bool* const assigned = cl->assigned;
	scc_Clustering out_cl =  {
		.vertices = cl->vertices,
		.num_clusters = cl->num_clusters,
		.cluster_label = cl->cluster_label,
	};
	*cl = SCC_NULL_TEMP_SEED_CLUSTERING;
	
	for (scc_Vid v = 0; v < out_cl.vertices; ++v) {
		if (!assigned[v]) {
			out_cl.cluster_label[v] = SCC_CLABEL_MAX;

			const scc_Vid* const v_arc_stop = priority_graph->head + priority_graph->tail_ptr[v + 1];
			for (const scc_Vid* v_arc = priority_graph->head + priority_graph->tail_ptr[v];
			        v_arc != v_arc_stop; ++v_arc) {
				if (assigned[*v_arc]) {
					out_cl.cluster_label[v] = out_cl.cluster_label[*v_arc];
					break;
				}
			}
		}
	}

	free(assigned);

	return out_cl;
}


scc_Clustering scc_assign_remaining_desired_size(scc_TempSeedClustering* const cl,
                                                 const scc_Digraph* const priority_graph,
                                                 const scc_Vid desired_size)
{
	if (!cl || !cl->assigned || !cl->cluster_label || !scc_digraph_is_initialized(priority_graph)) {
		return SCC_NULL_CLUSTERING;
	}

	free(cl->seeds);
	bool* const assigned = cl->assigned;
	scc_Clustering out_cl =  {
		.vertices = cl->vertices,
		.num_clusters = cl->num_clusters,
		.cluster_label = cl->cluster_label,
	};
	*cl = SCC_NULL_TEMP_SEED_CLUSTERING;

	scc_Vid* cluster_size = calloc(out_cl.num_clusters, sizeof(scc_Vid));
	if (!cluster_size) {
		free(assigned);
		scc_free_Clustering(&out_cl);
		return SCC_NULL_CLUSTERING;
	}

	for (scc_Vid v = 0; v < out_cl.vertices; ++v) {
		if (!assigned[v]) {
			scc_Clabel best_cluster = SCC_CLABEL_MAX;
			scc_Vid best_size = 0;

			const scc_Vid* const v_arc_stop = priority_graph->head + priority_graph->tail_ptr[v + 1];
			for (const scc_Vid* v_arc = priority_graph->head + priority_graph->tail_ptr[v];
			        v_arc != v_arc_stop; ++v_arc) {
				if (assigned[*v_arc]) {
					if ((best_cluster == SCC_CLABEL_MAX) || (best_size < cluster_size[out_cl.cluster_label[*v_arc]])) {
						best_cluster = out_cl.cluster_label[*v_arc];
						best_size = cluster_size[best_cluster];
					}
				}
			}

			out_cl.cluster_label[v] = best_cluster;
			if (best_cluster != SCC_CLABEL_MAX) {
				++cluster_size[best_cluster];
				if ((desired_size != 0) && (cluster_size[best_cluster] % desired_size == 0)) {
					cluster_size[best_cluster] = 0;
				} 
			}
		}
	}

	free(assigned);
	free(cluster_size);

	return out_cl;
}
