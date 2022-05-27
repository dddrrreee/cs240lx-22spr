// checks that trivial alloc and memset works.
#include "rpi.h"
#include "purify.h"

void notmain(void) {
    trace("should have no error\n");

    purify_init();
    char *p = purify_alloc(4);
    trace("allocated [addr=%x]: about to store\n", p);
    memset(p, 0, 4);
    trace("SUCCESS: no bug\n");
}
