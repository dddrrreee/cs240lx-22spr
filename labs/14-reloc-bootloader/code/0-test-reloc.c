// simple test: copy the code over itself repeatedly.
#include "rpi.h"
#include "cpyjmp.h"

enum { N = 10 };

// gross hack: initializing it puts in the init segment where it
// won't get zeroed across runs.
volatile int cnt = 1;

void notmain(void) {
    extern char __code_start__, __code_end__;

    char *s = (void*)&__code_start__;
    char *e = (void*)&__code_end__;
    unsigned nbytes = e - s;

    cnt++;
    if(cnt <= N) {
        output("cnt=%d: about to copy our code over itself: nbytes=%d\n", cnt, nbytes);
        cpyjmp_default((unsigned)s, s, nbytes);
        panic("should not return!\n");
    } 

    output("stopping iteration cnt=%d\n", cnt);
}
