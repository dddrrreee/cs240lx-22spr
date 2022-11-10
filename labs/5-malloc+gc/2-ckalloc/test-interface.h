#ifndef __TEST_INTERFACE_H__
#define __TEST_INTERFACE_H__

#include "kr-malloc.h"

#ifdef COMPILE_FOR_UNIX
#   include "libunix.h"

    void notmain(void);
    void clean_reboot(void);

#else
#   include "rpi.h"

    // you have to implement this: fail if it tries to free.
    void *sbrk(long increment);
#endif
#endif
