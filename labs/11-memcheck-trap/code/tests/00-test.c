// sanity check: make sure we can setup all the memory checking.  
// using the pinned vm routines from lab 7.
//
// should complete without issue.
#include "rpi.h"
#include "pinned-vm.h"
#include "mmu.h"

void notmain() {
    output("test to sanity check pinned vm works\n");
    assert(!mmu_is_enabled());

    // should wrap this up better.
    enum { mb = 1024 * 1024 };
    kmalloc_init_set_start(mb, mb);
    procmap_t p = procmap_default_mk(1);
    pin_mmu_on(&p);
    assert(mmu_is_enabled());

    trace("if you see this, mmu is working\n");
    trace("about to read a couple things\n");

    int x = 12;
    assert(get32(&x) == 12);
    put32(&x, 14);
    assert(get32(&x) == 14);
    trace_clean_exit("success!!\n");
}
