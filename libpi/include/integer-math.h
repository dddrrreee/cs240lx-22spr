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
#include "trig-tables.h"

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
  assert(b != 0);
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


#ifdef USE_NEWTONS_METHOD_ISQRT
/**
 * Computes the integer square root of a number using Newton's Method.
 *
 * Originally from Hacker's Delight (Warren, 2nd Edition), p. 281.
 */
static inline int isqrt(unsigned x) {
  unsigned x1;
  int s, g0, g1;
  if (x <= 1) return x;
  s = 16 - iclz(x - 1)/2;

  g0 = 1 << s;
  g1 = (g0 + (x >> s)) >> 1;

  while (g1 < g0) {
    g0 = g1;
    g1 = (g0 + divlu(x, g0)) >> 1;
  }
  return g0;
}
#else
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
#endif

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

/**
 * Computes the base-2 logarithm of a number.
 *
 * From Hacker's Delight (Warren, 2nd Edition), p. 291 (but it's also a trivial
 * algorithm in this particular case).
 */
static inline int ilog2x(unsigned x) {
  return 31 - __builtin_clz(x);
}

/**
 * Computes the base-10 logarithm of a number.
 *
 * From Hacker's Delight (Warren, 2nd Edition), p. 295.
 */
static inline int ilog10(unsigned x) {
   int y;
   static unsigned table2[11] = {0, 9, 99, 999, 9999,
      99999, 999999, 9999999, 99999999, 999999999,
      0xFFFFFFFF};

   y = (19*(31 - iclz(x))) >> 6;
   y = y + ((table2[y+1] - x) >> 31);
   return y;
}

/**
 * Computes the cosine of a number using a lookup table.
 * The lookup table contains the cosine of every integral degree from 0 to 90,
 * multiplied by 65535 (The maximum uint16_t).
 *
 * @param theta   The value to compute the cosine of, in degrees (0-360).
 * @return        The cosine of the value, scaled between -65536 and 65535.
 */
static inline int icos(int theta) {
  theta = iabs(theta % 360);
  if (theta == 0) return 65535;
  if (theta == 90) return 0;
  if (theta == 180) return -65536;
  if (theta == 270) return 0;
  int negate = 0;
  if (theta > 180) {
    theta = 360 - theta;
  }
  if (theta > 90) {
    negate = 1;
    theta = 180 - theta;
  }
  int value = cos_table[theta % 90];
  if (negate) {
    value *= -1;
  }
  return value;
}

/**
 * Computes the sine of a number using the cosine lookup table.
 *
 * @param theta   The value to compute the sine of, in degrees (0-360).
 * @return        The sine of the value, scaled between -65535 and 65535.
 */
static inline int isin(int theta) {
  return icos(90 - theta);
}

/**
 * Computes the tangent of a number using a lookup table.
 * The lookup table contains the cosine of every integral degree from 0 to 90,
 * multiplied by 256.
 *
 * @param theta   The value to compute the tangent of, in degrees (0-360).
 * @return        The tangent of the value, scaled by 256.
 */
static inline int itan(unsigned theta) {
  if (theta < 0) return -itan(-theta);
  theta = theta % 180;
  int negate = 0;
  if (theta > 90) {
    theta = 180 - theta;
    negate = 1;
  }
  int value = tan_table[iabs(theta)];
  if (negate) {
    value *= -1;
  }
  return value;
}

/**
 * Converts cartesian coordinates to polar coordinates.
 *
 * @param[in] x   The cartesian x-coordinate.
 * @param[in] y   The cartesian y-coordinate.
 * @param[out] r  The polar radius.
 * @param[out] t  The polar angle (in degrees).
 */
static inline void cartesian_to_polar(int x, int y, int *r, int *t) {
  *r = isqrt(x*x + y*y);
  int upper = 90;
  int lower = -90;
  int target = x != 0 ? iabs(idiv(y*256, x)) : 65536;
  while (upper != lower) {
    if (upper == lower + 1) {
      int a = itan(upper) - target;
      int b = target - itan(lower);
      if (a < b) {
        lower = upper;
      } else {
        upper = lower;
      }
      break;
    }
    int theta = (upper + lower) / 2;
    int tan_theta = itan(theta);
    if (tan_theta == target) {
      upper = theta;
      lower = theta;
      break;
    }
    if (tan_theta < target) {
      lower = theta;
    }
    if (tan_theta > target) {
      upper = theta;
    }
  }
  if (x >= 0 && y >= 0) {
    *t = upper;
  } else if (x >= 0 && y < 0) {
    *t = 270 + (90 - upper);
  } else if (x < 0 && y < 0) {
    *t = 180 + upper;
  } else if (x < 0 && y >= 0) {
    *t = 90 + (90 - upper);
  } else {
    *t = upper;
  }

}

#endif
