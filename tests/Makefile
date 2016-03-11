CFLAGS=-std=c99 -O2 -pedantic -Wall -Wextra -Wconversion -Wfloat-equal -Werror
EXLIBS=-lcmocka

OBJECTS=digraph_core.o digraph_debug.o digraph_operations.o dist_search.o error.o greedy_clustering.o \
        nng_clustering.o nng_core.o nng_findseeds.o scclust.o scc_data_obj.o 
DEBUGDIR=../dbg
BUILDDIR=build

STDTESTS=test_data_obj test_digraph_core test_digraph_debug test_digraph_operations \
         test_dist_search test_error test_greedy_clustering test_greedy_clustering_stress \
         test_nng_clustering test_nng_core test_nng_findseeds test_scclust
SPECTESTS=test_digraph_operations_internal test_greedy_clustering_internal test_nng_core_internal \
          test_nng_findseeds_internal test_nng_findseeds_stable

ALLTESTS=$(STDTESTS) $(SPECTESTS)
DEBUGOBJS=$(addprefix $(DEBUGDIR)/,$(OBJECTS))


.PHONY: all clean $(ALLTESTS)

all: $(ALLTESTS)

clean:
	cd .. && $(MAKE) clean-debug
	$(RM) -R $(BUILDDIR)

$(STDTESTS): $(DEBUGOBJS)
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $@.c $^ $(EXLIBS) -o $(BUILDDIR)/$@.out


test_digraph_operations_internal: $(filter-out $(DEBUGDIR)/digraph_operations.o,$(DEBUGOBJS))
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $@.c $^ $(EXLIBS) -o $(BUILDDIR)/$@.out

test_greedy_clustering_internal: $(filter-out $(DEBUGDIR)/greedy_clustering.o,$(DEBUGOBJS))
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $@.c $^ $(EXLIBS) -o $(BUILDDIR)/$@.out

test_nng_core_internal: $(filter-out $(DEBUGDIR)/nng_core.o,$(DEBUGOBJS))
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $@.c $^ $(EXLIBS) -o $(BUILDDIR)/$@.out

test_nng_findseeds_internal: $(filter-out $(DEBUGDIR)/nng_findseeds.o,$(DEBUGOBJS))
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $@.c $^ $(EXLIBS) -o $(BUILDDIR)/$@.out

test_nng_findseeds_stable: $(filter-out $(DEBUGDIR)/nng_findseeds.o,$(DEBUGOBJS))
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $@.c $^ $(EXLIBS) -o $(BUILDDIR)/$@.out


$(DEBUGOBJS):
	cd .. && $(MAKE) debug
