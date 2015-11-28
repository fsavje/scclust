#ifndef THM_DIGRAPH_DEBUG_HG
#define THM_DIGRAPH_DEBUG_HG

#include <stdbool.h>

#include "config.h"
#include "digraph.h"

bool thm_valid_digraph(const thm_Digraph* test_dg, thm_Vid vertices);
bool thm_sound_digraph(const thm_Digraph* test_dg, thm_Vid vertices);
bool thm_digraphs_equal(const thm_Digraph* dg_a, const thm_Digraph* dg_b);
thm_Digraph thm_digraph_from_pieces(thm_Vid vertices, thm_Arcref max_arcs, const thm_Arcref tail_ptr[vertices], const thm_Vid head[max_arcs]);
thm_Digraph thm_digraph_from_string(const char dg_str[]);
void thm_print_digraph(const thm_Digraph* dg);

#endif
