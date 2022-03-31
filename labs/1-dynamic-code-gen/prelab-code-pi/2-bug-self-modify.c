// try to isolate down why we need to seperate the store from the 
// routine call --- similar happens on x86 (i believe) where you 
// need an indirect jump after SMC to flush the pipeline.
#include "rpi.h"

int check_reuse(void) { 
    // generate the code to return 11
	volatile unsigned code[3];
    code[0] = 0xe3a0000b;   // mov r0, #11
    code[1] = 0xe12fff1e;   // bx  lr

    // cast array address to function pointer.
	int (*fp)(void) = (int (*)(void))code;

    // call it.
    unsigned x = fp();
    if(x != 11)
        return 0;

    // change the code to return 10
    asm volatile (".align 5");

#if 0
    // ok: this fails: bumps to start of next?
    asm volatile ("nop");
    code[0] = 0xe3a0000a; // mov r0, #10
    asm volatile ("nop");


    // and this fixes
    asm volatile ("nop");
    code[0] = 0xe3a0000a; // mov r0, #10
    asm volatile ("nop");

    // or, if assign 2x
    code[0] = 0xe3a0000a; // mov r0, #10
    code[0] = 0xe3a0000a; // mov r0, #10

#endif

    // if you fp() call next in the same block will prefetch the 
    // stale value (i think)

    /* 1: */ code[0] = 0xe3a0000a; // mov r0, #10
    /* 2: */ asm volatile ("nop");   // if delete fails.
    /* 3: */ x = fp();

    if(x != 10)
        return 0;
    return 1;
}

// keep "minimal" amount of stuff in this routine so we can 
// look at disassembly easily
int check_no_reuse(void) { 
    {
        // generate the code to return 11
	    unsigned code[3];
        code[0] = 0xe3a0000b;
        code[1] = 0xe12fff1e;

        // cast array address to function pointer.
	    int (*fp)(void) = (int (*)(void))code;

        // call it.
        if(fp() != 11)
            return 0;
    }

    {
        // change the code to return 10
	    volatile unsigned code[3];
        code[0] = 0xe3a0000a;
        code[1] = 0xe12fff1e;

	    int (*fp)(void) = (int (*)(void))code;
        if(fp() != 10)
            return 0;
    }

    return 1;
}

void notmain() { 
    output("checking if code works when don't reuse memory\n");
    if(!check_no_reuse())
        panic("failed without reuse\n");
    output("SUCCESS: without memory reuse\n");

    output("checking result when reuse memory\n");
    if(!check_reuse())
        panic("failed when reused memory\n");
    output("SUCCESS: with memory reuse\n");
}
