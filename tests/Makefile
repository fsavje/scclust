CC=gcc
FLAGS=-std=c99 -O2 -Wall -Wfloat-equal -include test_suite.h
INCLUDES=-I/usr/local/include
LIBS=/usr/local/lib/libcmocka.dylib
TESTS=test_digraph test_digraph_debug
MAINOBJECTS=digraph.o digraph_debug.o

test_digraph_DEP=$(filter-out digraph.o,$(MAINOBJECTS))
test_digraph_debug_DEP=$(filter-out digraph_debug.o,$(MAINOBJECTS))

all: $(TESTS)

$(TESTS): clean $(MAINOBJECTS)
	$(CC) $(FLAGS) $(INCLUDES) $@.c $($@_DEP) $(LIBS) -o $@.out

%.o: ../%.c
	$(CC) -c $(FLAGS) $(INCLUDES) $< -o $@

clean:
	$(RM) *.o *.out
