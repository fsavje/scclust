#include "baseblocks.h"
#include <stdlib.h>
#include "digraph.h"

static void ithm_fs_lexical(const thm_Digraph* nng, thm_Blocking* blocking);

thm_Blocking thm_base_blocks(const thm_Digraph* const nng, const thm_SeedMethod sm) {

	thm_Blocking blocking = {
		.vertices = nng->num_vertices,
		.blocks = 0,
		.assigned = calloc(nng->num_vertices, sizeof(bool)),
		.seed = calloc(nng->num_vertices, sizeof(bool)),
		.block_label = malloc(sizeof(thm_Blklab[nng->num_vertices]))
	};

	ithm_fs_lexical(nng, &blocking);

	return blocking;
}

static inline void ithm_set_seed(const thm_Vid s, const thm_Vid* a, const thm_Vid* const a_stop, thm_Blocking* const blocking) {
	blocking->seed[s] = true;
	blocking->assigned[s] = true;
	blocking->block_label[s] = blocking->blocks;
	for (; a != a_stop; ++a) {
		blocking->assigned[*a] = true;
		blocking->block_label[*a] = blocking->blocks;
	}
	++(blocking->blocks);
}

static void ithm_fs_lexical(const thm_Digraph* const nng, thm_Blocking* const blocking) {
	for (size_t c = 0; c < nng->num_vertices; ++c) {
		if (!blocking->assigned[c]) {
			const thm_Vid* a = nng->head + nng->tail_ptr[c];
			const thm_Vid* const a_stop = nng->head + nng->tail_ptr[c + 1];
			for (; a != a_stop && !blocking->assigned[*a]; ++a) { }
			if (a == a_stop) ithm_set_seed(c, (nng->head + nng->tail_ptr[c]), a_stop, blocking);
		}
	}
}
