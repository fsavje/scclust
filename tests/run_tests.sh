#!/bin/sh

run_test()
{
	build/$1.out
	if [ "$?" != "0" ]; then
		exit 1
	fi
}

make clean
make all

run_test test_data_obj
run_test test_digraph_core
run_test test_digraph_debug
run_test test_digraph_operations_internal
run_test test_digraph_operations
run_test test_dist_search
run_test test_error
run_test test_greedy_clustering_internal
run_test test_greedy_clustering
run_test test_nng_clustering
run_test test_nng_core_internal
run_test test_nng_core
run_test test_nng_findseeds_internal
run_test test_nng_findseeds
run_test test_scclust

echo "\n*** ALL TESTS PASSED! ***"
