/*  scclust -- A C library for size constrained clustering
    https://github.com/fsavje/scclust

    Copyright (C) 2015  Fredrik Savje -- http://fredriksavje.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
==============================================================================*/

#ifndef SCC_NNG_CLUSTERING_HG
#define SCC_NNG_CLUSTERING_HG


#include "config.h"

// Consider changing to array of structs instead
// Faster when assigning but slower when reading

typedef struct scc_Clustering scc_Clustering;
struct scc_Clustering {
	scc_Vid vertices;
	scc_Clulab num_clusters;
	bool* assigned;
	bool* seed;
	scc_Clulab* cluster_label;
};


#endif
