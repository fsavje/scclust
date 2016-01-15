CXXFLAGS=-O2 -Iinclude

OBJECTS = ANN.o brute.o kd_tree.o kd_util.o kd_split.o \
	kd_dump.o kd_search.o kd_pr_search.o kd_fix_rad_search.o \
	bd_tree.o bd_search.o bd_pr_search.o bd_fix_rad_search.o \
	perf.o
BUILDDIR=build

BUILDOBJS=$(addprefix $(BUILDDIR)/,$(OBJECTS))


.PHONY: all clean library

all: library

clean:
	$(RM) -R $(BUILDDIR)/ lib/

$(BUILDOBJS): | $(BUILDDIR)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)


library: $(BUILDOBJS)
	mkdir -p lib
	$(AR) rcs lib/libANN.a $^


$(BUILDDIR)/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
