#ifndef __EXCEPTION_FULL_REGS_H__
#define __EXCEPTION_FULL_REGS_H__
// exception handlers that load all registers [r0-r15 and spsr]
// can resume back.
#include "vector-base.h"


// offsets used for registers in <regs> array.
// exception trampoline should save in these
// locations.
enum {
    R_SP_OFF = 13,
    R_LR_OFF = 14,
    R_PC_OFF = 15,
    R_CPSR_OFF = 16
};

// can always pull <spsr> and <pc> from <regs> but they are 
// already around and this saves a cache miss.
typedef void (*full_except_t)(uint32_t regs[17], uint32_t spsr, uint32_t pc);

// call this first: <override_p>=0: give an error if there is already
// a vector installed
void full_except_install(int override_p);

// get exception vectors.
static inline void * full_except_get_vec(void) {
    extern uint32_t full_except_ints[];
    return full_except_ints;
}

// call to set prefetch
full_except_t full_except_set_prefetch(full_except_t h);
// call to set data abort handler
full_except_t full_except_set_data_abort(full_except_t h);

#endif
