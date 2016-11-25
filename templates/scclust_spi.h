/* =============================================================================
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
 * ========================================================================== */

/** @file
 *
 *  The scclust library SPI...
 */

#ifndef SCC_SCCLUST_SPI_HG
#define SCC_SCCLUST_SPI_HG

{% limits_include %}#ifdef __cplusplus
{% limits_include %}// So g++ defines integer limits
{% limits_include %}#define __STDC_LIMIT_MACROS
{% limits_include %}#endif
{% limits_include %}
{% limits_include %}#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


// =============================================================================
// Internal types
// =============================================================================

/** Type used for data point IDs. May be unsigned or signed.
 *
 *  \note
 *  Number of data points in any clustering problem must be strictly less
 *  than the maximum number that can be stored in #iscc_Dpid.
 */
typedef {% dpid_type %} iscc_Dpid;

static const iscc_Dpid ISCC_DPID_NA = {% dpid_na %};

/** Type used for arc indices. Must be unsigned.
 *
 *  \note
 *  Number of arcs in any digraph must be less or equal to
 *  the maximum number that can be stored in #iscc_Arci.
 */
typedef {% arc_type %} iscc_Arci;

typedef struct iscc_MaxDistObject iscc_MaxDistObject;

typedef struct iscc_NNSearchObject iscc_NNSearchObject;

#define ISCC_M_DPID_TYPE_{% dpid_type %}
#define ISCC_M_DPID_NA {% dpid_na %}
#define ISCC_M_ARCI_TYPE_{% arc_type %}


// =============================================================================
// Distance search functions
// =============================================================================

typedef bool (*scc_check_data_set) (void*,
                                    size_t);

typedef bool (*scc_get_dist_matrix) (void*,
                                     size_t,
                                     const iscc_Dpid*,
                                     double*);

typedef bool (*scc_get_dist_rows) (void*,
                                   size_t,
                                   const iscc_Dpid*,
                                   size_t,
                                   const iscc_Dpid*,
                                   double*);

typedef bool (*scc_init_max_dist_object) (void*,
                                          size_t,
                                          const iscc_Dpid*,
                                          iscc_MaxDistObject**);

typedef bool (*scc_get_max_dist) (iscc_MaxDistObject*,
                                  size_t,
                                  const iscc_Dpid*,
                                  iscc_Dpid*,
                                  double*);

typedef bool (*scc_close_max_dist_object) (iscc_MaxDistObject**);

typedef bool (*scc_init_nn_search_object) (void*,
                                           size_t,
                                           const iscc_Dpid*,
                                           iscc_NNSearchObject**);

typedef bool (*scc_nearest_neighbor_search_digraph) (iscc_NNSearchObject*,
                                                     size_t,
                                                     const bool*,
                                                     bool*,
                                                     uint32_t,
                                                     bool,
                                                     double,
                                                     iscc_Arci*,
                                                     iscc_Dpid*);

typedef bool (*scc_nearest_neighbor_search_index) (iscc_NNSearchObject*,
                                                   size_t,
                                                   const iscc_Dpid*,
                                                   uint32_t,
                                                   bool,
                                                   double,
                                                   iscc_Dpid*);

typedef bool (*scc_close_nn_search_object) (iscc_NNSearchObject**);


// =============================================================================
// SPI functions
// =============================================================================

bool scc_set_dist_functions(scc_check_data_set,
                            scc_get_dist_matrix,
                            scc_get_dist_rows,
                            scc_init_max_dist_object,
                            scc_get_max_dist,
                            scc_close_max_dist_object,
                            scc_init_nn_search_object,
                            scc_nearest_neighbor_search_digraph,
                            scc_nearest_neighbor_search_index,
                            scc_close_nn_search_object);


#ifdef __cplusplus
}
#endif

#endif // ifndef SCC_SCCLUST_HG
