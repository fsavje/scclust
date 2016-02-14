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

#ifndef SCC_DIST_SEARCH_HG
#define SCC_DIST_SEARCH_HG

#include <stdbool.h>
#include <stddef.h>
#include "../include/config.h"


// ==============================================================================
// Miscellaneous functions
// ==============================================================================

size_t scc_get_data_point_count(scc_DataSetObject* data_set_object);

bool scc_is_valid_data_set_object(scc_DataSetObject* data_set_object);

// `output_dists` must be of length `(n_points - 1) n_points / 2`
bool scc_get_dist_matrix(scc_DataSetObject* data_set_object,
                         size_t n_points,
                         const scc_Vid* point_indices,
                         scc_Distance* output_dists);


// ==============================================================================
// Distance row generation functions
// ==============================================================================

typedef struct scc_DistColObject scc_DistColObject;

scc_DistColObject* scc_init_dist_column_object(scc_DataSetObject* data_set_object,
                                               size_t n_columns,
                                               const scc_Vid* column_indices,
                                               size_t n_query_hint);

// `output_dists` must be of length `n_columns * n_query_rows`
bool scc_get_dist_row(scc_DistColObject* dist_column_object,
                      size_t n_query_rows,
                      const scc_Vid* query_indices,
                      scc_Distance* output_dists);

bool scc_close_dist_column_object(scc_DistColObject* dist_column_object);


// ==============================================================================
// Max dist functions
// ==============================================================================

typedef struct scc_MaxDistObject scc_MaxDistObject;

scc_MaxDistObject* scc_init_max_dist_object(scc_DataSetObject* data_set_object,
                                            size_t n_search_points,
                                            const scc_Vid* search_indices,
                                            size_t n_query_hint);

// `max_indices` and `max_dists` must be of length `n_query_points`
bool scc_get_max_dist(scc_MaxDistObject* max_dist_object,
                      size_t n_query_points,
                      const scc_Vid* query_indices,
                      scc_Vid* max_indices,
                      scc_Distance* max_dists);

bool scc_close_max_dist_object(scc_MaxDistObject* max_dist_object);


// ==============================================================================
// Nearest neighbor search functions
// ==============================================================================

typedef struct scc_NNSearchObject scc_NNSearchObject;

scc_NNSearchObject* scc_init_nn_search_object(scc_DataSetObject* data_set_object,
                                              size_t k,
                                              bool radius_search,
                                              scc_Distance radius,
                                              size_t n_search_points,
                                              const scc_Vid* search_indices,
                                              size_t n_query_hint);

// `nn_indices` and `nn_dists` must be of length `k * n_query_points`
bool scc_nearest_neighbor_search(scc_NNSearchObject* nn_search_object,
                                 size_t n_query_points,
                                 const scc_Vid* query_indices,
                                 scc_Vid* nn_indices,
                                 scc_Distance* nn_dists);

bool scc_close_nn_search_object(scc_NNSearchObject* nn_search_object);


#endif
