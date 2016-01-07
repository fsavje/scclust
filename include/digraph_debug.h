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

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * ============================================================================== */


#ifndef SCC_DIGRAPH_DEBUG_HG
#define SCC_DIGRAPH_DEBUG_HG

#include <stdbool.h>
#include "config.h"
#include "digraph.h"


bool scc_is_valid_digraph(const scc_Digraph* dg);
bool scc_is_sound_digraph(const scc_Digraph* dg);
bool scc_is_empty_digraph(const scc_Digraph* dg);
bool scc_is_balanced_digraph(const scc_Digraph* dg, scc_Vid arcs_per_vertex);
bool scc_digraphs_equal(const scc_Digraph* dg_a, const scc_Digraph* dg_b);
scc_Digraph scc_digraph_from_pieces(scc_Vid vertices, scc_Arcref max_arcs, const scc_Arcref tail_ptr[vertices], const scc_Vid head[max_arcs]);
scc_Digraph scc_digraph_from_string(const char dg_str[]);
void scc_print_digraph(const scc_Digraph* dg);


#endif
