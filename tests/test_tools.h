#ifndef SCC_UT_TEST_TOOLS_HG
#define SCC_UT_TEST_TOOLS_HG

#include "test_suite.h"

#include <stdio.h>
#include "../include/config.h"

#define scc_ut_print_Vid_array(to_print, length) _scc_ut_print_Vid_array(to_print, length, #to_print, __LINE__)

void _scc_ut_print_Vid_array(const scc_Vid* const to_print,
                                    const size_t length,
                                    const char* const name,
                                    const int line) {
	printf("%i:%s: ", line, name);

	if (length == 0) {
		printf("[]\n");
	} else {
		printf("[%ju", (uintmax_t)to_print[0]);
		for (size_t i = 1; i < length; ++i) printf(", %ju", (uintmax_t)to_print[i]);
		printf("]\n");
	}
}

#endif
