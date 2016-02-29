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
 *  Config header with internal type definitions.
 */

#ifndef SCC_CONFIG_HG
#define SCC_CONFIG_HG

#include <stdint.h>


/** Type used for data point IDs. May be unsigned or signed.
 *
 *  \note
 *  Number of data points in any clustering problem must be strictly less
 *  than the maximum number that can be stored in #scc_Dpid. I.e., 
 *  possible data point IDs must be in the sequence `[0, 1, ..., SCC_DPID_MAX - 1]`, 
 *  and `SCC_DPID_NA` may not be in this sequence (but it may be `SCC_DPID_MAX`).
 */
typedef uint32_t scc_Dpid;

/// Maximum number that can be stored in #scc_Dpid. May not be greater than `SIZE_MAX`.
static const scc_Dpid SCC_DPID_MAX = UINT32_MAX;

/// Value to indicate invalid data_points.
static const scc_Dpid SCC_DPID_NA = UINT32_MAX;

/** Type used for arc indices. Must be unsigned.
 *  
 *  \note
 *  Number of arcs in any digraph must be less or equal to 
 *  the maximum number that can be stored in #scc_Arci.
 */
typedef uint32_t scc_Arci;

/// Maximum number that can be stored in #scc_Arci.
static const scc_Arci SCC_ARCI_MAX = UINT32_MAX;


#endif
