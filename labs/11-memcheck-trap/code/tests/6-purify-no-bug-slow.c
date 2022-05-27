// no bug: allocate a couple blocks, memset, free.
#include "rpi.h"
#include "purify.h"

void notmain(void) {
    trace("should have no error: running with verbose off\n");
    purify_init();

    memtrace_verbose_set(0);

    unsigned n0 = rpi_rand32() % 1024;
    unsigned n1 = rpi_rand32() % 1024;

    void *p0 = purify_alloc(n0);
    void *p1 = purify_alloc(n1);

    memset(p0, 0, n0);
    memset(p1, 0, n1);
    purify_free(p0);
    purify_free(p1);

    trace("SUCCESS: no bug\n");
}
