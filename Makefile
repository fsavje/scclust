CC=gcc
AR=ar
FLAGS=-std=c99 -O2 -Wall -Wextra -Wconversion -Wfloat-equal
OBJECTS=digraph_core.o digraph_operations.o digraph_debug.o

BUILDOBJS=$(addprefix build/,$(OBJECTS))

all: library

library: $(BUILDOBJS)
	$(AR) rcs lib/libscclust.a $(BUILDOBJS)

build/%.o: src/%.c
	$(CC) -c $(FLAGS) $< -o $@

clean:
	$(RM) build/*.o lib/*.a 
