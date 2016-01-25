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
 * User-serviceable parts of the library.
 *
 * Change this file in order to change memory usage, and to toggle stable
 * clustering when debugging.
 */


#ifndef SCC_CONFIG_HG
#define SCC_CONFIG_HG

#include <stdint.h>


/* Uncomment to make clustering stable by sorting on vertex ID
 * This increases runtime considerably, generally only recommended for debugging.
 */
//#define SCC_STABLE_CLUSTERING


/** Type used for arc indices. Must be unsigned.
 *  
 *  \note
 *  Number of arcs in any digraph must be less or equal to 
 *  the maximum number that can be stored in #scc_Arci.
 */
typedef uint32_t scc_Arci;

/// Maximum number that can be stored in #scc_Arci.
#define SCC_ARCI_MAX UINT32_MAX

/** Type used for cluster labels. May be unsigned or signed.
 *  
 *  \note
 *  Possible cluster labels are the sequence `[0, 1, ..., SCC_CLABEL_MAX - 1]`. 
 *  `SCC_CLABEL_NA` may not be in this sequence.
 */
typedef uint32_t scc_Clabel;

/// Maximum number that can be stored in #scc_Clabel.
#define SCC_CLABEL_MAX UINT32_MAX

/** Number to indicate that an unassigned vertex.
 *  
 *  If #scc_Clabel is unsigned, `SCC_CLABEL_NA` will typically be `SCC_CLABEL_MAX`.
 *  If #scc_Clabel is signed, `SCC_CLABEL_NA` will typically be negative or `SCC_CLABEL_MAX`.
 */
#define SCC_CLABEL_NA UINT32_MAX

/// Type used for distances. Must be float, and distances must be non-negative.
typedef double scc_Distance;

/** Type used for vertex IDs. Must be unsigned.
 *  
 *  \note
 *  Number of vertices must be strictly less than the maximum number
 *  that can be stored in #scc_Vid (i.e., less than #SCC_VID_MAX).
 */
typedef uint32_t scc_Vid;

/// Maximum number that can be stored in #scc_Vid.
#define SCC_VID_MAX UINT32_MAX


#endif
