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
#include "../include/config.h"
#include "../include/digraph.h"
#include "findseeds.h"


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
