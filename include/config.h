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

#ifndef SCC_CONFIG_HG
#define SCC_CONFIG_HG

#include <stdint.h>


// Vertex ID type (must be unsigned)
// Number of vertices < MAX(scc_Vid)
typedef uint32_t scc_Vid;

// MAX(scc_Vid): maximum value of scc_Vid
#define SCC_VID_MAX UINT32_MAX

// Arc reference type (must be unsigned)
// Number of arcs in any digraph <= MAX(scc_Arcref)
// Should be MAX(scc_Arcref) >= MAX(scc_Vid)
typedef uint32_t scc_Arcref;


#endif
