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


#include "../include/scclust.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "clustering.h"
#include "digraph_core.h"
#include "dist_search.h"
#include "error.h"
#include "nng_core.h"
#include "nng_findseeds.h"

// ==============================================================================
// Internal structs and variables
// ==============================================================================

static const scc_SeedMethod SCC_SM_MAX = SCC_SM_EXCLUSION_UPDATING;
static const scc_AssignMethod SCC_AM_MAX = SCC_AM_CLOSEST_SEED;


// ==============================================================================
// External function implementations
// ==============================================================================

scc_ErrorCode scc_nng_clusterng(scc_Clustering* const clustering,
                                scc_DataSetObject* const data_set_object,
                                const size_t num_data_points,
                                const size_t size_constraint,
                                const scc_SeedMethod seed_method,
                                const scc_AssignMethod assign_method,
                                const bool main_data_points[const],
                                const bool assign_secondary_points,
                                const bool main_radius_constraint,
                                const double main_radius,
                                const bool secondary_radius_constraint,
                                const double secondary_radius)
{
	if (!iscc_check_input_clustering(clustering)) return iscc_make_error(SCC_ER_INVALID_CLUSTERING);
	if (clustering->num_data_points != num_data_points) return iscc_make_error(SCC_ER_INVALID_INPUT);
	if (!iscc_check_data_set_object(data_set_object, num_data_points)) return iscc_make_error(SCC_ER_INVALID_DATA_OBJ);
	if (num_data_points < 2) return iscc_make_error(SCC_ER_INVALID_INPUT);
	if (size_constraint < 2) return iscc_make_error(SCC_ER_INVALID_INPUT);
	if (seed_method > SCC_SM_MAX) return iscc_make_error(SCC_ER_INVALID_INPUT);
	if (assign_method > SCC_AM_MAX) return iscc_make_error(SCC_ER_INVALID_INPUT);
	if (main_radius_constraint && main_radius <= 0.0) return iscc_make_error(SCC_ER_INVALID_INPUT);
	if (secondary_radius_constraint && secondary_radius <= 0.0) return iscc_make_error(SCC_ER_INVALID_INPUT);

	if (clustering->num_clusters != 0) return iscc_make_error(SCC_ER_NOT_IMPLEMENTED);

	scc_ErrorCode ec;
	iscc_Digraph nng;
	if ((ec = iscc_get_nng_with_size_constraint(data_set_object,
                                                num_data_points,
                                                size_constraint,
                                                main_data_points,
                                                main_radius_constraint,
                                                main_radius,
                                                &nng)) != SCC_ER_OK) {
		return ec;
	}

	iscc_SeedResult sr = {
		.capacity = 1 + (num_data_points / size_constraint),
		.count = 0,
		.seeds = NULL,
	};
	if ((ec = iscc_find_seeds(&nng, seed_method, &sr)) != SCC_ER_OK) {
		iscc_free_digraph(&nng);
		return ec;
	}

	bool* const unassigned = malloc(sizeof(bool[num_data_points]));
	if (unassigned == NULL) {
		iscc_free_digraph(&nng);
		free(sr.seeds);
		return iscc_make_error(SCC_ER_NO_MEMORY);
	}

	if ((ec = iscc_assign_seed_neighbors(clustering, &nng, &sr, unassigned)) != SCC_ER_OK) {
		iscc_free_digraph(&nng);
		free(sr.seeds);
		free(unassigned);
		return ec;
	}

	switch (assign_method) {
		case SCC_AM_IGNORE:
			iscc_free_digraph(&nng);
			free(sr.seeds);
			ec = iscc_remaining_ignore(clustering, unassigned);
			break;
		case SCC_AM_CLOSEST_ASSIGNED:
			free(sr.seeds);
			ec = iscc_remaining_to_nearest_assigned(data_set_object,
			                                        clustering,
			                                        &nng,
			                                        sr.count * size_constraint,
			                                        unassigned,
			                                        main_data_points,
			                                        assign_secondary_points,
			                                        secondary_radius_constraint,
			                                        secondary_radius);
			iscc_free_digraph(&nng);
			break;
		case SCC_AM_CLOSEST_SEED:
			iscc_free_digraph(&nng);
			ec = iscc_remaining_to_nearest_seed(data_set_object,
			                                    clustering,
			                                    &sr,
			                                    sr.count * size_constraint,
			                                    unassigned,
			                                    main_data_points,
			                                    assign_secondary_points,
			                                    main_radius_constraint,
			                                    main_radius,
			                                    secondary_radius_constraint,
			                                    secondary_radius);
			free(sr.seeds);
			break;
		default:
			assert(false);
			iscc_free_digraph(&nng);
			free(sr.seeds);
			ec = iscc_make_error(SCC_ER_NOT_IMPLEMENTED);
			break;
	}
	free(unassigned);

	if (ec != SCC_ER_OK) return ec;

	return iscc_no_error();
}


scc_ErrorCode scc_nng_clusterng_with_types(scc_Clustering* const clustering,
                                           scc_DataSetObject* const data_set_object,
                                           const size_t num_data_points,
                                           const size_t num_types,
                                           const scc_TypeLabel type_labels[const],
                                           const size_t type_constraints[const],
                                           const size_t size_constraint,
                                           const scc_SeedMethod seed_method,
                                           const scc_AssignMethod assign_method,
                                           const bool main_data_points[const],
                                           const bool assign_secondary_points,
                                           const bool main_radius_constraint,
                                           const double main_radius,
                                           const bool secondary_radius_constraint,
                                           const double secondary_radius)
{
	if (!iscc_check_input_clustering(clustering)) return iscc_make_error(SCC_ER_INVALID_CLUSTERING);
	if (clustering->num_data_points != num_data_points) return iscc_make_error(SCC_ER_INVALID_INPUT);
	if (!iscc_check_data_set_object(data_set_object, num_data_points)) return iscc_make_error(SCC_ER_INVALID_DATA_OBJ);
	if (num_data_points < 2) return iscc_make_error(SCC_ER_INVALID_INPUT);
	if (num_types < 2) return iscc_make_error(SCC_ER_INVALID_INPUT);
	if (type_labels == NULL) return iscc_make_error(SCC_ER_NULL_INPUT);
	if (type_constraints == NULL) return iscc_make_error(SCC_ER_NULL_INPUT);
	if (size_constraint < 1) return iscc_make_error(SCC_ER_INVALID_INPUT);
	if (seed_method > SCC_SM_MAX) return iscc_make_error(SCC_ER_INVALID_INPUT);
	if (assign_method > SCC_AM_MAX) return iscc_make_error(SCC_ER_INVALID_INPUT);
	if (main_radius_constraint && main_radius <= 0.0) return iscc_make_error(SCC_ER_INVALID_INPUT);
	if (secondary_radius_constraint && secondary_radius <= 0.0) return iscc_make_error(SCC_ER_INVALID_INPUT);

	if (clustering->num_clusters != 0) return iscc_make_error(SCC_ER_NOT_IMPLEMENTED);

	scc_ErrorCode ec;
	iscc_Digraph nng;
	if ((ec = iscc_get_nng_with_type_constraint(data_set_object,
                                                num_data_points,
                                                num_types,
                                                type_labels,
                                                type_constraints,
                                                size_constraint,
                                                main_data_points,
                                                main_radius_constraint,
                                                main_radius,
                                                &nng)) != SCC_ER_OK) {
		return ec;
	}

	iscc_SeedResult sr = {
		.capacity = 1 + (num_data_points / size_constraint),
		.count = 0,
		.seeds = NULL,
	};
	if ((ec = iscc_find_seeds(&nng, seed_method, &sr)) != SCC_ER_OK) {
		iscc_free_digraph(&nng);
		return ec;
	}

	bool* const unassigned = malloc(sizeof(bool[num_data_points]));
	if (unassigned == NULL) {
		iscc_free_digraph(&nng);
		free(sr.seeds);
		return iscc_make_error(SCC_ER_NO_MEMORY);
	}

	if ((ec = iscc_assign_seed_neighbors(clustering, &nng, &sr, unassigned)) != SCC_ER_OK) {
		iscc_free_digraph(&nng);
		free(sr.seeds);
		free(unassigned);
		return ec;
	}

	switch (assign_method) {
		case SCC_AM_IGNORE:
			iscc_free_digraph(&nng);
			free(sr.seeds);
			ec = iscc_remaining_ignore(clustering, unassigned);
			break;
		case SCC_AM_CLOSEST_ASSIGNED:
			free(sr.seeds);
			ec = iscc_remaining_to_nearest_assigned(data_set_object,
			                                        clustering,
			                                        &nng,
			                                        sr.count * size_constraint,
			                                        unassigned,
			                                        main_data_points,
			                                        assign_secondary_points,
			                                        secondary_radius_constraint,
			                                        secondary_radius);
			iscc_free_digraph(&nng);
			break;
		case SCC_AM_CLOSEST_SEED:
			iscc_free_digraph(&nng);
			ec = iscc_remaining_to_nearest_seed(data_set_object,
			                                    clustering,
			                                    &sr,
			                                    sr.count * size_constraint,
			                                    unassigned,
			                                    main_data_points,
			                                    assign_secondary_points,
			                                    main_radius_constraint,
			                                    main_radius,
			                                    secondary_radius_constraint,
			                                    secondary_radius);
			free(sr.seeds);
			break;
		default:
			assert(false);
			iscc_free_digraph(&nng);
			free(sr.seeds);
			ec = iscc_make_error(SCC_ER_NOT_IMPLEMENTED);
			break;
	}
	free(unassigned);

	if (ec != SCC_ER_OK) return ec;

	return iscc_no_error();
}


