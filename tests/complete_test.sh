#!/bin/sh

REDCOLOR="\033[0;31m"
NOCOLOR="\033[0m"

run_tests()
{
	printf "Test: $1\n"
	rm -f -r scc_build
	mkdir scc_build
	cd scc_build
	../../configure $STANDARD_FLAGS $1 &> ../tmp_output_results
	if [ "$?" != "0" ]; then
		printf "${REDCOLOR}*** TEST SUITE FAILED, ABORTING:${NOCOLOR}\n"
		printf "$(cat ../tmp_output_results)\n"
		rm ../tmp_output_results
		exit 1
	fi
	cd ..
	./run_tests.sh $STRESS &> tmp_output_results
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

for CLABEL_FOR in uint32_t uint64_t int int_mna; do
	if [ "$CLABEL_FOR" = "int_mna" ]; then
		CLABEL="int"
		CLABEL_NA="-1"
	else
		CLABEL="$CLABEL_FOR"
		CLABEL_NA="max"
	fi
	for TYPELABEL in uint_fast16_t int; do
		for DPID in uint32_t uint64_t int int_mna; do
			for ARC in uint32_t uint64_t; do
				# Add ANN SEARCH
				run_tests "--with-clabel=$CLABEL --with-clabel-na=$CLABEL_NA --with-typelabel=$TYPELABEL --with-dpid=$DPID --with-arc=$ARC"
				if [ "$?" != "0" ]; then
					printf "${REDCOLOR}***TEST FAILED***${NOCOLOR}\n"
					exit 1
				fi
			done
		done
	done
done

printf "${REDCOLOR}ALL COMPILER TESTS DONE${NOCOLOR}\n"
