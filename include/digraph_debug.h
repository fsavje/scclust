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

#ifndef TBG_DEBUG_HG
#define TBG_DEBUG_HG

#include <stdbool.h>

#include "config.h"
#include "digraph.h"


bool tbg_is_valid_digraph(const tbg_Digraph* dg);
bool tbg_is_sound_digraph(const tbg_Digraph* dg);
bool tbg_is_empty_digraph(const tbg_Digraph* dg);
bool tbg_is_balanced_digraph(const tbg_Digraph* dg, tbg_Vid arcs_per_vertex);
bool tbg_digraphs_equal(const tbg_Digraph* dg_a, const tbg_Digraph* dg_b);
tbg_Digraph tbg_digraph_from_pieces(tbg_Vid vertices, tbg_Arcref max_arcs, const tbg_Arcref tail_ptr[vertices], const tbg_Vid head[max_arcs]);
tbg_Digraph tbg_digraph_from_string(const char dg_str[]);
void tbg_print_digraph(const tbg_Digraph* dg);


#endif
