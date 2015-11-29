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

#ifndef TBG_STRUCT_HG
#define TBG_STRUCT_HG

#include "config.h"


// Main digraph struct
typedef struct tbg_Digraph tbg_Digraph;
struct tbg_Digraph {
	// Number of vertices in graph.
	tbg_Vid vertices;

	// Maximum number of arcs in graph (i.e., length of `head').
	tbg_Arcref max_arcs;

	// Array of tbg_Vid: [0, ..., max_arcs - 1].
	// Contains the head of each arc, indicated by vertex ID.
	// May be NULL if max_arcs == 0.
	tbg_Vid* head;

	// Array of tbg_Arcref: [0, ..., vertices].
	// For arcs for which vertex i is the tail, head[tail_ptr[i]] is the head of the first arc and head[tail_ptr[i+1] - 1] is the last arc.
	// May never be NULL.
	tbg_Arcref* tail_ptr;
};

#endif
