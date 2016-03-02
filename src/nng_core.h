/* ==============================================================================
 * scclust -- A C library for size constrained clustering
 * https://github.com/fsavje/scclust
 *
 * Copyright (C) 2015-2016  Fredrik Savje -- http://fredriksavje.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * ============================================================================== */

#ifndef SCC_NNG_CORE_HG
#define SCC_NNG_CORE_HG

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "../include/scclust.h"
#include "clustering.h"
#include "digraph_core.h"
#include "nng_findseeds.h"


// ==============================================================================
// Function prototypes
// ==============================================================================

scc_ErrorCode iscc_get_nng_with_size_constraint(scc_DataSetObject* data_set_object,
                                                size_t num_data_points,
                                                uint32_t size_constraint,
                                                const bool main_data_points[],
                                                bool radius_constraint,
                                                double radius,
                                                iscc_Digraph* out_nng);

scc_ErrorCode iscc_get_nng_with_type_constraint(scc_DataSetObject* data_set_object,
                                                size_t num_data_points,
                                                uint32_t size_constraint,
                                                uint_fast16_t num_types,
                                                const uint32_t type_size_constraints[static num_types],
                                                const scc_TypeLabel type_labels[static num_data_points],
                                                const bool main_data_points[],
                                                bool radius_constraint,
                                                double radius,
                                                iscc_Digraph* out_nng);

scc_ErrorCode iscc_assign_seed_neighbors(scc_Clustering* clustering,
                                         const iscc_Digraph* nng,
                                         const iscc_SeedResult* sr,
                                         bool out_unassigned[restrict static clustering->num_data_points]);

scc_ErrorCode iscc_remaining_ignore(scc_Clustering* clustering,
                                    const bool unassigned[restrict static clustering->num_data_points]);

scc_ErrorCode iscc_remaining_by_nng(scc_Clustering* clustering,
                                    iscc_Digraph* nng,
                                    const bool unassigned[restrict static clustering->num_data_points]);

scc_ErrorCode iscc_remaining_to_nearest_assigned(scc_Clustering* clustering,
                                                 scc_DataSetObject* data_set_object,
                                                 iscc_Digraph* nng,
                                                 size_t num_assigned,
                                                 bool unassigned[restrict static clustering->num_data_points],
                                                 const bool main_data_points[restrict],
                                                 bool assign_secondary_points,
                                                 bool main_radius_constraint,
                                                 double main_radius,
                                                 bool secondary_radius_constraint,
                                                 double secondary_radius);

scc_ErrorCode iscc_remaining_to_nearest_seed(scc_Clustering* clustering,
                                             scc_DataSetObject* data_set_object,
                                             const iscc_SeedResult* sr,
                                             size_t num_assigned,
                                             bool unassigned[restrict static clustering->num_data_points],
                                             const bool main_data_points[restrict],
                                             bool assign_secondary_points,
                                             bool main_radius_constraint,
                                             double main_radius,
                                             bool secondary_radius_constraint,
                                             double secondary_radius);


#endif // ifndef SCC_NNG_CORE_HG
