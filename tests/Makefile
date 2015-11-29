CC=gcc
FLAGS=-std=c99 -O2 -Wall -Wextra -Wconversion -Wno-missing-field-initializers -Wfloat-equal -include test_suite.h
INCLUDES=-I/usr/local/include
EXLIBS=/usr/local/lib/libcmocka.dylib
LIBOBJECTS=core.o debug.o operations.o
STDTESTS=test_core test_debug test_operations
SPECTESTS=test_special
ALLTESTS=$(STDTESTS) $(SPECTESTS)

test_special_DEP=$(filter-out operations.o,$(LIBOBJECTS))

all: $(ALLTESTS)

$(STDTESTS): clean $(LIBOBJECTS)
	$(CC) $(FLAGS) $(INCLUDES) $@.c $(LIBOBJECTS) $(EXLIBS) -o $@.out

$(SPECTESTS): clean $(LIBOBJECTS)
	$(CC) $(FLAGS) $(INCLUDES) $@.c $($@_DEP) $(EXLIBS) -o $@.out

%.o: ../%.c
	$(CC) -c $(FLAGS) $(INCLUDES) $< -o $@

clean:
	$(RM) *.o *.out
