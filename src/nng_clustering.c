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

#include <stdbool.h>
#include <stdlib.h>
#include "../include/config.h"
#include "../include/digraph.h"
#include "findseeds.h"


// ==============================================================================
// External function implementations
// ==============================================================================

void scc_free_Clustering(scc_Clustering* const cl) {
	if (cl) {
		free(cl->assigned);
		free(cl->seeds);
		free(cl->cluster_label);
		*cl = scc_null_clustering();
	}
}

scc_Clustering scc_base_clustering(const scc_Digraph* const nng, const scc_SeedMethod sm, scc_Vid seed_init_capacity) {

	if (!nng || !nng->tail_ptr) return scc_null_clustering();
	if (seed_init_capacity < 100) seed_init_capacity = 100;

	scc_Clustering clustering = scc_null_clustering();

	switch(sm) {

		case lexical:
			clustering = iscc_findseeds_lexical(nng, seed_init_capacity);
			break;

		case inwards_order:
			clustering = iscc_findseeds_inwards(nng, seed_init_capacity, false);
			break;

		case inwards_updating:
			clustering = iscc_findseeds_inwards(nng, seed_init_capacity, true);
			break;

		case exclusion_order:
			clustering = iscc_findseeds_exclusion(nng, seed_init_capacity, false);
			break;

		case exclusion_updating:
			clustering = iscc_findseeds_exclusion(nng, seed_init_capacity, true);
			break;

		default:
			break;
	}

	return clustering;
}

bool scc_assign_remaining_lexical(scc_Clustering* const clustering, const scc_Digraph* const priority_graph) {

	bool* const assigned = clustering->assigned;
	scc_Clulab* const cluster_label = clustering->cluster_label;

	for (scc_Vid v = 0; v < vertices; ++v) {
		if (!assigned[v]) {
			cluster_label[v] = SCC_CLULAB_MAX;

			const scc_Vid* const v_arc_stop = priority_graph->head + priority_graph->tail_ptr[v + 1];
			for (const scc_Vid* v_arc = priority_graph->head + priority_graph->tail_ptr[v];
			        v_arc != v_arc_stop; ++v_arc) {
				if (assigned[*v_arc]) {
					cluster_label[v] = cluster_label[*v_arc];
					break;
				}
			}
		}
	}

	return true;
}

bool scc_assign_remaining_keep_even(scc_Clustering* const clustering, const scc_Digraph* const priority_graph, const scc_Vid desired_size) {

	scc_Vid* cluster_size = calloc(clustering->num_clusters, sizeof(scc_Vid));
	if (!cluster_size) return false;

	const scc_Vid vertices = clustering->vertices;
	bool* const assigned = clustering->assigned;
	scc_Clulab* const cluster_label = clustering->cluster_label;

	for (scc_Vid v = 0; v < vertices; ++v) {
		if (!assigned[v]) {
			scc_Clulab best_cluster = SCC_CLULAB_MAX;
			scc_Vid best_size = 0;

			const scc_Vid* const v_arc_stop = priority_graph->head + priority_graph->tail_ptr[v + 1];
			for (const scc_Vid* v_arc = priority_graph->head + priority_graph->tail_ptr[v];
			        v_arc != v_arc_stop; ++v_arc) {
				if (assigned[*v_arc]) {
					if ((best_cluster == SCC_CLULAB_MAX) || (best_size < cluster_size[cluster_label[*v_arc]])) {
						best_cluster = cluster_label[*v_arc];
						best_size = cluster_size[best_cluster];
					}
				}
			}

			cluster_label[v] = best_cluster;
			if (best_cluster != SCC_CLULAB_MAX) {
				++cluster_size[best_cluster];
				if (cluster_size[best_cluster] % desired_size == 0) cluster_size[best_cluster] = 0;
			}
		}
	}

	free(cluster_size);

	return true;
}
