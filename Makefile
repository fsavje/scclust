CC=gcc
FLAGS=-std=c99 -O2 -Wall -Wextra -Wconversion -Wfloat-equal
OBJECTS=build/core.o build/debug.o build/operations.o

all: $(OBJECTS)

build/%.o: src/%.c
	$(CC) -c $(FLAGS) $< -o $@

clean:
	$(RM) *.o 
