#ifndef __ERASER_H__
#define __ERASER_H__

#include "src-loc.h"

void eraser_init(void);

// just for the debug macro.
#include "memtrace.h"

static inline void eraser_verbose_set(int v_p) {
    memtrace_verbose_set(v_p);
}

// different levels of eraser.
enum { ERASER_TRIVIAL = 1,
    ERASER_SHARED_EX = 2,
    ERASER_SHARED = 3,
    ERASER_HIGHEST = ERASER_SHARED
};

// Tell eraser that [addr, addr+nbytes) is allocated (mark as 
// Virgin).
void eraser_mark_alloc(void *addr, unsigned nbytes);

// Tell eraser that [addr, addr+nbytes) is free (stop tracking).
// We don't try to catch use-after-free errors.
void eraser_mark_free(void *addr, unsigned nbytes);

// mark bytes [l, l+nbytes) as holding a lock.
void eraser_mark_lock(void *l, unsigned nbytes);

// indicate that we acquired/released lock <l>
void eraser_lock(void *l);
void eraser_unlock(void *l);

// indicate that we are running thread <tid>
void eraser_set_thread_id(int tid);

#endif
