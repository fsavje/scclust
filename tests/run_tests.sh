#!/bin/sh

run_test()
{
	build/$1.out
	if [ "$?" != "0" ]; then
		exit 1
	fi
}

STRESS=false
while getopts ":s" opt; do
  case $opt in
    s)
      echo "Running stress tests." >&2
      STRESS=true
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      exit 1
      ;;
  esac
done

make clean
make all

run_test test_data_obj
run_test test_digraph_core
run_test test_digraph_debug
run_test test_digraph_operations_internal
run_test test_digraph_operations
run_test test_dist_search
run_test test_error
run_test test_greedy_top_clustering_internal
run_test test_greedy_top_clustering
run_test test_nng_clustering_internal
run_test test_nng_clustering
run_test test_nng_core_internal
run_test test_nng_core
run_test test_nng_findseeds_internal
run_test test_nng_findseeds_stable
run_test test_nng_findseeds
run_test test_scclust

if [ $STRESS = true ]; then
	run_test stress_greedy_top_clustering
	run_test stress_nng_clustering
fi

echo "\n*** ALL TESTS PASSED! ***"
