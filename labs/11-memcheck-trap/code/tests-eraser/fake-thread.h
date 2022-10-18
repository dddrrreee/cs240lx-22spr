#ifndef __FAKE_THREAD_H__
#define __FAKE_THREAD_H__

/************************************************************************
 * this is what you would do to your posix implementation.
 */

static inline void lock_init(int *l) {
    eraser_mark_lock(&l, sizeof l);
}

static inline void lock(int *l) {
    eraser_lock(l);
}

static inline void unlock(int *l) {
    eraser_unlock(l);
}

// for mmu_is_enabled
#include "memtrace-internal.h"


static inline void *alloc(unsigned n) {
    // need a better check for tracing.
    assert(mmu_is_enabled());
    memtrace_off();
        void *ptr = kmalloc(n);
        eraser_mark_alloc(ptr, n);
    memtrace_on();
    return ptr;
}

#define kmalloc alloc

#endif
