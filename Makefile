CFLAGS=-std=c99 -O2 -pedantic -Wall -Wextra -Wconversion -Wfloat-equal -Werror

OBJECTS=clustering.o digraph_core.o digraph_debug.o digraph_operations.o findseeds.o nng_clustering.o
BUILDDIR=build

BUILDOBJS=$(addprefix $(BUILDDIR)/,$(OBJECTS))


.PHONY: all clean doc library

all: doc library

clean:
	$(RM) -R $(BUILDDIR)/ doc/ lib/

$(BUILDOBJS): | $(BUILDDIR)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)


library: $(BUILDOBJS)
	mkdir -p lib
	$(AR) rcs lib/libscclust.a $^

doc:
	mkdir -p doc
	doxygen DoxyAPI


$(BUILDDIR)/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@
