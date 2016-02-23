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


#ifndef SCC_FINDSEEDS_HG
#define SCC_FINDSEEDS_HG

#include <stddef.h>
#include "../include/scclust.h"
#include "digraph_core.h"

typedef struct iscc_SeedResult iscc_SeedResult;
struct iscc_SeedResult {
	size_t capacity;
	size_t count;
	scc_Dpid* seeds;
};

static const iscc_SeedResult ISCC_NULL_SEED_RESULT = { 0, 0, NULL };

scc_ErrorCode iscc_find_seeds(const iscc_Digraph* nng,
                              scc_SeedMethod sm,
                              iscc_SeedResult* out_seeds);


#endif
