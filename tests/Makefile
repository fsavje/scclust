CFLAGS=-std=c99 -O2 -pedantic -Wall -Wextra -Wconversion -Wfloat-equal -Werror
EXLIBS=-lcmocka

OBJECTS=clustering.o digraph_core.o digraph_debug.o digraph_operations.o dist_search.o greedy_clustering.o \
        nng_core.o nng_findseeds.o
DEBUGDIR=../dbg
BUILDDIR=build

STDTESTS=test_clustering test_digraph_core test_digraph_debug test_digraph_operations test_dist_search \
         test_findseeds test_greedy test_nng_clust_caliper test_nng_clust_loops test_nng_clustering
SPECTESTS=test_findseeds_internals test_findseeds_stable test_greedy_internals

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

test_findseeds_internals: $(filter-out $(DEBUGDIR)/nng_findseeds.o,$(DEBUGOBJS))
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $@.c $^ $(EXLIBS) -o $(BUILDDIR)/$@.out

test_findseeds_stable: $(filter-out $(DEBUGDIR)/nng_findseeds.o,$(DEBUGOBJS))
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $@.c $^ $(EXLIBS) -o $(BUILDDIR)/$@.out

test_greedy_internals: $(filter-out $(DEBUGDIR)/greedy_clustering.o,$(DEBUGOBJS))
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $@.c $^ $(EXLIBS) -o $(BUILDDIR)/$@.out

$(DEBUGOBJS):
	cd .. && $(MAKE) debug
