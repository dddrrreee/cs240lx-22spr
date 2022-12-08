/**
 * Various math functions which give approximately-correct answers without
 * using any floating point instructions.  These are slower and less accurate
 * than using hardware support, but they can be useful if you don't want to
 * drag in libgcc just for a few math operations.  Especially useful is the
 * integer division algorithm, since the Raspberry Pi doesn't seem to have an
 * integer division instruction.
 */
#ifndef __MATH_HELPERS_H__
#define __MATH_HELPERS_H__
#include <stdint.h>

/**
 * Computes the absolute value of a number.
 */
static inline unsigned iabs(int x) {
  return __builtin_abs(x);
}

/**
 * Counts the number of leading zeros of a number.
 */
static inline unsigned iclz(int x) {
  return __builtin_clz(x);
}

/**
 * Divides a uint64_t by a uint32_t.
 *
 * From Hacker's Delight (Warren, 2nd Edition), p. 193.
 */
static inline uint32_t divlu(uint64_t a, uint32_t b) {
  int i;
  // The original algorithm provided this in terms of two 32-bit dividends and
  // a 32-bit divisor.
  uint32_t x = a >> 32 & 0xffffffff;
  uint32_t y = a & 0xffffffff;
  uint32_t z = b;
  uint32_t t;

  for (i = 1; i <= 32; i++) {
    t = (int)x >> 31;
    x = (x << 1) | (y >> 31);
    y = y << 1;
    if ((x | t) >= z) {
      x = x - z;
      y = y + 1;
    }
  }
  return y;
}

/**
 * Divides a uint32_t by a uint32_t.  Supplies the equivalent of x=0, as
 * suggested by the commentary on `divlu`.
 */
static inline int udiv(uint32_t a, uint32_t b) {
  return divlu(a, b);
}

/**
 * Divides an int32_t by an int32_t.
 *
 * From Hacker's Delight (Warren, 2nd Edition), p. 193.
 */
static inline int idiv(int32_t a, int32_t b) {
  if (a == 0) return 0;
  uint32_t result = udiv(iabs(a), iabs(b));
  if ((a < 0 && b < 0) || (a > 0 && b > 0)) {
    return result;
  } else {
    return -result;
  }
}

static inline int32_t imod(int32_t a, int32_t b) {
  if (b == 0) return a;
  int32_t mul = idiv(a, b);
  return a - (b * mul);
}

static inline uint32_t umod(uint32_t a, uint32_t b) {
  if (b == 0) return a;
  uint32_t mul = udiv(a, b);
  return a - (b * mul);
}

/**
 * Computes the integer square root of a number using shifts and subtractions.
 *
 * Originally from Hacker's Delight (Warren, 2nd Edition), p. 286.
 */
static inline int isqrt(unsigned x) {
  unsigned m, y, b;
  m = 0x40000000;
  y = 0;
  while (m != 0) {
    b = y | m;
    y = y >> 1;
    if (x >= b) {
      x = x - b;
      y = y | m;
    }
    m = m >> 2;
  }
  return y;
}

/**
 * Computes the integer cube root of a number using a hardware algorithm.
 *
 * Originally from Hacker's Delight (Warren, 2nd Edition), p. 288.
 */
static inline int icbrt(unsigned x) {
  int s;
  unsigned y, b;

  y = 0;
  for (s = 30; s >= 0; s = s - 3) {
    y = 2*y;
    b = (3*y*(y+1) + 1) << s;
    if (x >= b) {
      x = x - b;
      y = y + 1;
    }
  }
  return y;
}

/**
 * Computes the exponentiation of an integer using binary decomposition.
 *
 * Originally from Hacker's Delight (Warren, 2nd Edition), p. 289.
 */
static inline int iexp(int x, unsigned n) {
  int p, y;

  y = 1;
  p = x;

  while(1) {
    if (n & 1) y = p*y;
    n = n >> 1;
    if (n == 0) return y;
    p = p * p;
  }
}

#endif