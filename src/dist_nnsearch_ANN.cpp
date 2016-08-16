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
 * License along with this library. If not, see http://www.gnu.org/licenses/
 * ============================================================================== */

// So g++ defines integer limits
#define __STDC_LIMIT_MACROS

#include "../src/dist_search.h"

#include <stddef.h>
#include <stdint.h>
#include <cassert>
#include <climits>
#include <cmath>
#include "ANN.h"
#include "../src/config.h"
#include "../src/scc_data_obj_int.h"

typedef struct scc_DataSetObject scc_DataSetObject;


// ==============================================================================
// Internal structs and variables
// ==============================================================================

#define ANNpointSetConstructor ANNkd_tree
//#define ANNpointSetConstructor ANNbd_tree
//#define ANNpointSetConstructor ANNbruteForce

static const double EPS = 0.0;

static bool iscc_open_search_object = false;

struct iscc_NNSearchObject {
	scc_DataSetObject* data_set_object;
	size_t len_search_indices;
	const iscc_Dpid* search_indices;
	ANNpoint* search_points;
	ANNpointSet* search_tree;
};


// ==============================================================================
// External function implementations
// ==============================================================================

bool iscc_init_nn_search_object(void* const data_set_object,
                                const size_t len_search_indices,
                                const iscc_Dpid* const search_indices,
                                iscc_NNSearchObject** const out_nn_search_object)
{
	assert(iscc_check_data_set_object(data_set_object, 1));
	assert(len_search_indices > 0);
	assert(out_nn_search_object != NULL);

	scc_DataSetObject* const data_set_object_cast = (scc_DataSetObject*) data_set_object;

	if (len_search_indices > INT_MAX) return false;

	if (iscc_open_search_object) return false;
	iscc_open_search_object = true;

	ANNpoint* search_points;
	try {
		search_points = new ANNpoint[len_search_indices];
	} catch (...) {
		return false;
	}

	if (search_indices == NULL) {
		assert(len_search_indices <= data_set_object_cast->num_data_points);
		double* search_point = data_set_object_cast->data_matrix;
		for (size_t i = 0; i < len_search_indices; ++i, search_point += data_set_object_cast->num_dimensions) {
			search_points[i] = search_point;
		}
	} else if (search_indices != NULL) {
		for (size_t i = 0; i < len_search_indices; ++i) {
			assert(search_indices[i] < data_set_object_cast->num_data_points);
			search_points[i] = data_set_object_cast->data_matrix + search_indices[i] * data_set_object_cast->num_dimensions;
		}
	}

	ANNpointSet* search_tree;
	try {
		search_tree = new ANNpointSetConstructor(search_points,
		                                         static_cast<int>(len_search_indices),
		                                         static_cast<int>(data_set_object_cast->num_dimensions));
	} catch (...) {
		delete[] search_points;
		return false;
	}

	try {
		*out_nn_search_object = new iscc_NNSearchObject;
	} catch (...) {
		delete search_tree;
		delete[] search_points;
		return false;
	}

	(*out_nn_search_object)->data_set_object = data_set_object_cast;
	(*out_nn_search_object)->len_search_indices = len_search_indices;
	(*out_nn_search_object)->search_indices = search_indices;
	(*out_nn_search_object)->search_points = search_points;
	(*out_nn_search_object)->search_tree = search_tree;

	return true;
}


bool iscc_nearest_neighbor_search(iscc_NNSearchObject* const nn_search_object,
                                  const size_t len_query_indicators,
                                  const bool* const query_indicators,
                                  bool* const out_query_indicators,
                                  const uint32_t k,
                                  const bool radius_search,
                                  double radius,
                                  const bool accept_partial,
                                  iscc_Arci* const out_nn_ref,
                                  iscc_Dpid* const out_nn_indices)
{
	assert(nn_search_object != NULL);
	scc_DataSetObject* const data_set_object = nn_search_object->data_set_object;
	#ifndef NDEBUG
		const size_t len_search_indices = nn_search_object->len_search_indices;
	#endif
	const iscc_Dpid* const search_indices = nn_search_object->search_indices;
	ANNpointSet* const search_tree = nn_search_object->search_tree;

	assert(iscc_open_search_object);
	assert(iscc_check_data_set_object(data_set_object, len_query_indicators));
	assert(len_search_indices > 0);
	assert(search_tree != NULL);
	assert(len_query_indicators > 0);
	assert(k > 0);
	assert(k <= len_search_indices);
	assert(!radius_search || (radius > 0.0));
	assert(out_nn_ref != NULL);
	assert(out_nn_indices != NULL);

	ANNdist* sort_scratch;
	try {
		sort_scratch = new ANNdist[k];
	} catch (...) {
		return false;
	}

	if (k > INT_MAX) return false;
	const int k_int = static_cast<int>(k);

	out_nn_ref[0] = 0;
	if (radius_search) {
		radius = std::pow(radius, 2.0);
		for (size_t q = 0; q < len_query_indicators; ++q) {
			if ((query_indicators == NULL) || query_indicators[q]) {
				const ANNpoint query_point = data_set_object->data_matrix + q * data_set_object->num_dimensions;
				const int num_found = search_tree->annkFRSearch(query_point,              // pointer to query point
				                                                radius,                   // squared caliper
				                                                k_int,                    // number of neighbors
				                                                out_nn_indices + out_nn_ref[q], // pointer to start of index result
				                                                sort_scratch,             // pointer to start of distance result
				                                                EPS);                     // error margin 
				assert(num_found >= 0);
				if (num_found >= k_int) {
					out_nn_ref[q + 1] = out_nn_ref[q] + k;
				} else if (accept_partial) {
					out_nn_ref[q + 1] = out_nn_ref[q] + static_cast<iscc_Arci>(num_found);
				} else {
					out_nn_ref[q + 1] = out_nn_ref[q];
					if (out_query_indicators != NULL) out_query_indicators[q] = false;
				}
			} else {
				out_nn_ref[q + 1] = out_nn_ref[q];
			}
		}

	} else {
		for (size_t q = 0; q < len_query_indicators; ++q) {
			if ((query_indicators == NULL) || query_indicators[q]) {
				const ANNpoint query_point = data_set_object->data_matrix + q * data_set_object->num_dimensions;
				search_tree->annkSearch(query_point,              // pointer to query point
				                        k_int,                    // number of neighbors
				                        out_nn_indices + out_nn_ref[q], // pointer to start of index result
				                        sort_scratch,             // pointer to start of distance result
				                        EPS);                     // error margin
				out_nn_ref[q + 1] = out_nn_ref[q] + k;
			} else {
				out_nn_ref[q + 1] = out_nn_ref[q];
			}
		}
	}

	delete[] sort_scratch;

	if (search_indices != NULL) {
		const iscc_Dpid* const arc_stop = out_nn_indices + out_nn_ref[len_query_indicators];
		for (iscc_Dpid* arc = out_nn_indices; arc != arc_stop; ++arc) {
			assert((*arc > 0) || (*arc == 0));
			assert(*arc < len_search_indices);
			*arc = search_indices[*arc];
		}
	}

	return true;
}


bool iscc_close_nn_search_object(iscc_NNSearchObject** const nn_search_object)
{
	if (nn_search_object != NULL) {
		delete (*nn_search_object)->search_tree;
		delete[] (*nn_search_object)->search_points;
		delete *nn_search_object;
		*nn_search_object = NULL;
	}

	if (!iscc_open_search_object) return false;

	annClose();
	iscc_open_search_object = false;

	return true;
}
