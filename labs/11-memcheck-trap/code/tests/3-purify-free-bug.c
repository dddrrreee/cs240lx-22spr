// checks a single memory overflow.
#include "rpi.h"
#include "purify.h"

void notmain(void) {
    trace("should detect use after free bug (store at offset 3)\n");
    purify_init();

    char *p = purify_alloc(4);
    purify_free(p); 
    p[3] = 1;
    panic("should have caught use-after-free before\n");
}
