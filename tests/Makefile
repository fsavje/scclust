CC=gcc
FLAGS=-std=c99 -O2 -Wall -Wextra -Wconversion -Wfloat-equal
INCLUDES=-I/usr/local/include
EXLIBS=/usr/local/lib/libcmocka.dylib
TESTS=test_core test_debug test_operations

all: $(TESTS)

$(TESTS):
	$(CC) $(FLAGS) $(INCLUDES) $@.c $(EXLIBS) -o $@.out

clean:
	$(RM) *.o *.out
