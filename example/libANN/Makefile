
FLAGS=-O2 -Iinclude

OBJECTS = ANN.o brute.o kd_tree.o kd_util.o kd_split.o \
	kd_dump.o kd_search.o kd_pr_search.o kd_fix_rad_search.o \
	bd_tree.o bd_search.o bd_pr_search.o bd_fix_rad_search.o \
	perf.o

BUILDOBJS=$(addprefix build/,$(OBJECTS))

all: library

library: $(BUILDOBJS)
	$(AR) rcs lib/libANN.a $(BUILDOBJS)

build/%.o: src/%.cpp
	$(CXX) $(FLAGS) -c $< -o $@

clean:
	$(RM) build/*.o lib/*.a lib/*.h
