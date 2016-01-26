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


#include "test_suite.h"
 
#include "../src/greedy.c"


void scc_ut_init_cl_stack(void** state)
{
	(void) state;
	assert_null(iscc_gr_vertex_store);
	assert_null(iscc_gr_data_object);
	assert_null(iscc_gr_search_object);
	assert_null(iscc_gr_index_scratch);
	assert_null(iscc_gr_dist_scratch);

	const scc_Clabel M = SCC_CLABEL_NA;
	scc_Clabel labels1[20] = { 1, 2, 4, 3, 1,
	                           2, 1, 5, 1, 2,
	                           3, 2, 5, 3, 4,
	                           3, 0, 4, 1, 5  };
	scc_Clabel labels2[25] = { 0, M, 2, 3, 3,
	                           5, 3, 0, 4, M,
	                           2, M, 4, 0, 5,
	                           5, 5, 2, 5, M,
	                           6, M, 6, 6, 3  };

	scc_Clustering cl1 = {
		.vertices = 20,
		.num_clusters = 6,
		.external_labels = true,
		.cluster_label = labels1,
	};
	scc_Clustering cl2 = {
		.vertices = 25,
		.num_clusters = 7,
		.external_labels = true,
		.cluster_label = labels2,
	};

	iscc_gr_ClusterStack cl_stack1 = iscc_gr_init_cl_stack(&cl1);

	assert_int_equal(cl_stack1.capacity, 49);
	assert_int_equal(cl_stack1.first_empty_pos, 6);
	assert_non_null(cl_stack1.clusters);

	assert_int_equal(cl_stack1.clusters[0].size, 1);
	assert_int_equal(cl_stack1.clusters[1].size, 5);
	assert_int_equal(cl_stack1.clusters[2].size, 4);
	assert_int_equal(cl_stack1.clusters[3].size, 4);
	assert_int_equal(cl_stack1.clusters[4].size, 3);
	assert_int_equal(cl_stack1.clusters[5].size, 3);

	scc_Vid ref1_cl0[1] = { 16 };
	scc_Vid ref1_cl1[5] = { 0, 4, 6, 8, 18 };
	scc_Vid ref1_cl2[4] = { 1, 5, 9, 11 };
	scc_Vid ref1_cl3[4] = { 3, 10, 13, 15 };
	scc_Vid ref1_cl4[3] = { 2, 14, 17 };
	scc_Vid ref1_cl5[3] = { 7, 12, 19 };

	assert_memory_equal(cl_stack1.clusters[0].members, ref1_cl0, 1 * sizeof(scc_Vid));
	assert_memory_equal(cl_stack1.clusters[1].members, ref1_cl1, 5 * sizeof(scc_Vid));
	assert_memory_equal(cl_stack1.clusters[2].members, ref1_cl2, 4 * sizeof(scc_Vid));
	assert_memory_equal(cl_stack1.clusters[3].members, ref1_cl3, 4 * sizeof(scc_Vid));
	assert_memory_equal(cl_stack1.clusters[4].members, ref1_cl4, 3 * sizeof(scc_Vid));
	assert_memory_equal(cl_stack1.clusters[5].members, ref1_cl5, 3 * sizeof(scc_Vid));

	for (size_t i = 6; i < cl_stack1.capacity; ++i) {
		assert_int_equal(cl_stack1.clusters[i].size, 0);
		assert_null(cl_stack1.clusters[i].members);
	}


	iscc_gr_ClusterStack cl_stack2 = iscc_gr_init_cl_stack(&cl2);

	assert_int_equal(cl_stack2.capacity, 53);
	assert_int_equal(cl_stack2.first_empty_pos, 7);
	assert_non_null(cl_stack2.clusters);

	assert_int_equal(cl_stack2.clusters[0].size, 3);
	assert_int_equal(cl_stack2.clusters[1].size, 0);
	assert_int_equal(cl_stack2.clusters[2].size, 3);
	assert_int_equal(cl_stack2.clusters[3].size, 4);
	assert_int_equal(cl_stack2.clusters[4].size, 2);
	assert_int_equal(cl_stack2.clusters[5].size, 5);
	assert_int_equal(cl_stack2.clusters[6].size, 3);

	scc_Vid ref2_cl0[3] = { 0, 7, 13 };
	scc_Vid ref2_cl2[3] = { 2, 10, 17 };
	scc_Vid ref2_cl3[4] = { 3, 4, 6, 24 };
	scc_Vid ref2_cl4[2] = { 8, 12 };
	scc_Vid ref2_cl5[5] = { 5, 14, 15, 16, 18 };
	scc_Vid ref2_cl6[3] = { 20, 22, 23 };

	assert_memory_equal(cl_stack2.clusters[0].members, ref2_cl0, 3 * sizeof(scc_Vid));
	assert_null(cl_stack2.clusters[1].members);
	assert_memory_equal(cl_stack2.clusters[2].members, ref2_cl2, 3 * sizeof(scc_Vid));
	assert_memory_equal(cl_stack2.clusters[3].members, ref2_cl3, 4 * sizeof(scc_Vid));
	assert_memory_equal(cl_stack2.clusters[4].members, ref2_cl4, 2 * sizeof(scc_Vid));
	assert_memory_equal(cl_stack2.clusters[5].members, ref2_cl5, 5 * sizeof(scc_Vid));
	assert_memory_equal(cl_stack2.clusters[6].members, ref2_cl6, 3 * sizeof(scc_Vid));

	for (size_t i = 7; i < cl_stack2.capacity; ++i) {
		assert_int_equal(cl_stack2.clusters[i].size, 0);
		assert_null(cl_stack2.clusters[i].members);
	}

	free(cl_stack1.clusters[0].members);
	free(cl_stack1.clusters[1].members);
	free(cl_stack1.clusters[2].members);
	free(cl_stack1.clusters[3].members);
	free(cl_stack1.clusters[4].members);
	free(cl_stack1.clusters[5].members);
	free(cl_stack1.clusters);

	free(cl_stack2.clusters[0].members);
	free(cl_stack2.clusters[2].members);
	free(cl_stack2.clusters[3].members);
	free(cl_stack2.clusters[4].members);
	free(cl_stack2.clusters[5].members);
	free(cl_stack2.clusters[6].members);
	free(cl_stack2.clusters);
}


void scc_ut_empty_cl_stack(void** state)
{
	(void) state;
	assert_null(iscc_gr_vertex_store);
	assert_null(iscc_gr_data_object);
	assert_null(iscc_gr_search_object);
	assert_null(iscc_gr_index_scratch);
	assert_null(iscc_gr_dist_scratch);

	iscc_gr_ClusterStack cl_stack1 = iscc_gr_empty_cl_stack(10);

	assert_int_equal(cl_stack1.capacity, 67);
	assert_int_equal(cl_stack1.first_empty_pos, 1);
	assert_non_null(cl_stack1.clusters);

	assert_int_equal(cl_stack1.clusters[0].size, 10);

	scc_Vid ref1_cl[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

	assert_memory_equal(cl_stack1.clusters[0].members, ref1_cl, 10 * sizeof(scc_Vid));

	cl_stack1.clusters[cl_stack1.capacity - 1].size = 0;


	iscc_gr_ClusterStack cl_stack2 = iscc_gr_empty_cl_stack(20);

	assert_int_equal(cl_stack2.capacity, 87);
	assert_int_equal(cl_stack2.first_empty_pos, 1);
	assert_non_null(cl_stack2.clusters);

	assert_int_equal(cl_stack2.clusters[0].size, 20);

	scc_Vid ref2_cl[20] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };

	assert_memory_equal(cl_stack2.clusters[0].members, ref2_cl, 20 * sizeof(scc_Vid));

	cl_stack2.clusters[cl_stack2.capacity - 1].size = 0;


	free(cl_stack1.clusters[0].members);
	free(cl_stack1.clusters);

	free(cl_stack2.clusters[0].members);
	free(cl_stack2.clusters);
}


void scc_ut_peek_at_stack(void** state)
{
	(void) state;
	assert_null(iscc_gr_vertex_store);
	assert_null(iscc_gr_data_object);
	assert_null(iscc_gr_search_object);
	assert_null(iscc_gr_index_scratch);
	assert_null(iscc_gr_dist_scratch);

	scc_Clabel labels[20] = { 1, 2, 4, 3, 1,
	                          2, 1, 5, 1, 2,
	                          3, 2, 5, 3, 4,
	                          3, 0, 4, 1, 5  };
	scc_Clustering cl = {
		.vertices = 20,
		.num_clusters = 6,
		.external_labels = true,
		.cluster_label = labels,
	};

	iscc_gr_ClusterStack cl_stack = iscc_gr_init_cl_stack(&cl);

	assert_int_equal(cl_stack.capacity, 49);
	assert_int_equal(cl_stack.first_empty_pos, 6);
	assert_non_null(cl_stack.clusters);

	assert_ptr_equal(iscc_gr_peek_at_stack(&cl_stack), &cl_stack.clusters[5]);
	assert_int_equal(cl_stack.first_empty_pos, 6);
	assert_ptr_equal(iscc_gr_peek_at_stack(&cl_stack), &cl_stack.clusters[5]);
	assert_int_equal(cl_stack.first_empty_pos, 6);

	cl_stack.first_empty_pos = 4;

	assert_ptr_equal(iscc_gr_peek_at_stack(&cl_stack), &cl_stack.clusters[3]);
	assert_int_equal(cl_stack.first_empty_pos, 4);
	assert_ptr_equal(iscc_gr_peek_at_stack(&cl_stack), &cl_stack.clusters[3]);
	assert_int_equal(cl_stack.first_empty_pos, 4);

	cl_stack.first_empty_pos = 0;

	assert_null(iscc_gr_peek_at_stack(&cl_stack));
	assert_int_equal(cl_stack.first_empty_pos, 0);
	assert_null(iscc_gr_peek_at_stack(&cl_stack));
	assert_int_equal(cl_stack.first_empty_pos, 0);

	cl_stack.first_empty_pos = 6;


	assert_int_equal(cl_stack.capacity, 49);
	assert_int_equal(cl_stack.first_empty_pos, 6);
	assert_non_null(cl_stack.clusters);

	assert_int_equal(cl_stack.clusters[0].size, 1);
	assert_int_equal(cl_stack.clusters[1].size, 5);
	assert_int_equal(cl_stack.clusters[2].size, 4);
	assert_int_equal(cl_stack.clusters[3].size, 4);
	assert_int_equal(cl_stack.clusters[4].size, 3);
	assert_int_equal(cl_stack.clusters[5].size, 3);

	scc_Vid ref1_cl0[1] = { 16 };
	scc_Vid ref1_cl1[5] = { 0, 4, 6, 8, 18 };
	scc_Vid ref1_cl2[4] = { 1, 5, 9, 11 };
	scc_Vid ref1_cl3[4] = { 3, 10, 13, 15 };
	scc_Vid ref1_cl4[3] = { 2, 14, 17 };
	scc_Vid ref1_cl5[3] = { 7, 12, 19 };

	assert_memory_equal(cl_stack.clusters[0].members, ref1_cl0, 1 * sizeof(scc_Vid));
	assert_memory_equal(cl_stack.clusters[1].members, ref1_cl1, 5 * sizeof(scc_Vid));
	assert_memory_equal(cl_stack.clusters[2].members, ref1_cl2, 4 * sizeof(scc_Vid));
	assert_memory_equal(cl_stack.clusters[3].members, ref1_cl3, 4 * sizeof(scc_Vid));
	assert_memory_equal(cl_stack.clusters[4].members, ref1_cl4, 3 * sizeof(scc_Vid));
	assert_memory_equal(cl_stack.clusters[5].members, ref1_cl5, 3 * sizeof(scc_Vid));

	for (size_t i = 6; i < cl_stack.capacity; ++i) {
		assert_int_equal(cl_stack.clusters[i].size, 0);
		assert_null(cl_stack.clusters[i].members);
	}

	free(cl_stack.clusters[0].members);
	free(cl_stack.clusters[1].members);
	free(cl_stack.clusters[2].members);
	free(cl_stack.clusters[3].members);
	free(cl_stack.clusters[4].members);
	free(cl_stack.clusters[5].members);
	free(cl_stack.clusters);
}


void scc_ut_pop_from_stack(void** state)
{
	(void) state;
	assert_null(iscc_gr_vertex_store);
	assert_null(iscc_gr_data_object);
	assert_null(iscc_gr_search_object);
	assert_null(iscc_gr_index_scratch);
	assert_null(iscc_gr_dist_scratch);

	scc_Clabel labels[20] = { 1, 2, 4, 3, 1,
	                          2, 1, 5, 1, 2,
	                          3, 2, 5, 3, 4,
	                          3, 0, 4, 1, 5  };
	scc_Clustering cl = {
		.vertices = 20,
		.num_clusters = 6,
		.external_labels = true,
		.cluster_label = labels,
	};

	iscc_gr_ClusterStack cl_stack = iscc_gr_init_cl_stack(&cl);

	assert_int_equal(cl_stack.capacity, 49);
	assert_int_equal(cl_stack.first_empty_pos, 6);
	assert_non_null(cl_stack.clusters);

	scc_Vid ref1_cl0[1] = { 16 };
	scc_Vid ref1_cl1[5] = { 0, 4, 6, 8, 18 };
	scc_Vid ref1_cl2[4] = { 1, 5, 9, 11 };
	scc_Vid ref1_cl3[4] = { 3, 10, 13, 15 };
	scc_Vid ref1_cl4[3] = { 2, 14, 17 };
	scc_Vid ref1_cl5[3] = { 7, 12, 19 };

	iscc_gr_Cluster next_cl;

	next_cl = iscc_gr_pop_from_stack(&cl_stack);
	assert_int_equal(cl_stack.capacity, 49);
	assert_int_equal(cl_stack.first_empty_pos, 5);
	assert_non_null(cl_stack.clusters);
	assert_int_equal(next_cl.size, 3);
	assert_memory_equal(next_cl.members, ref1_cl5, 3 * sizeof(scc_Vid));
	free(next_cl.members);

	next_cl = iscc_gr_pop_from_stack(&cl_stack);
	assert_int_equal(cl_stack.capacity, 49);
	assert_int_equal(cl_stack.first_empty_pos, 4);
	assert_non_null(cl_stack.clusters);
	assert_int_equal(next_cl.size, 3);
	assert_memory_equal(next_cl.members, ref1_cl4, 3 * sizeof(scc_Vid));
	free(next_cl.members);

	next_cl = iscc_gr_pop_from_stack(&cl_stack);
	assert_int_equal(cl_stack.capacity, 49);
	assert_int_equal(cl_stack.first_empty_pos, 3);
	assert_non_null(cl_stack.clusters);
	assert_int_equal(next_cl.size, 4);
	assert_memory_equal(next_cl.members, ref1_cl3, 4 * sizeof(scc_Vid));
	free(next_cl.members);

	next_cl = iscc_gr_pop_from_stack(&cl_stack);
	assert_int_equal(cl_stack.capacity, 49);
	assert_int_equal(cl_stack.first_empty_pos, 2);
	assert_non_null(cl_stack.clusters);
	assert_int_equal(next_cl.size, 4);
	assert_memory_equal(next_cl.members, ref1_cl2, 4 * sizeof(scc_Vid));
	free(next_cl.members);

	next_cl = iscc_gr_pop_from_stack(&cl_stack);
	assert_int_equal(cl_stack.capacity, 49);
	assert_int_equal(cl_stack.first_empty_pos, 1);
	assert_non_null(cl_stack.clusters);
	assert_int_equal(next_cl.size, 5);
	assert_memory_equal(next_cl.members, ref1_cl1, 5 * sizeof(scc_Vid));
	free(next_cl.members);

	next_cl = iscc_gr_pop_from_stack(&cl_stack);
	assert_int_equal(cl_stack.capacity, 49);
	assert_int_equal(cl_stack.first_empty_pos, 0);
	assert_non_null(cl_stack.clusters);
	assert_int_equal(next_cl.size, 1);
	assert_memory_equal(next_cl.members, ref1_cl0, 1 * sizeof(scc_Vid));
	free(next_cl.members);

	free(cl_stack.clusters);
}


void scc_ut_push_to_stack(void** state)
{
	(void) state;
	assert_null(iscc_gr_vertex_store);
	assert_null(iscc_gr_data_object);
	assert_null(iscc_gr_search_object);
	assert_null(iscc_gr_index_scratch);
	assert_null(iscc_gr_dist_scratch);

	iscc_gr_ClusterStack cl_stack = {
		.capacity = 3,
		.first_empty_pos = 2,
		.clusters = calloc(3, sizeof(iscc_gr_Cluster)),
	};

	iscc_gr_Cluster cl1 = {
		.size = 10,
		.members = malloc(sizeof(scc_Vid[10])),
	};
	iscc_gr_Cluster cl2 = {
		.size = 15,
		.members = malloc(sizeof(scc_Vid[15])),
	};
	iscc_gr_Cluster cl3 = {
		.size = 20,
		.members = malloc(sizeof(scc_Vid[20])),
	};

	assert_true(iscc_gr_push_to_stack(&cl_stack, cl1));
	assert_int_equal(cl_stack.capacity, 3);
	assert_int_equal(cl_stack.first_empty_pos, 3);
	assert_non_null(cl_stack.clusters);
	assert_memory_equal(&cl_stack.clusters[2], &cl1, sizeof(iscc_gr_Cluster));

	assert_true(iscc_gr_push_to_stack(&cl_stack, cl2));
	assert_int_equal(cl_stack.capacity, 82);
	assert_int_equal(cl_stack.first_empty_pos, 4);
	assert_non_null(cl_stack.clusters);
	assert_memory_equal(&cl_stack.clusters[3], &cl2, sizeof(iscc_gr_Cluster));

	assert_true(iscc_gr_push_to_stack(&cl_stack, cl3));
	assert_int_equal(cl_stack.capacity, 82);
	assert_int_equal(cl_stack.first_empty_pos, 5);
	assert_non_null(cl_stack.clusters);
	assert_memory_equal(&cl_stack.clusters[4], &cl3, sizeof(iscc_gr_Cluster));

	free(cl_stack.clusters);
	free(cl1.members);
	free(cl2.members);
	free(cl3.members);
}


void scc_ut_panic_free_everything(void** state)
{
	(void) state;
	assert_null(iscc_gr_vertex_store);
	assert_null(iscc_gr_data_object);
	assert_null(iscc_gr_search_object);
	assert_null(iscc_gr_index_scratch);
	assert_null(iscc_gr_dist_scratch);

	iscc_gr_data_object = (void*) 1234;

	scc_Clabel labels[20] = { 1, 2, 4, 3, 1,
	                          2, 1, 5, 1, 2,
	                          3, 2, 5, 3, 4,
	                          3, 0, 4, 1, 5  };
	scc_Clustering cl = {
		.vertices = 20,
		.num_clusters = 6,
		.external_labels = true,
		.cluster_label = labels,
	};

	iscc_gr_ClusterStack cl_stack = iscc_gr_init_cl_stack(&cl);

	iscc_gr_vertex_store = calloc(20, sizeof(iscc_gr_Vertex));

	iscc_gr_vertex_store[0].dist_list = malloc(sizeof(iscc_gr_DistanceEdgeList));
	iscc_gr_vertex_store[0].dist_list->distances_store = malloc(sizeof(iscc_gr_DistanceEdge[5]));

	iscc_gr_vertex_store[1].dist_list = malloc(sizeof(iscc_gr_DistanceEdgeList));
	iscc_gr_vertex_store[1].dist_list->distances_store = malloc(sizeof(iscc_gr_DistanceEdge[5]));

	iscc_gr_vertex_store[5].dist_list = malloc(sizeof(iscc_gr_DistanceEdgeList));
	iscc_gr_vertex_store[5].dist_list->distances_store = malloc(sizeof(iscc_gr_DistanceEdge[5]));

	iscc_gr_vertex_store[10].dist_list = malloc(sizeof(iscc_gr_DistanceEdgeList));
	iscc_gr_vertex_store[10].dist_list->distances_store = malloc(sizeof(iscc_gr_DistanceEdge[5]));

	iscc_gr_vertex_store[12].dist_list = malloc(sizeof(iscc_gr_DistanceEdgeList));
	iscc_gr_vertex_store[12].dist_list->distances_store = malloc(sizeof(iscc_gr_DistanceEdge[5]));

	iscc_gr_panic_free_everything(&cl_stack, 20);

	assert_null(iscc_gr_vertex_store);
	assert_null(iscc_gr_data_object);
	assert_int_equal(cl_stack.capacity, 0);
	assert_int_equal(cl_stack.first_empty_pos, 0);
	assert_null(cl_stack.clusters);
}


//static iscc_gr_Cluster iscc_gr_break_cluster_into_two(iscc_gr_Cluster* cluster_to_break,
//                                                      size_t k,
//                                                      bool batch_assign);
void scc_ut_break_cluster_into_two(void** state)
{
	(void) state;
	assert_null(iscc_gr_vertex_store);
	assert_null(iscc_gr_data_object);
	assert_null(iscc_gr_search_object);
	assert_null(iscc_gr_index_scratch);
	assert_null(iscc_gr_dist_scratch);


}


//static bool iscc_gr_find_centers(const iscc_gr_Cluster* cl,
//                                 size_t k,
 //                                scc_Vid* center1,
 //                                scc_Vid* center2);
void scc_ut_find_centers(void** state)
{
	(void) state;
	assert_null(iscc_gr_vertex_store);
	assert_null(iscc_gr_data_object);
	assert_null(iscc_gr_search_object);
	assert_null(iscc_gr_index_scratch);
	assert_null(iscc_gr_dist_scratch);


}


void scc_ut_init_free_search_object(void** state)
{
	(void) state;
	assert_null(iscc_gr_vertex_store);
	assert_null(iscc_gr_data_object);
	assert_null(iscc_gr_search_object);
	assert_null(iscc_gr_index_scratch);
	assert_null(iscc_gr_dist_scratch);

	#error Fix with real data object!
	iscc_gr_data_object = (void*) 435453;

	scc_Vid members[4] = { 10, 3, 5, 2 };
	iscc_gr_Cluster cl = {
		.size = 4,
		.members = members,
	};

	assert_true(iscc_gr_init_search_object(&cl));

	assert_non_null(iscc_gr_search_object);
	assert_non_null(iscc_gr_index_scratch);
	assert_non_null(iscc_gr_dist_scratch);

	iscc_gr_index_scratch[0] = 1;
	iscc_gr_index_scratch[1] = 2;
	iscc_gr_index_scratch[2] = 3;
	iscc_gr_index_scratch[3] = 4;

	iscc_gr_dist_scratch[0] = 1.0;
	iscc_gr_dist_scratch[1] = 2.0;
	iscc_gr_dist_scratch[2] = 3.0;
	iscc_gr_dist_scratch[3] = 4.0;

	assert_true(iscc_gr_free_search_object());

	assert_null(iscc_gr_search_object);
	assert_null(iscc_gr_index_scratch);
	assert_null(iscc_gr_dist_scratch);

	iscc_gr_data_object = NULL;
}



//static bool iscc_gr_populate_dist_list(scc_Vid v_id,
//                                       const iscc_gr_Cluster* cl);
void scc_ut_populate_dist_list(void** state)
{
	(void) state;
	assert_null(iscc_gr_vertex_store);
	assert_null(iscc_gr_data_object);
	assert_null(iscc_gr_search_object);
	assert_null(iscc_gr_index_scratch);
	assert_null(iscc_gr_dist_scratch);


}


//static inline iscc_gr_DistanceEdge* iscc_gr_get_first_dist(iscc_gr_DistanceEdgeList* dist_list,
 //                                                          scc_Clabel required_sub_label);
void scc_ut_get_first_dist(void** state)
{
	(void) state;
	assert_null(iscc_gr_vertex_store);
	assert_null(iscc_gr_data_object);
	assert_null(iscc_gr_search_object);
	assert_null(iscc_gr_index_scratch);
	assert_null(iscc_gr_dist_scratch);


}


//static inline iscc_gr_DistanceEdge* iscc_gr_get_last_dist(iscc_gr_DistanceEdgeList* dist_list,
//                                                          scc_Clabel required_sub_label);
void scc_ut_get_last_dist(void** state)
{
	(void) state;
	assert_null(iscc_gr_vertex_store);
	assert_null(iscc_gr_data_object);
	assert_null(iscc_gr_search_object);
	assert_null(iscc_gr_index_scratch);
	assert_null(iscc_gr_dist_scratch);


}


//static inline iscc_gr_DistanceEdge* iscc_gr_get_next_dist(iscc_gr_DistanceEdge* de,
//                                                         scc_Clabel required_sub_label);
void scc_ut_get_next_dist(void** state)
{
	(void) state;
	assert_null(iscc_gr_vertex_store);
	assert_null(iscc_gr_data_object);
	assert_null(iscc_gr_search_object);
	assert_null(iscc_gr_index_scratch);
	assert_null(iscc_gr_dist_scratch);


}


//static inline iscc_gr_DistanceEdge* iscc_gr_get_first_k_nn(iscc_gr_DistanceEdgeList* dist_list,
//                                                           size_t k,
 //                                                          scc_Vid out_dist_array[static k],
 //                                                          scc_Clabel required_sub_label);
void scc_ut_get_first_k_nn(void** state)
{
	(void) state;
	assert_null(iscc_gr_vertex_store);
	assert_null(iscc_gr_data_object);
	assert_null(iscc_gr_search_object);
	assert_null(iscc_gr_index_scratch);
	assert_null(iscc_gr_dist_scratch);


}


//static inline iscc_gr_DistanceEdge* iscc_gr_get_next_k_nn(iscc_gr_DistanceEdge* prev_dist,
//                                                          size_t k,
//                                                          scc_Vid out_dist_array[static k],
//                                                          scc_Clabel required_sub_label);
void scc_ut_get_next_k_nn(void** state)
{
	(void) state;
	assert_null(iscc_gr_vertex_store);
	assert_null(iscc_gr_data_object);
	assert_null(iscc_gr_search_object);
	assert_null(iscc_gr_index_scratch);
	assert_null(iscc_gr_dist_scratch);


}


void scc_ut_move_v_to_cluster(void** state)
{
	(void) state;
	assert_null(iscc_gr_vertex_store);
	assert_null(iscc_gr_data_object);
	assert_null(iscc_gr_search_object);
	assert_null(iscc_gr_index_scratch);
	assert_null(iscc_gr_dist_scratch);

	iscc_gr_Cluster cl = {
		.size = 3,
		.members = calloc(20, sizeof(scc_Vid)),
	};
	cl.members[0] = 4;
	cl.members[1] = 1;
	cl.members[2] = 2;

	iscc_gr_vertex_store = calloc(20, sizeof(iscc_gr_Vertex));

	iscc_gr_move_v_to_cluster(11, &cl, 1);
	assert_int_equal(cl.size, 4);
	iscc_gr_move_v_to_cluster(3, &cl, 2);
	assert_int_equal(cl.size, 5);
	iscc_gr_move_v_to_cluster(6, &cl, 3);
	assert_int_equal(cl.size, 6);
	iscc_gr_move_v_to_cluster(10, &cl, 4);
	assert_int_equal(cl.size, 7);
	iscc_gr_move_v_to_cluster(14, &cl, 5);
	assert_int_equal(cl.size, 8);

	scc_Clabel v_labels[20] = { 0, 0, 0, 2, 0,
	                            0, 3, 0, 0, 0,
	                            4, 1, 0, 0, 5,
	                            0, 0, 0, 0, 0  };

	for (size_t i = 0; i < 20; ++i) {
		assert_int_equal(iscc_gr_vertex_store[i].sub_label, v_labels[i]);
	}

	scc_Vid cl_members[20] = { 4, 1, 2, 11, 3,
	                           6, 10, 14, 0, 0,
	                           0, 0, 0, 0, 0,
	                           0, 0, 0, 0, 0  };

	assert_memory_equal(cl.members, cl_members, 20 * sizeof(scc_Vid));

	free(cl.members);
	free(iscc_gr_vertex_store);
	iscc_gr_vertex_store = NULL;
}


void scc_ut_adjust_cluster_memory(void** state)
{
	(void) state;
	assert_null(iscc_gr_vertex_store);
	assert_null(iscc_gr_data_object);
	assert_null(iscc_gr_search_object);
	assert_null(iscc_gr_index_scratch);
	assert_null(iscc_gr_dist_scratch);

	iscc_gr_Cluster cl1 = {
		.size = 2,
		.members = malloc(sizeof(scc_Vid[6])),
	};
	cl1.members[0] = 4;
	cl1.members[1] = 1;

	iscc_gr_Cluster cl2 = {
		.size = 3,
		.members = malloc(sizeof(scc_Vid[6])),
	};
	cl2.members[0] = 2;
	cl2.members[1] = 8;
	cl2.members[2] = 3;

	iscc_gr_Cluster cl3 = {
		.size = 4,
		.members = malloc(sizeof(scc_Vid[4])),
	};
	cl3.members[0] = 9;
	cl3.members[1] = 0;
	cl3.members[2] = 10;
	cl3.members[3] = 12;

	iscc_gr_vertex_store = calloc(20, sizeof(iscc_gr_Vertex));

	iscc_gr_vertex_store[0].dist_list = malloc(sizeof(iscc_gr_DistanceEdgeList));
	iscc_gr_vertex_store[0].dist_list->distances_store = malloc(sizeof(iscc_gr_DistanceEdge[5]));

	iscc_gr_vertex_store[1].dist_list = malloc(sizeof(iscc_gr_DistanceEdgeList));
	iscc_gr_vertex_store[1].dist_list->distances_store = malloc(sizeof(iscc_gr_DistanceEdge[5]));

	iscc_gr_vertex_store[2].dist_list = malloc(sizeof(iscc_gr_DistanceEdgeList));
	iscc_gr_vertex_store[2].dist_list->distances_store = malloc(sizeof(iscc_gr_DistanceEdge[5]));

	iscc_gr_vertex_store[3].dist_list = malloc(sizeof(iscc_gr_DistanceEdgeList));
	iscc_gr_vertex_store[3].dist_list->distances_store = malloc(sizeof(iscc_gr_DistanceEdge[5]));

	iscc_gr_vertex_store[4].dist_list = malloc(sizeof(iscc_gr_DistanceEdgeList));
	iscc_gr_vertex_store[4].dist_list->distances_store = malloc(sizeof(iscc_gr_DistanceEdge[5]));

	iscc_gr_vertex_store[8].dist_list = malloc(sizeof(iscc_gr_DistanceEdgeList));
	iscc_gr_vertex_store[8].dist_list->distances_store = malloc(sizeof(iscc_gr_DistanceEdge[5]));

	iscc_gr_vertex_store[9].dist_list = malloc(sizeof(iscc_gr_DistanceEdgeList));
	iscc_gr_vertex_store[9].dist_list->distances_store = malloc(sizeof(iscc_gr_DistanceEdge[5]));

	iscc_gr_vertex_store[10].dist_list = malloc(sizeof(iscc_gr_DistanceEdgeList));
	iscc_gr_vertex_store[10].dist_list->distances_store = malloc(sizeof(iscc_gr_DistanceEdge[5]));

	iscc_gr_vertex_store[12].dist_list = malloc(sizeof(iscc_gr_DistanceEdgeList));
	iscc_gr_vertex_store[12].dist_list->distances_store = malloc(sizeof(iscc_gr_DistanceEdge[5]));


	iscc_gr_DistanceEdge d_tmp = {
		.head = 2,
		.distance = 1.0,
		.prev_dist = NULL,
		.next_dist = NULL,
	};

	iscc_gr_adjust_cluster_memory(&cl1, 2);
	assert_int_equal(cl1.size, 2);
	iscc_gr_adjust_cluster_memory(&cl1, 2);
	assert_int_equal(cl1.size, 2);
	assert_null(iscc_gr_vertex_store[4].dist_list);
	assert_null(iscc_gr_vertex_store[1].dist_list);
	cl1.members[1] = 0;

	iscc_gr_adjust_cluster_memory(&cl2, 2);
	assert_int_equal(cl2.size, 3);
	iscc_gr_adjust_cluster_memory(&cl2, 2);
	assert_int_equal(cl2.size, 3);
	assert_null(iscc_gr_vertex_store[2].dist_list);
	assert_null(iscc_gr_vertex_store[8].dist_list);
	assert_null(iscc_gr_vertex_store[3].dist_list);
	cl2.members[2] = 0;

	iscc_gr_adjust_cluster_memory(&cl3, 2);
	assert_int_equal(cl3.size, 4);
	iscc_gr_adjust_cluster_memory(&cl3, 2);
	assert_int_equal(cl3.size, 4);
	assert_non_null(iscc_gr_vertex_store[9].dist_list);
	assert_non_null(iscc_gr_vertex_store[9].dist_list->distances_store);
	iscc_gr_vertex_store[9].dist_list->distances_store[4] = d_tmp;
	assert_non_null(iscc_gr_vertex_store[0].dist_list);
	assert_non_null(iscc_gr_vertex_store[0].dist_list->distances_store);
	iscc_gr_vertex_store[0].dist_list->distances_store[4] = d_tmp;
	assert_non_null(iscc_gr_vertex_store[10].dist_list);
	assert_non_null(iscc_gr_vertex_store[10].dist_list->distances_store);
	iscc_gr_vertex_store[10].dist_list->distances_store[4] = d_tmp;
	assert_non_null(iscc_gr_vertex_store[12].dist_list);
	assert_non_null(iscc_gr_vertex_store[12].dist_list->distances_store);
	iscc_gr_vertex_store[12].dist_list->distances_store[4] = d_tmp;
	cl3.members[3] = 0;

	free(iscc_gr_vertex_store[9].dist_list->distances_store);
	free(iscc_gr_vertex_store[9].dist_list);
	free(iscc_gr_vertex_store[0].dist_list->distances_store);
	free(iscc_gr_vertex_store[0].dist_list);
	free(iscc_gr_vertex_store[10].dist_list->distances_store);
	free(iscc_gr_vertex_store[10].dist_list);
	free(iscc_gr_vertex_store[12].dist_list->distances_store);
	free(iscc_gr_vertex_store[12].dist_list);

	free(cl1.members);
	free(cl2.members);
	free(cl3.members);

	free(iscc_gr_vertex_store);
	iscc_gr_vertex_store = NULL;
}



void scc_ut_greedy_finished(void** state)
{
	(void) state;
	assert_null(iscc_gr_vertex_store);
	assert_null(iscc_gr_data_object);
	assert_null(iscc_gr_search_object);
	assert_null(iscc_gr_index_scratch);
	assert_null(iscc_gr_dist_scratch);
}


int main(void)
{
	const struct CMUnitTest test_greedy_internals[] = {
		cmocka_unit_test(scc_ut_init_cl_stack),
		cmocka_unit_test(scc_ut_empty_cl_stack),
		cmocka_unit_test(scc_ut_peek_at_stack),
		cmocka_unit_test(scc_ut_pop_from_stack),
		cmocka_unit_test(scc_ut_push_to_stack),
		cmocka_unit_test(scc_ut_panic_free_everything),
		//cmocka_unit_test(scc_ut_break_cluster_into_two),
		//cmocka_unit_test(scc_ut_find_centers),
		cmocka_unit_test(scc_ut_init_free_search_object),
		//cmocka_unit_test(scc_ut_populate_dist_list),
		//cmocka_unit_test(scc_ut_get_first_dist),
		//cmocka_unit_test(scc_ut_get_last_dist),
		//cmocka_unit_test(scc_ut_get_next_dist),
		//cmocka_unit_test(scc_ut_get_first_k_nn),
		//cmocka_unit_test(scc_ut_get_next_k_nn),
		cmocka_unit_test(scc_ut_move_v_to_cluster),
		cmocka_unit_test(scc_ut_adjust_cluster_memory),
		cmocka_unit_test(scc_ut_greedy_finished),
	};

	return cmocka_run_group_tests_name("internal greedy module", test_greedy_internals, NULL, NULL);
}
