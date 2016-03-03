CFLAGS=-std=c99 -O2 -pedantic -Wall -Wextra -Wconversion -Wfloat-equal -Werror

OBJECTS=data_obj.o digraph_core.o digraph_operations.o dist_search.o error.o greedy_clustering.o nng_clustering.o nng_core.o nng_findseeds.o scclust.o
BUILDDIR=build
DEBUGDIR=dbg

BUILDOBJS=$(addprefix $(BUILDDIR)/,$(OBJECTS))
DEBUGOBJS=$(addprefix $(DEBUGDIR)/,$(OBJECTS) digraph_debug.o)

.PHONY: all clean doc library debug clean-debug

all: doc library

clean:
	$(RM) -R $(BUILDDIR)/ $(DEBUGDIR)/ doc/ lib/

clean-debug:
	$(RM) -R $(DEBUGDIR)/

$(BUILDOBJS): | $(BUILDDIR)
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(DEBUGOBJS): | $(DEBUGDIR)
$(DEBUGDIR):
	mkdir -p $(DEBUGDIR)

library: $(BUILDOBJS)
	mkdir -p lib
	$(AR) rcs lib/libscc.a $^

debug: $(DEBUGOBJS)

doc:
	mkdir -p doc
	doxygen DoxyAPI

$(BUILDDIR)/%.o: src/%.c
	$(CC) $(CFLAGS) -c -DNDEBUG $< -o $@

$(DEBUGDIR)/%.o: src/%.c
	$(CC) $(CFLAGS) -include tests/test_suite.h -c $< -o $@
