# ==============================================================================
# scclust -- A C library for size constrained clustering
# https://github.com/fsavje/scclust
#
# Copyright (C) 2015-2016  Fredrik Savje -- http://fredriksavje.com
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library. If not, see http://www.gnu.org/licenses/
# ==============================================================================

CFLAGS = -std=c99 -O2 -pedantic -Wall -Wextra -Wconversion -Wfloat-equal -Werror
SCC_PATHS = -I../../include -L../../lib

.PHONY: all clean

all: simple_example.out

clean:
	$(RM) simple_example.out

simple_example.out: simple_example.c
	$(CC) $(CFLAGS) $(SCC_PATHS) $< -lscclust -lm -o $@
