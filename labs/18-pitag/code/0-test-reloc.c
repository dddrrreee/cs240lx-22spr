// simple test: copy the code over itself repeatedly.
#include "rpi.h"
#include "cpyjmp.h"

enum { N = 10 };

// gross hack: initializing it puts in the init segment where it
// won't get zeroed across runs.
volatile int cnt = 1;

void notmain(void) {
    extern char __code_start__, __code_end__;

    output("memsetting\n");
    memset(0, 0, 12);
#if 0
    volatile unsigned *addr = (void*)0x8;
    *addr = 0xe1000070    ;
    *addr = 0;
#endif
    output("done\n");

    char *s = (void*)&__code_start__;
    char *e = (void*)&__code_end__;
    unsigned nbytes = e - s;

    debug("here\n");
    cnt++;
    if(cnt <= N) {
        debug("here2\n");
        output("cnt=%d: about to copy our code over itself: %d\n", cnt, nbytes);
        cpyjmp_default((unsigned)s, s, nbytes);
        panic("should not return!\n");
    } 

        debug("here3\n");
    output("stopping iteration cnt=%d\n", cnt);
}
