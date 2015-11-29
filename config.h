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

#ifndef TBG_CONFIG_HG
#define TBG_CONFIG_HG

#include <stdint.h>


// Vertex ID type (must be unsigned)
// Number of vertices < MAX(tbg_Vid)
typedef uint32_t tbg_Vid;

// MAX(tbg_Vid): maximum value of tbg_Vid
#define TBG_VID_MAX UINT32_MAX

// Arc reference type (must be unsigned)
// Number of arcs in any digraph <= MAX(tbg_Arcref)
// Should be MAX(tbg_Arcref) >= MAX(tbg_Vid)
typedef uint32_t tbg_Arcref;

// Uncomment this define for header only library
//#define TBG_HEADER_ONLY_LIB


/*==============================================================================
Non-user parts below this line
==============================================================================*/

// Force library build
#ifdef TBG_FORCE_LIB_BUILD
  #undef TBG_HEADER_ONLY_LIB
#endif

// Add static linkage if making header only library
#ifdef TBG_HEADER_ONLY_LIB
	#define TBG_LINKAGE static
#else 
	#define TBG_LINKAGE 
#endif

#endif
