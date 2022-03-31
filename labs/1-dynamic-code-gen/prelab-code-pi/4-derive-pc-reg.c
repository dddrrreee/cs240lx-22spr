// figure out the value of <pc> register (r15) when read with a
// <mov> by using some code and reasoning about where stuff is.  
// 
// in general, its better to use the arch manual, but we do it 
// this way to get used to kicking the tires.
#include "rpi.h"

/*
 * from .list file:
 * 00008038 <pc_val_get>:
 *  8038:   e1a0000f    mov r0, pc
 *  803c:   e12fff1e    bx  lr
 */
uint32_t pc_val_get(void) {
    uint32_t pc;
    asm volatile ("mov %0, pc" : "=r" (pc));
    return pc;
}

void notmain() { 
    // the mov is the first instruction in the routine, so its
    // addrress and address of <pc_val_get> are the same.
    uint32_t fn = (uint32_t)pc_val_get;
    uint32_t pc = pc_val_get();
    int diff = pc - fn;

    output("inst addr=%x, pc=%x (diff=%d)\n", fn, pc, diff);
    // check layout: if we all have the same varions of gcc the
    // routine will be laid out at the same location in memory.
    assert(fn == 0x8038);
    // should be a difference of 8
    assert(pc - 8 == 0x8038);
}
