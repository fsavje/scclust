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


#ifndef SCC_NNG_CLUSTERING_HG
#define SCC_NNG_CLUSTERING_HG

#include <stdbool.h>
#include "config.h"
#include "digraph.h"


enum scc_SeedMethod {
	lexical,
	inwards_order,
	inwards_updating,
	exclusion_order,
	exclusion_updating,
	//simulated_annealing,
};
typedef enum scc_SeedMethod scc_SeedMethod;


typedef struct scc_Clustering scc_Clustering;
struct scc_Clustering {
	scc_Vid vertices;
	scc_Vid seed_capacity;
	scc_Clulab num_clusters;
	bool* assigned;
	scc_Vid* seeds;
	scc_Clulab* cluster_label;
};

static inline scc_Clustering scc_null_clustering(void) {
	return (scc_Clustering) { 0, 0, 0, NULL, NULL, NULL };
}

void scc_free_Clustering(scc_Clustering* cl);
scc_Clustering scc_base_clustering(const scc_Digraph* nng, scc_SeedMethod sm, scc_Vid seed_init_capacity);
bool scc_assign_remaining_lexical(scc_Clustering* clustering, const scc_Digraph* priority_graph);
bool scc_assign_remaining_keep_even(scc_Clustering* clustering, const scc_Digraph* priority_graph, scc_Vid desired_size);


#endif
