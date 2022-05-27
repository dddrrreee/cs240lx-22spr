#ifndef __MEMTRACE_H__
#define __MEMTRACE_H__
#include "rpi.h"
#include "procmap.h"

// we mix the public and internal interfaces to keep stuff simple: easy to 
// seperate.

// call first.
//
// setup the mappings in <pmap>; do any other
// setup for memory tracing (e.g., shadow mem).
//
// install the handlers <vec> [this part is janky since
// we don't override anything]

// client supplied handler to call on each faulting
// load or store:
//  - the address of the faulting instruction <pc>
//  - the faulting address <addr>
//  - whether it was a load (<load_p>=1) or store (<load_p>=0)
// 
//  return value: should we resume instruction?
//      - 1 yes
//      - 0 no.
//      [this lets client emulate or whatever]
//
// TODO:
//  - currently this interface just works for clients that only need
//    read-only observations of the address stream: likely would 
//    make sense to allow mutation of the given address etc.
//    should give an additional method that takes all the registers
typedef int (*memtrace_fn_t)(uint32_t pc, uint32_t addr, unsigned load_p);

// initialize memtracing: have to have a follow on memtrace_on call
// if you want to start tracing.  
void memtrace_init(procmap_t *pmap, memtrace_fn_t h, uint32_t vec[]);

// default setup: kmalloc starts at 1MB and is 1MB big.
//
// if you want shadow memory will have to make a different
// version.

// trap on each load from or store to [addr,addr+nbytes)
//  - must call before memtrace_init
int memtrace_trap_region(procmap_t *pmap, void *addr, unsigned nbytes) ;

static inline void 
memtrace_init_default_v(memtrace_fn_t h, uint32_t vecs[]) {
    unsigned mb = 1024*1024; 
    kmalloc_init_set_start(mb, mb);
    procmap_t p = procmap_default_mk(1);
    if(memtrace_trap_region(&p, kmalloc_heap_start(), 1024*1024) < 0)
        panic("tracking failed\n");
    memtrace_init(&p, h, vecs); 
}

#include "full-except.h"

static inline void 
memtrace_init_default(memtrace_fn_t h) {
    void *v = full_except_get_vec();
    memtrace_init_default_v(h, v);
}

// turn memtracing on.
void memtrace_on(void);
// turn memtracing off.
void memtrace_off(void);

// remove r/w access by changing domain perm
void memtrace_trap_enable(void);
// enable r/w access by changing domain perm
void memtrace_trap_disable(void);
// is trapping off?
int memtrace_is_trap_off(void);

// run <fn(arg)> with memory tracing: 
//  - returns 
// result  
//  - must have done <memtrace_init>
static inline int 
memtrace_fn(int (*fn)(void *), void *arg) {
    memtrace_on();
    int ret = fn(arg);
    memtrace_off();
    return ret;
}

extern int memtrace_verbose_p;
static inline void 
memtrace_verbose_set(int verbose_p) {
    memtrace_verbose_p = verbose_p;
}
#define mem_debug(msg, args...) \
    do { if(memtrace_verbose_p) debug(msg,args); } while(0)

#endif
