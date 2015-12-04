CC=gcc
AR=ar
FLAGS=-std=c99 -O2 -pedantic -Wall -Wextra -Wconversion -Wfloat-equal -Werror
OBJECTS=digraph_core.o digraph_operations.o digraph_debug.o findseeds.o nng_clustering.o
BUILDOBJS=$(addprefix build/,$(OBJECTS))

all: library

library: $(BUILDOBJS)
	$(AR) rcs lib/libscclust.a $(BUILDOBJS)

build/%.o: src/%.c
	$(CC) $(FLAGS) -c $< -o $@

clean:
	$(RM) build/*.o lib/*.a 
