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

#ifndef SCC_DATA_OBJ_HG
#define SCC_DATA_OBJ_HG

#include <stddef.h>
#include <stdint.h>
#include "scclust.h"

#ifdef __cplusplus
extern "C" {
#endif


// ==============================================================================
// Data set type
// ==============================================================================

typedef struct scc_DataSetObject scc_DataSetObject;


// ==============================================================================
// Data set utility functions
// ==============================================================================

void scc_free_data_set_object(scc_DataSetObject** out_data_set_object);

scc_ErrorCode scc_get_data_set_object(uint64_t num_data_points,
                                      uint64_t num_dimensions,
                                      size_t len_data_matrix,
                                      double data_matrix[],
                                      bool deep_matrix_copy,
                                      scc_DataSetObject** out_data_set_object);


#ifdef __cplusplus
}
#endif

#endif // ifndef SCC_DATA_OBJ_HG
