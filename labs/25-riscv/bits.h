#pragma once

#include <stdint.h>

static inline uint32_t bit_clr(uint32_t x, unsigned bit) {
    return x & ~(1 << bit);
}

static inline uint32_t bit_set(uint32_t x, unsigned bit) {
    return x | (1 << bit);
}

static inline uint32_t bit_assign(uint32_t x, unsigned bit, unsigned val) {
    x = bit_clr(x, bit);
    return x | (val << bit);
}

#define bit_isset bit_is_on
#define bit_get bit_is_on

static inline unsigned bit_is_on(uint32_t x, unsigned bit) {
    return (x >> bit) & 1;
}
static inline unsigned bit_is_off(uint32_t x, unsigned bit) {
    return bit_is_on(x, bit) == 0;
}

static inline uint32_t bits_mask(unsigned nbits) {
    if (nbits == 32)
        return ~0;
    return (1 << nbits) - 1;
}

static inline uint32_t bits_get(uint32_t x, unsigned ub, unsigned lb) {
    return (x >> lb) & bits_mask(ub - lb + 1);
}

static inline uint32_t bits_clr(uint32_t x, unsigned ub, unsigned lb) {
    uint32_t mask = bits_mask(ub - lb + 1);
    return x & ~(mask << lb);
}

static inline uint32_t bits_set(uint32_t x,
                                unsigned ub,
                                unsigned lb,
                                uint32_t v) {
    return bits_clr(x, lb, ub) | (v << lb);
}

static inline uint32_t bit_remap(uint32_t i, unsigned from, unsigned to) {
    return bit_get(i, from) << to;
}

static inline uint32_t bits_remap(uint32_t i, unsigned from_ub, unsigned from_lb,
        unsigned to_ub, unsigned to_lb) {
    return bits_get(i, from_ub, from_lb) << to_lb;
}


static inline int sext(unsigned x, unsigned width) {
    unsigned n = 31 - (width-1);
    return ((int)(x << n)) >> n;
}

