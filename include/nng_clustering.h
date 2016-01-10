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
 * Size constrained clustering using nearest neighbor graphs.
 */


#ifndef SCC_NNG_CLUSTERING_HG
#define SCC_NNG_CLUSTERING_HG

#include <stdbool.h>
#include "config.h"
#include "digraph.h"


enum scc_SeedMethod {
	SCC_LEXICAL,
	SCC_INWARDS_ORDER,
	SCC_INWARDS_UPDATING,
	SCC_EXCLUSION_ORDER,
	SCC_EXCLUSION_UPDATING,
	//SCC_SIMULATED_ANNEALING,
};
typedef enum scc_SeedMethod scc_SeedMethod;


typedef struct scc_SeedClustering scc_SeedClustering;
struct scc_SeedClustering {
	scc_Vid vertices;
	scc_Clabel num_clusters;
	bool* assigned;
	scc_Vid* seeds;
	scc_Clabel* cluster_label;
};

static const scc_SeedClustering SCC_NULL_SEED_CLUSTERING = { 0, 0, NULL, NULL, NULL };

void scc_free_SeedClustering(scc_SeedClustering* cl);

scc_SeedClustering scc_get_seed_clustering(const scc_Digraph* nng,
                                           scc_SeedMethod sm,
                                           scc_Vid seed_init_capacity);


bool scc_assign_remaining_lexical(scc_SeedClustering* clustering,
                                  const scc_Digraph* priority_graph);


bool scc_assign_remaining_keep_even(scc_SeedClustering* clustering,
                                    const scc_Digraph* priority_graph,
                                    scc_Vid desired_size);


#endif
