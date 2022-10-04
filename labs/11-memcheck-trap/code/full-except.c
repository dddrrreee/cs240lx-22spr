// simplistic setup so can:
//   (1) call prefetch and data abort with the complete register state needed
//     to switch to another process/another priviledged level.  (all 16 gp regs
//     and the spsr: not handling floating point)
//   (2) dynamically override the handlers for different exceptions.  previously
//      we used the same name to indicate which code would be called.  this
//      is faster (no load) but this makes it awkward to package code together 
//      b/c of duplicate symbol names.  (could just do self-modifying
//      code to patch the jump)
//
// limits:
//  - only handles data/prefetch abort (easy to change)
//  - only handles a single override: in reality may want a stack of these.
//  - pretty slow.  can do various tricks (JIT + customize and swap vectors).
//
// i think want to make it so things are pushed last in / first run.  that way
// you can setup the virtual memory with a handler of last resort and then 
// push other things on top of it that get to try first.   maybe have some
// way to tell it to stop calling subsequent handlers.  issue is that we don't
// know what is really going on.
#include "rpi.h"
#include "vector-base.h"
#include "full-except.h"

static full_except_t prefetch_handler, data_abort_handler;

// forward the prefetch abort exception to client supplied handler.
//
// could have the assembly do this directly.
void prefetch_abort_full_except(uint32_t regs[17], uint32_t spsr, uint32_t pc) {
    if(!prefetch_handler)
        panic("unhandled prefetch abort from pc=%x\n", pc);
    prefetch_handler(regs, spsr, pc);
}

// forward the data abort exception to client supplied handler.
void data_abort_full_except(uint32_t regs[17], uint32_t spsr, uint32_t pc) {
    if(!data_abort_handler)
        panic("unhandled data abort from pc=%x\n", pc);
    data_abort_handler(regs, spsr, pc);
}

// set client exception handler.
//
// overrides the existing one: could extend to push them 
// in a stack (so can do multiple tools).  would swap the
// vector in vector base reg
full_except_t full_except_set_prefetch(full_except_t h) {
    full_except_t o = prefetch_handler;
    prefetch_handler = h;
    return o;
}
full_except_t full_except_set_data_abort(full_except_t h) {
    full_except_t o = data_abort_handler; 
    data_abort_handler = h;
    return o;
}

// install the default exception vectors: client can
// have their own as long as they call the full except
// handlers above.
//
// <override_p>: overwrite whatever is there.
void full_except_install(int override_p) {
    extern uint32_t full_except_ints[];
    void *v = full_except_ints;

    if(override_p) 
        vector_base_set(v);
    else {
        void *addr = vector_base_get();
        if(!addr)
            vector_base_set(v);
        if(addr != v)
            panic("already have exception handlers installed: addr=%x\n", addr);
    }
}

