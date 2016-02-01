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


#include "nn_search.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include "../include/config.h"


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
	scc_Distance radius;
	size_t n_search_points;
	const scc_Vid* search_indices;
	scc_Distance* dist_scratch;
};


// ==============================================================================
// Internal function prototypes
// ==============================================================================

static inline void iscc_nns_populate_dist_list(const scc_DataSetObject* data_set_object,
                                               scc_Distance* dist_list,
                                               scc_Vid query_index,
                                               size_t n_search_points,
                                               const scc_Vid* search_indices);


// ==============================================================================
// External function implementations
// ==============================================================================

scc_Vid scc_get_data_point_count(scc_DataSetObject* const data_set_object)
{
	assert(data_set_object != NULL);
	return (scc_Vid) data_set_object->rows;
}


scc_DistColObject* scc_init_dist_column_object(scc_DataSetObject* const data_set_object,
                                               const size_t n_columns,
                                               const scc_Vid* const column_indices,
                                               const size_t n_query_hint)
{
	assert(data_set_object != NULL);
	assert(n_columns > 0);
	assert((n_columns == data_set_object->rows) || (column_indices != NULL));

	(void) n_query_hint;

	if (column_indices != NULL) {
		for (size_t c = 0; c < n_columns; ++c) {
			assert(column_indices[c] < data_set_object->rows);
		}
	}

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
                      scc_Distance* const output_dists)
{
	assert(dist_column_object != NULL);
	scc_DistColObject dco = *dist_column_object;

	assert(dco.n_columns > 0);
	assert((dco.n_columns == dco.data_set_object->rows) || (dco.column_indices != NULL));
	assert(n_query_rows > 0);
	assert((n_query_rows == dco.data_set_object->rows) || (query_indices != NULL));
	assert(output_dists != NULL);

	scc_Distance* dist_pos = output_dists;
	scc_Vid current_query;
	for (size_t qp = 0; qp < n_query_rows; ++qp, dist_pos += dco.n_columns) {
		if (query_indices == NULL) {
			current_query = (scc_Vid) qp;
		} else {
			current_query = query_indices[qp];
			assert(current_query < dco.data_set_object->rows);
		}
		iscc_nns_populate_dist_list(dco.data_set_object, dist_pos, current_query, dco.n_columns, dco.column_indices);
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
	assert(data_set_object != NULL);
	assert(n_search_points > 0);
	assert((n_search_points == data_set_object->rows) || (search_indices != NULL));

	(void) n_query_hint;

	if (search_indices != NULL) {
		for (size_t sp = 0; sp < n_search_points; ++sp) {
			assert(search_indices[sp] < data_set_object->rows);
		}
	}

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
                      scc_Distance* const max_dists)
{
	assert(max_dist_object != NULL);
	scc_MaxDistObject mdo = *max_dist_object;

	assert(mdo.n_search_points > 0);
	assert((mdo.n_search_points == mdo.data_set_object->rows) || (mdo.search_indices != NULL));
	assert(n_query_points > 0);
	assert((n_query_points == mdo.data_set_object->rows) || (query_indices != NULL));
	assert(max_indices != NULL);

	scc_Vid current_query;
	scc_Vid search_point;
	scc_Distance tmp_dist;
	scc_Distance max_dist = -1.0;

	for (size_t qp = 0; qp < n_query_points; ++qp) {
		if (query_indices == NULL) {
			current_query = (scc_Vid) qp;
		} else {
			current_query = query_indices[qp];
			assert(current_query < mdo.data_set_object->rows);
		}

		max_dist = -1.0;

		for (size_t sp = 0; sp < mdo.n_search_points; ++sp) {
			if (mdo.search_indices == NULL) {
				search_point = (scc_Vid) sp;
			} else {
				search_point = mdo.search_indices[sp];
				assert(search_point < mdo.data_set_object->rows);
			}

			tmp_dist = 0.0;
			for (size_t dim = 0; dim < mdo.data_set_object->cols; ++dim) {
				const double value_diff = mdo.data_set_object->elements[mdo.data_set_object->cols * current_query + dim] -
				                              mdo.data_set_object->elements[mdo.data_set_object->cols * search_point + dim];
				tmp_dist += value_diff * value_diff;
			}
			tmp_dist = sqrt(tmp_dist);

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
                                              const scc_Distance radius,
                                              const size_t n_search_points,
                                              const scc_Vid* const search_indices,
                                              const size_t n_query_hint)
{
	assert(data_set_object != NULL);
	assert(k > 0);
	assert(!radius_search || (radius > 0.0));
	assert(n_search_points > 0);
	assert(k <= n_search_points);
	assert((n_search_points == data_set_object->rows) || (search_indices != NULL));
	
	(void) n_query_hint;

	if (search_indices != NULL) {
		for (size_t sp = 0; sp < n_search_points; ++sp) {
			assert(search_indices[sp] < data_set_object->rows);
		}
	}

	scc_NNSearchObject* so = malloc(sizeof(scc_NNSearchObject));
	if (so == NULL) return NULL;

	*so = (scc_NNSearchObject) {
		.data_set_object = data_set_object,
		.k = k,
		.radius_search = radius_search,
		.radius = radius,
		.n_search_points = n_search_points,
		.search_indices = search_indices,
		.dist_scratch = malloc(sizeof(scc_Distance[n_search_points])),
	};
	if (so->dist_scratch == NULL) {
		free(so);
		return NULL;
	}

	return so;
}


bool scc_nearest_neighbor_search(scc_NNSearchObject* const nn_search_object,
                                 const size_t n_query_points,
                                 const scc_Vid* const query_indices,
                                 scc_Vid* const nn_indices,
                                 scc_Distance* const nn_dists)
{
	assert(nn_search_object != NULL);
	scc_NNSearchObject so = *nn_search_object;

	assert(so.n_search_points > 0);
	assert((so.n_search_points == so.data_set_object->rows) || (so.search_indices != NULL));
	assert(n_query_points > 0);
	assert((n_query_points == so.data_set_object->rows) || (query_indices != NULL));
	assert(nn_indices != NULL);

	scc_Vid current_query;
	scc_Vid* index_pos = nn_indices;
	scc_Distance* dist_pos;
	scc_Distance* sort_scratch;

	if (nn_dists == NULL) {
		sort_scratch = malloc(sizeof(scc_Distance[so.k]));
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
			assert(current_query < so.data_set_object->rows);
		}
		if (nn_dists == NULL) dist_pos = sort_scratch;

		iscc_nns_populate_dist_list(so.data_set_object, so.dist_scratch, current_query, so.n_search_points, so.search_indices);

		const scc_Distance* const start_dist_pos = dist_pos;
		const scc_Distance* const stop_dist_pos = dist_pos + so.k;

		for (size_t sp = 0; sp < so.n_search_points; ++sp) {
			if (so.radius_search && so.dist_scratch[sp] >= so.radius) continue;

			if (dist_pos != stop_dist_pos) {
				if (so.search_indices == NULL) {
					index_pos[0] = (scc_Vid) sp;
				} else {
					index_pos[0] = so.search_indices[sp];
				}
				dist_pos[0] = so.dist_scratch[sp];
				++index_pos;
				++dist_pos;
			} else if (dist_pos[-1] > so.dist_scratch[sp]) {
				if (so.search_indices == NULL) {
					index_pos[-1] = (scc_Vid) sp;
				} else {
					index_pos[-1] = so.search_indices[sp];
				}
				dist_pos[-1] = so.dist_scratch[sp];
			}

			scc_Vid* check_index = &index_pos[-1];
			scc_Distance* check_dist = &dist_pos[-1];
			for (; (check_dist != start_dist_pos) && (check_dist[-1] > check_dist[0]); --check_index, --check_dist) {
				const scc_Vid tmp_index = check_index[0];
				const scc_Distance tmp_dist = check_dist[0];
				check_index[0] = check_index[-1];
				check_dist[0] = check_dist[-1];
				check_index[-1] = tmp_index;
				check_dist[-1] = tmp_dist;
			}
		}

		for (; dist_pos != stop_dist_pos; ++index_pos, ++dist_pos) {
			index_pos[0] = SCC_VID_MAX;
			dist_pos[0] = -1.0;
		}
	}

	if (nn_dists == NULL) free(sort_scratch);

	return true;
}


bool scc_close_nn_search_object(scc_NNSearchObject* const nn_search_object)
{
	free(nn_search_object->dist_scratch);
	free(nn_search_object);
	return true;
}


// ==============================================================================
// Internal function implementations 
// ==============================================================================

static inline void iscc_nns_populate_dist_list(const scc_DataSetObject* const data_set_object,
                                               scc_Distance* const dist_list,
                                               const scc_Vid query_index,
                                               const size_t n_search_points,
                                               const scc_Vid* const search_indices)
{
	assert(query_index < data_set_object->rows);
	assert((n_search_points == data_set_object->rows) || (search_indices != NULL));
	scc_Vid search_point;
	for (size_t sp = 0; sp < n_search_points; ++sp) {
		if (search_indices == NULL) {
			search_point = (scc_Vid) sp;
		} else {
			search_point = search_indices[sp];
			assert(search_point < data_set_object->rows);
		}
		dist_list[sp] = 0.0;
		for (size_t dim = 0; dim < data_set_object->cols; ++dim) {
			const double value_diff = data_set_object->elements[data_set_object->cols * query_index + dim] -
			                              data_set_object->elements[data_set_object->cols * search_point + dim];
			dist_list[sp] += value_diff * value_diff;
		}
		dist_list[sp] = sqrt(dist_list[sp]);
	}
}
