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

//size_t iscc_get_data_point_count(scc_DataSetObject* data_set_object);

bool iscc_check_data_set_object(scc_DataSetObject* data_set_object,
                                size_t required_data_points);

// `output_dists` must be of length `(n_points - 1) n_points / 2`
bool iscc_get_dist_matrix(scc_DataSetObject* data_set_object,
                          size_t n_points,
                          const scc_Dpid* point_indices,
                          double* output_dists);

// `output_dists` must be of length `n_columns * n_query_rows`
bool iscc_get_dist_row(scc_DataSetObject* data_set_object,
                       size_t n_columns,
                       const scc_Dpid* column_indices,
                       size_t n_query_rows,
                       const scc_Dpid* query_indices,
                       double* output_dists);


// ==============================================================================
// Max dist functions
// ==============================================================================

typedef struct iscc_MaxDistObject iscc_MaxDistObject;

bool iscc_init_max_dist_object(scc_DataSetObject* data_set_object,
                               size_t n_search_points,
                               const scc_Dpid* search_indices,
                               iscc_MaxDistObject** out_max_dist_object);

// `max_indices` and `max_dists` must be of length `n_query_points`
bool iscc_get_max_dist(iscc_MaxDistObject* max_dist_object,
                       size_t n_query_points,
                       const scc_Dpid* query_indices,
                       scc_Dpid* max_indices,
                       double* max_dists);

bool iscc_close_max_dist_object(iscc_MaxDistObject** max_dist_object);


// ==============================================================================
// Nearest neighbor search functions
// ==============================================================================

bool iscc_nearest_neighbor_search(scc_DataSetObject* data_set_object,
                                  size_t n_search_points,
                                  const scc_Dpid search_indices[],
                                  size_t num_query_indicators,
                                  const bool query_indicators[],
                                  size_t k,
                                  bool radius_search,
                                  double radius,
                                  bool accept_partial,
                                  bool out_query_indicators[],
                                  scc_Arci* out_nn_ref,
                                  scc_Dpid* out_nn_indices);
                                     //    double* nn_dists);


#endif
