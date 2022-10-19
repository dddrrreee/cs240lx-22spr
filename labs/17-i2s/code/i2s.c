#include "i2s.h"

volatile i2s_regs_t *i2s_regs = (volatile i2s_regs_t *)I2S_REGS_BASE;
volatile cm_regs_t *cm_regs = (volatile cm_regs_t *)CM_REGS_BASE;

#define addr(x) ((uint32_t)&(x))

void i2s_init(void) {
    unimplemented();
}

int32_t i2s_read_sample(void) {
    unimplemented();
}