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

#ifndef TBG_OPERATIONS_HG
#define TBG_OPERATIONS_HG

#include <stdbool.h>
#include <stddef.h>

#include "config.h"
#include "digraph.h"


tbg_Digraph tbg_digraph_union(size_t num_dgs, const tbg_Digraph* const dgs[static num_dgs]);
tbg_Digraph tbg_digraph_transpose(const tbg_Digraph* dg);
tbg_Digraph tbg_adjacency_product(const tbg_Digraph* dg_a, const tbg_Digraph* dg_b, bool force_diagonal, bool ignore_diagonal);


#endif
