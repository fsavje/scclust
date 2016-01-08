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


#ifndef SCC_FINDSEED_DEBUG_HG
#define SCC_FINDSEED_DEBUG_HG

#include <assert.h>
#include "../include/config.h"


static inline void iscc_fs_debug_bucket_sort(scc_Vid* const bucket_start,
                                             scc_Vid* pos,
                                             const scc_Vid* const inwards_count,
                                             scc_Vid** const vertex_index) {
	scc_Vid tmp_v = *pos;
	for (; pos != bucket_start; --pos) {
		assert(inwards_count[tmp_v] == inwards_count[*(pos - 1)]);
		if (tmp_v >= *(pos - 1)) break;
		*pos = *(pos - 1);
		vertex_index[*pos] = pos;
	}
	*pos = tmp_v;
	vertex_index[*pos] = pos;
}

static inline void iscc_fs_debug_check_sort(const scc_Vid* current_pos,
                                            const scc_Vid* const last_pos,
                                            const scc_Vid* const inwards_count) {
	for (; current_pos != last_pos; ++current_pos) {
		assert(inwards_count[*(current_pos)] <= inwards_count[*(current_pos + 1)]);
		if (inwards_count[*(current_pos)] == inwards_count[*(current_pos + 1)]) {
			assert(*(current_pos) < *(current_pos + 1));
		}
	}
}

#endif

