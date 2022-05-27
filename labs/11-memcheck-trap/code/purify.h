#ifndef __PURIFY_H__
#define __PURIFY_H__

#include "src-loc.h"


void purify_init(void);
#define purify_alloc(_n) purify_alloc_raw(_n, SRC_LOC_MK())
#define purify_free(_ptr) purify_free_raw(_ptr, SRC_LOC_MK())

void *purify_alloc_raw(unsigned n, src_loc_t loc);
void purify_free_raw(void *p, src_loc_t loc);

int purify_heap_errors(void);

// just for the debug macro.
#include "memtrace.h"

static inline void purify_verbose_set(int v_p) {
    memtrace_verbose_set(v_p);
}
#endif
