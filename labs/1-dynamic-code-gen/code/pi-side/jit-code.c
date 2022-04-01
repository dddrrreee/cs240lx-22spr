#include "rpi.h"

// trivial piece of code to generate the routine:
//      int plus1(int x) { return x + 1; }
//
void notmain(void) {
    uint32_t code[2];

    code[0] = 0xe2800001; // add r0, r0, #1
    code[1] = 0xe12fff1e; // bx lr

    int (*plus_one)(int) = (void*)code;

    unsigned x = 0x12345678;
    printk("hello: plus1(%d) = %d\n", x, plus_one(x));
    clean_reboot();
}
