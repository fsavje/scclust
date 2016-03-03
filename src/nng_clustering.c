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
#include <stdint.h>
#include <stdlib.h>
#include "../include/scclust.h"
#include "digraph_core.h"
#include "dist_search.h"
#include "error.h"
#include "nng_core.h"
#include "nng_findseeds.h"
#include "scclust_int.h"


// ==============================================================================
// Internal variables
// ==============================================================================

static const scc_SeedMethod SCC_SM_MAX = SCC_SM_EXCLUSION_UPDATING;
static const scc_AssignMethod SCC_AM_MAX = SCC_AM_CLOSEST_SEED;


// ==============================================================================
// External function implementations
// ==============================================================================

scc_ErrorCode scc_nng_clusterng(scc_Clustering* const clustering,
                                void* const data_set_object,
                                const uint32_t size_constraint,
                                const size_t len_main_data_points,
                                const bool main_data_points[const],
                                const scc_SeedMethod seed_method,
                                const scc_AssignMethod assign_method,
                                const bool assign_secondary_points,
                                const bool main_radius_constraint,
                                const double main_radius,
                                const bool secondary_radius_constraint,
                                const double secondary_radius)
{
	if (!iscc_check_input_clustering(clustering)) return iscc_make_error(SCC_ER_INVALID_CLUSTERING);
	if (clustering->num_data_points < 2) return iscc_make_error(SCC_ER_INVALID_INPUT);
	if (!iscc_check_data_set_object(data_set_object, clustering->num_data_points)) return iscc_make_error(SCC_ER_INVALID_DATA_OBJ);
	if (size_constraint < 2) return iscc_make_error(SCC_ER_INVALID_INPUT);
	if ((main_data_points != NULL) && (len_main_data_points < clustering->num_data_points)) return iscc_make_error(SCC_ER_INVALID_INPUT);
	if (seed_method > SCC_SM_MAX) return iscc_make_error(SCC_ER_INVALID_INPUT);
	if (assign_method > SCC_AM_MAX) return iscc_make_error(SCC_ER_INVALID_INPUT);
	if ((assign_method == SCC_AM_IGNORE) && assign_secondary_points) return iscc_make_error(SCC_ER_INVALID_INPUT);
	if ((assign_method == SCC_AM_BY_NNG) && assign_secondary_points) return iscc_make_error(SCC_ER_INVALID_INPUT);
	if (main_radius_constraint && main_radius <= 0.0) return iscc_make_error(SCC_ER_INVALID_INPUT);
	if (secondary_radius_constraint && secondary_radius <= 0.0) return iscc_make_error(SCC_ER_INVALID_INPUT);

	if (clustering->num_clusters != 0) return iscc_make_error(SCC_ER_NOT_IMPLEMENTED);

	scc_ErrorCode ec;
	iscc_Digraph nng;
	if ((ec = iscc_get_nng_with_size_constraint(data_set_object,
	                                            clustering->num_data_points,
	                                            size_constraint,
	                                            main_data_points,
	                                            main_radius_constraint,
	                                            main_radius,
	                                            &nng)) != SCC_ER_OK) {
		return ec;
	}

	iscc_SeedResult sr = {
		.capacity = 1 + (clustering->num_data_points / size_constraint),
		.count = 0,
		.seeds = NULL,
	};
	if ((ec = iscc_find_seeds(&nng, seed_method, &sr)) != SCC_ER_OK) {
		iscc_free_digraph(&nng);
		return ec;
	}

	bool* const unassigned = malloc(sizeof(bool[clustering->num_data_points]));
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

	const size_t num_assigned = sr.count * size_constraint;
	assert(num_assigned <= clustering->num_data_points);
	if (num_assigned == clustering->num_data_points) {
		iscc_free_digraph(&nng);
		free(sr.seeds);
		free(unassigned);
		return iscc_no_error();
	}

	switch (assign_method) {
		case SCC_AM_IGNORE:
			iscc_free_digraph(&nng);
			free(sr.seeds);
			ec = iscc_remaining_ignore(clustering, unassigned);
			break;
		case SCC_AM_BY_NNG:
			free(sr.seeds);
			ec = iscc_remaining_by_nng(clustering, &nng, unassigned);
			iscc_free_digraph(&nng);
			break;
		case SCC_AM_CLOSEST_ASSIGNED:
			free(sr.seeds);
			ec = iscc_remaining_to_nearest_assigned(clustering,
			                                        data_set_object,
			                                        &nng,
			                                        num_assigned,
			                                        unassigned,
			                                        main_data_points,
			                                        assign_secondary_points,
			                                        main_radius_constraint,
			                                        main_radius,
			                                        secondary_radius_constraint,
			                                        secondary_radius);
			// `nng` is freed by `iscc_remaining_to_nearest_assigned`
			break;
		case SCC_AM_CLOSEST_SEED:
			iscc_free_digraph(&nng);
			ec = iscc_remaining_to_nearest_seed(clustering,
			                                    data_set_object,
			                                    &sr,
			                                    num_assigned,
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
			iscc_free_digraph(&nng);
			free(sr.seeds);
			assert(false);
			ec = iscc_make_error(SCC_ER_NOT_IMPLEMENTED);
			break;
	}

	free(unassigned);

	if (ec != SCC_ER_OK) return ec;

	return iscc_no_error();
}


scc_ErrorCode scc_nng_clusterng_with_types(scc_Clustering* const clustering,
                                           void* const data_set_object,
                                           const uint32_t size_constraint,
                                           const uint64_t num_types,
                                           const uint32_t type_size_constraints[const],
                                           const size_t len_type_labels,
                                           const scc_TypeLabel type_labels[const],
                                           const size_t len_main_data_points,
                                           const bool main_data_points[const],
                                           const scc_SeedMethod seed_method,
                                           const scc_AssignMethod assign_method,
                                           const bool assign_secondary_points,
                                           const bool main_radius_constraint,
                                           const double main_radius,
                                           const bool secondary_radius_constraint,
                                           const double secondary_radius)
{
	if (!iscc_check_input_clustering(clustering)) return iscc_make_error(SCC_ER_INVALID_CLUSTERING);
	if (clustering->num_data_points < 2) return iscc_make_error(SCC_ER_INVALID_INPUT);
	if (!iscc_check_data_set_object(data_set_object, clustering->num_data_points)) return iscc_make_error(SCC_ER_INVALID_DATA_OBJ);
	if (size_constraint == 0) return iscc_make_error(SCC_ER_INVALID_INPUT);
	if (num_types < 2) return iscc_make_error(SCC_ER_INVALID_INPUT);
	if (num_types > UINT16_MAX) return iscc_make_error(SCC_ER_TOO_LARGE_PROBLEM);
	if (type_size_constraints == NULL) return iscc_make_error(SCC_ER_NULL_INPUT);
	if (len_type_labels < clustering->num_data_points) return iscc_make_error(SCC_ER_INVALID_INPUT);
	if (type_labels == NULL) return iscc_make_error(SCC_ER_NULL_INPUT);
	if ((main_data_points != NULL) && (len_main_data_points < clustering->num_data_points)) return iscc_make_error(SCC_ER_INVALID_INPUT);
	if (seed_method > SCC_SM_MAX) return iscc_make_error(SCC_ER_INVALID_INPUT);
	if (assign_method > SCC_AM_MAX) return iscc_make_error(SCC_ER_INVALID_INPUT);
	if ((assign_method == SCC_AM_IGNORE) && assign_secondary_points) return iscc_make_error(SCC_ER_INVALID_INPUT);
	if ((assign_method == SCC_AM_BY_NNG) && assign_secondary_points) return iscc_make_error(SCC_ER_INVALID_INPUT);
	if (main_radius_constraint && main_radius <= 0.0) return iscc_make_error(SCC_ER_INVALID_INPUT);
	if (secondary_radius_constraint && secondary_radius <= 0.0) return iscc_make_error(SCC_ER_INVALID_INPUT);

	if (clustering->num_clusters != 0) return iscc_make_error(SCC_ER_NOT_IMPLEMENTED);

	assert(num_types <= UINT_FAST16_MAX);
	const uint_fast16_t num_types_f16 = (uint_fast16_t) num_types;

	scc_ErrorCode ec;
	iscc_Digraph nng;
	if ((ec = iscc_get_nng_with_type_constraint(data_set_object,
	                                            clustering->num_data_points,
	                                            size_constraint,
	                                            num_types_f16,
	                                            type_size_constraints,
	                                            type_labels,
	                                            main_data_points,
	                                            main_radius_constraint,
	                                            main_radius,
	                                            &nng)) != SCC_ER_OK) {
		return ec;
	}

	iscc_SeedResult sr = {
		.capacity = 1 + (clustering->num_data_points / size_constraint),
		.count = 0,
		.seeds = NULL,
	};
	if ((ec = iscc_find_seeds(&nng, seed_method, &sr)) != SCC_ER_OK) {
		iscc_free_digraph(&nng);
		return ec;
	}

	bool* const unassigned = malloc(sizeof(bool[clustering->num_data_points]));
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

	const size_t num_assigned = sr.count * size_constraint;
	assert(num_assigned <= clustering->num_data_points);
	if (num_assigned == clustering->num_data_points) {
		iscc_free_digraph(&nng);
		free(sr.seeds);
		free(unassigned);
		return iscc_no_error();
	}

	switch (assign_method) {
		case SCC_AM_IGNORE:
			iscc_free_digraph(&nng);
			free(sr.seeds);
			ec = iscc_remaining_ignore(clustering, unassigned);
			break;
		case SCC_AM_BY_NNG:
			free(sr.seeds);
			ec = iscc_remaining_by_nng(clustering, &nng, unassigned);
			iscc_free_digraph(&nng);
			break;
		case SCC_AM_CLOSEST_ASSIGNED:
			free(sr.seeds);
			iscc_free_digraph(&nng);
			ec = iscc_remaining_to_nearest_assigned(clustering,
			                                        data_set_object,
			                                        NULL,
			                                        num_assigned,
			                                        unassigned,
			                                        main_data_points,
			                                        assign_secondary_points,
			                                        main_radius_constraint,
			                                        main_radius,
			                                        secondary_radius_constraint,
			                                        secondary_radius);
			break;
		case SCC_AM_CLOSEST_SEED:
			iscc_free_digraph(&nng);
			ec = iscc_remaining_to_nearest_seed(clustering,
			                                    data_set_object,
			                                    &sr,
			                                    num_assigned,
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
			iscc_free_digraph(&nng);
			free(sr.seeds);
			assert(false);
			ec = iscc_make_error(SCC_ER_NOT_IMPLEMENTED);
			break;
	}

	free(unassigned);

	if (ec != SCC_ER_OK) return ec;

	return iscc_no_error();
}
