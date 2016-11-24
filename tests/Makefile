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

SCC_OBJECTS = \
	data_set.o \
	digraph_core.o \
	digraph_debug.o \
	digraph_operations.o \
	dist_nnsearch_c.o \
	dist_search.o \
	error.o \
	hierarchical_clustering.o \
	nng_clustering.o \
	nng_batch_clustering.o \
	nng_core.o \
	nng_findseeds.o \
	scclust.o

SCC_DIR = scc_build
SCC_OBJECTS := $(addprefix $(SCC_DIR)/src/,$(SCC_OBJECTS))

STDTESTS = \
	stress_hierarchical_clustering.out \
	stress_nng_clustering.out \
	test_data_set.out \
	test_digraph_core.out \
	test_digraph_debug.out \
	test_digraph_operations.out \
	test_dist_search.out \
	test_error.out \
	test_hierarchical_clustering.out \
	test_nng_clustering_batches.out \
	test_nng_clustering.out \
	test_nng_core.out \
	test_nng_findseeds.out \
	test_scclust.out

SPECTESTS = \
	test_digraph_operations_internal.out \
	test_hierarchical_clustering_internal.out \
	test_nng_clustering_internal.out \
	test_nng_core_internal.out \
	test_nng_core_stable.out \
	test_nng_findseeds_internal.out \
	test_nng_findseeds_stable.out

BUILD_DIR = build
ALLTESTS = $(addprefix $(BUILD_DIR)/,$(STDTESTS) $(SPECTESTS))
$(ALLTESTS): | $(BUILD_DIR)

CFLAGS = -std=c99 -O2 -pedantic -Wall -Wextra -Wconversion -Wfloat-equal -Werror
XTRA_FLAGS = -I$(SCC_DIR)
XTRA_LIBS = -lcmocka -lm
LINKER = $(CC)
CONFIG_FLAGS = \
	--enable-assert \
	--enable-digraph-debug \
	--enable-cmocka-headers \
	--disable-documentation


.PHONY: all

all: $(ALLTESTS)

clean:
	$(RM) -R $(BUILD_DIR)

$(BUILD_DIR)/%.out: $(BUILD_DIR)/%.o $(SCC_OBJECTS)
	$(LINKER) $^ $(XTRA_LIBS) -o $@

$(BUILD_DIR)/test_digraph_operations_internal.out: $(BUILD_DIR)/test_digraph_operations_internal.o $(filter-out $(SCC_DIR)/src/digraph_operations.o,$(SCC_OBJECTS))
	$(LINKER) $^ $(XTRA_LIBS) -o $@

$(BUILD_DIR)/test_hierarchical_clustering_internal.out: $(BUILD_DIR)/test_hierarchical_clustering_internal.o $(filter-out $(SCC_DIR)/src/hierarchical_clustering.o,$(SCC_OBJECTS))
	$(LINKER) $^ $(XTRA_LIBS) -o $@

$(BUILD_DIR)/test_nng_clustering_internal.out: $(BUILD_DIR)/test_nng_clustering_internal.o $(filter-out $(SCC_DIR)/src/nng_clustering.o,$(SCC_OBJECTS))
	$(LINKER) $^ $(XTRA_LIBS) -o $@

$(BUILD_DIR)/test_nng_core_internal.out: $(BUILD_DIR)/test_nng_core_internal.o $(filter-out $(SCC_DIR)/src/nng_core.o,$(SCC_OBJECTS))
	$(LINKER) $^ $(XTRA_LIBS) -o $@

$(BUILD_DIR)/test_nng_core_stable.out: $(BUILD_DIR)/test_nng_core_stable.o $(filter-out $(SCC_DIR)/src/nng_core.o,$(SCC_OBJECTS))
	$(LINKER) $^ $(XTRA_LIBS) -o $@

$(BUILD_DIR)/test_nng_findseeds_internal.out: $(BUILD_DIR)/test_nng_findseeds_internal.o $(filter-out $(SCC_DIR)/src/nng_findseeds.o,$(SCC_OBJECTS))
	$(LINKER) $^ $(XTRA_LIBS) -o $@

$(BUILD_DIR)/test_nng_findseeds_stable.out: $(BUILD_DIR)/test_nng_findseeds_stable.o $(filter-out $(SCC_DIR)/src/nng_findseeds.o,$(SCC_OBJECTS))
	$(LINKER) $^ $(XTRA_LIBS) -o $@

$(BUILD_DIR)/%.o: %.c $(BUILD_DIR) $(SCC_DIR)/include/scclust.h
	$(CC) -c $(CFLAGS) $(XTRA_FLAGS) $< -o $@

$(SCC_DIR)/include/scclust.h:
	mkdir -p $(SCC_DIR)
	cd $(SCC_DIR) && ../../configure $(CONFIG_FLAGS)

$(SCC_DIR)/src/%.o: $(SCC_DIR)/include/scclust.h
	cd $(SCC_DIR) && $(MAKE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
