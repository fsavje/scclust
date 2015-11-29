CC=gcc
FLAGS=-std=c99 -O2 -Wall -Wextra -Wconversion -Wfloat-equal -DTBG_FORCE_LIB_BUILD
OBJECTS=core.o debug.o operations.o

all: $(OBJECTS)

%.o: %.c
	$(CC) -c $(FLAGS) $< -o $@

clean:
	$(RM) *.o 
