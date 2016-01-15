CFLAGS=-std=c99 -O2 -pedantic -Wall -Wextra -Wconversion -Wfloat-equal -Werror
CXXFLAGS=-std=c++11 -O2 -pedantic -Wall -Wextra -Wconversion -Wfloat-equal -Werror


.PHONY: all clean examples

all: examples

clean:
	$(RM) *.o *.out


examples: example.out

example.out: nng.o ../lib/libscclust.a libANN/lib/libANN.a
	$(CC) $(CFLAGS) example1.c $^ -lstdc++ -o $@


nng.o: nng.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

../lib/libscclust.a:
	cd .. && $(MAKE) library

libANN/lib/libANN.a:
	cd libANN && $(MAKE) library
