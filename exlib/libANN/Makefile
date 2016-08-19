CXXFLAGS = -O2
BUILDDIR = build
OBJECTS = ANN.o brute.o kd_tree.o kd_util.o kd_split.o kd_dump.o kd_search.o kd_pr_search.o \
          kd_fix_rad_search.o bd_tree.o bd_search.o bd_pr_search.o bd_fix_rad_search.o perf.o
OBJECTS := $(addprefix $(BUILDDIR)/,$(OBJECTS))


# Main targets

.PHONY: all clean

all: $(OBJECTS)

clean:
	$(RM) -R $(BUILDDIR)/

# Build objects

$(BUILDDIR)/%.o: src/%.cpp
	$(CXX) -c $(CXXFLAGS) -Iinclude $< -o $@

# Build folders

$(OBJECTS): | $(BUILDDIR)
$(BUILDDIR):
	mkdir -p $(BUILDDIR)
