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


#include "error.h"

#include <assert.h>
#include <stdio.h>
#include "../include/scclust.h"


// ==============================================================================
// Internal variables
// ==============================================================================

static const char* iscc_error_file = "unknown file";

static int iscc_error_line = -1;

static const scc_ErrorCode iscc_max_error_code = 10;


// ==============================================================================
// External function implementations
// ==============================================================================

scc_ErrorCode iscc_make_error_func(const scc_ErrorCode ec,
                                   const char* const file,
	                               const int line)
{
	assert((ec > SCC_ER_OK) && (ec < iscc_max_error_code));

	iscc_error_file = file;
	iscc_error_line = line;

	return ec;
}


void scc_get_error_message(const scc_ErrorCode ec,
                           char error_message_buffer[const],
                           const size_t buffer_size)
{
	if ((error_message_buffer == NULL) || (buffer_size == 0)) return;

	if (ec == SCC_ER_OK) {
		snprintf(error_message_buffer, buffer_size, "%s", "No error.");
		return;
	}

	const char* error_message = "";
	switch (ec) {
		case SCC_ER_NO_MEMORY:
			error_message = "Cannot allocate required memory.";
			break;
		default:
			error_message = "Unknown error code.";
			break;
	}

	snprintf(error_message_buffer, buffer_size, "%s (%s:%d)", error_message, iscc_error_file, iscc_error_line);
}
