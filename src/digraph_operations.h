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
 * Operations on digraphs.
 */


#ifndef SCC_DIGRAPH_OP_HG
#define SCC_DIGRAPH_OP_HG

#include <stdbool.h>
#include <stddef.h>
#include "../include/config.h"
#include "digraph_core.h"


/** Calculates the union of arbitrary number of digraphs.
 *
 *  This function produces the union of the inputted digraphs. If no digraphs are inputted
 *  (i.e., `num_dgs == 0`), the function returns an empty digraph with no vertices.
 *
 *  The union of the following first two digraphs is the third digraph:
 *  \dot
 *  digraph example {
 *      A -> C;
 *      B -> C;
 *      C -> D;
 *
 *      A2 [ label="A" ];
 *      B2 [ label="B" ];
 *      C2 [ label="C" ];
 *      D2 [ label="D" ];
 *      A2 -> C2;
 *      B2 -> D2;
 *      A2 -> D2;
 *
 *      A3 [ label="A" ];
 *      B3 [ label="B" ];
 *      C3 [ label="C" ];
 *      D3 [ label="D" ];
 *
 *      A3 -> C3;
 *      B3 -> C3;
 *      C3 -> D3;
 *      B3 -> D3;
 *      A3 -> D3;
 *  }
 *  \enddot
 *
 *  \param num_dgs number of digraph to calculate union for.
 *  \param[in] dgs the digraphs. Must be of length \p num_dgs.
 *  \param     ignore_loops when \c true, ignores self-loops in all digraphs in \p dgs (i.e., all arcs where the tail and head is the same vertex
 *                          are ignored).
 *
 *  \return the union of \p dgs.
 *
 *  \note All digraphs in \p dgs must contain equally many vertices.
 */
scc_Digraph scc_digraph_union(size_t num_dgs,
                              const scc_Digraph dgs[],
                              bool ignore_loops);

/** Derives the digraph transpose a digraph.
 *
 *  This function produces the transpose of the inputted digraph. That is, a digraph where
 *  all the arcs are reversed.
 *
 *  The transpose of the following first digraph is the second digraph:
 *  \dot
 *  digraph example {
 *      A -> C;
 *      B -> C;
 *      C -> D;
 *
 *      A2 [ label="A" ];
 *      B2 [ label="B" ];
 *      C2 [ label="C" ];
 *      D2 [ label="D" ];
 *      C2 -> A2;
 *      C2 -> B2;
 *      D2 -> C2;
 *  }
 *  \enddot
 *
 *  \param[in] dg digraph to transpose.
 *
 *  \return the transpose of \p dg.
 */
scc_Digraph scc_digraph_transpose(const scc_Digraph* dg);

/** Calculates the product of the adjacency matrices of two digraphs.
 *
 *  Digraphs are stored as sparse adjacency matrices. By multiplying the underlying
 *  matrices one can derive paths and powers of the graphs.
 *
 *  Let \f$A\f$ and \f$B\f$ be the adjacency matrices of two arbitrary digraph. #scc_adjacency_product
 *  returns \f$A B\f$.
 *
 *  The main purpose of this function is to derive paths and powers. Let \f$A\f$ be an adjacency matrix of some digraph.
 *  \f$A A\f$ then gives the adjacency matrix of the digraph that contains all path of length two in the original digraph.
 *  Let \f$I\f$ be the identity matrix, then \f$(A + I) A\f$ gives the second power of the digraph. That is,
 *  all possible paths of length two or less. Moreover, if \f$A_2 = A A\f$ then \f$A A_2\f$ gives
 *  all paths of length three and \f$A_2 A_2\f$ of length four. If \f$A_p = (A + I) A\f$, then \f$(A + I) A_p\f$
 *  gives the third power and \f$(A_p + I) A_p\f$ gives the fourth power.
 *
 *  \code
 *	// Dummy digraph generator
 *  scc_Digraph my_dg = some_digraph(); 
 *
 *  // All paths of length 2 in `my_dg`
 *  scc_Digraph my_dg_path2 = scc_adjacency_product(&my_dg, &my_dg, false, false);
 *  
 *  // All paths of length 3 in `my_dg`
 *  scc_Digraph my_dg_path3 = scc_adjacency_product(&my_dg, &my_dg_path2, false, false);
 *  
 *  // Second power of `my_dg`
 *  scc_Digraph my_dg_power2 = scc_adjacency_product(&my_dg, &my_dg, true, false);
 *  
 *  // Fourth power of `my_dg`
 *  scc_Digraph my_dg_power4 = scc_adjacency_product(&my_dg_power2, &my_dg_power2, true, false);
 *  
 *  // Free all digraphs
 *  scc_free_digraph(&my_dg); scc_free_digraph(&my_dg_path2); [...]
 *  \endcode
 *  
 *  \param[in] dg_a the first digraph of the product.
 *  \param[in] dg_b the second digraph of the product.
 *  \param     force_loops when \c true, forces self-loops in \p dg_a (i.e., all vertices have an arc to themselves).
 *  \param     ignore_loops when \c true, ignores self-loops in \p dg_a (i.e., all arcs where the tail and head is the same vertex
 *                          are ignored).
 *
 *  \return The digraph described by the product of the adjacency matrices of \p dg_a and \p dg_b.
 *
 *  \note \p dg_a and \p dg_b must contain equally many vertices.
 *
 *  \note \p force_loops and \p ignore_loops cannot both be \c true. 
 */
scc_Digraph scc_adjacency_product(const scc_Digraph* dg_a,
                                  const scc_Digraph* dg_b,
                                  bool force_loops,
                                  bool ignore_loops);


#endif
