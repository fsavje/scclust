#!/bin/bash

REDCOLOR='\033[0;31m'
NOCOLOR='\033[0m'

run_tests()
{
	printf "${REDCOLOR}Running tests compiled with: $SUITEOPTS $1${NOCOLOR}\n"
	./run_tests.sh $STRESS -c "$1"
	if [ "$?" != "0" ]; then
		printf "${REDCOLOR}REMAINING TESTS ABORTED.${NOCOLOR}\n"
		mv ../include/scclust_save.h ../include/scclust.h
		exit 1
	fi
}

run_test_suite()
{
	cp ../include/scclust_save.h ../include/scclust.h

	if [ "$1" == "UINT32" ]; then
		SUITEOPTS="CLABEL=UINT32"
		sed -i '60i#define SCC_RUN_CLABEL_TYPE_TESTS' ../include/scclust.h

	elif [ "$1" == "UINT64" ]; then
		SUITEOPTS="CLABEL=UINT64"
		sed -i '40,80s/typedef uint32_t scc_Clabel;/typedef uint64_t scc_Clabel;/' ../include/scclust.h
		sed -i '40,80s/static const scc_Clabel SCC_CLABEL_MAX = UINT32_MAX;/static const scc_Clabel SCC_CLABEL_MAX = UINT64_MAX;/' ../include/scclust.h
		sed -i '40,80s/static const scc_Clabel SCC_CLABEL_NA = UINT32_MAX;/static const scc_Clabel SCC_CLABEL_NA = UINT64_MAX;/' ../include/scclust.h

	elif [ "$1" == "INT" ]; then
		SUITEOPTS="CLABEL=INT"
		sed -i '60i#define SCC_RUN_CLABEL_TYPE_TESTS' ../include/scclust.h
		sed -i '40,80s/typedef uint32_t scc_Clabel;/typedef int scc_Clabel;/' ../include/scclust.h
		sed -i '40,80s/static const scc_Clabel SCC_CLABEL_MAX = UINT32_MAX;/static const scc_Clabel SCC_CLABEL_MAX = INT_MAX;/' ../include/scclust.h
		sed -i '40,80s/static const scc_Clabel SCC_CLABEL_NA = UINT32_MAX;/static const scc_Clabel SCC_CLABEL_NA = INT_MAX;/' ../include/scclust.h

	elif [ "$1" == "INT_MNA" ]; then
		SUITEOPTS="CLABEL=INT"
		sed -i '60i#define SCC_RUN_CLABEL_TYPE_TESTS' ../include/scclust.h
		sed -i '40,80s/typedef uint32_t scc_Clabel;/typedef int scc_Clabel;/' ../include/scclust.h
		sed -i '40,80s/static const scc_Clabel SCC_CLABEL_MAX = UINT32_MAX;/static const scc_Clabel SCC_CLABEL_MAX = INT_MAX;/' ../include/scclust.h
		sed -i '40,80s/static const scc_Clabel SCC_CLABEL_NA = UINT32_MAX;/static const scc_Clabel SCC_CLABEL_NA = -1;/' ../include/scclust.h

	else
		printf "${REDCOLOR}UNKNOWN CLABEL.${NOCOLOR}\n"
		mv ../include/scclust_save.h ../include/scclust.h
		exit 1
	fi


	if [ "$2" == "UINT16" ]; then
		SUITEOPTS=$SUITEOPTS" TYPELABEL=UINT16"

	elif [ "$2" == "INT" ]; then
		SUITEOPTS=$SUITEOPTS" TYPELABEL=INT"
		sed -i '40,80s/typedef uint_fast16_t scc_TypeLabel;/typedef int scc_TypeLabel;/' ../include/scclust.h

	else
		printf "${REDCOLOR}UNKNOWN TYPELABEL.${NOCOLOR}\n"
		mv ../include/scclust_save.h ../include/scclust.h
		exit 1
	fi

	run_tests

	run_tests "DPID=SCC_DPID_INT ARC=SCC_ARC32 ANN_SEARCH=N"
	run_tests "DPID=SCC_DPID_INT ARC=SCC_ARC32 ANN_SEARCH=Y ANNTREE=SCC_ANN_KDTREE"
	run_tests "DPID=SCC_DPID_INT ARC=SCC_ARC32 ANN_SEARCH=Y ANNTREE=SCC_ANN_BDTREE"
	run_tests "DPID=SCC_DPID_INT ARC=SCC_ARC64 ANN_SEARCH=N"
	run_tests "DPID=SCC_DPID_INT ARC=SCC_ARC64 ANN_SEARCH=Y ANNTREE=SCC_ANN_KDTREE"
	run_tests "DPID=SCC_DPID_INT ARC=SCC_ARC64 ANN_SEARCH=Y ANNTREE=SCC_ANN_BDTREE"

	run_tests "DPID=SCC_DPID_UINT32 ARC=SCC_ARC32 ANN_SEARCH=N"
	run_tests "DPID=SCC_DPID_UINT32 ARC=SCC_ARC32 ANN_SEARCH=Y ANNTREE=SCC_ANN_KDTREE"
	run_tests "DPID=SCC_DPID_UINT32 ARC=SCC_ARC32 ANN_SEARCH=Y ANNTREE=SCC_ANN_BDTREE"
	run_tests "DPID=SCC_DPID_UINT32 ARC=SCC_ARC64 ANN_SEARCH=N"
	run_tests "DPID=SCC_DPID_UINT32 ARC=SCC_ARC64 ANN_SEARCH=Y ANNTREE=SCC_ANN_KDTREE"
	run_tests "DPID=SCC_DPID_UINT32 ARC=SCC_ARC64 ANN_SEARCH=Y ANNTREE=SCC_ANN_BDTREE"

	run_tests "DPID=SCC_DPID_UINT64 ARC=SCC_ARC32 ANN_SEARCH=N"
	run_tests "DPID=SCC_DPID_UINT64 ARC=SCC_ARC32 ANN_SEARCH=Y ANNTREE=SCC_ANN_KDTREE"
	run_tests "DPID=SCC_DPID_UINT64 ARC=SCC_ARC32 ANN_SEARCH=Y ANNTREE=SCC_ANN_BDTREE"
	run_tests "DPID=SCC_DPID_UINT64 ARC=SCC_ARC64 ANN_SEARCH=N"
	run_tests "DPID=SCC_DPID_UINT64 ARC=SCC_ARC64 ANN_SEARCH=Y ANNTREE=SCC_ANN_KDTREE"
	run_tests "DPID=SCC_DPID_UINT64 ARC=SCC_ARC64 ANN_SEARCH=Y ANNTREE=SCC_ANN_BDTREE"
}


STRESS=
while getopts ":s" opt; do
	case $opt in
		s)
			printf "${REDCOLOR}Running stress tests.${NOCOLOR}\n"
			STRESS=-s
			;;
		\?)
			printf "${REDCOLOR}Invalid option: -$OPTARG${NOCOLOR}\n"
			exit 1
			;;
	esac
done


cp ../include/scclust.h ../include/scclust_save.h

run_test_suite UINT32 UINT16
run_test_suite UINT64 UINT16
run_test_suite INT UINT16
run_test_suite INT_MNA UINT16

run_test_suite UINT32 INT
run_test_suite UINT64 INT
run_test_suite INT INT
run_test_suite INT_MNA INT

mv ../include/scclust_save.h ../include/scclust.h

printf "${REDCOLOR}ALL TYPE TESTS DONE${NOCOLOR}\n"
