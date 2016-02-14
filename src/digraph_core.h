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
 

#ifndef SCC_DIGRAPH_CORE_HG
#define SCC_DIGRAPH_CORE_HG

#include <stdbool.h>
#include <stddef.h>
#include "../include/scclust.h"


/// Typedef for scc_Digraph struct
typedef struct scc_Digraph scc_Digraph;

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
	size_t vertices;

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
	scc_Arci* tail_ptr;
};

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
 *  \return \c true on successful deletion, \c false otherwise. On fail, \p dg is in a undefined state.
 *
 *  \note Arc memory space that is freed due to the deletion is *not* deallocated.
 *
 *  \note The deletion is stable so that the internal ordering of remaining arcs in \p dg->head is unchanged.
 */
void scc_delete_arcs_by_tails(scc_Digraph* dg,
                              const bool to_delete[static dg->vertices]);

/** Delete all self-loops.
 *
 *  This function deletes all arcs where the tail and the head is the same vertex.
 *
 *  \param[in,out] dg digraph to delete self-loops from.
 *
 *  \return \c true on successful deletion, \c false otherwise. On fail, \p dg is in a undefined state.
 *
 *  \note Arc memory space that is freed due to the deletion is *not* deallocated.
 *
 *  \note The deletion is stable so that the internal ordering of remaining arcs in \p dg->head is unchanged.
 */
void scc_delete_loops(scc_Digraph* dg);


#endif
