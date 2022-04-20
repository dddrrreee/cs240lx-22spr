#include "kr-malloc.h"

#define roundup(x,n) (((x)+((n)-1))&(~((n)-1)))

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


#error " implement this code"
