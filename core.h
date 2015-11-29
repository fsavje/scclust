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

#ifndef TBG_CORE_HG
#define TBG_CORE_HG

#include <stdbool.h>
#include "def.h"


tbg_Digraph tbg_init_digraph(tbg_Vid vertices, tbg_Arcref max_arcs);
void tbg_free_digraph(tbg_Digraph* dg);
bool tbg_change_arc_storage(tbg_Digraph* dg, tbg_Arcref new_max_arcs);
tbg_Digraph tbg_empty_digraph(tbg_Vid vertices, tbg_Arcref max_arcs);
tbg_Digraph tbg_identity_digraph(tbg_Vid vertices);
tbg_Digraph tbg_balanced_digraph(tbg_Vid vertices, tbg_Vid arcs_per_vertex, tbg_Vid* heads);
tbg_Digraph tbg_copy_digraph(const tbg_Digraph* dg);

#endif
