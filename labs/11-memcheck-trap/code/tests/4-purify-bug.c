
// checks a single memory overflow.
#include "rpi.h"
#include "purify.h"

void notmain(void) {
    trace("should detect use after free\n");
    purify_init();


    enum { N = 6 };
    void *p[N];
    unsigned sz[N];

    for(int i = 0; i < N; i++) {
        sz[i] = rpi_rand32() % 1024;
        p[i] = purify_alloc(sz[i]);
    }

    for(int i = 0; i < N; i++)
        purify_free(p[i]);

    int i = rpi_rand32() % N;
    char *x = p[i];
    unsigned off = sz[i] + rpi_rand32() % 32;
    trace("corrupting [%p] at offset %d [%p]\n", x, off, &x[off]);
    x[off] = 1;
    panic("should not reach here.\n");
}

