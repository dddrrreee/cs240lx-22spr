// checks a single memory overflow.
#include "rpi.h"
#include "purify.h"

void notmain(void) {
    trace("should detect memory overflow at 1 byte past block end\n");

    purify_init();
    char *p = purify_alloc(4);
    trace("allocated [addr=%x]: about to store\n", p);
    memset(p, 0, 4);
    p[4] = 1;   // one past end of block

    trace("should have caught the corruption before now!\n");
}
