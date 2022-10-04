// checks a single memory overflow.
#include "rpi.h"
#include "purify.h"

void notmain(void) {
    trace("should detect memory underflow at offset -1\n");
    purify_init();

    char *p = purify_alloc(4);
    trace("allocated [%x]: about to store\n", p);
    memset(p, 0, 4);
    p[-1] = 1;   // past end of block

    panic("should have caught the corruption before\n");
}
