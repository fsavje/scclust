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

#include "dist_search.h"

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include "../include/data_obj.h"
#include "../include/scclust.h"


// ==============================================================================
// Internal struct definitions
// ==============================================================================

struct iscc_MaxDistObject {
	const scc_DataSetObject* data_set_object;
	size_t len_search_indices;
	const iscc_Dpid* search_indices;
};

struct iscc_NNSearchObject {
	const scc_DataSetObject* data_set_object;
	size_t len_search_indices;
	const iscc_Dpid* search_indices;
};


// ==============================================================================
// Internal function prototypes
// ==============================================================================

static inline double iscc_get_sq_dist(const scc_DataSetObject* data_set_object,
                                      size_t index1,
                                      size_t index2);

static inline void iscc_add_dist_to_list(double add_dist,
                                         iscc_Dpid add_index,
                                         double* dist_list,
                                         iscc_Dpid* index_list,
                                         const double* dist_list_start);


// ==============================================================================
// External function implementations
// ==============================================================================

bool iscc_check_data_set_object(scc_DataSetObject* const data_set_object,
                                const uint64_t required_data_points)
{
	if ((data_set_object == NULL) || (data_set_object->elements == NULL)) return false;
	if (data_set_object->cols == 0) return false;
	if (data_set_object->rows < required_data_points) return false;
	return true;
}


bool iscc_get_dist_matrix(scc_DataSetObject* const data_set_object,
                          const size_t len_point_indices,
                          const iscc_Dpid point_indices[const],
                          double output_dists[])
{
	assert(iscc_check_data_set_object(data_set_object, 0));
	assert(len_point_indices > 1);
	assert(output_dists != NULL);

	if (point_indices == NULL) {
		for (size_t p1 = 0; p1 < len_point_indices; ++p1) {
			for (size_t p2 = p1 + 1; p2 < len_point_indices; ++p2) {
				*output_dists = sqrt(iscc_get_sq_dist(data_set_object, p1, p2));
				++output_dists;
			}
		}
	} else {
		for (size_t p1 = 0; p1 < len_point_indices; ++p1) {
			for (size_t p2 = p1 + 1; p2 < len_point_indices; ++p2) {
				*output_dists = sqrt(iscc_get_sq_dist(data_set_object, (size_t) point_indices[p1], (size_t) point_indices[p2]));
				++output_dists;
			}
		}
	}
	
	return true;
}


bool iscc_get_dist_rows(scc_DataSetObject* const data_set_object,
                        const size_t len_query_indices,
                        const iscc_Dpid query_indices[const],
                        const size_t len_column_indices,
                        const iscc_Dpid column_indices[const],
                        double output_dists[])
{
	assert(iscc_check_data_set_object(data_set_object, 0));
	assert(len_query_indices > 0);
	assert(len_column_indices > 0);
	assert(output_dists != NULL);

	if ((query_indices != NULL) && (column_indices != NULL)) {
		for (size_t q = 0; q < len_query_indices; ++q) {
			for (size_t c = 0; c < len_column_indices; ++c) {
				*output_dists = sqrt(iscc_get_sq_dist(data_set_object, (size_t) query_indices[q], (size_t) column_indices[c]));
				++output_dists;
			}
		}

	} else if ((query_indices == NULL) && (column_indices != NULL)) {
		for (size_t q = 0; q < len_query_indices; ++q) {
			for (size_t c = 0; c < len_column_indices; ++c) {
				*output_dists = sqrt(iscc_get_sq_dist(data_set_object, q, (size_t) column_indices[c]));
				++output_dists;
			}
		}

	} else if ((query_indices != NULL) && (column_indices == NULL)) {
		for (size_t q = 0; q < len_query_indices; ++q) {
			for (size_t c = 0; c < len_column_indices; ++c) {
				*output_dists = sqrt(iscc_get_sq_dist(data_set_object, (size_t) query_indices[q], c));
				++output_dists;
			}
		}

	} else if ((query_indices == NULL) && (column_indices == NULL)) {
		for (size_t q = 0; q < len_query_indices; ++q) {
			for (size_t c = 0; c < len_column_indices; ++c) {
				*output_dists = sqrt(iscc_get_sq_dist(data_set_object, q, c));
				++output_dists;
			}
		}
	}

	return true;
}


bool iscc_init_max_dist_object(scc_DataSetObject* const data_set_object,
                               const size_t len_search_indices,
                               const iscc_Dpid search_indices[const],
                               iscc_MaxDistObject** const out_max_dist_object)
{
	assert(iscc_check_data_set_object(data_set_object, 0));
	assert(len_search_indices > 0);
	assert(out_max_dist_object != NULL);

	*out_max_dist_object = malloc(sizeof(iscc_MaxDistObject));
	if (*out_max_dist_object == NULL) return false;

	**out_max_dist_object = (iscc_MaxDistObject) {
		.data_set_object = data_set_object,
		.len_search_indices = len_search_indices,
		.search_indices = search_indices,
	};

	return true;
}


bool iscc_get_max_dist(iscc_MaxDistObject* const max_dist_object,
                       const size_t len_query_indices,
                       const iscc_Dpid query_indices[const],
                       iscc_Dpid max_indices[const],
                       double max_dists[const])
{
	assert(max_dist_object != NULL);
	const scc_DataSetObject* const data_set_object = max_dist_object->data_set_object;
    const size_t len_search_indices = max_dist_object->len_search_indices;
	const iscc_Dpid* const search_indices = max_dist_object->search_indices;

	assert(len_search_indices > 0);
	assert(len_query_indices > 0);
	assert(max_indices != NULL);
	assert(max_dists != NULL);

	double tmp_dist;
	double max_dist;

	if ((query_indices != NULL) && (search_indices != NULL)) {
		for (size_t q = 0; q < len_query_indices; ++q) {
			max_dist = -1.0;
			for (size_t s = 0; s < len_search_indices; ++s) {
				tmp_dist = iscc_get_sq_dist(data_set_object, (size_t) query_indices[q], (size_t) search_indices[s]);
				if (max_dist < tmp_dist) {
					max_dist = tmp_dist;
					max_indices[q] = search_indices[s];
				}
			}
			max_dists[q] = sqrt(max_dist);
		}

	} else if ((query_indices == NULL) && (search_indices != NULL)) {
		for (size_t q = 0; q < len_query_indices; ++q) {
			max_dist = -1.0;
			for (size_t s = 0; s < len_search_indices; ++s) {
				tmp_dist = iscc_get_sq_dist(data_set_object, q, (size_t) search_indices[s]);
				if (max_dist < tmp_dist) {
					max_dist = tmp_dist;
					max_indices[q] = search_indices[s];
				}
			}
			max_dists[q] = sqrt(max_dist);
		}

	} else if ((query_indices != NULL) && (search_indices == NULL)) {
		for (size_t q = 0; q < len_query_indices; ++q) {
			max_dist = -1.0;
			for (size_t s = 0; s < len_search_indices; ++s) {
				tmp_dist = iscc_get_sq_dist(data_set_object, (size_t) query_indices[q], s);
				if (max_dist < tmp_dist) {
					max_dist = tmp_dist;
					max_indices[q] = (iscc_Dpid) s;
				}
			}
			max_dists[q] = sqrt(max_dist);
		}

	} else if ((query_indices == NULL) && (search_indices == NULL)) {
		for (size_t q = 0; q < len_query_indices; ++q) {
			max_dist = -1.0;
			for (size_t s = 0; s < len_search_indices; ++s) {
				tmp_dist = iscc_get_sq_dist(data_set_object, q, s);
				if (max_dist < tmp_dist) {
					max_dist = tmp_dist;
					max_indices[q] = (iscc_Dpid) s;
				}
			}
			max_dists[q] = sqrt(max_dist);
		}
	}

	return true;
}


bool iscc_close_max_dist_object(iscc_MaxDistObject** const max_dist_object)
{
	if (max_dist_object != NULL) {
		free(*max_dist_object);
		*max_dist_object = NULL;
	}
	return true;
}


bool iscc_init_nn_search_object(scc_DataSetObject* const data_set_object,
                                const size_t len_search_indices,
                                const iscc_Dpid search_indices[const],
                                iscc_NNSearchObject** const out_nn_search_object)
{
	assert(iscc_check_data_set_object(data_set_object, 0));
	assert(len_search_indices > 0);
	assert(out_nn_search_object != NULL);

	*out_nn_search_object = malloc(sizeof(iscc_NNSearchObject));
	if (*out_nn_search_object == NULL) return false;

	**out_nn_search_object = (iscc_NNSearchObject) {
		.data_set_object = data_set_object,
		.len_search_indices = len_search_indices,
		.search_indices = search_indices,
	};

	return true;
}


bool iscc_nearest_neighbor_search(iscc_NNSearchObject* const nn_search_object,
                                  const size_t len_query_indicators,
                                  const bool query_indicators[const],
                                  bool out_query_indicators[const],
                                  const uint32_t k,
                                  const bool radius_search,
                                  const double radius,
                                  const bool accept_partial,
                                  iscc_Arci out_nn_ref[const],
                                  iscc_Dpid out_nn_indices[const])
{
	assert(nn_search_object != NULL);
	const scc_DataSetObject* const data_set_object = nn_search_object->data_set_object;
    const size_t len_search_indices = nn_search_object->len_search_indices;
	const iscc_Dpid* const search_indices = nn_search_object->search_indices;

	assert(len_search_indices > 0);
	assert(len_query_indicators > 0);
	assert(k > 0);
	assert(k <= len_search_indices);
	assert(!radius_search || (radius > 0.0));
	assert(out_nn_ref != NULL);
	assert(out_nn_indices != NULL);

	double tmp_dist;
	iscc_Dpid* index_write = out_nn_indices;
	double* const sort_scratch = malloc(sizeof(double[k]));
	if (sort_scratch == NULL) return false;
	double* const sort_scratch_end = sort_scratch + k - 1;

	out_nn_ref[0] = 0;
	if (search_indices == NULL) {
		for (size_t q = 0; q < len_query_indicators; ++q) {
			if ((query_indicators == NULL) || query_indicators[q]) {
				size_t s = 0;
				uint32_t found = 0;
				iscc_Dpid* const index_write_end = index_write + k - 1;

				for (; (s < len_search_indices) && (found < k); ++s) {
					tmp_dist = iscc_get_sq_dist(data_set_object, q, s);
					if (radius_search && (tmp_dist > radius)) continue;
					iscc_add_dist_to_list(tmp_dist, (iscc_Dpid) s, sort_scratch + found, index_write + found, sort_scratch);
					++found;
				}

				for (; s < len_search_indices; ++s) {
					assert(found == k);
					tmp_dist = iscc_get_sq_dist(data_set_object, q, s);
					if (tmp_dist >= *sort_scratch_end) continue;
					iscc_add_dist_to_list(tmp_dist, (iscc_Dpid) s, sort_scratch_end, index_write_end, sort_scratch);
				}

				if (radius_search && !accept_partial && (found < k)) {
					found = 0;
					if (out_query_indicators != NULL) out_query_indicators[q] = false;
				} 
				out_nn_ref[q + 1] = out_nn_ref[q] + found;
				index_write += found;
			} else {
				out_nn_ref[q + 1] = out_nn_ref[q];
			}
		}

	} else if (search_indices != NULL) {
		for (size_t q = 0; q < len_query_indicators; ++q) {
			if ((query_indicators == NULL) || query_indicators[q]) {
				size_t s = 0;
				uint32_t found = 0;
				iscc_Dpid* const index_write_end = index_write + k - 1;

				for (; (s < len_search_indices) && (found < k); ++s) {
					tmp_dist = iscc_get_sq_dist(data_set_object, q, (size_t) search_indices[s]);
					if (radius_search && (tmp_dist > radius)) continue;
					iscc_add_dist_to_list(tmp_dist, search_indices[s], sort_scratch + found, index_write + found, sort_scratch);
					++found;
				}

				for (; s < len_search_indices; ++s) {
					assert(found == k);
					tmp_dist = iscc_get_sq_dist(data_set_object, q, (size_t) search_indices[s]);
					if (tmp_dist >= *sort_scratch_end) continue;
					iscc_add_dist_to_list(tmp_dist, search_indices[s], sort_scratch_end, index_write_end, sort_scratch);
				}

				if (radius_search && !accept_partial && (found < k)) {
					found = 0;
					if (out_query_indicators != NULL) out_query_indicators[q] = false;
				} 
				out_nn_ref[q + 1] = out_nn_ref[q] + found;
				index_write += found;
			} else {
				out_nn_ref[q + 1] = out_nn_ref[q];
			}
		}
	}

	free(sort_scratch);

	return true;
}


bool iscc_close_nn_search_object(iscc_NNSearchObject** const nn_search_object)
{
	if (nn_search_object != NULL) {
		free(*nn_search_object);
		*nn_search_object = NULL;
	}
	return true;
}


// ==============================================================================
// Internal function implementations 
// ==============================================================================

static inline double iscc_get_sq_dist(const scc_DataSetObject* const data_set_object,
                                      size_t index1,
                                      size_t index2)
{
	assert(index1 < data_set_object->rows);
	assert(index2 < data_set_object->rows);

	const double* data1 = &data_set_object->elements[index1 * data_set_object->cols];
	const double* const data1_stop = data1 + data_set_object->cols;
	const double* data2 = &data_set_object->elements[index2 * data_set_object->cols];

	double tmp_dist = 0.0;
	while (data1 != data1_stop) {
		const double value_diff = (*data1 - *data2);
		++data1;
		++data2;
		tmp_dist += value_diff * value_diff;
	}
	return tmp_dist;
}


static inline void iscc_add_dist_to_list(const double add_dist,
                                         const iscc_Dpid add_index,
                                         double* dist_list,
                                         iscc_Dpid* index_list,
                                         const double* const dist_list_start)
{
	assert(dist_list != NULL);
	assert(index_list != NULL);
	assert(dist_list_start != NULL);

	for (; (dist_list != dist_list_start) && (add_dist < dist_list[-1]); --dist_list, --index_list) {
		dist_list[0] = dist_list[-1];
		index_list[0] = index_list[-1];
	}
	dist_list[0] = add_dist;
	index_list[0] = add_index;
}
