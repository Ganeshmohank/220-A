#ifndef FACTORIAL_H
#define FACTORIAL_H

/*
 * Recursive factorial declaration.
 * n must be non-negative. Values above ~12 overflow a 32-bit signed int;
 * the driver clamps input accordingly.
 */
int factorial(int n);

#endif /* FACTORIAL_H */
