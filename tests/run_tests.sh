#!/bin/bash

REDCOLOR='\033[0;31m'
NOCOLOR='\033[0m'

run_test()
{
	build/$1.out
	if [ "$?" != "0" ]; then
		printf "${REDCOLOR}ERROR FOUND!${NOCOLOR}\n"
		exit 1
	fi
}

STRESS=false
while getopts ":c:s" opt; do
	case $opt in
		c)
			COMPOPT=$OPTARG
			printf "${REDCOLOR}Compiling with: $OPTARG${NOCOLOR}\n"
			;;
		s)
			STRESS=true
			printf "${REDCOLOR}Running stress tests.${NOCOLOR}\n"
			;;
		\?)
			printf "${REDCOLOR}Invalid option: -$OPTARG${NOCOLOR}\n"
			exit 1
			;;
	esac
done

make clean
make all $COMPOPT

run_test test_data_obj
run_test test_digraph_core
run_test test_digraph_debug
run_test test_digraph_operations_internal
run_test test_digraph_operations
run_test test_dist_search
run_test test_error
run_test test_hierarchical_clustering_internal
run_test test_hierarchical_clustering
run_test test_nng_clustering_internal
run_test test_nng_clustering
run_test test_nng_core_internal
run_test test_nng_core_stable
run_test test_nng_core
run_test test_nng_findseeds_internal
run_test test_nng_findseeds_stable
run_test test_nng_findseeds
run_test test_scclust

if [ $STRESS = true ]; then
	run_test stress_hierarchical_clustering
	run_test stress_nng_clustering
fi

printf "${REDCOLOR}*** ALL TESTS PASSED! ***${NOCOLOR}\n"
