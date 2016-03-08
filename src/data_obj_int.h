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

#ifndef SCC_DATA_OBJ_INT_HG
#define SCC_DATA_OBJ_INT_HG

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


// ==============================================================================
// Structs, types and variables
// ==============================================================================

static const int64_t ISCC_CURRENT_DATASETOBJ_VERSION = 1;

struct scc_DataSetObject {
	size_t num_data_points;
	uint_fast16_t num_dimensions;
	double* data_matrix;
	bool external_matrix;
	int64_t data_set_object_version;
};


#ifdef __cplusplus
}
#endif

#endif // ifndef SCC_DATA_OBJ_INT_HG
