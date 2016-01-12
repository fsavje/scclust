
FLAGS=-O2 -Iinclude

OBJECTS = ANN.o brute.o kd_tree.o kd_util.o kd_split.o \
	kd_dump.o kd_search.o kd_pr_search.o kd_fix_rad_search.o \
	bd_tree.o bd_search.o bd_pr_search.o bd_fix_rad_search.o \
	perf.o

BUILDOBJS=$(addprefix build/,$(OBJECTS))

.PHONY: builddirs library clean

all: library

clean:
	$(RM) -R build/ lib/

builddirs:
	mkdir -p build lib


library: builddirs $(BUILDOBJS)
	$(AR) rcs lib/libANN.a $(BUILDOBJS)

build/%.o: src/%.cpp builddirs
	$(CXX) $(FLAGS) -c $< -o $@
