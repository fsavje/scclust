FLAGS=-std=c99 -O2 -pedantic -Wall -Wextra -Wconversion -Wfloat-equal -Werror
OBJECTS=digraph_core.o digraph_operations.o digraph_debug.o findseeds.o nng_clustering.o
BUILDOBJS=$(addprefix build/,$(OBJECTS))

.PHONY: builddirs library clean

all: library

clean:
	$(RM) -R build/ doc/ lib/

builddirs:
	mkdir -p build doc lib


library: $(BUILDOBJS) builddirs
	$(AR) rcs lib/libscclust.a $(BUILDOBJS)

build/%.o: src/%.c builddirs
	$(CC) $(FLAGS) -c $< -o $@


doc: builddirs
	doxygen DoxyAPI

doc-source: builddirs
	doxygen DoxySource
