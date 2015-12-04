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


#ifndef SCC_FINDSEEDS_HG
#define SCC_FINDSEEDS_HG

#include <stdbool.h>
#include "../include/digraph.h"
#include "../include/nng_clustering.h"


bool iscc_findseeds_lexical(const scc_Digraph* nng, scc_Clustering* clustering);
bool iscc_findseeds_inwards(const scc_Digraph* nng, scc_Clustering* clustering, bool updating);
bool iscc_findseeds_inwards_onearc(const scc_Digraph* nng, scc_Clustering* clustering, bool updating);
bool iscc_findseeds_exclusion(const scc_Digraph* nng, scc_Clustering* clustering, bool updating);
//iscc_findseeds_simulated_annealing();
//iscc_findseeds_approximation();


#endif
