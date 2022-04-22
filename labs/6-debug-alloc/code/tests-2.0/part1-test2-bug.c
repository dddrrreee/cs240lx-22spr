// checks a single memory overflow.
#include "rpi.h"
#include "ckalloc.h"

void notmain(void) {
    printk("test2: memory overflow at offset 4\n");
    char *p = ckalloc(4);
    memset(p, 0, 4);
    p[4] = 1;   // past end of block
    ckfree(p);  // should catch this.

    panic("should not get here: should detect an error in ckfree\n");
    
}
