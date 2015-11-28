CC=gcc
FLAGS=-std=c99 -O2 -Wall -Wextra -Wno-missing-field-initializers -Wfloat-equal
INCLUDES=
LIBS=
OBJECTS=digraph.o digraph_debug.o

all: $(OBJECTS)

%.o: %.c
	$(CC) -c $(FLAGS) $(INCLUDES) $< -o $@

clean:
	$(RM) *.o *.out
