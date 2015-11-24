#ifndef THM_BASEBLOCKS_HG
#define THM_BASEBLOCKS_HG

#include <stddef.h>
#include <stdbool.h>
#include "config.h"

typedef struct thm_Digraph thm_Digraph; // see "digraph.h"

typedef struct thm_Blocking thm_Blocking;
struct thm_Blocking {
	thm_Vid vertices;
	thm_Blklab blocks;
	bool* assigned;
	bool* seed;
	thm_Blklab* block_label;
};

typedef enum thm_SeedMethod thm_SeedMethod;
enum thm_SeedMethod { lexical, order_outwards, order_second_power, simulated_annealing, maximum_is };

thm_Blocking thm_base_blocks(const thm_Digraph* nng, thm_SeedMethod sm);

#endif
