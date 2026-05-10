#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "factorial.h"

#define DEFAULT_N        5
#define FACTORIAL_MAX_N 12 /* keeps n! within typical 32-bit signed int range */

/*
 * Driver program: calls the recursive factorial and prints the final result.
 * Trace lines come from factorial.c so you can see each call enter and return.
 *
 * Usage: recursion_demo [n]
 *   If n is omitted, defaults to 5 (course demo). n must satisfy 0 <= n <= 12.
 */
static int parse_n(int argc, char *argv[])
{
    if (argc < 2) {
        return DEFAULT_N;
    }

    char *end = NULL;
    errno = 0;
    long v = strtol(argv[1], &end, 10);

    if (errno != 0 || end == argv[1] || *end != '\0' || v < 0 ||
        v > FACTORIAL_MAX_N) {
        fprintf(stderr,
                "usage: %s [n]\n"
                "  n is optional; allowed range 0..%d (default %d).\n",
                argv[0], FACTORIAL_MAX_N, DEFAULT_N);
        exit(EXIT_FAILURE);
    }

    return (int)v;
}

int main(int argc, char *argv[])
{
    const int n = parse_n(argc, argv);

    printf("=== Recursion demo: factorial(%d) ===\n", n);
    printf("main: calling factorial(%d) ...\n\n", n);

    int answer = factorial(n);

    printf("\nmain: factorial(%d) returned %d\n", n, answer);
    if (n == 5) {
        printf("Expected: 5! = 120\n");
    }

    return EXIT_SUCCESS;
}
