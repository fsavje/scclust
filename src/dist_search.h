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
#include "../include/scclust.h"


// ==============================================================================
// Miscellaneous functions
// ==============================================================================

size_t iscc_get_data_point_count(scc_DataSetObject* data_set_object);

bool iscc_is_valid_data_set_object(scc_DataSetObject* data_set_object);

// `output_dists` must be of length `(n_points - 1) n_points / 2`
bool iscc_get_dist_matrix(scc_DataSetObject* data_set_object,
                          size_t n_points,
                          const scc_Dpid* point_indices,
                          double* output_dists);


// ==============================================================================
// Distance row generation functions
// ==============================================================================

typedef struct iscc_DistColObject iscc_DistColObject;

iscc_DistColObject* iscc_init_dist_column_object(scc_DataSetObject* data_set_object,
                                                 size_t n_columns,
                                                 const scc_Dpid* column_indices,
                                                 size_t n_query_hint);

// `output_dists` must be of length `n_columns * n_query_rows`
bool iscc_get_dist_row(iscc_DistColObject* dist_column_object,
                       size_t n_query_rows,
                       const scc_Dpid* query_indices,
                       double* output_dists);

bool iscc_close_dist_column_object(iscc_DistColObject* dist_column_object);


// ==============================================================================
// Max dist functions
// ==============================================================================

typedef struct iscc_MaxDistObject iscc_MaxDistObject;

iscc_MaxDistObject* iscc_init_max_dist_object(scc_DataSetObject* data_set_object,
                                              size_t n_search_points,
                                              const scc_Dpid* search_indices,
                                              size_t n_query_hint);

// `max_indices` and `max_dists` must be of length `n_query_points`
bool iscc_get_max_dist(iscc_MaxDistObject* max_dist_object,
                       size_t n_query_points,
                       const scc_Dpid* query_indices,
                       scc_Dpid* max_indices,
                       double* max_dists);

bool iscc_close_max_dist_object(iscc_MaxDistObject* max_dist_object);


// ==============================================================================
// Nearest neighbor search functions
// ==============================================================================

typedef struct iscc_NNSearchObject iscc_NNSearchObject;

iscc_NNSearchObject* iscc_init_nn_search_object(scc_DataSetObject* data_set_object,
                                                size_t k,
                                                bool radius_search,
                                                double radius,
                                                size_t n_search_points,
                                                const scc_Dpid* search_indices,
                                                size_t n_query_hint);

// `nn_indices` and `nn_dists` must be of length `k * n_query_points`
bool iscc_nearest_neighbor_search(iscc_NNSearchObject* nn_search_object,
                                  size_t n_query_points,
                                  const scc_Dpid* query_indices,
                                  scc_Dpid* nn_indices,
                                  double* nn_dists);

bool iscc_close_nn_search_object(iscc_NNSearchObject* nn_search_object);


#endif
