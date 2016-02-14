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
#include <stdlib.h>
#include "../include/scclust.h"


// ==============================================================================
// External structs definitions
// ==============================================================================

struct scc_DataSetObject {
	size_t cols;
	size_t rows;
	double* elements;
};

struct scc_DistColObject {
	scc_DataSetObject* data_set_object;
	size_t n_columns;
	const scc_Vid* column_indices;
};

struct scc_MaxDistObject {
	scc_DataSetObject* data_set_object;
	size_t n_search_points;
	const scc_Vid* search_indices;
};

struct scc_NNSearchObject {
	scc_DataSetObject* data_set_object;
	size_t k;
	bool radius_search;
	double radius;
	size_t n_search_points;
	const scc_Vid* search_indices;
};


// ==============================================================================
// Internal function prototypes
// ==============================================================================

static inline double iscc_get_dist(const scc_DataSetObject* data_set_object,
                                   scc_Vid index1,
                                   scc_Vid index2);


// ==============================================================================
// External function implementations
// ==============================================================================

size_t scc_get_data_point_count(scc_DataSetObject* const data_set_object)
{
	assert(data_set_object != NULL);
	return data_set_object->rows;
}


bool scc_is_valid_data_set_object(scc_DataSetObject* const data_set_object)
{
	if ((data_set_object == NULL) || (data_set_object->elements == NULL)) return false;
	if (data_set_object->cols == 0) return false;
	if (data_set_object->rows == 0) return false;
	return true;
}


bool scc_get_dist_matrix(scc_DataSetObject* const data_set_object,
                         const size_t n_points,
                         const scc_Vid* const point_indices,
                         double* output_dists)
{
	assert(scc_is_valid_data_set_object(data_set_object));
	assert(n_points > 1);
	assert((n_points == data_set_object->rows) || (point_indices != NULL));
	assert(output_dists != NULL);

	if (point_indices == NULL) {
		assert(n_points < SCC_VID_MAX);
		const scc_Vid n_points_vid = (scc_Vid) n_points; // If `n_points` is signed
		for (scc_Vid p1 = 0; p1 < n_points_vid; ++p1) {
			for (scc_Vid p2 = p1 + 1; p2 < n_points_vid; ++p2) {
				*output_dists = iscc_get_dist(data_set_object, p1, p2);
				++output_dists;
			}
		}
	} else {
		for (size_t p1 = 0; p1 < n_points; ++p1) {
			for (size_t p2 = p1 + 1; p2 < n_points; ++p2) {
				*output_dists = iscc_get_dist(data_set_object, point_indices[p1], point_indices[p2]);
				++output_dists;
			}
		}
	}
	
	return true;
}


scc_DistColObject* scc_init_dist_column_object(scc_DataSetObject* const data_set_object,
                                               const size_t n_columns,
                                               const scc_Vid* const column_indices,
                                               const size_t n_query_hint)
{
	assert(scc_is_valid_data_set_object(data_set_object));
	assert(n_columns > 0);
	assert((n_columns == data_set_object->rows) || (column_indices != NULL));

	(void) n_query_hint;

	scc_DistColObject* dco = malloc(sizeof(scc_DistColObject));
	if (dco == NULL) return NULL;

	*dco = (scc_DistColObject) {
		.data_set_object = data_set_object,
		.n_columns = n_columns,
		.column_indices = column_indices,
	};

	return dco;
}


bool scc_get_dist_row(scc_DistColObject* const dist_column_object,
                      const size_t n_query_rows,
                      const scc_Vid* const query_indices,
                      double* output_dists)
{
	assert(dist_column_object != NULL);
	scc_DistColObject dco = *dist_column_object;
	assert(n_query_rows > 0);
	assert((n_query_rows == dco.data_set_object->rows) || (query_indices != NULL));
	assert(output_dists != NULL);

	scc_Vid current_query;
	for (size_t qp = 0; qp < n_query_rows; ++qp) {
		if (query_indices == NULL) {
			current_query = (scc_Vid) qp;
		} else {
			current_query = query_indices[qp];
		}
		scc_Vid current_column;
		for (size_t c = 0; c < dco.n_columns; ++c) {
			if (dco.column_indices == NULL) {
				current_column = (scc_Vid) c;
			} else {
				current_column = dco.column_indices[c];
			}

			*output_dists = iscc_get_dist(dco.data_set_object, current_query, current_column);
			++output_dists;
		}
	}
	
	return true;
}


bool scc_close_dist_column_object(scc_DistColObject* const dist_column_object)
{
	free(dist_column_object);
	return true;
}


scc_MaxDistObject* scc_init_max_dist_object(scc_DataSetObject* const data_set_object,
                                            const size_t n_search_points,
                                            const scc_Vid* const search_indices,
                                            const size_t n_query_hint)
{
	assert(scc_is_valid_data_set_object(data_set_object));
	assert(n_search_points > 0);
	assert((n_search_points == data_set_object->rows) || (search_indices != NULL));

	(void) n_query_hint;

	scc_MaxDistObject* mdo = malloc(sizeof(scc_MaxDistObject));
	if (mdo == NULL) return NULL;

	*mdo = (scc_MaxDistObject) {
		.data_set_object = data_set_object,
		.n_search_points = n_search_points,
		.search_indices = search_indices,
	};

	return mdo;
}


bool scc_get_max_dist(scc_MaxDistObject* const max_dist_object,
                      const size_t n_query_points,
                      const scc_Vid* const query_indices,
                      scc_Vid* const max_indices,
                      double* const max_dists)
{
	assert(max_dist_object != NULL);
	scc_MaxDistObject mdo = *max_dist_object;
	assert(n_query_points > 0);
	assert((n_query_points == mdo.data_set_object->rows) || (query_indices != NULL));
	assert(max_indices != NULL);

	scc_Vid current_query;
	scc_Vid search_point;
	double tmp_dist;
	double max_dist = -1.0;

	for (size_t qp = 0; qp < n_query_points; ++qp) {
		if (query_indices == NULL) {
			current_query = (scc_Vid) qp;
		} else {
			current_query = query_indices[qp];
			assert(((size_t) current_query) < mdo.data_set_object->rows);
		}

		max_dist = -1.0;

		for (size_t sp = 0; sp < mdo.n_search_points; ++sp) {
			if (mdo.search_indices == NULL) {
				search_point = (scc_Vid) sp;
			} else {
				search_point = mdo.search_indices[sp];
				assert(((size_t) search_point) < mdo.data_set_object->rows);
			}

			tmp_dist = iscc_get_dist(mdo.data_set_object, current_query, search_point);

			if (max_dist < tmp_dist) {
				max_dist = tmp_dist;
				max_indices[qp] = search_point;
			}
		}

		if (max_dists != NULL) max_dists[qp] = max_dist;
	}

	return true;
}


bool scc_close_max_dist_object(scc_MaxDistObject* const max_dist_object)
{
	free(max_dist_object);
	return true;
}


scc_NNSearchObject* scc_init_nn_search_object(scc_DataSetObject* const data_set_object,
                                              const size_t k,
                                              const bool radius_search,
                                              const double radius,
                                              const size_t n_search_points,
                                              const scc_Vid* const search_indices,
                                              const size_t n_query_hint)
{
	assert(scc_is_valid_data_set_object(data_set_object));
	assert(k > 0);
	assert(!radius_search || (radius > 0.0));
	assert(n_search_points > 0);
	assert(k <= n_search_points);
	assert((n_search_points == data_set_object->rows) || (search_indices != NULL));
	
	(void) n_query_hint;

	scc_NNSearchObject* so = malloc(sizeof(scc_NNSearchObject));
	if (so == NULL) return NULL;

	*so = (scc_NNSearchObject) {
		.data_set_object = data_set_object,
		.k = k,
		.radius_search = radius_search,
		.radius = radius,
		.n_search_points = n_search_points,
		.search_indices = search_indices,
	};

	return so;
}


bool scc_nearest_neighbor_search(scc_NNSearchObject* const nn_search_object,
                                 const size_t n_query_points,
                                 const scc_Vid* const query_indices,
                                 scc_Vid* const nn_indices,
                                 double* const nn_dists)
{
	assert(nn_search_object != NULL);
	scc_NNSearchObject so = *nn_search_object;
	assert(n_query_points > 0);
	assert((n_query_points == so.data_set_object->rows) || (query_indices != NULL));
	assert(nn_indices != NULL);

	scc_Vid current_query;
	scc_Vid* index_pos = nn_indices;
	double* dist_pos;
	double* sort_scratch;

	if (nn_dists == NULL) {
		sort_scratch = malloc(sizeof(double[so.k]));
		if (sort_scratch == NULL) return false;
		dist_pos = sort_scratch;
	} else {
		dist_pos = nn_dists;
	}

	for (size_t qp = 0; qp < n_query_points; ++qp) {
		if (query_indices == NULL) {
			current_query = (scc_Vid) qp;
		} else {
			current_query = query_indices[qp];
		}
		if (nn_dists == NULL) dist_pos = sort_scratch;


		scc_Vid search_point;
		const double* const start_dist_pos = dist_pos;
		const double* const stop_dist_pos = dist_pos + so.k;
		for (size_t sp = 0; sp < so.n_search_points; ++sp) {
			if (so.search_indices == NULL) {
				search_point = (scc_Vid) sp;
			} else {
				search_point = so.search_indices[sp];
			}

			double tmp_dist = iscc_get_dist(so.data_set_object, current_query, search_point);

			if (so.radius_search && tmp_dist >= so.radius) continue;

			if (dist_pos != stop_dist_pos) {
				index_pos[0] = search_point;
				dist_pos[0] = tmp_dist;
				++index_pos;
				++dist_pos;
			} else if (dist_pos[-1] > tmp_dist) {
				index_pos[-1] = search_point;
				dist_pos[-1] = tmp_dist;
			}

			scc_Vid* check_index = &index_pos[-1];
			double* check_dist = &dist_pos[-1];
			for (; (check_dist != start_dist_pos) && (check_dist[-1] > check_dist[0]); --check_index, --check_dist) {
				const scc_Vid tmp_index = check_index[0];
				const double tmp_dist = check_dist[0];
				check_index[0] = check_index[-1];
				check_dist[0] = check_dist[-1];
				check_index[-1] = tmp_index;
				check_dist[-1] = tmp_dist;
			}
		}

		for (; dist_pos != stop_dist_pos; ++index_pos, ++dist_pos) {
			index_pos[0] = SCC_VID_NA;
			dist_pos[0] = -1.0;
		}
	}

	if (nn_dists == NULL) free(sort_scratch);

	return true;
}


bool scc_close_nn_search_object(scc_NNSearchObject* const nn_search_object)
{
	free(nn_search_object);
	return true;
}


// ==============================================================================
// Internal function implementations 
// ==============================================================================

static inline double iscc_get_dist(const scc_DataSetObject* const data_set_object,
                                   const scc_Vid index1,
                                   const scc_Vid index2)
{
	assert(((size_t) index1) < data_set_object->rows);
	assert(((size_t) index2) < data_set_object->rows);

	double tmp_dist = 0.0;
	for (size_t dim = 0; dim < data_set_object->cols; ++dim) {
		const double value_diff = data_set_object->elements[data_set_object->cols * ((size_t) index1) + dim] -
		                              data_set_object->elements[data_set_object->cols * ((size_t) index2) + dim];
		tmp_dist += value_diff * value_diff;
	}
	return sqrt(tmp_dist);
}
