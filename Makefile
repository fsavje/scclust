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

# To compile the scclust library, documentation or both, call, respectively:
# > make library
# > make doc
# > make

# This makefile has the following options (see documentation for more details):

# Compile with debug asserts (Y/N)
DEBUG = N

# Compile with unit test headers for testing overflows etc (Y/N)
TESTH = N

# Data point ID data type (SCC_DPID_INT/SCC_DPID_UINT32/SCC_DPID_UINT64)
DPID = SCC_DPID_UINT32

# Arc reference data type (SCC_ARC32/SCC_ARC64)
ARC = SCC_ARC32

# Compile with nearest neighbor search using the ANN library (Y/N)
ANN_SEARCH = N

# Tree data type used by ANN (SCC_ANN_KDTREE/SCC_ANN_BDTREE)
ANN_TREE = SCC_ANN_KDTREE


# Set up

CFLAGS = -std=c99 -O2 -pedantic -Wall -Wextra -Wconversion -Wfloat-equal -Werror
CXXFLAGS = -std=c++11 -O2 -pedantic -Wall -Wextra -Wconversion -Wfloat-equal -Werror
XTRA_FLAGS =
XTRA_CFLAGS =

LIBDIR = lib
BUILDDIR = build
OBJECTS = digraph_core.o digraph_operations.o dist_nnsearch_c.o dist_search.o error.o \
          hierarchical_clustering.o nng_clustering.o nng_batch_clustering.o \
          nng_core.o nng_findseeds.o scc_data_set.o scclust.o
OBJECTS := $(addprefix $(BUILDDIR)/,$(OBJECTS))
XTRA_OBJECTS =


# Main targets

.PHONY: all library doc clean clean-all
.SECONDEXPANSION:

all: doc library

library: $$(OBJECTS) $$(XTRA_OBJECTS)
	$(AR) rcs $(LIBDIR)/libscc.a $^

doc:
	mkdir -p doc
	doxygen DoxyAPI

clean-all: clean
	cd exlib/libANN/ && $(MAKE) clean

clean:
	$(RM) -R doc/ $(BUILDDIR)/ $(LIBDIR)/


# Build objects

$(BUILDDIR)/%.o: src/%.c
	$(CC) -c $(CFLAGS) $(XTRA_FLAGS) $(XTRA_CFLAGS) $< -o $@

$(BUILDDIR)/dist_nnsearch_ANN.o: src/dist_nnsearch_ANN.cpp
	$(CXX) -c $(CXXFLAGS) $(XTRA_FLAGS) $< -o $@


# Dependencies

exlib/libANN/build/%.o:
	cd exlib/libANN/ && $(MAKE)


# Parse options

ifeq ($(DEBUG), Y)
OBJECTS += $(BUILDDIR)/digraph_debug.o
else
XTRA_FLAGS += -DNDEBUG
endif

ifeq ($(TESTH), Y)
XTRA_CFLAGS += -include tests/test_suite.h
endif

ifneq ($(DPID), SCC_DPID_UINT32)
XTRA_FLAGS += -D$(DPID)
endif

ifneq ($(ARC), SCC_ARC32)
XTRA_FLAGS += -D$(ARC)
endif

ifeq ($(ANN_SEARCH), Y)
OBJECTS := $(filter-out $(BUILDDIR)/dist_nnsearch_c.o,$(OBJECTS)) $(BUILDDIR)/dist_nnsearch_ANN.o
XTRA_OBJECTS += $(addprefix exlib/libANN/build/,ANN.o brute.o kd_tree.o kd_util.o kd_split.o kd_dump.o \
                                                kd_search.o kd_pr_search.o kd_fix_rad_search.o bd_tree.o \
                                                bd_search.o bd_pr_search.o bd_fix_rad_search.o perf.o)
endif

ifneq ($(ANN_TREE), SCC_ANN_KDTREE)
XTRA_FLAGS += -D$(ANN_TREE)
endif


# Build folders

$(OBJECTS): | $(BUILDDIR)
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

library: | $(LIBDIR)
$(LIBDIR):
	mkdir -p $(LIBDIR)
