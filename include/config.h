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


#ifndef SCC_CONFIG_HG
#define SCC_CONFIG_HG

#include <stdint.h>


// Vertex ID type
// Number of vertices < MAX(scc_Vid)
typedef uint32_t scc_Vid;

// MAX(scc_Vid): maximum value of scc_Vid
#define SCC_VID_MAX UINT32_MAX


// Arc reference type
// Number of arcs in any digraph <= MAX(scc_Arcref)
// Should be MAX(scc_Arcref) >= MAX(scc_Vid)
typedef uint32_t scc_Arcref;


// Cluster label
// Number of clusters <= MAX(scc_Clulab)
typedef uint32_t scc_Clulab;

// MAX(scc_Clulab): maximum value of scc_Clulab
#define SCC_CLULAB_MAX UINT32_MAX


// Uncomment to make clustering stable by sorting on vertex id
// This increases runtime considerably, generally only recommended for debugging.
//#define SCC_STABLE_CLUSTERING


#endif
