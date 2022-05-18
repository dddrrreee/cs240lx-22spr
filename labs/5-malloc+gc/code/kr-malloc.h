#ifndef __KR_MALLOC_H__
#define __KR_MALLOC_H__

// sorta gross.
#ifdef COMPILE_FOR_UNIX
#   include <unistd.h>
#   include <assert.h>
#   include <stdio.h>
#   include <stdlib.h>
#   include <string.h>

#   include "libunix.h"

// bad results if we use malloc/free when intend to use
// our internal [don't need this for rpi since libpi
// doesn't have]

#   define malloc "error: should not be using malloc!"
#   define free "error: should not be using malloc!"

#else

#   include "rpi.h"

union align {
        double d;
        void *p;
        void (*fp)(void);
};

typedef union header { /* block header */
	struct {
    		union header *ptr; /* next block if on free list */
    		unsigned size; /* size of this block */
  	} s;
  	union align x; /* force alignment of blocks */
} Header;

    // you have to implement this: fail if it tries to free.
    void *sbrk(long increment);

#endif

void *kr_malloc(unsigned nbytes);
void kr_free(void *ptr);

#endif
