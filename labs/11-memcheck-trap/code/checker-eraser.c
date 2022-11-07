// engler: cs240lx: a trivial eraser checker that uses our memtrace
// infrastructure.
//
// this initial checker works but is degenerate: assumes a single
// piece of memory and a single shadow state (global variable).
//
// you should generalize to more state (of course) and to the 
// complete eraser lockset state machine.
//
// tons of other limits.  the more you remove the more you understand!
#include "memtrace.h"
#include "eraser.h"

// eraser states.
enum {
    SH_INVALID     = 1 << 0,
    SH_VIRGIN     = 1 << 1,      // writeable, allocated
    SH_FREED       = 1 << 2,
    SH_SHARED = 1 << 3,
    SH_EXCLUSIVE   = 1 << 4,
    SH_SHARED_MOD      = 1 << 5,
};

typedef struct {
    unsigned char state;
    unsigned char tid;
    unsigned short ls;
} state_t;

_Static_assert(sizeof(state_t) == 4, "invalid size");




_Static_assert((SH_INVALID ^ SH_VIRGIN ^ SH_FREED ^ SH_SHARED
                ^ SH_EXCLUSIVE ^ SH_SHARED_MOD) == 0b111111,
                "illegal value: must not overlap");



/**************************************************************
 * simplistic eraser state: assumes one lock per thread and one
 * allocation (you should change this)
 */

// our initial ridiculous implementation has a single piece of state.
static state_t state = { .state = SH_INVALID };

// current thread lockset: just assume a single lock.
#define MAXTHREADS 4
static int *lockset[MAXTHREADS];
static int cur_thread_id = 0;

// single contiguous heap.
static void * heap_start, *heap_end;

enum { VERBOSE_P = 1 };
#define etrace(args...) do {    \
    if(VERBOSE_P)               \
        { debug("TRACE:ERASER:tid=%d:", cur_thread_id); output(args); }\
} while(0)


/*****************************************************************
 * internal routines.
 */

// is addresss <addr> in the heap?  if so can check.
// 
// [currently we only fault on the heap, so is just
// a sanity check.]
static inline int in_heap(uint32_t addr) {
    void *p = (void*)addr;
    if(p >= heap_start && p < heap_end)
        return 1;
    return 1;
}

// given address <addr> return its associated shadow state.
static state_t *sh_lookup(uint32_t addr) {
    if(!in_heap(addr))
        return 0;

    return &state;
}
static state_t *sh_lookup_ptr(void *addr) {
    return sh_lookup((uint32_t)addr);
}

// mark [addr, addr+nbytes) as having state <state>.
//
// you'll need to change this for multiple allocations.
static void sh_mark_range(void *addr, uint32_t nbytes, unsigned state) {
    // again we assume one allocation.
    assert(nbytes == 4);

    state_t *s = sh_lookup_ptr(addr);
    assert(s);
    s->state = state;
}

// called on each load/store to <addr>: intersect <addr>'s 
// lockset with that of the current thread.
static int 
ls_intersect(state_t *s, int load_p, uint32_t pc, uint32_t addr) {
    assert(cur_thread_id);
    int *ls = lockset[cur_thread_id];

    // for level 0: if we hold any lock assume we're ok.
    if(ls == 0)
        etrace("empty lockset for addr=%p!\n", addr);
    return ls != 0;
}

// check that a store is legal
// XXX: need to know the size of the load/store.
static int shadow_check_st(uint32_t pc, uint32_t addr) {
    state_t *s = sh_lookup(addr);
    if(!s)
        panic("untracked load: pc=%p: addr=%p: failing right away\n", pc, addr);

    if(!ls_intersect(s, 0, pc, addr))
        panic("store error at pc=%p, addr=%p: lockset empty!\n", pc, addr);

    etrace("store at pc=%p, addr=%p checked out\n", pc, addr);
    return 1;
}

// check that a load is legal.
// XXX: you need to know the size of the load/store.
static int shadow_check_ld(uint32_t pc, uint32_t addr) {
    state_t *s = sh_lookup(addr);
    if(!s)
        panic("untracked store: pc=%p addr=%p: failing right away\n", pc, addr);

    if(!ls_intersect(s, 1, pc, addr))
        panic("load error at pc=%p, addr=%p: lockset empty!\n", pc, addr);

    etrace("load at pc=%p, addr=%p checked out\n", pc, addr);
    return 1;
}

// exception handler called by memtrace to handle a domain protection fault 
// caused by a load (<load_p>=1) or store (<load_p>=0) to <addr>.
//
//  - <regs> has the full set of registers.  you can modify these to 
//    change resumption behavior.  you'll have to disassemble the 
//    faulting instruction to get what registers it uses.
//
//  - note: called with trapping protection disabled.  if you want to 
//    resume directly will need to re-enable.
static int eraser_handler(uint32_t pc, uint32_t addr, unsigned load_p) {
    const char *op = load_p ? "load from" : "store to";

    // can be a lot of output.  for today, we leave it.
    etrace("ERASER: [pc=%x]: %s address %x\n", pc, op, addr);

    if(!in_heap(addr))
        panic("\t%x is not a heap addr: how are we faulting?\n", addr);

    if(load_p) {
        if(!shadow_check_ld(pc, addr))
            panic("shadow failed\n");
    } else {
        if(!shadow_check_st(pc, addr))
            panic("shadow failed\n");
    }

    // rerun the memory instruction.
    return 1;
}

/******************************************************************
 * public eraser routines: you would modify your thread implementation
 * to call these.  see "fake-thread.h" in the tests directory.
 */

// Tell eraser that [addr, addr+nbytes) is allocated (mark as 
// Virgin).
void eraser_mark_alloc(void *addr, unsigned nbytes) {
    // only handle one allocation right now: hou should change this.
    assert(nbytes == 4);
    assert(state.state == SH_INVALID);

    assert(nbytes % 4 == 0);
    etrace("in mark_alloc: addr=%p, nbytes=%d\n", addr, nbytes);
    sh_mark_range(addr, nbytes, SH_VIRGIN);
}

// Tell eraser that [addr, addr+nbytes) is free (stop tracking).
// We don't try to catch use-after-free errors.
void eraser_mark_free(void *addr, unsigned nbytes) {
    assert(nbytes % 4 == 0);
    etrace("in mark_free: addr=%p, nbytes=%d\n", addr, nbytes);
    sh_mark_range(addr, nbytes, SH_FREED);
}

// called on lock to add <l> to current thread's lockset.
void eraser_lock(void *l) {
    int id = cur_thread_id;
    assert(id < MAXTHREADS);

    if(lockset[id])
        panic("thread id=%d: only handling one lock: <%p>\n", id, l);
    etrace("thread id=%d: acquired lock <%p>\n", id, l);
    lockset[id] = l;
}

// called on unlock to remove <l> from current thread's lockset.
void eraser_unlock(void *l) {
    int id = cur_thread_id;
    assert(id < MAXTHREADS);

    if(lockset[id] != l)
        panic("thread id=%d: releasing unheld lock: <%p>\n", id, l);
    etrace("thread id=%d: released lock <%p>\n", id, l);
    lockset[id] = 0;
}

// tell eraser id of current thread: called on each cswitch.
// until this is called, nothing is "running".
void eraser_set_thread_id(int tid) {
    assert(tid);
    assert(tid < MAXTHREADS);
    cur_thread_id = tid;
}

// initialize the memtrace system.
void eraser_init(void) {
    // if you want to add shadow memory: just split heap 
    // in half (make sure you update in_heap).  
    //      - key: client can't corrupt b/c we'd trap.
    memtrace_init_default(eraser_handler);

    // gross that this is hardcoded.
    heap_start = kmalloc_heap_start();
    heap_end = heap_start + 1024*1024;

    memtrace_on();
}
