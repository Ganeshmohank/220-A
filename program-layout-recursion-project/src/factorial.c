#include <stdio.h>

#include "factorial.h"

/*
 * factorial(n) computes n! using recursion.
 *
 * Base case: n <= 1
 *   0! and 1! are both defined as 1. When n is 0 or 1, there is no deeper
 *   call; we return 1 immediately.
 *
 * Recursive case: n > 1
 *   n! = n * (n-1)!, so we return n multiplied by factorial(n-1).
 *   Each call waits for the result of the next call before it can finish.
 */
int factorial(int n)
{
    printf("  -> factorial(%d) ENTER\n", n);

    if (n <= 1) {
        printf("  <- factorial(%d) RETURN 1 (base case)\n", n);
        return 1;
    }

    int smaller = factorial(n - 1);
    int result = n * smaller;

    printf("  <- factorial(%d) RETURN %d (= %d * factorial(%d))\n",
           n, result, n, n - 1);

    return result;
}
