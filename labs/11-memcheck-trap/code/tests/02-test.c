// sanity check: 
//   check that memtrace_init does not fault [we don't
//   reference heap so should work]
#include "memtrace-internal.h"

static int mem_handler(uint32_t pc, uint32_t addr, unsigned load_p) {
    panic("should not get a fault!\n");
}

void notmain() {
    trace("test that memtrace_init sets things up and doesn't crash\n");

    assert(!mmu_is_enabled());
    memtrace_init_default(mem_handler);
    memtrace_on();
    assert(mmu_is_enabled());

    trace("about to reference stack memory: should not fault\n");
    int x = 12;
    assert(get32(&x) == 12);
    put32(&x, 14);
    assert(get32(&x) == 14);
    trace_clean_exit("success!!\n");
}
