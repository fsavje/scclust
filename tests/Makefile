CC=gcc
FLAGS=-std=c99 -O2 -pedantic -Wall -Wextra -Wconversion -Wfloat-equal -Werror
EXLIBS=-lcmocka
OBJECTS=digraph_core.o digraph_debug.o digraph_operations.o
STDTESTS=test_digraph_core test_digraph_debug test_digraph_operations
SPECTESTS=test_special
ALLTESTS=$(STDTESTS) $(SPECTESTS)

test_special_DEP=$(filter-out build/digraph_operations.o,$(BUILDOBJSBUILDOBJS))

BUILDOBJS=$(addprefix build/,$(OBJECTS))

all: $(STDTESTS)

$(STDTESTS): clean $(BUILDOBJS)
	$(CC) $(FLAGS) $@.c $(BUILDOBJS) $(EXLIBS) -o build/$@.out

$(SPECTESTS): clean $(BUILDOBJS)
	$(CC) $(FLAGS) $@.c $($@_DEP) $(EXLIBS) -o build/$@.out

build/%.o: ../src/%.c
	$(CC) -c $(FLAGS) -include test_suite.h $< -o $@

clean:
	$(RM) build/*.o build/*.out
