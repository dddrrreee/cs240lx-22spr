#ifndef __PTAG_H__
#define __PTAG_H__

/**************************************************************
 * code for loading programs (given that we don't assume a file system)
 * assumes they have been concatenated to the end of the binary using
 * ptag-linker
 *
 */
#include "ptag-structs.h"

// used on pi side for the linked list.
typedef struct ptag {
    struct ptag *next;
    ptag_hdr_t *hdr;
    union {
        ptag_prog_t *p;
    } u;
} ptag_t;



// get all the pi-tags: atm is just a single binary.
ptag_t *ptag_start(void);

// get next tag in the list.
ptag_t *ptag_next(ptag_t *p);

#if 0
// assumes the memory is free...
uint32_t ptag_load(ptag_t *p);
#endif

#endif
