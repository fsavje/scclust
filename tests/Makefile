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

DPID = SCC_DPID_UINT32
ARC = SCC_ARC32
ANN_SEARCH = N
ANN_TREE = SCC_ANN_KDTREE

LINKER = $(CC)
CFLAGS = -std=c99 -O2 -pedantic -Wall -Wextra -Wconversion -Wfloat-equal -Werror
XTRA_FLAGS =
XTRA_LIBS = -lcmocka -lm

BUILDDIR = build
OBJDIR = ../build
OBJECTS = digraph_core.o digraph_debug.o digraph_operations.o dist_nnsearch_c.o dist_search.o \
          error.o greedy_bottom_clustering.o greedy_top_clustering.o nng_clustering.o \
          nng_batch_clustering.o nng_core.o nng_findseeds.o scc_data_obj.o scclust.o
OBJECTS := $(addprefix $(OBJDIR)/,$(OBJECTS))
XTRA_OBJECTS =

STDTESTS = test_data_obj test_digraph_core test_digraph_debug test_digraph_operations \
           test_dist_search test_error test_greedy_top_clustering test_nng_clustering \
           test_nng_clustering_batches test_nng_core test_nng_findseeds test_scclust \
           stress_greedy_top_clustering stress_nng_clustering
SPECTESTS = test_digraph_operations_internal test_greedy_top_clustering_internal test_nng_clustering_internal \
            test_nng_core_internal test_nng_core_stable test_nng_findseeds_internal test_nng_findseeds_stable

ALLTESTS = $(addprefix $(BUILDDIR)/,$(addsuffix .out,$(STDTESTS) $(SPECTESTS)))


# Parse options

ifneq ($(DPID), SCC_DPID_UINT32)
XTRA_FLAGS += -D$(DPID)
endif

ifneq ($(ARC), SCC_ARC32)
XTRA_FLAGS += -D$(ARC)
endif

ifeq ($(ANN_SEARCH), Y)
OBJECTS := $(filter-out $(OBJDIR)/dist_nnsearch_c.o,$(OBJECTS)) $(OBJDIR)/dist_nnsearch_ANN.o
LINKER = $(CXX)
XTRA_OBJECTS += $(addprefix ../exlib/libANN/build/,ANN.o brute.o kd_tree.o kd_util.o kd_split.o kd_dump.o \
                                                   kd_search.o kd_pr_search.o kd_fix_rad_search.o bd_tree.o \
                                                   bd_search.o bd_pr_search.o bd_fix_rad_search.o perf.o)
endif

ifneq ($(ANN_TREE), SCC_ANN_KDTREE)
XTRA_FLAGS += -D$(ANN_TREE)
endif


# Main targets

.PHONY: all clean

all: $(ALLTESTS)

clean:
	cd .. && $(MAKE) clean
	$(RM) -R $(BUILDDIR)

$(BUILDDIR)/%.out: $(BUILDDIR)/%.o $(OBJECTS) $(XTRA_OBJECTS)
	$(LINKER) $^ $(XTRA_LIBS) -o $@

$(BUILDDIR)/test_digraph_operations_internal.out: $(BUILDDIR)/test_digraph_operations_internal.o $(filter-out $(OBJDIR)/digraph_operations.o,$(OBJECTS)) $(XTRA_OBJECTS)
	$(LINKER) $^ $(XTRA_LIBS) -o $@

$(BUILDDIR)/test_greedy_top_clustering_internal.out: $(BUILDDIR)/test_greedy_top_clustering_internal.o $(filter-out $(OBJDIR)/greedy_top_clustering.o,$(OBJECTS)) $(XTRA_OBJECTS)
	$(LINKER) $^ $(XTRA_LIBS) -o $@

$(BUILDDIR)/test_nng_clustering_internal.out: $(BUILDDIR)/test_nng_clustering_internal.o $(filter-out $(OBJDIR)/nng_clustering.o,$(OBJECTS)) $(XTRA_OBJECTS)
	$(LINKER) $^ $(XTRA_LIBS) -o $@

$(BUILDDIR)/test_nng_core_internal.out: $(BUILDDIR)/test_nng_core_internal.o $(filter-out $(OBJDIR)/nng_core.o,$(OBJECTS)) $(XTRA_OBJECTS)
	$(LINKER) $^ $(XTRA_LIBS) -o $@

$(BUILDDIR)/test_nng_core_stable.out: $(BUILDDIR)/test_nng_core_stable.o $(filter-out $(OBJDIR)/nng_core.o,$(OBJECTS)) $(XTRA_OBJECTS)
	$(LINKER) $^ $(XTRA_LIBS) -o $@

$(BUILDDIR)/test_nng_findseeds_internal.out: $(BUILDDIR)/test_nng_findseeds_internal.o $(filter-out $(OBJDIR)/nng_findseeds.o,$(OBJECTS)) $(XTRA_OBJECTS)
	$(LINKER) $^ $(XTRA_LIBS) -o $@

$(BUILDDIR)/test_nng_findseeds_stable.out: $(BUILDDIR)/test_nng_findseeds_stable.o $(filter-out $(OBJDIR)/nng_findseeds.o,$(OBJECTS)) $(XTRA_OBJECTS)
	$(LINKER) $^ $(XTRA_LIBS) -o $@


# Dependencies

$(BUILDDIR)/%.o: %.c $(BUILDDIR)
	$(CC) -c $(CFLAGS) $(XTRA_FLAGS) $< -o $@

$(OBJECTS):
	cd .. && $(MAKE) library DEBUG=Y TESTH=Y DPID=$(DPID) ARC=$(ARC) ANN_SEARCH=$(ANN_SEARCH) ANN_TREE=$(ANN_TREE)

../exlib/libANN/build/%.o:
	cd ../exlib/libANN/ && $(MAKE)


# Build folders

$(ALLTESTS): | $(BUILDDIR)
$(BUILDDIR):
	mkdir -p $(BUILDDIR)
