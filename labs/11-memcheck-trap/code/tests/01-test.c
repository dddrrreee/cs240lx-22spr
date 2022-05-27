// sanity check: 
//   make sure we can setup all the memory checking.  
//   + we can run a routine at user level.
#include "memtrace-internal.h"

void trivial_user(void) {
    trace("at user level: going to read and write some stuff\n");

    int x = 12;
    assert(get32(&x) == 12);
    put32(&x, 14);
    assert(get32(&x) == 14);
    trace_clean_exit("successfully read/wrote memory\n");
    not_reached();
}

void notmain() {
    trace("test that can run at user level with vm\n");

    assert(!mmu_is_enabled());

    // should wrap this up better.
    enum { mb = 1024 * 1024 };
    kmalloc_init_set_start(mb, mb);
    procmap_t p = procmap_default_mk(1);
    pin_mmu_on(&p);
    assert(mmu_is_enabled());

    trace("about to run a routine at user level\n");
    run_fn_at_user_level(trivial_user);
    not_reached();
}
