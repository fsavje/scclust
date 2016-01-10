CC=gcc
FLAGS=-std=c99 -O2 -pedantic -Wall -Wextra -Wconversion -Wfloat-equal -Werror
EXLIBS=-lcmocka
OBJECTS=digraph_core.o digraph_operations.o digraph_debug.o findseeds.o nng_clustering.o
STDTESTS=test_digraph_core test_digraph_debug test_digraph_operations test_findseeds test_nng_clustering
SPECTESTS=test_findseeds_internals test_findseeds_stable
ALLTESTS=$(STDTESTS) $(SPECTESTS)
BUILDOBJS=$(addprefix build/,$(OBJECTS))

test_findseeds_internals_DEP=$(filter-out build/findseeds.o,$(BUILDOBJS))
test_findseeds_stable_DEP=$(filter-out build/findseeds.o,$(BUILDOBJS))


all: $(STDTESTS)

$(STDTESTS): clean $(BUILDOBJS)
	$(CC) $(FLAGS) $@.c $(BUILDOBJS) $(EXLIBS) -o build/$@.out

$(SPECTESTS): clean $(BUILDOBJS)
	$(CC) $(FLAGS) $@.c $($@_DEP) $(EXLIBS) -o build/$@.out

build/%.o: ../src/%.c
	$(CC) $(FLAGS) -include test_suite.h -c $< -o $@

clean:
	$(RM) build/*.o build/*.out
