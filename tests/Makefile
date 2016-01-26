CFLAGS=-std=c99 -O2 -pedantic -Wall -Wextra -Wconversion -Wfloat-equal -Werror
CXXFLAGS=-std=c++11 -O2 -pedantic -Wall -Wextra -Wconversion -Wfloat-equal -Werror
EXLIBS=-lcmocka

OBJECTS=clustering.o digraph_core.o digraph_debug.o digraph_operations.o findseeds.o greedy.o nn_search.o nng_clustering.o
BUILDDIR=build

STDTESTS=test_digraph_core test_digraph_debug test_digraph_operations test_findseeds test_nng_clustering \
         test_nng_clust_caliper test_nng_clust_loops test_clustering
SPECTESTS=test_ann_wrapper test_findseeds_internals test_findseeds_stable test_greedy_internals

ALLTESTS=$(STDTESTS) $(SPECTESTS)
BUILDOBJS=$(addprefix $(BUILDDIR)/,$(OBJECTS))


.PHONY: all clean $(ALLTESTS)

all: $(ALLTESTS)

clean:
	$(RM) -R $(BUILDDIR)

$(ALLTESTS) $(BUILDOBJS) $(BUILDDIR)/nng.o: | $(BUILDDIR)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)


$(STDTESTS): $(BUILDOBJS)
	$(CC) $(CFLAGS) $@.c $^ $(EXLIBS) -o $(BUILDDIR)/$@.out

test_findseeds_internals: $(filter-out $(BUILDDIR)/findseeds.o,$(BUILDOBJS))
	$(CC) $(CFLAGS) $@.c $^ $(EXLIBS) -o $(BUILDDIR)/$@.out

test_findseeds_stable: $(filter-out $(BUILDDIR)/findseeds.o,$(BUILDOBJS))
	$(CC) $(CFLAGS) $@.c $^ $(EXLIBS) -o $(BUILDDIR)/$@.out

test_greedy_internals: $(filter-out $(BUILDDIR)/greedy.o,$(BUILDOBJS))
	$(CC) $(CFLAGS) $@.c $^ $(EXLIBS) -o $(BUILDDIR)/$@.out

test_ann_wrapper: $(BUILDOBJS) $(BUILDDIR)/nng.o ../example/libANN/lib/libANN.a
	$(CC) $(CFLAGS) $@.c $^ $(EXLIBS) -lstdc++ -o $(BUILDDIR)/$@.out


$(BUILDDIR)/%.o: ../src/%.c
	$(CC) $(CFLAGS) -include test_suite.h -c $< -o $@

$(BUILDDIR)/nng.o: ../example/nng.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

../example/libANN/lib/libANN.a:
	cd ../example/libANN && $(MAKE) library
