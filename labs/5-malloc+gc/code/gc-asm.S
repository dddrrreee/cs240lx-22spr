#include "rpi-asm.h"

MK_FN(dump_regs)
	
	push {r0-r12} 		@ push registers up to sp (r13) to register array
	str sp, [r0, #0] 	@ store sp in r0 since we saved r0 already
	push {r0}			@ push the sp value to the register array
	push {lr, r15}		@ push lr and pc to the reg array

    bx lr;
