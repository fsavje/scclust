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

#define SCC_DOUBLE_ASSERT

#include "test_suite.h"

#include <stdbool.h>
#include <stddef.h>
#include "../src/dist_search.h"
#include "data_object_test.h"


void scc_ut_get_data_point_count(void** state)
{
	(void) state;

	assert_int_equal(scc_get_data_point_count(&scc_ut_test_data_small), 15);
	assert_int_equal(scc_get_data_point_count(&scc_ut_test_data_large), 100);
}


void scc_ut_get_dist_matrix(void** state)
{
	(void) state;

	scc_Vid mat1[2] = { 76, 33 };
	double output1[1];
    double ref_mat1[1] = { 68.691324 };
	assert_true(scc_get_dist_matrix(&scc_ut_test_data_large, 2, mat1, output1));
	for (size_t i = 0; i < 1; ++i) {
		assert_double_equal(output1[i], ref_mat1[i]);
	}

	scc_Vid mat2[5] = { 54, 11, 44, 38, 2 };
	double output2[10];
    double ref_mat2[10] = { 4.148036, 80.860085, 4.148036, 92.694933, 81.821457, 0.000000, 96.308263, 81.821457, 71.445761, 96.308263 };
	assert_true(scc_get_dist_matrix(&scc_ut_test_data_large, 5, mat2, output2));
	for (size_t i = 0; i < 10; ++i) {
		assert_double_equal(output2[i], ref_mat2[i]);
	}

	scc_Vid mat3[10] = { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18 };
	double output3[45];
    double ref_mat3[45] = { 91.684744, 58.356349, 77.155022, 38.696197, 12.578183, 70.499958, 47.339958, 72.284286,
                            62.749693, 63.103580, 103.030113, 104.702555, 102.986773, 74.637958, 48.931105, 83.120587,
                            134.211258, 72.125847, 89.098152, 67.606177, 47.737270, 53.822010, 79.327414, 84.164591,
                            82.249050, 76.285875, 30.550623, 80.566800, 43.918798, 58.452034, 35.180283, 83.442765,
                            56.243067, 62.616031, 75.469357, 74.667871, 57.641926, 73.970019, 54.646819, 61.219489,
                            43.831474, 73.953108, 57.093204, 97.699151, 84.582562 };
	assert_true(scc_get_dist_matrix(&scc_ut_test_data_large, 10, mat3, output3));
	for (size_t i = 0; i < 45; ++i) {
		assert_double_equal(output3[i], ref_mat3[i]);
	}

	scc_Vid mat4[2] = { 0, 6 };
	double output4[1];
    double ref_mat4[1] = { 1.308683 };
	assert_true(scc_get_dist_matrix(&scc_ut_test_data_small, 2, mat4, output4));
	for (size_t i = 0; i < 1; ++i) {
		assert_double_equal(output4[i], ref_mat4[i]);
	}

	scc_Vid mat5[5] = { 5, 2, 7, 3, 8 };
	double output5[10];
    double ref_mat5[10] = { 1.018566, 2.332226, 0.302972, 2.800108, 1.313659, 0.715594, 1.781541, 2.029254, 0.467882, 2.497136 };
	assert_true(scc_get_dist_matrix(&scc_ut_test_data_small, 5, mat5, output5));
	for (size_t i = 0; i < 10; ++i) {
		assert_double_equal(output5[i], ref_mat5[i]);
	}

	scc_Vid* mat6 = NULL;
	double output6[105];
    double ref_mat6[105] = { 0.573467, 1.691795, 2.407390, 0.974924, 2.710362, 1.308683, 0.378136, 0.089747, 2.065667, 1.387826,
                             3.583157, 1.327829, 1.049713, 1.271829, 1.118329, 1.833923, 0.401458, 2.136896, 0.735216, 0.195331,
                             0.663213, 1.492201, 0.814359, 3.009690, 0.754362, 0.476246, 0.698362, 0.715595, 0.716871, 1.018567,
                             0.383112, 1.313660, 1.781542, 0.373872, 0.303970, 1.891361, 0.363966, 0.642083, 0.419966, 1.432466,
                             0.302972, 1.098707, 2.029254, 2.497137, 0.341723, 1.019564, 1.175767, 1.079561, 1.357677, 1.135561,
                             1.735438, 0.333759, 0.596789, 1.064671, 1.090743, 0.412901, 2.608232, 0.352905, 0.074788, 0.296905,
                             1.401679, 2.332226, 2.800109, 0.644695, 1.322537, 0.872795, 1.382533, 1.660649, 1.438533, 0.930547,
                             1.398430, 0.756984, 0.079143, 2.274474, 0.019146, 0.258970, 0.036854, 0.467882, 1.687532, 1.009690,
                             3.205021, 0.949693, 0.671577, 0.893693, 2.155414, 1.477572, 3.672903, 1.417576, 1.139459, 1.361576,
                             0.677842, 1.517489, 0.737838, 1.015955, 0.793838, 2.195331, 0.059997, 0.338113, 0.115997, 2.255328,
                             2.533444, 2.311328, 0.278116, 0.056000, 0.222116 };
	assert_true(scc_get_dist_matrix(&scc_ut_test_data_small, 15, mat6, output6));
	for (size_t i = 0; i < 105; ++i) {
		assert_double_equal(output6[i], ref_mat6[i]);
	}
}


void scc_ut_init_close_max_dist_object(void** state)
{
	(void) state;

	scc_Vid cols1[10] = { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18 };
	scc_Vid cols2[5] = { 5, 2, 7, 3, 8 };

	scc_MaxDistObject* tmp_mdo1 = scc_init_max_dist_object(&scc_ut_test_data_large, 10, cols1, 1);
	assert_non_null(tmp_mdo1);
	assert_true(scc_close_max_dist_object(tmp_mdo1));

	scc_MaxDistObject* tmp_mdo2 = scc_init_max_dist_object(&scc_ut_test_data_small, 5, cols2, 10);
	assert_non_null(tmp_mdo2);
	assert_true(scc_close_max_dist_object(tmp_mdo2));

	scc_MaxDistObject* tmp_mdo3 = scc_init_max_dist_object(&scc_ut_test_data_large, 100, NULL, 50);
	assert_non_null(tmp_mdo3);
	assert_true(scc_close_max_dist_object(tmp_mdo3));

	scc_MaxDistObject* tmp_mdo4 = scc_init_max_dist_object(&scc_ut_test_data_small, 15, NULL, 100);
	assert_non_null(tmp_mdo4);
	assert_true(scc_close_max_dist_object(tmp_mdo4));
}


void scc_ut_get_max_dist(void** state)
{
	(void) state;

	scc_Vid cols1[10] = { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18 };
	scc_Vid rows1[5] = { 0, 1, 2, 3, 4 };
	scc_Vid out1_max_indices1[5];
	scc_Vid out2_max_indices1[5];
    double out2_max_dists1[5];
	scc_Vid ref_max_indices1[5] = { 2, 8, 18, 8, 8 };
    double ref_max_dists1[5] = { 91.684744, 114.066081, 134.211258, 88.515573, 89.098152 };

	scc_MaxDistObject* tmp_mdo1 = scc_init_max_dist_object(&scc_ut_test_data_large, 10, cols1, 5);
	assert_non_null(tmp_mdo1);

	assert_true(scc_get_max_dist(tmp_mdo1, 5, rows1, out1_max_indices1, NULL));
	assert_true(scc_get_max_dist(tmp_mdo1, 5, rows1, out2_max_indices1, out2_max_dists1));

	for (size_t i = 0; i < 5; ++i) {
		assert_int_equal(out1_max_indices1[i], ref_max_indices1[i]);
		assert_int_equal(out2_max_indices1[i], ref_max_indices1[i]);
		assert_double_equal(out2_max_dists1[i], ref_max_dists1[i]);
	}
	assert_true(scc_close_max_dist_object(tmp_mdo1));


	scc_Vid mat[5] = { 54, 11, 44, 38, 2 };
	scc_Vid out1_max_indices2[5];
	scc_Vid out2_max_indices2[5];
    double out2_max_dists2[5];
	scc_Vid ref_max_indices2[5] = { 2, 2, 0, 2, 0 };
    double ref_max_dists2[5] = { 92.694933, 96.308263, 81.821457, 96.308263, 96.308263 };

	scc_MaxDistObject* tmp_mdo2 = scc_init_max_dist_object(&scc_ut_test_data_large, 5, mat, 5);
	assert_non_null(tmp_mdo2);

	assert_true(scc_get_max_dist(tmp_mdo2, 5, mat, out1_max_indices2, NULL));
	assert_true(scc_get_max_dist(tmp_mdo2, 5, mat, out2_max_indices2, out2_max_dists2));

	assert_true((out1_max_indices2[2] == 11) || (out1_max_indices2[2] == 38));
	assert_true((out2_max_indices2[2] == 11) || (out2_max_indices2[2] == 38));
	assert_true((out1_max_indices2[4] == 11) || (out1_max_indices2[4] == 38));
	assert_true((out2_max_indices2[4] == 11) || (out2_max_indices2[4] == 38));
	out1_max_indices2[2] = out2_max_indices2[2] = 0;
	out1_max_indices2[4] = out2_max_indices2[4] = 0;

	for (size_t i = 0; i < 5; ++i) {
		assert_int_equal(out1_max_indices2[i], ref_max_indices2[i]);
		assert_int_equal(out2_max_indices2[i], ref_max_indices2[i]);
		assert_double_equal(out2_max_dists2[i], ref_max_dists2[i]);
	}
	assert_true(scc_close_max_dist_object(tmp_mdo2));


	scc_Vid col3[1] = { 88 };
	scc_Vid row3[1] = { 21 };
	scc_Vid out1_max_indices3[1];
	scc_Vid out2_max_indices3[1];
    double out2_max_dists3[1];
	scc_Vid ref_max_indices3[1] = { 88 };
    double ref_max_dists3[1] = { 79.929447 };

	scc_MaxDistObject* tmp_mdo3 = scc_init_max_dist_object(&scc_ut_test_data_large, 1, col3, 1);
	assert_non_null(tmp_mdo3);

	assert_true(scc_get_max_dist(tmp_mdo3, 1, row3, out1_max_indices3, NULL));
	assert_true(scc_get_max_dist(tmp_mdo3, 1, row3, out2_max_indices3, out2_max_dists3));

	for (size_t i = 0; i < 1; ++i) {
		assert_int_equal(out1_max_indices3[i], ref_max_indices3[i]);
		assert_int_equal(out2_max_indices3[i], ref_max_indices3[i]);
		assert_double_equal(out2_max_dists3[i], ref_max_dists3[i]);
	}
	assert_true(scc_close_max_dist_object(tmp_mdo3));


	scc_Vid col4[2] = { 76, 33 };
	scc_Vid* row4 = NULL;
	scc_Vid out1_max_indices4[100];
	scc_Vid out2_max_indices4[100];
    double out2_max_dists4[100];
	scc_Vid ref_max_indices4[100] = { 33, 76, 76, 76, 76, 76, 33, 33, 33, 76, 33, 33, 33, 33, 33, 76, 33, 33, 33, 33, 33, 76,
                                      33, 33, 33, 33, 76, 33, 33, 76, 76, 33, 76, 76, 33, 33, 33, 33, 33, 33, 33, 76, 76, 76,
                                      33, 76, 33, 76, 76, 33, 76, 33, 76, 33, 33, 33, 76, 76, 76, 33, 76, 76, 33, 33, 33, 33,
                                      33, 33, 76, 33, 76, 33, 76, 76, 76, 33, 33, 76, 76, 76, 33, 76, 76, 33, 76, 76, 33, 76,
                                      76, 76, 33, 76, 76, 76, 76, 33, 76, 33, 76, 76 };
    double ref_max_dists4[100] = { 77.277446, 80.093052, 94.889324, 68.565901, 70.575027, 83.160778, 60.359399, 61.768171, 105.937618, 71.940570,
                                   81.743054, 44.965012, 45.548182, 69.150261, 83.558136, 87.005917, 87.007858, 84.961070, 72.346768, 89.215457,
                                   78.212126, 63.110044, 46.166265, 86.281009, 50.620783, 85.941822, 85.899872, 90.108997, 92.771960, 60.167192,
                                   69.423729, 94.466461, 91.507430, 68.691324, 94.385270, 84.989001, 81.831985, 106.136260, 44.965012, 63.227943,
                                   60.761928, 92.404133, 66.322661, 93.348942, 75.365006, 71.439972, 63.181031, 55.125767, 43.001889, 95.004940,
                                   77.901815, 108.013525, 102.788825, 77.368718, 44.435285, 79.436473, 90.732436, 57.169604, 96.591368, 54.140311,
                                   49.471576, 53.563407, 71.275751, 101.262746, 59.499149, 119.615002, 115.579628, 68.848632, 64.493028, 83.034023,
                                   62.032676, 107.134058, 95.246184, 84.656849, 107.839511, 71.405803, 68.691324, 97.112011, 82.120114, 56.091020,
                                   90.499883, 85.099361, 43.922154, 84.511215, 87.042733, 61.973523, 76.025298, 80.325792, 88.565482, 104.014478,
                                   97.445144, 70.505331, 69.541621, 85.009612, 87.606463, 40.806661, 83.718828, 101.994229, 70.753405, 57.533607 };

	scc_MaxDistObject* tmp_mdo4 = scc_init_max_dist_object(&scc_ut_test_data_large, 2, col4, 100);
	assert_non_null(tmp_mdo4);

	assert_true(scc_get_max_dist(tmp_mdo4, 100, row4, out1_max_indices4, NULL));
	assert_true(scc_get_max_dist(tmp_mdo4, 100, row4, out2_max_indices4, out2_max_dists4));

	for (size_t i = 0; i < 100; ++i) {
		assert_int_equal(out1_max_indices4[i], ref_max_indices4[i]);
		assert_int_equal(out2_max_indices4[i], ref_max_indices4[i]);
		assert_double_equal(out2_max_dists4[i], ref_max_dists4[i]);
	}
	assert_true(scc_close_max_dist_object(tmp_mdo4));


	scc_Vid* col5 = NULL;
	scc_Vid row5[1] = { 15 };
	scc_Vid out1_max_indices5[1];
	scc_Vid out2_max_indices5[1];
    double out2_max_dists5[1];
	scc_Vid ref_max_indices5[1] = { 65 };
    double ref_max_dists5[1] = { 122.813377 };

	scc_MaxDistObject* tmp_mdo5 = scc_init_max_dist_object(&scc_ut_test_data_large, 100, col5, 1);
	assert_non_null(tmp_mdo5);

	assert_true(scc_get_max_dist(tmp_mdo5, 1, row5, out1_max_indices5, NULL));
	assert_true(scc_get_max_dist(tmp_mdo5, 1, row5, out2_max_indices5, out2_max_dists5));

	for (size_t i = 0; i < 1; ++i) {
		assert_int_equal(out1_max_indices5[i], ref_max_indices5[i]);
		assert_int_equal(out2_max_indices5[i], ref_max_indices5[i]);
		assert_double_equal(out2_max_dists5[i], ref_max_dists5[i]);
	}
	assert_true(scc_close_max_dist_object(tmp_mdo5));


	scc_Vid* col6 = NULL;
	scc_Vid* row6 = NULL;
	scc_Vid out1_max_indices6[15];
	scc_Vid out2_max_indices6[15];
    double out2_max_dists6[15];
	scc_Vid ref_max_indices6[15] = { 11, 11, 11, 8, 11, 8, 11, 11,
                                     11, 8, 11, 8, 11, 11, 11 };
    double ref_max_dists6[15] = { 3.583157, 3.009690, 1.891361, 2.497137, 2.608232,
                                  2.800109, 2.274474, 3.205021, 3.672903, 2.155414,
                                  2.195331, 3.672903, 2.255328, 2.533444, 2.311328 };

	scc_MaxDistObject* tmp_mdo6 = scc_init_max_dist_object(&scc_ut_test_data_small, 15, col6, 15);
	assert_non_null(tmp_mdo6);

	assert_true(scc_get_max_dist(tmp_mdo6, 15, row6, out1_max_indices6, NULL));
	assert_true(scc_get_max_dist(tmp_mdo6, 15, row6, out2_max_indices6, out2_max_dists6));

	for (size_t i = 0; i < 15; ++i) {
		assert_int_equal(out1_max_indices6[i], ref_max_indices6[i]);
		assert_int_equal(out2_max_indices6[i], ref_max_indices6[i]);
		assert_double_equal(out2_max_dists6[i], ref_max_dists6[i]);
	}
	assert_true(scc_close_max_dist_object(tmp_mdo6));
}


void scc_ut_init_close_dist_column_object(void** state)
{
	(void) state;

	scc_Vid cols1[10] = { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18 };
	scc_Vid cols2[5] = { 5, 2, 7, 3, 8 };

	scc_DistColObject* tmp_dco1 = scc_init_dist_column_object(&scc_ut_test_data_large, 10, cols1, 1);
	assert_non_null(tmp_dco1);
	assert_true(scc_close_dist_column_object(tmp_dco1));

	scc_DistColObject* tmp_dco2 = scc_init_dist_column_object(&scc_ut_test_data_small, 5, cols2, 10);
	assert_non_null(tmp_dco2);
	assert_true(scc_close_dist_column_object(tmp_dco2));

	scc_DistColObject* tmp_dco3 = scc_init_dist_column_object(&scc_ut_test_data_large, 100, NULL, 50);
	assert_non_null(tmp_dco3);
	assert_true(scc_close_dist_column_object(tmp_dco3));

	scc_DistColObject* tmp_dco4 = scc_init_dist_column_object(&scc_ut_test_data_small, 15, NULL, 100);
	assert_non_null(tmp_dco4);
	assert_true(scc_close_dist_column_object(tmp_dco4));
}


void scc_ut_get_dist_row(void** state)
{
	(void) state;

	scc_Vid cols1[10] = { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18 };
	scc_Vid rows1[5] = { 0, 1, 2, 3, 4 };
	double output1[50];
	double ref_distances1[50] = { 0.000000,  91.684744,  58.356349,  77.155022,  38.696197,  12.578183,  70.499958,  47.339958,  72.284286,  62.749693,
                                  88.720144,  59.278811,  33.024108,  76.867253, 114.066081,  97.455347,  48.434361,  71.928731,  85.971796, 105.476363,
                                  91.684744,   0.000000,  63.103580, 103.030113, 104.702555, 102.986773,  74.637958,  48.931105,  83.120587, 134.211258,
                                  56.785001,  70.375854,   7.608964,  69.018337,  88.515573,  65.042314,  46.933896,  57.984990,  79.917719,  77.909625,
                                  58.356349,  63.103580,   0.000000,  72.125847,  89.098152,  67.606177,  47.737270,  53.822010,  79.327414,  84.164591  };

	scc_DistColObject* tmp_dco1 = scc_init_dist_column_object(&scc_ut_test_data_large, 10, cols1, 5);
	assert_non_null(tmp_dco1);
	assert_true(scc_get_dist_row(tmp_dco1, 5, rows1, output1));
	for (size_t i = 0; i < 50; ++i) {
		assert_double_equal(output1[i], ref_distances1[i]);
	}
	assert_true(scc_close_dist_column_object(tmp_dco1));


	scc_Vid mat[5] = { 54, 11, 44, 38, 2 };
	double output2[25];
	double ref_distances2[25] = { 0.000000,  4.148036, 80.860085,  4.148036, 92.694933,
                                  4.148036,  0.000000, 81.821457,  0.000000, 96.308263,
                                  80.860085, 81.821457,  0.000000, 81.821457, 71.445761,
                                  4.148036,  0.000000, 81.821457,  0.000000, 96.308263,
                                  92.694933, 96.308263, 71.445761, 96.308263,  0.000000  };
	scc_DistColObject* tmp_dco2 = scc_init_dist_column_object(&scc_ut_test_data_large, 5, mat, 5);
	assert_non_null(tmp_dco2);
	assert_true(scc_get_dist_row(tmp_dco2, 5, mat, output2));
	assert_true(scc_close_dist_column_object(tmp_dco2));
	for (size_t i = 0; i < 25; ++i) {
		assert_double_equal(output2[i], ref_distances2[i]);
	}


	scc_Vid col3[1] = { 88 };
	scc_Vid row3[1] = { 21 };
	double output3[1];
	double ref_distances3[1] = { 79.929447 };
	scc_DistColObject* tmp_dco3 = scc_init_dist_column_object(&scc_ut_test_data_large, 1, col3, 1);
	assert_non_null(tmp_dco3);
	assert_true(scc_get_dist_row(tmp_dco3, 1, row3, output3));
	for (size_t i = 0; i < 1; ++i) {
		assert_double_equal(output3[i], ref_distances3[i]);
	}
	assert_true(scc_close_dist_column_object(tmp_dco3));


	scc_Vid col4[2] = { 76, 33 };
	scc_Vid* row4 = NULL;
	double output4[200];
	double ref_distances4[200] = { 64.954479, 77.277446, 80.093052, 41.084849, 94.889324, 92.523312, 68.565901, 30.870862, 70.575027, 36.443663, 83.160778,
	                               78.644710, 19.687715, 60.359399, 10.201721, 61.768171, 64.490922, 105.937618, 71.940570, 63.785980, 64.259032, 81.743054,
	                               33.642191, 44.965012, 32.686136, 45.548182, 40.261331, 69.150261, 66.249055, 83.558136, 87.005917, 37.712186, 27.758752,
	                               87.007858, 84.385329, 84.961070, 59.700359, 72.346768, 62.680087, 89.215457, 70.389142, 78.212126, 63.110044, 7.471416,
	                               43.165233, 46.166265, 34.257347, 86.281009, 29.226447, 50.620783, 29.646071, 85.941822, 85.899872, 83.726689, 37.505755,
	                               90.108997, 85.205667, 92.771960, 60.167192, 45.990500, 69.423729, 36.635603, 44.750433, 94.466461, 91.507430, 75.873249,
	                               68.691324, 0.000000, 42.389661, 94.385270, 36.170435, 84.989001, 58.682816, 81.831985, 54.620211, 106.136260, 33.642191,
	                               44.965012, 37.474746, 63.227943, 50.405637, 60.761928, 92.404133, 69.532905, 66.322661, 33.300246, 93.348942, 84.807365,
	                               69.355641, 75.365006, 71.439972, 45.091075, 6.396042, 63.181031, 55.125767, 38.539352, 43.001889, 32.769612, 81.043626,
	                               95.004940, 77.901815, 28.851888, 55.515113, 108.013525, 102.788825, 96.702206, 53.525145, 77.368718, 34.296490, 44.435285,
	                               54.814956, 79.436473, 90.732436, 74.288590, 57.169604, 11.688102, 96.591368, 84.102986, 19.893857, 54.140311, 49.471576,
	                               27.303330, 53.563407, 16.500643, 50.652472, 71.275751, 60.906530, 101.262746, 45.800263, 59.499149, 81.191117, 119.615002,
	                               60.100940, 115.579628, 50.730304, 68.848632, 64.493028, 39.430820, 81.997901, 83.034023, 62.032676, 7.287135, 60.800012,
	                               107.134058, 95.246184, 74.088124, 84.656849, 49.936476, 107.839511, 105.709720, 3.049435, 71.405803, 0.000000, 68.691324,
	                               97.112011, 89.140711, 82.120114, 34.636041, 56.091020, 15.072764, 75.692921, 90.499883, 85.099361, 66.166929, 43.922154,
	                               36.181404, 35.904932, 84.511215, 87.042733, 70.382595, 61.973523, 9.406425, 13.644408, 76.025298, 80.325792, 60.462478,
	                               88.565482, 81.381260, 104.014478, 90.090994, 51.057734, 97.445144, 70.505331, 67.358707, 69.541621, 32.631722, 85.009612,
	                               59.621327, 87.606463, 64.709366, 30.320633, 40.806661, 83.718828, 38.604955, 68.139004, 101.994229, 70.753405, 45.395039,
	                               57.533607, 54.059885 };
	scc_DistColObject* tmp_dco4 = scc_init_dist_column_object(&scc_ut_test_data_large, 2, col4, 100);
	assert_non_null(tmp_dco4);
	assert_true(scc_get_dist_row(tmp_dco4, 100, row4, output4));
	for (size_t i = 0; i < 200; ++i) {
		assert_double_equal(output4[i], ref_distances4[i]);
	}
	assert_true(scc_close_dist_column_object(tmp_dco4));


	scc_Vid* col5 = NULL;
	scc_Vid row5[1] = { 15 };
	double output5[100];
	double ref_distances5[100] = { 81.747845, 20.086538, 65.856761, 26.411570, 23.561895, 54.285899, 84.771271, 80.884683, 112.319962,
	                                     43.699940, 90.887926, 70.104053, 58.424566, 96.645475, 72.387324, 0.000000, 95.396451, 78.914345,
	                                     101.664933, 79.050218, 61.036678, 36.559903, 50.163704, 104.245732, 75.157336, 95.288270, 68.362965,
	                                     99.192765, 79.924004, 72.943678, 23.643384, 97.938708, 45.933384, 37.712186, 97.062955, 95.061226,
	                                     73.042551, 106.301904, 70.104053, 88.085451, 59.405346, 73.591962, 21.777506, 80.070839, 69.939208,
	                                     69.639193, 81.458834, 36.508160, 53.397240, 85.390351, 10.219802, 112.776687, 69.457342, 74.984984,
	                                     67.681343, 108.744849, 50.500860, 41.291009, 54.123760, 76.784635, 53.528564, 41.826771, 76.552296,
	                                     107.521222, 65.077630, 122.813377, 116.353634, 62.177638, 41.859877, 60.269301, 41.475091, 103.719673,
	                                     76.563669, 21.019934, 88.189064, 89.994046, 87.005917, 77.185367, 33.180906, 44.158321, 73.258443,
	                                     87.232747, 50.703717, 107.274405, 72.424998, 39.823449, 90.332805, 74.807777, 70.254873, 82.210163,
	                                     107.072656, 58.021162, 32.290715, 45.388989, 82.227376, 58.082855, 17.999359, 103.093322, 25.827839, 77.023313 };
	scc_DistColObject* tmp_dco5 = scc_init_dist_column_object(&scc_ut_test_data_large, 100, col5, 1);
	assert_non_null(tmp_dco5);
	assert_true(scc_get_dist_row(tmp_dco5, 1, row5, output5));
	for (size_t i = 0; i < 100; ++i) {
		assert_double_equal(output5[i], ref_distances5[i]);
	}
	assert_true(scc_close_dist_column_object(tmp_dco5));


	scc_Vid* col6 = NULL;
	scc_Vid* row6 = NULL;
	double output6[225];
	double ref_distances6[225] = { 0.00000000, 0.57346651, 1.69179534, 2.40739000, 0.97492432, 2.71036210, 1.30868297, 0.37813567, 0.08974655, 2.06566737,
	                                     1.38782555, 3.58315665, 1.32782897, 1.04971270, 1.27182896, 0.57346651, 0.00000000, 1.11832883, 1.83392349, 0.40145781,
	                                     2.13689559, 0.73521646, 0.19533084, 0.66321306, 1.49220086, 0.81435904, 3.00969014, 0.75436246, 0.47624619, 0.69836245,
	                                     1.69179534, 1.11832883, 0.00000000, 0.71559466, 0.71687102, 1.01856676, 0.38311237, 1.31365967, 1.78154189, 0.37387203,
	                                     0.30396979, 1.89136131, 0.36396637, 0.64208264, 0.41996638, 2.40739000, 1.83392349, 0.71559466, 0.00000000, 1.43246568,
	                                     0.30297210, 1.09870703, 2.02925433, 2.49713655, 0.34172263, 1.01956445, 1.17576665, 1.07956103, 1.35767730, 1.13556104,
	                                     0.97492432, 0.40145781, 0.71687102, 1.43246568, 0.00000000, 1.73543778, 0.33375865, 0.59678865, 1.06467087, 1.09074305,
	                                     0.41290123, 2.60823233, 0.35290465, 0.07478838, 0.29690464, 2.71036210, 2.13689559, 1.01856676, 0.30297210, 1.73543778,
	                                     0.00000000, 1.40167913, 2.33222643, 2.80010865, 0.64469473, 1.32253655, 0.87279455, 1.38253313, 1.66064940, 1.43853314,
	                                     1.30868297, 0.73521646, 0.38311237, 1.09870703, 0.33375865, 1.40167913, 0.00000000, 0.93054730, 1.39842952, 0.75698440,
	                                     0.07914258, 2.27447368, 0.01914600, 0.25897027, 0.03685401, 0.37813567, 0.19533084, 1.31365967, 2.02925433, 0.59678865,
	                                     2.33222643, 0.93054730, 0.00000000, 0.46788222, 1.68753170, 1.00968988, 3.20502098, 0.94969330, 0.67157703, 0.89369329,
	                                     0.08974655, 0.66321306, 1.78154189, 2.49713655, 1.06467087, 2.80010865, 1.39842952, 0.46788222, 0.00000000, 2.15541392,
	                                     1.47757210, 3.67290320, 1.41757552, 1.13945925, 1.36157551, 2.06566737, 1.49220086, 0.37387203, 0.34172263, 1.09074305,
	                                     0.64469473, 0.75698440, 1.68753170, 2.15541392, 0.00000000, 0.67784182, 1.51748928, 0.73783840, 1.01595467, 0.79383841,
	                                     1.38782555, 0.81435904, 0.30396979, 1.01956445, 0.41290123, 1.32253655, 0.07914258, 1.00968988, 1.47757210, 0.67784182,
	                                     0.00000000, 2.19533110, 0.05999658, 0.33811285, 0.11599659, 3.58315665, 3.00969014, 1.89136131, 1.17576665, 2.60823233,
	                                     0.87279455, 2.27447368, 3.20502098, 3.67290320, 1.51748928, 2.19533110, 0.00000000, 2.25532768, 2.53344395, 2.31132769,
	                                     1.32782897, 0.75436246, 0.36396637, 1.07956103, 0.35290465, 1.38253313, 0.01914600, 0.94969330, 1.41757552, 0.73783840,
	                                     0.05999658, 2.25532768, 0.00000000, 0.27811627, 0.05600001, 1.04971270, 0.47624619, 0.64208264, 1.35767730, 0.07478838,
	                                     1.66064940, 0.25897027, 0.67157703, 1.13945925, 1.01595467, 0.33811285, 2.53344395, 0.27811627, 0.00000000, 0.22211626,
	                                     1.27182896, 0.69836245, 0.41996638, 1.13556104, 0.29690464, 1.43853314, 0.03685401, 0.89369329, 1.36157551, 0.79383841,
	                                     0.11599659, 2.31132769, 0.05600001, 0.22211626, 0.00000000 };
	scc_DistColObject* tmp_dco6 = scc_init_dist_column_object(&scc_ut_test_data_small, 15, col6, 15);
	assert_non_null(tmp_dco6);
	assert_true(scc_get_dist_row(tmp_dco6, 15, row6, output6));
	for (size_t i = 0; i < 225; ++i) {
		assert_double_equal(output6[i], ref_distances6[i]);
	}
	assert_true(scc_close_dist_column_object(tmp_dco6));
}


void scc_ut_init_close_search_object(void** state)
{
	(void) state;

	scc_Vid search1[10] = { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18 };
	scc_Vid search2[5] = { 5, 2, 7, 3, 8 };

	scc_NNSearchObject* tmp_so1 = scc_init_nn_search_object(&scc_ut_test_data_large, 2, false, 0.0, 10, search1, 1);
	assert_non_null(tmp_so1);
	assert_true(scc_close_nn_search_object(tmp_so1));

	scc_NNSearchObject* tmp_so2 = scc_init_nn_search_object(&scc_ut_test_data_small, 2, false, 0.0, 5, search2, 1);
	assert_non_null(tmp_so2);
	assert_true(scc_close_nn_search_object(tmp_so2));

	scc_NNSearchObject* tmp_so3 = scc_init_nn_search_object(&scc_ut_test_data_large, 2, false, 0.0, 100, NULL, 10);
	assert_non_null(tmp_so3);
	assert_true(scc_close_nn_search_object(tmp_so3));

	scc_NNSearchObject* tmp_so4 = scc_init_nn_search_object(&scc_ut_test_data_small, 2, false, 0.0, 15, NULL, 10);
	assert_non_null(tmp_so4);
	assert_true(scc_close_nn_search_object(tmp_so4));

	scc_NNSearchObject* tmp_so5 = scc_init_nn_search_object(&scc_ut_test_data_large, 2, false, 0.0, 10, search1, 50);
	assert_non_null(tmp_so5);
	assert_true(scc_close_nn_search_object(tmp_so5));

	scc_NNSearchObject* tmp_so6 = scc_init_nn_search_object(&scc_ut_test_data_large, 1, false, 0.0, 10, search1, 50);
	assert_non_null(tmp_so6);
	assert_true(scc_close_nn_search_object(tmp_so6));

	scc_NNSearchObject* tmp_so7 = scc_init_nn_search_object(&scc_ut_test_data_large, 10, false, 0.0, 10, search1, 100);
	assert_non_null(tmp_so7);
	assert_true(scc_close_nn_search_object(tmp_so7));

	scc_NNSearchObject* tmp_so8 = scc_init_nn_search_object(&scc_ut_test_data_large, 2, true, 1.0, 10, search1, 100);
	assert_non_null(tmp_so8);
	assert_true(scc_close_nn_search_object(tmp_so8));

	scc_NNSearchObject* tmp_so9 = scc_init_nn_search_object(&scc_ut_test_data_large, 2, true, 1.0, 10, search1, 10);
	assert_non_null(tmp_so9);
	assert_true(scc_close_nn_search_object(tmp_so9));

	scc_NNSearchObject* tmp_so10 = scc_init_nn_search_object(&scc_ut_test_data_large, 100, false, 0.0, 100, NULL, 100);
	assert_non_null(tmp_so10);
	assert_true(scc_close_nn_search_object(tmp_so10));
}


void scc_ut_nearest_neighbor_search(void** state)
{
	(void) state;

	const scc_Vid M = SCC_VID_NA;

	scc_Vid search1[10] = { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18 };
	scc_Vid search2[5] = { 5, 2, 7, 3, 8 };
	scc_Vid queryA[1] = { 77 };
	scc_Vid queryB[6] = { 5, 8, 11, 23, 38, 88 };
	scc_Vid* queryC = NULL;
	scc_Vid queryD[2] = { 0, 6 };


	scc_NNSearchObject* tmp_so1 = scc_init_nn_search_object(&scc_ut_test_data_large, 2, false, 0.0, 10, search1, 107);
	assert_non_null(tmp_so1);

	scc_Vid out_indices1[2];
	scc_Vid ref_indices1[2] = { 0, 10 };
	assert_true(scc_nearest_neighbor_search(tmp_so1, 1, queryA, out_indices1, NULL));
	assert_memory_equal(out_indices1, ref_indices1, 2 * sizeof(scc_Vid));

	scc_Vid out_indices2[12];
	scc_Vid ref_indices2[12] = { 2, 14, 8, 10, 6, 12, 8, 10, 6, 12, 0, 14 };
	assert_true(scc_nearest_neighbor_search(tmp_so1, 6, queryB, out_indices2, NULL));
	assert_memory_equal(out_indices2, ref_indices2, 12 * sizeof(scc_Vid));

	scc_Vid out_indices3[200];
	scc_Vid ref_indices3[200] = { 0, 10, 4, 12, 2, 14, 4, 12, 4, 12, 2, 14, 6, 12, 6,
	                              12, 8, 10, 2, 4, 10, 0, 6, 12, 12, 6, 6, 12, 14, 0,
	                              4, 12, 16, 12, 0, 10, 18, 10, 14, 2, 14, 2, 4, 12,
	                              4, 12, 8, 10, 6, 18, 16, 8, 14, 0, 8, 16, 14, 0, 18,
	                              10, 4, 12, 16, 8, 2, 4, 4, 12, 16, 14, 16, 12, 14,
	                              16, 16, 14, 6, 12, 6, 12, 14, 0, 0, 10, 4, 12, 0, 10,
	                              0, 14, 18, 10, 6, 12, 4, 12, 12, 6, 14, 0, 4, 12, 8,
	                              16, 2, 14, 14, 0, 12, 6, 18, 6, 4, 14, 12, 4, 2, 14,
	                              6, 12, 12, 6, 4, 12, 0, 10, 8, 10, 0, 10, 8, 10, 16,
	                              14, 12, 14, 4, 0, 2, 14, 4, 12, 16, 14, 0, 10, 4, 2,
	                              14, 0, 6, 16, 6, 16, 0, 10, 4, 12, 4, 12, 2, 14, 18,
	                              10, 4, 12, 18, 6, 0, 10, 12, 4, 16, 6, 10, 0, 0, 14,
	                              0, 10, 8, 10, 14, 0, 12, 4, 4, 0, 18, 10, 12, 6, 4,
	                              12, 8, 0, 4, 14, 18, 10 };
	assert_true(scc_nearest_neighbor_search(tmp_so1, 100, queryC, out_indices3, NULL));
	assert_memory_equal(out_indices3, ref_indices3, 200 * sizeof(scc_Vid));
	
	assert_true(scc_close_nn_search_object(tmp_so1));


	scc_NNSearchObject* tmp_so2 = scc_init_nn_search_object(&scc_ut_test_data_small, 2, false, 0.0, 5, search2, 17);
	assert_non_null(tmp_so2);

	scc_Vid out_indices4[4];
	scc_Vid ref_indices4[4] = { 8, 7, 2, 7 };
	assert_true(scc_nearest_neighbor_search(tmp_so2, 2, queryD, out_indices4, NULL));
	assert_memory_equal(out_indices4, ref_indices4, 2 * sizeof(scc_Vid));

	scc_Vid out_indices5[30];
	scc_Vid ref_indices5[30] = { 8, 7, 7, 8, 2, 3, 3, 5, 7, 2, 5, 3, 2, 7, 7, 8, 8, 7, 3, 2, 2, 7, 5, 3, 2, 7, 2, 7, 2, 7 };
	assert_true(scc_nearest_neighbor_search(tmp_so2, 15, queryC, out_indices5, NULL));
	assert_memory_equal(out_indices5, ref_indices5, 30 * sizeof(scc_Vid));

	assert_true(scc_close_nn_search_object(tmp_so2));


	scc_NNSearchObject* tmp_so3 = scc_init_nn_search_object(&scc_ut_test_data_large, 2, false, 0.0, 100, NULL, 107);
	assert_non_null(tmp_so3);

	scc_Vid out_indices6[2];
	scc_Vid ref_indices6[2] = { 77, 88 };
	assert_true(scc_nearest_neighbor_search(tmp_so3, 1, queryA, out_indices6, NULL));
	assert_memory_equal(out_indices6, ref_indices6, 2 * sizeof(scc_Vid));

	scc_Vid out_indices7[12];
	scc_Vid ref_indices7[12] = { 5, 58, 8, 63, 0, 0, 23, 83, 0, 0, 88, 77 };
	assert_true(scc_nearest_neighbor_search(tmp_so3, 6, queryB, out_indices7, NULL));
	assert_true((out_indices7[4] == 11 && out_indices7[5] == 38) || (out_indices7[4] == 38 && out_indices7[5] == 11));
	assert_true((out_indices7[8] == 11 && out_indices7[9] == 38) || (out_indices7[8] == 38 && out_indices7[9] == 11));
	out_indices7[4] = out_indices7[5] = out_indices7[8] = out_indices7[9] = 0;
	assert_memory_equal(out_indices7, ref_indices7, 12 * sizeof(scc_Vid));

	scc_Vid out_indices8[200];
	scc_Vid ref_indices8[200] = { 0, 10, 1, 96, 2, 69, 3, 4, 4, 30, 5, 58, 6, 59, 7, 46, 8, 63, 9, 69,
	                              10, 0, 0, 0, 12, 95, 13, 39, 14, 36, 15, 50, 16, 86, 17, 88, 18, 55,
	                              19, 36, 20, 5, 21, 85, 22, 47, 23, 83, 24, 7, 25, 27, 26, 28, 27, 25,
	                              28, 49, 29, 99, 30, 4, 31, 34, 32, 58, 33, 70, 34, 37, 35, 86, 36, 19,
	                              37, 34, 0, 0, 39, 13, 40, 64, 41, 72, 42, 50, 43, 89, 44, 91, 45, 29,
	                              46, 7, 47, 22, 48, 60, 49, 28, 50, 15, 51, 31, 52, 58, 53, 40, 54, 11,
	                              55, 18, 56, 26, 57, 70, 58, 32, 59, 6, 60, 48, 61, 79, 62, 64, 63, 8,
	                              64, 40, 65, 8, 66, 71, 67, 9, 68, 3, 69, 2, 70, 85, 71, 66, 72, 41, 73,
	                              1, 74, 77, 75, 76, 76, 75, 77, 88, 78, 92, 79, 61, 80, 69, 81, 94, 82,
	                              22, 83, 23, 84, 72, 85, 21, 86, 75, 87, 94, 88, 77, 89, 43, 90, 63, 91,
	                              44, 92, 78, 93, 91, 94, 81, 95, 12, 96, 1, 97, 63, 98, 30, 99, 29 };
	assert_true(scc_nearest_neighbor_search(tmp_so3, 100, queryC, out_indices8, NULL));
	assert_true((out_indices8[22] == 11 && out_indices8[23] == 38) || (out_indices8[22] == 38 && out_indices8[23] == 11));
	assert_true((out_indices8[76] == 11 && out_indices8[77] == 38) || (out_indices8[76] == 38 && out_indices8[77] == 11));
	out_indices8[22] = out_indices8[23] = out_indices8[76] = out_indices8[77] = 0;
	assert_memory_equal(out_indices8, ref_indices8, 200 * sizeof(scc_Vid));

	assert_true(scc_close_nn_search_object(tmp_so3));


	scc_NNSearchObject* tmp_so4 = scc_init_nn_search_object(&scc_ut_test_data_small, 2, false, 0.0, 15, NULL, 17);
	assert_non_null(tmp_so4);

	scc_Vid out_indices9[4];
	scc_Vid ref_indices9[4] = { 0, 8, 6, 12 };
	assert_true(scc_nearest_neighbor_search(tmp_so4, 2, queryD, out_indices9, NULL));
	assert_memory_equal(out_indices9, ref_indices9, 2 * sizeof(scc_Vid));

	scc_Vid out_indices10[30];
	scc_Vid ref_indices10[30] = { 0, 8, 1, 7, 2, 10, 3, 5, 4, 13, 5, 3, 6, 12, 7, 1, 8, 0, 9, 3, 10, 12, 11, 5, 12, 6, 13, 4, 14, 6 };
	assert_true(scc_nearest_neighbor_search(tmp_so4, 15, queryC, out_indices10, NULL));
	assert_memory_equal(out_indices10, ref_indices10, 30 * sizeof(scc_Vid));

	assert_true(scc_close_nn_search_object(tmp_so4));


	scc_NNSearchObject* tmp_so5 = scc_init_nn_search_object(&scc_ut_test_data_large, 2, false, 0.0, 10, search1, 107);
	assert_non_null(tmp_so5);

	scc_Vid out_indices11[2];
	double out_dist11[2];
	scc_Vid ref_indices11[2] = { 0, 10 };
	double ref_dist11[2] = { 39.903422, 50.631589 };
	assert_true(scc_nearest_neighbor_search(tmp_so5, 1, queryA, out_indices11, out_dist11));
	assert_memory_equal(out_indices11, ref_indices11, 2 * sizeof(scc_Vid));
	for (size_t i = 0; i < 2; ++i) {
		assert_double_equal(out_dist11[i], ref_dist11[i]);
	}

	scc_Vid out_indices12[12];
	double out_dist12[12];
	scc_Vid ref_indices12[12] = { 2, 14, 8, 10, 6, 12, 8, 10, 6, 12, 0, 14 };
	double ref_dist12[12] = { 21.423179, 32.943949, 0.000000, 35.180283, 18.684657, 22.039685, 39.168896, 43.364387, 18.684657, 22.039685, 34.566185, 43.806750 };
	assert_true(scc_nearest_neighbor_search(tmp_so5, 6, queryB, out_indices12, out_dist12));
	assert_memory_equal(out_indices12, ref_indices12, 12 * sizeof(scc_Vid));
	for (size_t i = 0; i < 12; ++i) {
		assert_double_equal(out_dist12[i], ref_dist12[i]);
	}

	scc_Vid out_indices13[200];
	double out_dist13[200];
	scc_Vid ref_indices13[200] = { 0, 10, 4, 12, 2, 14, 4, 12, 4, 12, 2, 14, 6, 12, 6,
	                              12, 8, 10, 2, 4, 10, 0, 6, 12, 12, 6, 6, 12, 14, 0,
	                              4, 12, 16, 12, 0, 10, 18, 10, 14, 2, 14, 2, 4, 12,
	                              4, 12, 8, 10, 6, 18, 16, 8, 14, 0, 8, 16, 14, 0, 18,
	                              10, 4, 12, 16, 8, 2, 4, 4, 12, 16, 14, 16, 12, 14,
	                              16, 16, 14, 6, 12, 6, 12, 14, 0, 0, 10, 4, 12, 0, 10,
	                              0, 14, 18, 10, 6, 12, 4, 12, 12, 6, 14, 0, 4, 12, 8,
	                              16, 2, 14, 14, 0, 12, 6, 18, 6, 4, 14, 12, 4, 2, 14,
	                              6, 12, 12, 6, 4, 12, 0, 10, 8, 10, 0, 10, 8, 10, 16,
	                              14, 12, 14, 4, 0, 2, 14, 4, 12, 16, 14, 0, 10, 4, 2,
	                              14, 0, 6, 16, 6, 16, 0, 10, 4, 12, 4, 12, 2, 14, 18,
	                              10, 4, 12, 18, 6, 0, 10, 12, 4, 16, 6, 10, 0, 0, 14,
	                              0, 10, 8, 10, 14, 0, 12, 4, 4, 0, 18, 10, 12, 6, 4,
	                              12, 8, 0, 4, 14, 18, 10 };
	double ref_dist13[200] = { 0.000000, 12.578183, 33.024108, 48.434361, 0.000000, 48.931105, 7.608964, 46.933896, 0.000000, 47.737270,
	                                 21.423179, 32.943949, 0.000000, 30.550623, 21.866414, 31.130135, 0.000000, 35.180283, 30.522603, 40.308101,
	                                 0.000000, 12.578183, 18.684657, 22.039685, 0.000000, 30.550623, 22.219396, 45.405270, 0.000000, 47.339958,
	                                 23.561895, 58.424566, 0.000000, 43.831474, 24.527035, 35.335267, 0.000000, 54.646819, 18.431271, 45.262949,
	                                 21.190204, 29.611634, 33.920078, 38.415255, 29.582780, 29.997095, 39.168896, 43.364387, 29.955307, 36.830203,
	                                 23.791490, 39.585361, 30.771203, 41.581880, 29.158231, 34.089378, 27.771364, 39.946261, 30.758911, 49.830922,
	                                 2.010769, 46.210332, 31.306292, 35.081781, 21.286702, 45.135236, 36.443663, 45.548182, 21.726567, 42.429327,
	                                 26.653233, 40.142190, 10.686953, 47.391949, 31.987010, 45.222646, 18.684657, 22.039685, 19.948286, 36.312019,
	                                 26.925880, 32.328136, 42.347823, 46.057002, 18.944803, 37.237242, 31.822439, 39.934435, 21.942641, 29.912522,
	                                 40.720149, 54.063776, 20.063779, 29.114271, 15.993187, 34.856189, 16.438015, 33.716656, 27.567111, 35.706998,
	                                 19.939323, 49.391571, 26.025619, 43.627458, 34.780913, 41.185593, 19.390641, 31.630367, 18.807052, 20.956306,
	                                 12.937975, 54.368097, 37.959402, 39.745129, 34.391264, 35.170604, 19.831688, 46.474031, 8.341254, 22.374291,
	                                 25.836258, 37.609953, 31.481404, 32.856776, 16.011269, 22.165950, 4.869337, 32.251695, 25.057460, 31.886019,
	                                 17.042869, 44.416108, 32.778322, 61.931759, 34.107953, 38.719082, 20.106185, 41.865955, 12.975601, 39.336023,
	                                 37.962997, 39.284424, 34.142516, 52.350483, 42.144499, 46.094768, 33.075406, 49.906460, 49.653574, 55.508707,
	                                 20.224226, 27.361649, 19.687715, 27.758752, 39.903422, 50.631589, 46.774989, 50.437222, 34.068817, 37.582733,
	                                 24.953439, 36.085626, 42.069238, 52.137810, 31.108697, 32.618912, 37.560930, 44.985378, 33.292160, 38.608404,
	                                 37.311000, 37.723173, 17.022913, 27.046145, 42.215552, 42.441327, 34.566185, 43.806750, 43.347855, 51.266321,
	                                 14.124146, 35.278830, 29.496145, 31.665100, 37.315241, 39.194038, 28.445524, 46.498317, 49.644646, 51.288208,
	                                 9.703853, 27.688373, 34.613876, 52.081618, 16.446061, 32.841853, 12.494481, 46.667781, 28.780340, 38.552895 };
	assert_true(scc_nearest_neighbor_search(tmp_so5, 100, queryC, out_indices13, out_dist13));
	assert_memory_equal(out_indices13, ref_indices13, 200 * sizeof(scc_Vid));
	for (size_t i = 0; i < 200; ++i) {
		assert_double_equal(out_dist13[i], ref_dist13[i]);
	}

	assert_true(scc_close_nn_search_object(tmp_so5));


	scc_NNSearchObject* tmp_so6 = scc_init_nn_search_object(&scc_ut_test_data_large, 1, false, 0.0, 10, search1, 107);
	assert_non_null(tmp_so6);

	scc_Vid out_indices14[1];
	scc_Vid ref_indices14[1] = { 0 };
	assert_true(scc_nearest_neighbor_search(tmp_so6, 1, queryA, out_indices14, NULL));
	assert_memory_equal(out_indices14, ref_indices14, 1 * sizeof(scc_Vid));

	scc_Vid out_indices15[6];
	scc_Vid ref_indices15[6] = { 2, 8, 6, 8, 6, 0 };
	assert_true(scc_nearest_neighbor_search(tmp_so6, 6, queryB, out_indices15, NULL));
	assert_memory_equal(out_indices15, ref_indices15, 6 * sizeof(scc_Vid));

	scc_Vid out_indices16[100];
	scc_Vid ref_indices16[100] = { 0, 4, 2, 4, 4, 2, 6, 6, 8, 2, 10, 6, 12, 6, 14, 4, 16, 0, 18, 14, 14, 4,
	                              4, 8, 6, 16, 14, 8, 14, 18, 4, 16, 2, 4, 16, 16, 14, 16, 6, 6, 14, 0, 4,
	                              0, 0, 18, 6, 4, 12, 14, 4, 8, 2, 14, 12, 18, 4, 12, 2, 6, 12, 4, 0, 8, 0,
	                              8, 16, 12, 4, 2, 4, 16, 0, 4, 14, 6, 6, 0, 4, 4, 2, 18, 4, 18, 0, 12, 16,
	                              10, 0, 0, 8, 14, 12, 4, 18, 12, 4, 8, 4, 18 };
	assert_true(scc_nearest_neighbor_search(tmp_so6, 100, queryC, out_indices16, NULL));
	assert_memory_equal(out_indices16, ref_indices16, 100 * sizeof(scc_Vid));

	assert_true(scc_close_nn_search_object(tmp_so6));


	scc_NNSearchObject* tmp_so7 = scc_init_nn_search_object(&scc_ut_test_data_large, 10, false, 0.0, 10, search1, 107);
	assert_non_null(tmp_so7);

	scc_Vid out_indices17[10];
	scc_Vid ref_indices17[10] = { 0, 10, 14, 4, 8, 2, 12, 18, 16, 6 };
	assert_true(scc_nearest_neighbor_search(tmp_so7, 1, queryA, out_indices17, NULL));
	assert_memory_equal(out_indices17, ref_indices17, 10 * sizeof(scc_Vid));

	scc_Vid out_indices18[60];
	scc_Vid ref_indices18[60] = { 2, 14, 4, 12, 0, 16, 10, 8, 6, 18, 8, 10, 0, 14, 16, 18, 6, 12,
	                              4, 2, 6, 12, 16, 4, 18, 0, 14, 10, 8, 2, 8, 10, 16, 18, 6, 0,
	                              12, 14, 4, 2, 6, 12, 16, 4, 18, 0, 14, 10, 8, 2, 0, 14, 10, 4,
	                              8, 2, 12, 16, 18, 6 };
	assert_true(scc_nearest_neighbor_search(tmp_so7, 6, queryB, out_indices18, NULL));
	assert_memory_equal(out_indices18, ref_indices18, 60 * sizeof(scc_Vid));

	scc_Vid out_indices19[1000];
	scc_Vid ref_indices19[1000] = { 0, 10, 8, 14, 4, 18, 12, 16, 6, 2, 4, 12, 2, 14, 6, 16, 0, 10, 18,
	                              8, 2, 14, 4, 12, 16, 0, 10, 6, 8, 18, 4, 12, 0, 14, 10, 6, 2, 18,
	                              16, 8, 4, 12, 14, 0, 2, 10, 6, 16, 18, 8, 2, 14, 4, 12, 0, 16, 10,
	                              8, 6, 18, 6, 12, 16, 18, 4, 10, 0, 14, 8, 2, 6, 12, 16, 18, 10, 0,
	                              8, 14, 4, 2, 8, 10, 0, 14, 16, 18, 6, 12, 4, 2, 2, 4, 14, 12, 16,
	                              6, 0, 10, 8, 18, 10, 0, 8, 18, 14, 4, 16, 12, 6, 2, 6, 12, 16, 4,
	                              18, 0, 14, 10, 8, 2, 12, 6, 16, 4, 14, 0, 18, 2, 10, 8, 6, 12, 16,
	                              18, 4, 10, 0, 14, 8, 2, 14, 0, 2, 4, 8, 16, 10, 12, 6, 18, 4, 12,
	                              2, 14, 0, 6, 10, 16, 18, 8, 16, 12, 6, 14, 8, 0, 10, 4, 2, 18, 0,
	                              10, 14, 8, 4, 12, 2, 18, 16, 6, 18, 10, 6, 0, 12, 8, 4, 16, 14, 2,
	                              14, 2, 16, 12, 0, 8, 4, 10, 6, 18, 14, 2, 4, 12, 16, 0, 10, 8, 6,
	                              18, 4, 12, 6, 18, 0, 14, 10, 16, 2, 8, 4, 12, 14, 0, 6, 10, 16, 8,
	                              2, 18, 8, 10, 16, 18, 6, 0, 12, 14, 4, 2, 6, 18, 12, 10, 0, 16, 4,
	                              8, 14, 2, 16, 8, 14, 6, 12, 0, 10, 18, 4, 2, 14, 0, 4, 10, 2, 8, 12,
	                              16, 6, 18, 8, 16, 10, 0, 14, 6, 12, 18, 4, 2, 14, 0, 10, 8, 4, 2, 16,
	                              12, 6, 18, 18, 10, 0, 4, 6, 12, 8, 14, 16, 2, 4, 12, 14, 0, 2, 10, 6,
	                              16, 18, 8, 16, 8, 14, 0, 10, 12, 6, 4, 2, 18, 2, 4, 14, 12, 0, 16, 10,
	                              6, 8, 18, 4, 12, 6, 18, 0, 10, 14, 16, 2, 8, 16, 14, 8, 12, 6, 0, 10,
	                              2, 4, 18, 16, 12, 6, 14, 4, 2, 8, 0, 10, 18, 14, 16, 2, 0, 12, 4, 8,
	                              10, 6, 18, 16, 14, 8, 12, 0, 10, 6, 2, 4, 18, 6, 12, 16, 4, 18, 0, 14,
	                              10, 8, 2, 6, 12, 16, 18, 4, 14, 10, 0, 8, 2, 14, 0, 4, 10, 12, 16, 8,
	                              6, 2, 18, 0, 10, 4, 18, 14, 8, 12, 6, 16, 2, 4, 12, 14, 2, 6, 0, 16,
	                              10, 18, 8, 0, 10, 14, 4, 8, 18, 12, 2, 16, 6, 0, 14, 10, 4, 8, 12, 16,
	                              2, 18, 6, 18, 10, 0, 4, 12, 6, 8, 14, 16, 2, 6, 12, 16, 18, 10, 0, 14,
	                              8, 4, 2, 4, 12, 14, 0, 6, 10, 2, 16, 18, 8, 12, 6, 4, 16, 18, 14, 0,
	                              10, 2, 8, 14, 0, 10, 8, 4, 2, 16, 12, 6, 18, 4, 12, 2, 14, 6, 0, 10,
	                              16, 18, 8, 8, 16, 14, 10, 0, 6, 12, 18, 4, 2, 2, 14, 4, 0, 12, 10, 8,
	                              16, 6, 18, 14, 0, 10, 8, 16, 4, 12, 2, 6, 18, 12, 6, 16, 4, 18, 14, 0,
	                              10, 2, 8, 18, 6, 10, 0, 8, 12, 16, 4, 14, 2, 4, 14, 2, 0, 10, 12, 8,
	                              16, 6, 18, 12, 4, 6, 18, 0, 16, 10, 14, 2, 8, 2, 14, 4, 12, 0, 16, 10,
	                              8, 6, 18, 6, 12, 16, 18, 4, 0, 10, 14, 8, 2, 12, 6, 4, 16, 18, 0, 14,
	                              10, 2, 8, 4, 12, 6, 18, 0, 10, 14, 16, 2, 8, 0, 10, 8, 14, 4, 16, 12,
	                              18, 6, 2, 8, 10, 0, 14, 16, 18, 6, 12, 4, 2, 0, 10, 14, 4, 12, 8, 16,
	                              6, 18, 2, 8, 10, 0, 14, 16, 18, 12, 6, 4, 2, 16, 14, 8, 12, 6, 2, 0,
	                              10, 4, 18, 12, 14, 16, 2, 4, 6, 0, 10, 8, 18, 4, 0, 10, 12, 14, 18,
	                              6, 8, 2, 16, 2, 14, 4, 12, 16, 0, 6, 10, 8, 18, 4, 12, 6, 18, 0, 10,
	                              16, 14, 2, 8, 16, 14, 12, 2, 8, 6, 0, 4, 10, 18, 0, 10, 4, 18, 14, 8,
	                              12, 6, 2, 16, 4, 2, 12, 14, 6, 16, 0, 10, 18, 8, 14, 0, 10, 4, 8, 2,
	                              12, 16, 18, 6, 6, 16, 12, 18, 8, 10, 0, 14, 4, 2, 6, 16, 12, 18, 10,
	                              8, 0, 14, 4, 2, 0, 10, 14, 4, 8, 2, 12, 18, 16, 6, 4, 12, 6, 2, 14,
	                              16, 0, 18, 10, 8, 4, 12, 6, 18, 0, 10, 14, 16, 2, 8, 2, 14, 16, 12,
	                              4, 0, 6, 8, 10, 18, 18, 10, 0, 4, 6, 8, 12, 14, 16, 2, 4, 12, 0, 6,
	                              10, 14, 18, 16, 8, 2, 18, 6, 10, 8, 16, 0, 12, 14, 4, 2, 0, 10, 4,
	                              14, 18, 8, 12, 6, 16, 2, 12, 4, 6, 18, 0, 16, 14, 10, 2, 8, 16, 6,
	                              12, 14, 8, 0, 10, 18, 4, 2, 10, 0, 18, 4, 8, 12, 14, 6, 16, 2, 0,
	                              14, 10, 4, 8, 2, 12, 16, 18, 6, 0, 10, 4, 14, 8, 18, 2, 12, 16, 6,
	                              8, 10, 0, 16, 14, 18, 6, 12, 4, 2, 14, 0, 4, 10, 8, 12, 2, 16, 6,
	                              18, 12, 4, 6, 2, 14, 16, 0, 18, 10, 8, 4, 0, 14, 10, 2, 12, 8, 18,
	                              6, 16, 18, 10, 0, 4, 8, 12, 6, 14, 16, 2, 12, 6, 4, 16, 14, 0, 18,
	                              10, 8, 2, 4, 12, 2, 14, 6, 16, 0, 10, 18, 8, 8, 0, 10, 14, 16, 4,
	                              12, 18, 6, 2, 4, 14, 12, 2, 0, 10, 6, 16, 8, 18, 18, 10, 0, 4, 6,
	                              12, 8, 14, 16, 2 };
	assert_true(scc_nearest_neighbor_search(tmp_so7, 100, queryC, out_indices19, NULL));
	assert_memory_equal(out_indices19, ref_indices19, 1000 * sizeof(scc_Vid));

	assert_true(scc_close_nn_search_object(tmp_so7));


	scc_NNSearchObject* tmp_so8 = scc_init_nn_search_object(&scc_ut_test_data_large, 2, true, 30.0, 10, search1, 107);
	assert_non_null(tmp_so8);

	scc_Vid out_indices20[2];
	scc_Vid ref_indices20[2] = { M, M };
	assert_true(scc_nearest_neighbor_search(tmp_so8, 1, queryA, out_indices20, NULL));
	assert_memory_equal(out_indices20, ref_indices20, 2 * sizeof(scc_Vid));

	scc_Vid out_indices21[12];
	scc_Vid ref_indices21[12] = { 2, M, 8, M, 6, 12, M, M, 6, 12, M, M };
	assert_true(scc_nearest_neighbor_search(tmp_so8, 6, queryB, out_indices21, NULL));
	assert_memory_equal(out_indices21, ref_indices21, 12 * sizeof(scc_Vid));

	scc_Vid out_indices22[200];
	scc_Vid ref_indices22[200] = { 0, 10, M, M, 2, M, 4, M, 4, M, 2, M, 6, M, 6, M, 8, M, M, M, 10, 0,
	                              6, 12, 12, M, 6, M, 14, M, 4, M, 16, M, 0, M, 18, M, 14, M, 14, 2,
	                              M, M, 4, 12, M, M, 6, M, 16, M, M, M, 8, M, 14, M, M, M, 4, M, M, M,
	                              2, M, M, M, 16, M, 16, M, 14, M, M, M, 6, 12, 6, M, 14, M, M, M, 4,
	                              M, M, M, 0, 14, M, M, 6, 12, 4, M, 12, M, 14, M, 4, M, 8, M, M, M,
	                              14, M, 12, 6, 18, M, M, M, M, M, 2, M, 6, 12, 12, M, M, M, 0, 10,
	                              8, M, 0, M, 8, M, M, M, M, M, 4, M, 2, M, M, M, M, M, M, M, M, M,
	                              M, M, 6, 16, 6, 16, M, M, M, M, M, M, 2, M, M, M, M, M, M, M, M, M,
	                              M, M, 16, 6, M, M, M, M, M, M, 8, M, 14, M, M, M, 4, M, M, M, 12,
	                              6, M, M, 8, M, 4, M, 18, M };
	assert_true(scc_nearest_neighbor_search(tmp_so8, 100, queryC, out_indices22, NULL));
	assert_memory_equal(out_indices22, ref_indices22, 200 * sizeof(scc_Vid));

	assert_true(scc_close_nn_search_object(tmp_so8));


	scc_NNSearchObject* tmp_so9 = scc_init_nn_search_object(&scc_ut_test_data_large, 2, true, 40.0, 10, search1, 107);
	assert_non_null(tmp_so9);

	scc_Vid out_indices23[2];
	double out_dist23[2];
	scc_Vid ref_indices23[2] = { 0, M };
	double ref_dist23[2] = { 39.903422, -1.0 };
	assert_true(scc_nearest_neighbor_search(tmp_so9, 1, queryA, out_indices23, out_dist23));
	assert_memory_equal(out_indices23, ref_indices23, 2 * sizeof(scc_Vid));
	for (size_t i = 0; i < 2; ++i) {
		assert_double_equal(out_dist23[i], ref_dist23[i]);
	}

	scc_Vid out_indices24[12];
	double out_dist24[12];
	scc_Vid ref_indices24[12] = { 2, 14, 8, 10, 6, 12, 8, M, 6, 12, 0, M };
	double ref_dist24[12] = { 21.423179, 32.943949, 0.000000, 35.180283, 18.684657, 22.039685, 39.168896, -1.000000, 18.684657, 22.039685, 34.566185, -1.000000 };
	assert_true(scc_nearest_neighbor_search(tmp_so9, 6, queryB, out_indices24, out_dist24));
	assert_memory_equal(out_indices24, ref_indices24, 12 * sizeof(scc_Vid));
	for (size_t i = 0; i < 12; ++i) {
		assert_double_equal(out_dist24[i], ref_dist24[i]);
	}

	scc_Vid out_indices25[200];
	double out_dist25[200];
	scc_Vid ref_indices25[200] = { 0, 10, 4, M, 2, M, 4, M, 4, M, 2, 14, 6, 12, 6, 12, 8, 10, 2,
	                               M, 10, 0, 6, 12, 12, 6, 6, M, 14, M, 4, M, 16, M, 0, 10, 18,
	                               M, 14, M, 14, 2, 4, 12, 4, 12, 8, M, 6, 18, 16, 8, 14, M, 8,
	                               16, 14, 0, 18, M, 4, M, 16, 8, 2, M, 4, M, 16, M, 16, M, 14,
	                               M, 16, M, 6, 12, 6, 12, 14, 0, M, M, 4, 12, 0, 10, 0, 14, M,
	                               M, 6, 12, 4, 12, 12, 6, 14, 0, 4, M, 8, M, 2, M, 14, 0, 12,
	                               6, 18, M, 4, 14, 12, 4, 2, M, 6, 12, 12, 6, 4, 12, 0, 10, 8,
	                               10, 0, 10, 8, M, 16, M, 12, 14, 4, M, 2, 14, 4, 12, 16, M, M,
	                               M, 4, M, M, M, 6, 16, 6, 16, 0, M, M, M, 4, 12, 2, 14, M, M,
	                               4, 12, 18, M, 0, 10, 12, 4, 16, 6, M, M, 0, M, M, M, 8, 10,
	                               14, 0, 12, 4, 4, M, M, M, 12, 6, 4, M, 8, 0, 4, M, 18, 10 };
	double ref_dist25[200] = { 0.000000, 12.578183, 33.024108, -1.000000, 0.000000, -1.000000, 7.608964, -1.000000, 0.000000, -1.000000,
	                                 21.423179, 32.943949, 0.000000, 30.550623, 21.866414, 31.130135, 0.000000, 35.180283, 30.522603, -1.000000,
	                                 0.000000, 12.578183, 18.684657, 22.039685, 0.000000, 30.550623, 22.219396, -1.000000, 0.000000, -1.000000,
	                                 23.561895, -1.000000, 0.000000, -1.000000, 24.527035, 35.335267, 0.000000, -1.000000, 18.431271, -1.000000,
	                                 21.190204, 29.611634, 33.920078, 38.415255, 29.582780, 29.997095, 39.168896, -1.000000, 29.955307, 36.830203,
	                                 23.791490, 39.585361, 30.771203, -1.000000, 29.158231, 34.089378, 27.771364, 39.946261, 30.758911, -1.000000,
	                                 2.010769, -1.000000, 31.306292, 35.081781, 21.286702, -1.000000, 36.443663, -1.000000, 21.726567, -1.000000,
	                                 26.653233, -1.000000, 10.686953, -1.000000, 31.987010, -1.000000, 18.684657, 22.039685, 19.948286, 36.312019,
	                                 26.925880, 32.328136, -1.000000, -1.000000, 18.944803, 37.237242, 31.822439, 39.934435, 21.942641, 29.912522,
	                                 -1.000000, -1.000000, 20.063779, 29.114271, 15.993187, 34.856189, 16.438015, 33.716656, 27.567111, 35.706998,
	                                 19.939323, -1.000000, 26.025619, -1.000000, 34.780913, -1.000000, 19.390641, 31.630367, 18.807052, 20.956306,
	                                 12.937975, -1.000000, 37.959402, 39.745129, 34.391264, 35.170604, 19.831688, -1.000000, 8.341254, 22.374291,
	                                 25.836258, 37.609953, 31.481404, 32.856776, 16.011269, 22.165950, 4.869337, 32.251695, 25.057460, 31.886019,
	                                 17.042869, -1.000000, 32.778322, -1.000000, 34.107953, 38.719082, 20.106185, -1.000000, 12.975601, 39.336023,
	                                 37.962997, 39.284424, 34.142516, -1.000000, -1.000000, -1.000000, 33.075406, -1.000000, -1.000000, -1.000000,
	                                 20.224226, 27.361649, 19.687715, 27.758752, 39.903422, -1.000000, -1.000000, -1.000000, 34.068817, 37.582733,
	                                 24.953439, 36.085626, -1.000000, -1.000000, 31.108697, 32.618912, 37.560930, -1.000000, 33.292160, 38.608404,
	                                 37.311000, 37.723173, 17.022913, 27.046145, -1.000000, -1.000000, 34.566185, -1.000000, -1.000000, -1.000000,
	                                 14.124146, 35.278830, 29.496145, 31.665100, 37.315241, 39.194038, 28.445524, -1.000000, -1.000000, -1.000000,
	                                 9.703853, 27.688373, 34.613876, -1.000000, 16.446061, 32.841853, 12.494481, -1.000000, 28.780340, 38.552895 };
	assert_true(scc_nearest_neighbor_search(tmp_so9, 100, queryC, out_indices25, out_dist25));
	assert_memory_equal(out_indices25, ref_indices25, 200 * sizeof(scc_Vid));
	for (size_t i = 0; i < 200; ++i) {
		assert_double_equal(out_dist25[i], ref_dist25[i]);
	}

	assert_true(scc_close_nn_search_object(tmp_so9));


	scc_NNSearchObject* tmp_so10 = scc_init_nn_search_object(&scc_ut_test_data_small, 15, false, 0.0, 15, NULL, 17);
	assert_non_null(tmp_so10);

	scc_Vid out_indices26[30];
	scc_Vid ref_indices26[30] = { 0, 8, 7, 1, 4, 13, 14, 6, 12, 10,
	                              2, 9, 3, 5, 11, 6, 12, 14, 10, 13,
	                              4, 2, 1, 9, 7, 3, 0, 8, 5, 11 };
	assert_true(scc_nearest_neighbor_search(tmp_so10, 2, queryD, out_indices26, NULL));
	assert_memory_equal(out_indices26, ref_indices26, 30 * sizeof(scc_Vid));

	scc_Vid out_indices27[225];
	scc_Vid ref_indices27[225] = { 0, 8, 7, 1, 4, 13, 14, 6, 12, 10, 2, 9, 3, 5, 11, 1, 7,
	                               4, 13, 0, 8, 14, 6, 12, 10, 2, 9, 3, 5, 11, 2, 10, 12,
	                               9, 6, 14, 13, 3, 4, 5, 1, 7, 0, 8, 11, 3, 5, 9, 2, 10,
	                               12, 6, 14, 11, 13, 4, 1, 7, 0, 8, 4, 13, 14, 6, 12, 1,
	                               10, 7, 2, 0, 8, 9, 3, 5, 11, 5, 3, 9, 11, 2, 10, 12, 6,
	                               14, 13, 4, 1, 7, 0, 8, 6, 12, 14, 10, 13, 4, 2, 1, 9, 7,
	                               3, 0, 8, 5, 11, 7, 1, 0, 8, 4, 13, 14, 6, 12, 10, 2, 9,
	                               3, 5, 11, 8, 0, 7, 1, 4, 13, 14, 6, 12, 10, 2, 9, 3, 5,
	                               11, 9, 3, 2, 5, 10, 12, 6, 14, 13, 4, 1, 11, 7, 0, 8,
	                               10, 12, 6, 14, 2, 13, 4, 9, 1, 7, 3, 5, 0, 8, 11, 11,
	                               5, 3, 9, 2, 10, 12, 6, 14, 13, 4, 1, 7, 0, 8, 12, 6,
	                               14, 10, 13, 4, 2, 9, 1, 7, 3, 0, 5, 8, 11, 13, 4, 14,
	                               6, 12, 10, 1, 2, 7, 9, 0, 8, 3, 5, 11, 14, 6, 12, 10,
	                               13, 4, 2, 1, 9, 7, 3, 0, 8, 5, 11 };
	assert_true(scc_nearest_neighbor_search(tmp_so10, 15, queryC, out_indices27, NULL));
	assert_memory_equal(out_indices27, ref_indices27, 225 * sizeof(scc_Vid));

	assert_true(scc_close_nn_search_object(tmp_so10));
}


int main(void)
{
	const struct CMUnitTest test_nn_search[] = {
		cmocka_unit_test(scc_ut_get_data_point_count),
		cmocka_unit_test(scc_ut_get_dist_matrix),
		cmocka_unit_test(scc_ut_init_close_dist_column_object),
		cmocka_unit_test(scc_ut_get_dist_row),
		cmocka_unit_test(scc_ut_init_close_max_dist_object),
		cmocka_unit_test(scc_ut_get_max_dist),
		cmocka_unit_test(scc_ut_init_close_search_object),
		cmocka_unit_test(scc_ut_nearest_neighbor_search),
	};

	return cmocka_run_group_tests_name("nearest neighbor search module", test_nn_search, NULL, NULL);
}
