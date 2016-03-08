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
 * License along with this library. If not, see http://www.gnu.org/licenses/
 * ============================================================================== */

#ifndef SCC_UT_DATA_OBJ_HG
#define SCC_UT_DATA_OBJ_HG

#include <stdbool.h>
#include <stddef.h>
#include "../include/scc_data_obj.h"
#include "../src/scc_data_obj_int.h"


double coord1[300] = { 58.339591, 14.339080, 54.090796,
                       32.886948, 92.021040, 88.571806,
                       92.140101, 92.922734, 87.079084,
                       32.344173, 56.100729, 82.458722,
                       38.543205, 59.676832, 85.043257,
                       82.001498, 74.182602, 84.850008,
                       21.322119, 70.158653, 15.792239,
                       36.534482, 55.675589, 21.872118,
                       79.354471, 12.170747, 21.670604,
                       63.770776, 89.857501, 76.243263,
                       53.371495,  7.344993, 44.892347,
                       14.919279, 78.005771, 31.493920,//11
                       34.338812, 79.480676, 41.811581,
                        5.887355, 83.253596,  6.627519,
                       85.891995, 52.290730, 60.541291,
                       29.879933, 75.740223, 99.945541,
                       64.962215, 72.222075, 11.303909,
                       70.490957, 12.712523, 75.334008,
                        3.939498, 14.795181, 22.818670,
                       90.630233, 66.769753, 50.167346,
                       82.494498, 71.269884, 69.331823,
                        9.949003, 69.817485, 69.873865,
                       46.347801, 56.373678, 56.700386,
                       46.894460, 29.406858,  8.125620,
                       23.228474, 44.874572, 31.742207,
                       65.737269, 48.881091, 15.846237,
                       80.667612, 32.900477, 83.856326,
                       68.455981, 38.698123, 16.405275,
                       89.370536, 28.686353, 74.753322,
                        7.713893, 26.322787, 51.084884,
                       39.347995, 61.147265, 83.932703,
                       80.892733, 47.217385, 21.357167,
                       74.847292, 84.195988, 95.906673,
                        4.561497, 66.274131, 73.647654,
                       81.377143, 60.330542, 19.126012,
                       52.268988, 95.605107,  9.719418,
                       83.466367, 59.505656, 53.039815,
                       93.789849, 59.406580, 16.585901,
                       14.919279, 78.005771, 31.493920,//38
                       11.753413, 87.619314, 14.567774,
                       59.757315, 46.423242, 57.792821,
                       32.181688,  3.594144, 85.612988,
                       35.506798, 77.363095, 78.970212,
                       60.786589,  3.626127, 83.955700,
                       69.301042, 28.262189, 67.031937,
                        4.663703, 22.508679, 62.795013,
                       38.038527, 60.075997, 20.424405,
                       42.342771, 59.891944, 69.509453,
                       19.076576, 80.342755, 47.855528,
                       90.072253, 25.591246, 65.982668,
                       26.534974, 74.685339, 90.346436,
                       87.572172, 34.918291, 12.060636,
                       97.695843, 60.754337, 99.080746,
                       73.873324, 41.550219, 49.762148,
                       17.458345, 80.531653, 33.586589,
                        7.205715, 17.903486, 10.691780,
                       72.618230, 48.918716, 97.852308,
                       11.270814, 67.099807, 64.112716,
                       83.887429, 78.990053, 98.527649,
                       24.215942, 71.595540, 23.482341,
                        9.972400, 79.533549, 50.401541,
                       16.284051, 61.896190, 62.891884,
                       58.954614, 28.690646, 47.018664,
                       76.976491, 15.391909, 24.441843,
                       52.304383, 38.520523, 51.500356,
                       92.459243,  1.669367, 24.576690,
                       96.740340, 76.838140,  4.726590,
                       66.862238, 84.556481, 50.745527,
                       35.653769, 42.110943, 75.697167,
                       84.704605, 87.575692, 77.887307,
                        6.299561, 67.854808, 66.749616,
                       95.669224, 84.201003, 20.208756,
                       35.969900,  0.520402, 87.027096,
                       42.737651, 91.252008, 93.953776,
                       96.061797, 17.692969, 94.674083,
                       39.457180, 62.316336, 11.475185,
                       39.279860, 62.190025, 14.516839,
                       74.024772, 13.094714, 90.761080,
                       12.650365, 98.585201, 83.146091,
                       12.208834, 58.191906, 63.479857,
                       91.765811, 87.832402, 62.653226,
                        4.728129,  1.041594, 62.568358,
                       31.891112, 50.281168, 56.143046,
                       33.218935, 28.496397,  3.692386,
                       41.062248,  6.607704, 81.478515,
                        7.823402, 72.066094, 66.992282,
                       47.940626, 72.293607, 11.503764,
                       19.659573,  7.822981, 70.297675,
                       70.449265, 20.117346, 85.946542,
                       62.054011,  0.254697, 94.918113,
                       69.638404, 21.202993, 16.822090,
                       65.231583, 37.044916, 75.058009,
                       22.309348, 93.652466, 74.167099,
                       51.677666, 36.297625, 94.532538,
                        9.245187,  1.814177, 70.441501,
                       30.013153, 70.798231, 42.073466,
                       27.190943, 91.639037, 91.947169,
                       85.634553, 17.124530, 36.040487,
                       48.638345, 67.017505, 84.482248,
                       17.279514, 20.615376, 47.647630  };

double coord2[15] = { -1.40865056, -0.83518405,  0.28314478,
                       0.99873944, -0.43372624,  1.30171154,
                      -0.09996759, -1.03051489, -1.49839711,
                       0.65701681, -0.02082501,  2.17450609,
                      -0.08082159, -0.35893786, -0.13682160 };


// ==============================================================================
// Valid data objects
// ==============================================================================

scc_DataSetObject scc_ut_test_data_large_struct = {
	.num_data_points = 100,
	.num_dimensions = 3,
	.data_matrix = coord1,
	.external_matrix = true,
	.data_set_object_version = ISCC_CURRENT_DATASETOBJ_VERSION,
};

scc_DataSetObject scc_ut_test_data_small_struct = {
	.num_data_points = 15,
	.num_dimensions = 1,
	.data_matrix = coord2,
	.external_matrix = true,
	.data_set_object_version = ISCC_CURRENT_DATASETOBJ_VERSION,
};

scc_DataSetObject* const scc_ut_test_data_large = &scc_ut_test_data_large_struct;

scc_DataSetObject* const scc_ut_test_data_small = &scc_ut_test_data_small_struct;


// ==============================================================================
// Invalid data objects
// ==============================================================================

scc_DataSetObject scc_ut_test_data_invalid1_struct = {
	.num_data_points = 15,
	.num_dimensions = 0,
	.data_matrix = coord2,
	.external_matrix = true,
	.data_set_object_version = ISCC_CURRENT_DATASETOBJ_VERSION,
};

scc_DataSetObject scc_ut_test_data_invalid2_struct = {
	.num_data_points = 15,
	.num_dimensions = 1,
	.data_matrix = NULL,
	.external_matrix = true,
	.data_set_object_version = ISCC_CURRENT_DATASETOBJ_VERSION,
};

scc_DataSetObject scc_ut_test_data_invalid3_struct = {
	.num_data_points = 15,
	.num_dimensions = 1,
	.data_matrix = coord2,
	.external_matrix = true,
	.data_set_object_version = 0,
};

const size_t scc_ut_num_invalid_data = 3;

scc_DataSetObject* const scc_ut_test_data_invalid[3] = {
	&scc_ut_test_data_invalid1_struct,
	&scc_ut_test_data_invalid2_struct,
	&scc_ut_test_data_invalid3_struct,
};


#endif
