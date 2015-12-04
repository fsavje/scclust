/* scclust -- A C library for size constrained clustering
 * https://github.com/fsavje/scclust
 *
 * Copyright (C) 2015  Fredrik Savje -- http://fredriksavje.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
==============================================================================*/


#include "../include/nng_clustering.h"

#include <stdbool.h>
#include <stdlib.h>
#include "findseeds.h"


void scc_free_Clustering(scc_Clustering* const cl) {
	if (cl) {
		free(cl->assigned);
		free(cl->seed);
		free(cl->cluster_label);
		*cl = (scc_Clustering) { 0, 0, NULL, NULL, NULL };
	}
}

scc_Clustering scc_base_clustering(const scc_Digraph* const nng, const scc_SeedMethod sm) {

	if (!nng || !nng->tail_ptr) return (scc_Clustering) { 0, 0, NULL, NULL, NULL };

	scc_Clustering clustering = {
		.vertices = nng->vertices,
		.num_clusters = 0,
		.assigned = calloc(nng->vertices, sizeof(bool)),
		.seed = calloc(nng->vertices, sizeof(bool)),
		.cluster_label = malloc(sizeof(scc_Clulab[nng->vertices])),
	};

	if (!clustering.assigned || !clustering.seed || !clustering.cluster_label) {
		scc_free_Clustering(&clustering);
		return clustering;
	}

	bool seed_results = false;
	switch(sm) {

		case lexical:
			seed_results = iscc_findseeds_lexical(nng, &clustering);
			break;
		
		case inwards_once:
			seed_results = iscc_findseeds_inwards(nng, &clustering, false);
			break;
		
		case inwards_updating:
			seed_results = iscc_findseeds_inwards(nng, &clustering, true);
			break;
		
		case inwards_once_onearc:
			seed_results = iscc_findseeds_inwards_onearc(nng, &clustering, false);
			break;
		
		case inwards_updating_onearc:
			seed_results = iscc_findseeds_inwards_onearc(nng, &clustering, true);
			break;
		
		case exclusion_once:
			seed_results = iscc_findseeds_exclusion(nng, &clustering, false);
			break;
		
		case exclusion_updating:
			seed_results = iscc_findseeds_exclusion(nng, &clustering, true);
			break;

		default:
			break;
	}

	if (!seed_results) scc_free_Clustering(&clustering);

	return clustering;
}
