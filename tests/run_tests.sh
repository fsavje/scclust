#!/bin/bash

REDCOLOR="\033[0;31m"
NOCOLOR="\033[0m"

run_test()
{
	build/$1.out
	if [ "$?" != "0" ]; then
		printf "${REDCOLOR}ERROR FOUND!${NOCOLOR}\n"
		exit 1
	fi
}

STRESS="false"
ANN="N"
KEEP_SCC_BUILD="false"

while [ "$1" != "" ]; do
	case $1 in
		-a )
			ANN="Y"
			printf "${REDCOLOR}Running ANN search tests.${NOCOLOR}\n"
			;;
		-k )
			KEEP_SCC_BUILD="true" ;;
		-s )
			STRESS="true"
			printf "${REDCOLOR}Running stress tests.${NOCOLOR}\n"
			;;
		* )
			printf "${REDCOLOR}Invalid option: $1${NOCOLOR}\n"
			exit 1
			;;
	esac
	shift
done

make clean
if [ "$KEEP_SCC_BUILD" = "false" ]; then
	rm -rf scc_build
fi
make all ANN_SEARCH=$ANN

run_test test_data_set
run_test test_digraph_core
run_test test_digraph_debug
run_test test_digraph_operations_internal
run_test test_digraph_operations
run_test test_dist_search
run_test test_error
run_test test_hierarchical_clustering_internal
run_test test_hierarchical_clustering
#run_test test_nng_clustering_batches
run_test test_nng_clustering_internal
run_test test_nng_clustering
run_test test_nng_core_internal
run_test test_nng_core_stable
run_test test_nng_core
run_test test_nng_findseeds_internal
run_test test_nng_findseeds_stable
run_test test_nng_findseeds
run_test test_scclust

if [ "$STRESS" = "true" ]; then
	run_test stress_hierarchical_clustering
	run_test stress_nng_clustering
fi

printf "${REDCOLOR}*** ALL TESTS PASSED! ***${NOCOLOR}\n"
