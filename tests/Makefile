CC=gcc
FLAGS=-std=c99 -O2 -Wall -Wextra -Wconversion -Wfloat-equal -include test_suite.h
INCLUDES=-I/usr/local/include
EXLIBS=/usr/local/lib/libcmocka.dylib
LIBOBJECTS=build/digraph_core.o build/digraph_debug.o build/digraph_operations.o
STDTESTS=test_digraph_core test_digraph_debug test_digraph_operations
SPECTESTS=test_special
ALLTESTS=$(STDTESTS) $(SPECTESTS)

test_special_DEP=$(filter-out build/digraph_operations.o,$(LIBOBJECTS))

all: $(ALLTESTS)

$(STDTESTS): clean $(LIBOBJECTS)
	$(CC) $(FLAGS) $(INCLUDES) $@.c $(LIBOBJECTS) $(EXLIBS) -o build/$@.out

$(SPECTESTS): clean $(LIBOBJECTS)
	$(CC) $(FLAGS) $(INCLUDES) $@.c $($@_DEP) $(EXLIBS) -o build/$@.out

build/%.o: ../src/%.c
	$(CC) -c $(FLAGS) $(INCLUDES) $< -o $@

clean:
	$(RM) build/*.o build/*.out
