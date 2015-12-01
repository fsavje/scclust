/* Copyright 2015 Fredrik Savje

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
==============================================================================*/

#ifndef SCC_DEBUG_HG
#define SCC_DEBUG_HG

#include <stdbool.h>

#include "config.h"
#include "digraph.h"


bool scc_is_valid_digraph(const scc_Digraph* dg);
bool scc_is_sound_digraph(const scc_Digraph* dg);
bool scc_is_empty_digraph(const scc_Digraph* dg);
bool scc_is_balanced_digraph(const scc_Digraph* dg, scc_Vid arcs_per_vertex);
bool scc_digraphs_equal(const scc_Digraph* dg_a, const scc_Digraph* dg_b);
scc_Digraph scc_digraph_from_pieces(scc_Vid vertices, scc_Arcref max_arcs, const scc_Arcref tail_ptr[vertices], const scc_Vid head[max_arcs]);
scc_Digraph scc_digraph_from_string(const char dg_str[]);
void scc_print_digraph(const scc_Digraph* dg);


#endif
