#ifndef UT_THM_MALLOC_OVERRIDE_HG
#define UT_THM_MALLOC_OVERRIDE_HG

#include <stdlib.h>
#define malloc(size) _test_malloc(size, __FILE__, __LINE__)
#define calloc(num, size) _test_calloc(num, size, __FILE__, __LINE__)
#define realloc(ptr, size) _test_realloc(ptr, size, __FILE__, __LINE__)
#define free(ptr) _test_free(ptr, __FILE__, __LINE__)

#endif
