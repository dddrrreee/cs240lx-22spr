#ifndef __BACKTRACE_H__
#define __BACKTRACE_H__

#include "rpi.h"
// Code to print backtrace using CS107e Assignment 4.
// See http://cs107e.github.io/assignments/assign4/.

const char *name_of(uintptr_t fn_start_addr)
{
    // The mpoke address is 4 bytes behind the function start address
    unsigned int* mpoke_addr = (unsigned int*)fn_start_addr - 1;
    if ((*mpoke_addr & 0xff000000) == 0xff000000) {
      return (char*)mpoke_addr - (*mpoke_addr & 0x00ffffff);
    }
    return "???";
}


void backtrace(void) {
    unsigned int* fp;
    __asm__("mov %0, fp" : "=r" (fp));

    while (*(fp - 3) != 0x0) {
        unsigned int lr = *(fp - 1);
        fp = (unsigned int*)(*(fp - 3));
        unsigned int* func_start = (unsigned int*)(*fp - 12);
        debug("# 0x%x at %s+%d\n", lr, name_of((uintptr_t)func_start), lr - (unsigned int)func_start);
    }

}

#endif
