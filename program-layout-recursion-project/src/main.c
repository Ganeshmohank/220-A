#include <stdio.h>

#include "factorial.h"

/*
 * Driver program: calls the recursive factorial and prints the final result.
 * Trace lines come from factorial.c so you can see each call enter and return.
 */
int main(void)
{
    const int n = 5;

    printf("=== Recursion demo: factorial(%d) ===\n", n);
    printf("main: calling factorial(%d) ...\n\n", n);

    int answer = factorial(n);

    printf("\nmain: factorial(%d) returned %d\n", n, answer);
    printf("Expected: 5! = 120\n");

    return 0;
}
