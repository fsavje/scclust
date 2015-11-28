#ifndef THM_BASEBLOCKS_HG
#define THM_BASEBLOCKS_HG

#include <stddef.h>
#include <stdbool.h>

#include "config.h"
#include "digraph.h"


typedef struct thm_Blocking thm_Blocking;
struct thm_Blocking {
	thm_Vid vertices;
	thm_Blklab blocks;
	bool* assigned;
	bool* seed;
	thm_Blklab* block_label;
	bool valid;
};

typedef enum thm_SeedMethod thm_SeedMethod;
enum thm_SeedMethod { lexical, order_inwards_arcs, order_second_power, simulated_annealing, maximum_is };

thm_Blocking thm_base_blocks(const thm_Digraph* nng, thm_SeedMethod sm);

#endif
