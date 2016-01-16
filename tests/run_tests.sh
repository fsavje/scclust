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

run_test test_clustering
run_test test_digraph_core
run_test test_digraph_debug
run_test test_digraph_operations
run_test test_findseeds
run_test test_nng_caliper
run_test test_nng_clustering
run_test test_ann_wrapper
run_test test_findseeds_internals
run_test test_findseeds_stable

echo "\n*** ALL TESTS PASSED! ***"