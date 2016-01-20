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
 * Digraph structs and generic functions.
 *
 * Sparse digraphs are the backbone of the scclust library.
 * This header defines the digraph struct and functions to generate and manipulate them.
 */


#ifndef SCC_DIGRAPH_HG
#define SCC_DIGRAPH_HG

#include <stdbool.h>
#include <stddef.h>
#include "config.h"


/** Main digraph struct stored as sparse matrix.
 *
 *  Stores the digraph in Yale sparse matrix format. For any vertex `i` in the digraph,
 *  `#tail_ptr[i]` indicates the arc index in #head of the first arc for which `i` is the tail, and `#tail_ptr[i+1]-1`
 *  indicates the last arc. If `#tail_ptr[i] == #tail_ptr[i+1]`, there exists no arc for which `i` is the tail.
 *  Thus if `i` is a tail in at least one arc, `#head[#tail_ptr[i]]` is the head of the first arc for which
 *  `i` is the tail, and `#head[#tail_ptr[i+1]-1]` is the last.
 *
 *  In other words, if there is an arc `i` -> `j`, there exists some `k` such that `#tail_ptr[i] <= k < #tail_ptr[i+1]` and `#head[k]==j`.
 */
struct scc_Digraph {

	/** Number of vertices in the digraph. Must be strictly less than `SCC_VID_MAX`.
	 *  
	 *  \note Valid vertices in this digraph is any `i` such that `0 <= i < #vertices`.
	 */
	const size_t vertices;

	/// Maximum number of arcs in digraph.
	size_t max_arcs;

	/** Array of vertex IDs indicating arc heads.
	 *  
	 *  If `#max_arcs == 0`, #head must be `NULL`. If `#max_arcs > 0`,
	 *  #head should point a memory area of length #max_arcs.
	 *
	 *  \note All used elements of #head must be less than #vertices.
	 */
	scc_Vid* head;

	/** Array of arc indices indicating arcs for which a vertex is the tail.
	 *  
	 *  #tail_ptr may never be `NULL` and must point a memory area of length `#vertices + 1`.
	 *
	 *  The first element of #tail_ptr must be zero (`#tail_ptr[0] == 0`). For all `i < #vertices`, 
	 *  we must have `#tail_ptr[i] <= #tail_ptr[i+1] <= #max_arcs`.
	 */
	scc_Arci* const tail_ptr;
};

/// Typedef for scc_Digraph struct
typedef struct scc_Digraph scc_Digraph;

/** The null digraph.
 *
 *  The null digraph is an easily detectable invalid digraph. It is mainly used as return
 *  value when functions encounter errors.
 */
static const scc_Digraph SCC_NULL_DIGRAPH = { 0, 0, NULL, NULL };

/** Generic constructor for digraphs.
 *
 *  Initializes and allocates memory for specified digraph. The memory spaces
 *  (i.e., scc_Digraph::head and scc_Digraph::tail_ptr) are uninitialized, thus
 *  the produced digraph is in general invalid.
 *
 *  \param vertices number of vertices that can be represented in the digraph.
 *                  This cannot be changed after initialization.
 *  \param max_arcs memory space to be allocated for arcs.
 *
 *  \return a scc_Digraph with allocated memory.
 */
scc_Digraph scc_init_digraph(size_t vertices,
                             size_t max_arcs);

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
bool scc_digraph_is_initialized(const scc_Digraph* dg);

/** Destructor for digraphs.
 *
 *  Frees the memory allocated by the input and writes the null digraph to it.
 *  Assumes the memory was allocated by the standard `malloc`, `calloc` or `realloc` functions.
 *
 *  \param[in,out] dg digraph to destroy. When #scc_free_digraph returns, \p dg is set to #SCC_NULL_DIGRAPH.
 */
void scc_free_digraph(scc_Digraph* dg);

/** Reallocate arc memory.
 *
 *  Increases or decreases the memory space for arcs in \p dg to fit exactly \p new_max_arcs arcs.
 *  Requires that the number of arcs in \p dg is less or equally to \p new_max_arcs. 
 *  If `new_max_arcs == 0`, the memory space is deallocated and scc_Digraph::head is set to `NULL`.
 *
 *  \param[in,out] dg digraph to reallocate arc memory for.
 *  \param         new_max_arcs new size of memory.
 *
 *  \return `true` if reallocation was successful, otherwise `false`.
 */
bool scc_change_arc_storage(scc_Digraph* dg,
                            size_t new_max_arcs);

/** Construct an empty digraph.
 *
 *  This function returns a digraph where all elements of scc_Digraph::tail_ptr are set to `0`. 
 *  The memory space pointed to by scc_Digraph::head is left uninitialized.
 *
 *  \param vertices number of vertices that can be represented in the digraph.
 *                  This cannot be changed after initialization.
 *  \param max_arcs memory space to be allocated for arcs.
 *
 *  \return a scc_Digraph representing an empty digraph.
 */
scc_Digraph scc_empty_digraph(size_t vertices,
                              size_t max_arcs);

/** Deep copy of a digraph.
 *
 *  This function produces a deep copy of the inputted digraph.
 *
 *  \param[in] dg digraph to copy.
 *
 *  \return a copy of \p dg that does not share memory space with it.
 *
 *  \note This function allocates memory space to fit the arcs actually in \p dg. If \p dg
 *        contains excess space, scc_Digraph::max_arcs will differ between the original and copy.
 */
scc_Digraph scc_copy_digraph(const scc_Digraph* dg);

/** Delete arcs by tails.
 *
 *  This function deletes the arcs where the indicated vertices are tails. If `to_delete[i]`
 *  is `true` for vertex `i`, all arcs pointing outwards from `i` will be deleted. All
 *  arcs pointing towards `i` are kept (unless the corresponding tail is also marked for deletion).
 * 
 *  If the following first digraph is the input to `scc_delete_arcs_by_tails(dg, [false, false, true])`,
 *  the second digraph is its output.
 *  \dot
 *  digraph example {
 *      0 -> 1;
 *      1 -> 2;
 *      2 -> 0;

 *      02 [ label="0" ];
 *      12 [ label="1" ];
 *      22 [ label="2" ];
 *      02 -> 12;
 *      12 -> 22;
 *  }
 *  \enddot
 *
 *  \param[in,out] dg digraph to delete arcs from.
 *  \param[in] to_delete array of indicators of the tails whose arcs should be deleted. Must be at least of length \p dg->vertices.
 *
 *  \note Arc memory space that is freed due to the deletion is *not* deallocated.
 *
 *  \note The deletion is stable so that the internal ordering of remaining arcs in \p dg->head is unchanged.
 */
void scc_delete_arcs_by_tails(scc_Digraph* dg,
                              const bool to_delete[]);

/** Delete all self-loops.
 *
 *  This function deletes all arcs where the tail and the head is the same vertex.
 *
 *  \param[in,out] dg digraph to delete self-loops from.
 *
 *  \note Arc memory space that is freed due to the deletion is *not* deallocated.
 *
 *  \note The deletion is stable so that the internal ordering of remaining arcs in \p dg->head is unchanged.
 */
void scc_delete_loops(scc_Digraph* dg);

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
                              const scc_Digraph* const dgs[],
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
