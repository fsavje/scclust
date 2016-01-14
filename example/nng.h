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


#ifndef NNG_HG
#define NNG_HG

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "../include/config.h"
#include "../include/digraph.h"

typedef struct Matrix Matrix;
struct Matrix {
	uint8_t cols;
	size_t rows;
	double* elements;
};

scc_Digraph get_nng(const Matrix* data_matrix,
                    const bool* query_indicators,
                    size_t n_search_points,
                    const scc_Vid* search_indices,
                    scc_Vid k,
                    bool use_caliper,
                    double caliper,
                    bool accept_partial);

scc_Digraph get_all_nng(const Matrix* data_matrix,
                        scc_Vid k,
                        bool use_caliper,
                        double caliper,
                    	const bool accept_partial);

#ifdef __cplusplus
}
#endif

#endif
