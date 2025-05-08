# Signed Multi-Word Division (x86-64 Assembly)

This project implements a low-level division routine in **x86-64 Assembly** that divides a signed multi-word integer by a signed 64-bit divisor. The result is stored in-place, and the signed remainder is returned in `RAX`.

---

## Function Overview

```c
int64_t mdiv(int64_t *x, size_t n, int64_t y);
```
- x: pointer to an array of n signed 64-bit integers (most significant word last)
- y: signed 64-bit divisor

The function overwrites x with the quotient, returns the signed remainder in RAX and properly handles signed values, two’s complement, and overflow detection.

---

## How It Works
- Detects signs of the dividend and divisor.
- Normalizes both to positive using two’s complement negation.
- Performs division from most to least significant word.
- Adjusts signs of both quotient and remainder according to standard C semantics.
- Checks for edge-case overflow before exiting.

---

## Example Usage (in C)

```c
#include <stdint.h>
#include <stdio.h>

extern int64_t mdiv(int64_t *x, size_t n, int64_t y);

int main() {
    int64_t x[2] = {0, -5}; // represents -5 * 2^64
    int64_t divisor = -2;
    int64_t rem = mdiv(x, 2, divisor);

    printf("Quotient: %ld %ld\n", x[1], x[0]);
    printf("Remainder: %ld\n", rem);
    return 0;
}