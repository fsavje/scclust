CFLAGS=-std=c99 -O2 -pedantic -Wall -Wextra -Wconversion -Wfloat-equal -Werror
CXXFLAGS=-std=c++11 -O2 -pedantic -Wall -Wextra -Wconversion -Wfloat-equal -Werror

.PHONY: builddirs example clean

all: example test_nng

clean:
	$(RM) -R build/

builddirs:
	mkdir -p build


example: builddirs build/nng.o ../lib/scclust.a libANN/lib/libANN.a


build/nng.o: nng.cpp builddirs
	$(CXX) $(CXXFLAGS) -c $< -o $@

../lib/scclust.a:
	cd .. && $(MAKE) library

libANN/lib/libANN.a:
	cd libANN && $(MAKE) library


test_nng: clean builddirs build/nng.o build/t_digraph_core.o build/t_digraph_debug.o libANN/lib/libANN.a
	$(CC) $(CFLAGS) test_nng.c build/nng.o build/t_digraph_core.o build/t_digraph_debug.o libANN/lib/libANN.a -lcmocka -lstdc++ -o build/$@.out

build/t_digraph_core.o: ../src/digraph_core.c builddirs
	$(CC) $(CFLAGS) -include ../tests/test_suite.h -c $< -o $@

build/t_digraph_debug.o: ../src/digraph_debug.c builddirs
	$(CC) $(CFLAGS) -include ../tests/test_suite.h -c $< -o $@
