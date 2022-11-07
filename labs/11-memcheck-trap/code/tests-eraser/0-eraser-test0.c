// engler: doesn't do anything interesting: just checks translation 
// and mmu on/off as expected.
#include "eraser.h"

// for the checks.
#include "memtrace-internal.h"

int x = 0x12345678;

void notmain(void) {
    trace("check that we are running at in superuser mode\n");

    assert(!mmu_is_enabled());
    eraser_init();

    assert(mmu_is_enabled());
    assert(mode_eq(SUPER_MODE));
    //make sure translation works.
    assert(x == 0x12345678);

    int *y = eraser_alloc(4);
    *y = 1;
    assert(*y == 1);

    memtrace_off();
    // still on, just don't trap.  [not that intuitive]
    assert(mmu_is_enabled());

    trace("trivial asserts should pass\n");
}
