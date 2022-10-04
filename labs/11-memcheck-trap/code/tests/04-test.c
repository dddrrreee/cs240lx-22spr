// sanity check that simple memory trapping works (but no single
// step) by having the fault handler return 0.
//
// basically same test as 03.
#include "memtrace.h"
#include "mmu.h"

static volatile uint32_t expected_pc;
static volatile uint32_t nfaults = 0;

static int mem_handler(uint32_t pc, uint32_t addr, unsigned load_p) {
    trace("have a data abort at pc=%x, expected: %x\n", pc, expected_pc);
    assert(pc == expected_pc);
    trace("going to turn off tracing and resume!\n");
    nfaults++;
    memtrace_trap_disable();
    return 0;
}

// do some simple loads and stores: these should trap.
static void do_test(void) {
    assert(mmu_is_enabled());

    // we disable trapping so, shouldn't trap.
    trace("about allocate: should not fault\n");
    memtrace_trap_disable();

    uint32_t *p = kmalloc(4);
    trace("going to write: should succeed\n");
    put32(p, 13);
    assert(get32(p) == 13);


    // we enable trapping so should trap.
    trace("disabled heap access: should trap on %x\n", p);
    memtrace_trap_enable();
    expected_pc = (uint32_t)get32;
    assert(get32(p) == 13);
    trace("nfaults=%d\n", nfaults);
    assert(nfaults == 1);

    // one more time to make sure resumption is ok.
    memtrace_trap_enable();
    trace("about to write: should get a trap on %x\n", p);
    expected_pc = (uint32_t)put32;
    put32(p, 14);

    assert(get32(p) == 14);
    trace("nfaults=%d\n", nfaults);
    assert(nfaults == 2);
}

void notmain() {
    trace("test that memory trapping (no single step) doesn't crash\n");

    assert(!mmu_is_enabled());

    // override the memtrace exception handlers
    memtrace_init_default(mem_handler);
    memtrace_on();

    // turn off the single stepping/resume by returning 0 in the handler
    trace("-----------------------------------------------------\n");
    trace("checking that handler returning 0 works\n");
    nfaults = 0;
    do_test();
    trace("-------------SUCCESS with handler return 0 -------------------------\n\n");

    trace_clean_exit("success!!\n");
}
