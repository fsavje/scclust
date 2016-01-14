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

#include <iostream>

#include "nng.h"

#include <cassert>
#include <cstddef>
#include <cmath>
#include "libANN/include/ANN/ANN.h"
extern "C" {
	#include "../include/config.h"
	#include "../include/digraph.h"
}

#define ANNpointSetConstructor ANNkd_tree
//#define ANNpointSetConstructor ANNbruteForce
//#define ANNpointSetConstructor ANNbd_tree

#define EPS 0.0


scc_Digraph get_nng(const Matrix* const data_matrix,
                    const bool* const query_indicators,
                    const size_t n_search_points,
                    const scc_Vid* const search_indices,
                    const scc_Vid k,
                    const bool use_caliper,
                    const double caliper,
                    const bool accept_partial)
{
	if (!data_matrix || !data_matrix->elements || !query_indicators ||
	        n_search_points < k || !search_indices || k == 0 || (use_caliper && caliper < 0.0)) {
		return SCC_NULL_DIGRAPH;
	}

	scc_Arci est_arcs = 0;
	for (std::size_t q = 0; q < data_matrix->rows; ++q) {
		if (query_indicators[q]) est_arcs += k;
	}
	scc_Digraph nng_out = scc_init_digraph(data_matrix->rows, est_arcs);
	
	
	ANNpoint* search_points = new ANNpoint[n_search_points];
	for (std::size_t i = 0; i < n_search_points; ++i) {
		assert(search_indices[i] >= 0 && search_indices[i] < data_matrix->rows);
		search_points[i] = data_matrix->elements + search_indices[i] * data_matrix->cols;
	}

	ANNpointSet* search_tree = new ANNpointSetConstructor(search_points,
	                                                      static_cast<int>(n_search_points),
	                                                      static_cast<int>(data_matrix->cols));

	int* const tmp_indices = new int[k + 1];
	double* const tmp_dists = new double[k + 1];

	nng_out.tail_ptr[0] = 0;
	for (std::size_t qi = 0; qi < data_matrix->rows; ++qi) {
		if (!query_indicators[qi]) {
			nng_out.tail_ptr[qi + 1] = nng_out.tail_ptr[qi];
		} else {
			const ANNpoint query_point = data_matrix->elements + qi * data_matrix->cols;
			
			if (use_caliper) {
				search_tree->annkFRSearch(query_point,              // pointer to query point
				                          pow(caliper, 2.0),        // squared caliper
				                          static_cast<int>(k + 1),  // number of neighbors
				                          tmp_indices,              // pointer to start of index result
				                          tmp_dists,                // pointer to start of distance result
				                          EPS);                     // error margin
			} else {
				search_tree->annkSearch(query_point,              // pointer to query point
				                        static_cast<int>(k + 1),  // number of neighbors
				                        tmp_indices,              // pointer to start of index result
				                        tmp_dists,                // pointer to start of distance result
				                        EPS);                     // error margin
			}

			scc_Vid num_nn = 0;
			scc_Vid* const qi_nng_head = nng_out.head + nng_out.tail_ptr[qi];
			for (std::size_t i = 0; i < k + 1 && num_nn < k; ++i) {
				if ((tmp_indices[i] != ANN_NULL_IDX) && (search_indices[tmp_indices[i]] != qi)) {
					qi_nng_head[num_nn] = search_indices[tmp_indices[i]];
					++num_nn;
				}
			}

			if (accept_partial || num_nn == k) {
				nng_out.tail_ptr[qi + 1] = nng_out.tail_ptr[qi] + num_nn;
			} else {
				nng_out.tail_ptr[qi + 1] = nng_out.tail_ptr[qi];
			}
		}
	}

	delete[] search_points;
	delete search_tree;
	annClose();
	delete[] tmp_indices;
	delete[] tmp_dists;

	scc_change_arc_storage(&nng_out, nng_out.tail_ptr[nng_out.vertices]);

	return nng_out;
}


scc_Digraph get_all_nng(const Matrix* const data_matrix,
                        const scc_Vid k,
                        const bool use_caliper,
                        const double caliper,
                    	const bool accept_partial)
{
	if (!data_matrix || !data_matrix->elements || data_matrix->rows < k ||
	        k == 0 || (use_caliper && caliper < 0.0)) {
		return SCC_NULL_DIGRAPH;
	}

	scc_Digraph nng_out = scc_init_digraph(data_matrix->rows, data_matrix->rows * k);

	ANNpoint* search_points = new ANNpoint[data_matrix->rows];
	search_points[0] = data_matrix->elements;
	for (std::size_t i = 1; i < data_matrix->rows; ++i) {
		search_points[i] = search_points[i - 1] + data_matrix->cols;
	}

	ANNpointSet* search_tree = new ANNpointSetConstructor(search_points,
	                                                      static_cast<int>(data_matrix->rows),
	                                                      static_cast<int>(data_matrix->cols));

	int* const tmp_indices = new int[k + 1];
	double* const tmp_dists = new double[k + 1];

	nng_out.tail_ptr[0] = 0;
	for (std::size_t qi = 0; qi < data_matrix->rows; ++qi) {
		const ANNpoint query_point = data_matrix->elements + qi * data_matrix->cols;

		if (use_caliper) {
			search_tree->annkFRSearch(query_point,              // pointer to query point
			                          pow(caliper, 2.0),        // squared caliper
			                          static_cast<int>(k + 1),  // number of neighbors
			                          tmp_indices,              // pointer to start of index result
			                          tmp_dists,                // pointer to start of distance result
			                          EPS);                     // error margin
		} else {
			search_tree->annkSearch(query_point,              // pointer to query point
			                        static_cast<int>(k + 1),  // number of neighbors
			                        tmp_indices,              // pointer to start of index result
			                        tmp_dists,                // pointer to start of distance result
			                        EPS);                     // error margin
		}

		scc_Vid num_nn = 0;
		scc_Vid* const qi_nng_head = nng_out.head + nng_out.tail_ptr[qi];
		for (std::size_t i = 0; i < k + 1 && num_nn < k; ++i) {
			if ((tmp_indices[i] != ANN_NULL_IDX) && (static_cast<scc_Vid>(tmp_indices[i]) != qi)) {
				qi_nng_head[num_nn] = static_cast<scc_Vid>(tmp_indices[i]);
				++num_nn;
			}
		}

		if (accept_partial || num_nn == k) {
			nng_out.tail_ptr[qi + 1] = nng_out.tail_ptr[qi] + num_nn;
		} else {
			nng_out.tail_ptr[qi + 1] = nng_out.tail_ptr[qi];
		}
	}

	delete[] search_points;
	delete search_tree;
	annClose();
	delete[] tmp_indices;
	delete[] tmp_dists;

	scc_change_arc_storage(&nng_out, nng_out.tail_ptr[nng_out.vertices]);

	return nng_out;
}
