#ifndef __VECTOR_BASE_SET_H__
#define __VECTOR_BASE_SET_H__
#include "libc/bit-support.h"
#include "asm-helpers.h"

/*
 * vector base address register:
 *   3-121 --- let's us control where the exception jump table is!
 *
 * defines: 
 *  - vector_base_set  
 *  - vector_base_get
 */

static inline void *vector_base_get(void) {
	void* ret;
	asm volatile("MRC p15, 0, %0, c12, c0, 0" : "=r" (ret):);
	return ret;
}

// set the vector register to point to <vector_base>.
// must: 
//    - check that it satisfies the alignment restriction.
static inline void vector_base_set(void *vector_base) {
	//check alignement
	unsigned mask = 0x1F;
	unsigned alignment = (unsigned)vector_base & mask;
	if (alignment != 0) {
		panic("invalid vector base address: not aligned. %x\n", (unsigned)vector_base);
	}
	asm volatile("MCR p15, 0, %0, c12, c0, 0" : : "r" (vector_base));
	// printk("%p\n", vector_base_get());
    assert(vector_base_get() == vector_base);
}
#endif
