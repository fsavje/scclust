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
 *  Functions for debugging digraphs.
 *
 *  These function should in general not be used in production, but they can
 *  be useful when debugging.
 */


#ifndef SCC_DIGRAPH_DEBUG_HG
#define SCC_DIGRAPH_DEBUG_HG

#include <stdbool.h>
#include "config.h"
#include "digraph.h"


/** Checks whether provided digraph is initialized.
 *
 *  This function returns \c true if \p dg is initialized. That is, scc_Digraph::tail_ptr
 *  and scc_Digraph::head are allocated. If scc_Digraph::max_arcs is zero, it checks so
 *  scc_Digraph::head is \c NULL.
 *
 *  \param[in] dg digraph to check.
 *
 *  \return \c true if \p dg is correctly initialized, otherwise \c false.
 */
bool scc_is_initialized_digraph(const scc_Digraph* dg);

/** Checks whether provided digraph is valid.
 *
 *  This function returns \c true if \p dg is a valid scc_Digraph instance. That is,
 *  \p dg describes a valid digraph. 
 *
 *  \param[in] dg digraph to check.
 *
 *  \return \c true if \p dg is valid, otherwise \c false.
 */
bool scc_is_valid_digraph(const scc_Digraph* dg);

/** Checks whether provided digraph is empty.
 *
 *  This function returns \c true if \p dg is valid and does not contain any arcs.
 *
 *  \param[in] dg digraph to check.
 *
 *  \return \c true if \p dg is empty, otherwise \c false.
 */
bool scc_is_empty_digraph(const scc_Digraph* dg);

/** Checks whether provided digraph is a balanced digraph.
 *
 *  This function returns \c true if \p dg is valid and all vertices have equally many
 *  outwards pointing arcs.
 *
 *  \param[in] dg digraph to check.
 *  \param arcs_per_vertex number of outwards pointing arcs each vertex is expected to have.
 *
 *  \return \c true if \p dg is balanced, otherwise \c false.
 */
bool scc_is_balanced_digraph(const scc_Digraph* dg,
                             scc_Vid arcs_per_vertex);

/** Checks whether two digraphs are logically identical.
 *
 *  Two digraphs are considered logically identical if they contain
 *  equally many vertices and they contain the same set of arcs as judged
 *  by vertex IDs. Duplicate arcs are ignored. 
 *
 *  \param[in] dg_a first digraph to compare.
 *  \param[in] dg_b second digraph to compare.
 *
 *  \return \c true if \p dg_a and \p dg_b are identical.
 */
bool scc_digraphs_equal(const scc_Digraph* dg_a,
                        const scc_Digraph* dg_b);

/** Constructs digraph from separate struct parts.
 *
 *  This function constructs a new scc_Digraph instance and initializes it
 *  with the provided values. It takes a deep copy of \p tail_ptr and \p head.
 *
 *  \param vertices number of vertices in the digraph.
 *  \param max_arcs length of the memory space for arcs.
 *  \param[in] tail_ptr array of arc indices (see scc_Digraph::tail_ptr).
 *  \param[in] head array of arc heads (see scc_Digraph::head).
 *
 *  \return the constructed scc_Digraph.
 */
scc_Digraph scc_digraph_from_pieces(scc_Vid vertices,
                                    scc_Arci max_arcs,
                                    const scc_Arci tail_ptr[vertices],
                                    const scc_Vid head[max_arcs]);

/** Constructs digraph from human readable strings.
 *
 *  This function builds a digraph from a string describing an adjacency matrix, where rows
 *  indicate tails and columns heads. `#` denotes an arc, `.` denotes absence of an arc and
 *  `/` the end of a row. All other characters are ignored.
 *  
 *  Thus, the string ".##./..#./...#/#..#/" describes the following digraph:
 *  \dot
 *  digraph example {
 *      0 -> 1;
 *      0 -> 2;
 *      1 -> 2;
 *      2 -> 3;
 *      3 -> 0;
 *      3 -> 3;
 *  }
 *  \enddot
 *
 *  \param[in] dg_str a null terminated string describing an adjacency matrix.
 *
 *  \return the digraph described by \p dg_str.
 */
scc_Digraph scc_digraph_from_string(const char dg_str[]);

/** Print a digraph in human readable format.
 *
 *  This function prints the adjacency matrix of the provided digraph. Rows
 *  indicate tails and columns heads. `#` denotes an arc, `.` denotes absence of an arc.
 *  Digraphs without vertices are printed `[]`.
 *
 *  Thus, the following digraph:
 *  \dot
 *  digraph example {
 *      0 -> 1;
 *      0 -> 2;
 *      1 -> 2;
 *      2 -> 3;
 *      3 -> 0;
 *      3 -> 3;
 *  }
 *  \enddot
 *  would be printed as:
 *  \code
 *	.##.
 *	..#.
 *	...#
 *	#..#
 *  \endcode
 *
 *  \param[in] dg digraph to print.
 */
void scc_print_digraph(const scc_Digraph* dg);


#endif
