#!/bin/bash

REDCOLOR="\033[0;31m"
NOCOLOR="\033[0m"

run_tests()
{
	printf "Test: $1 $2\n"
	rm -f -r build scc_build
	mkdir scc_build
	cd scc_build
	../../configure $STANDARD_FLAGS $2 &> ../tmp_output_results
	if [ "$?" != "0" ]; then
		printf "${REDCOLOR}*** TEST SUITE FAILED, ABORTING:${NOCOLOR}\n"
		printf "$(cat ../tmp_output_results)\n"
		rm ../tmp_output_results
		exit 1
	fi
	cd ..
	./run_tests.sh -k $1 $STRESS &> tmp_output_results
	if [ "$?" != "0" ]; then
		printf "${REDCOLOR}*** TEST SUITE FAILED, ABORTING:${NOCOLOR}\n"
		printf "$(cat tmp_output_results)\n"
		rm tmp_output_results
		exit 1
	fi
	rm tmp_output_results
}

STANDARD_FLAGS="
	--enable-assert
	--enable-digraph-debug
	--enable-cmocka-headers
	--disable-documentation"

STRESS=

while [ "$1" != "" ]; do
	case $1 in
		-s )
			printf "${REDCOLOR}Running stress tests.${NOCOLOR}\n"
			STRESS=-s
			;;
		* )
			printf "${REDCOLOR}Invalid option: $1${NOCOLOR}\n"
			exit 1
			;;
	esac
	shift
done

for ANN_SEARCH in "" "-a"; do
	for CLABEL_FOR in uint32_t uint64_t int int_min int_mna; do
		if [ "$CLABEL_FOR" = "int_min" ]; then
			CLABEL="int"
			CLABEL_NA="min"
		elif [ "$CLABEL_FOR" = "int_mna" ]; then
			CLABEL="int"
			CLABEL_NA="-1"
		else
			CLABEL="$CLABEL_FOR"
			CLABEL_NA="max"
		fi
		for TYPELABEL in uint_fast16_t int; do
			for POINTINDEX in uint32_t uint64_t int; do
				for ARC in uint32_t uint64_t; do
					run_tests "$ANN_SEARCH" "--with-clabel=$CLABEL --with-clabel-na=$CLABEL_NA --with-typelabel=$TYPELABEL --with-pointindex=$POINTINDEX --with-arcindex=$ARC"
					if [ "$?" != "0" ]; then
						printf "${REDCOLOR}***TEST FAILED***${NOCOLOR}\n"
						exit 1
					fi
				done
			done
		done
	done
done

printf "${REDCOLOR}ALL COMPILER TESTS DONE${NOCOLOR}\n"
