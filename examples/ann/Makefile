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
CXXFLAGS = -std=c++11 -O2 -pedantic -Wall -Wextra -Wconversion -Wfloat-equal -Werror
WRAPPER_PATHS = -I../.. -Iann_1.1.2
SCC_PATHS = -I../../include
LIB_PATHS = -L../../lib -Lann_1.1.2/lib


.PHONY: all clean

all: ann_example.out

clean:
	$(RM) *.out *.o

ann_example.out: ann_example.o ann_wrapper.o ann_1.1.2/lib/libANN.a
	$(CXX) $^ $(LIB_PATHS) -lscclust -lANN -o $@

ann_example.o: ann_example.c
	$(CC) -c $(CFLAGS) $(SCC_PATHS) $< -o $@

ann_wrapper.o: ann_wrapper.cpp ann_1.1.2
	$(CXX) -c $(CXXFLAGS) $(WRAPPER_PATHS) $< -o $@

ann_1.1.2:
	./download_ann.sh

ann_1.1.2/lib/libANN.a: ann_1.1.2
	cd ann_1.1.2/src && $(MAKE) linux-g++
