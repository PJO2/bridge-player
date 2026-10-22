#ifndef BRIDGE_TEST_CHECK_H
#define BRIDGE_TEST_CHECK_H

#include <stdio.h>
#include <stdlib.h>

/* Keep checks active in Release builds, independently of NDEBUG. */
#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "%s:%d: check failed: %s\n", \
                __FILE__, __LINE__, #condition); \
        exit(EXIT_FAILURE); \
    } \
} while (0)

#endif
