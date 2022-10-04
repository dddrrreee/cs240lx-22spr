// sanity check: 
//   check that we can take a domain protection fault and resume.
//
//   to isolate: we install our own exception handlers so
//   that memtrace system doesn't see the data abort.
#include "memtrace.h"
#include "mmu.h"

static volatile uint32_t expected_pc;

static int mem_handler(uint32_t pc, uint32_t addr, unsigned load_p) {
    panic("should not get a fault!\n");
}

void data_abort_vector(unsigned pc) {
    // put32/get32 these should be at the same location on everyone's pi
    trace("have a data abort at pc=%x, expected: %x\n", pc, expected_pc);
    assert(pc == expected_pc);
    trace("going to turn off tracing and resume!\n");
    memtrace_trap_disable();
}

void notmain() {
    trace("test that memtrace_init sets things up and doesn't crash\n");

    assert(!mmu_is_enabled());

    // override the memtrace exception handlers
    extern uint32_t default_vec_ints[];
    memtrace_init_default_v(mem_handler, default_vec_ints);
    memtrace_on();

    assert(mmu_is_enabled());

    // we disable trapping so, shouldn't trap.
    trace("about allocate: should not fault\n");
    memtrace_trap_disable();

    // these calls should not trap since we've disabled.
    uint32_t *p = kmalloc(4);
    trace("going to write: should succeed\n");
    put32(p, 13);
    assert(get32(p) == 13);

    // we enable trapping so should trap.
    trace("disabled heap access: should trap\n");
    memtrace_trap_enable();
    expected_pc = (uint32_t)get32;
    assert(get32(p) == 13);

    // one more trap to make sure resumption is ok.
    memtrace_trap_enable();
    trace("about to write: should get a trap\n");
    expected_pc = (uint32_t)put32;
    put32(p, 14);
    assert(get32(p) == 14);
    trace_clean_exit("success!!\n");
}
