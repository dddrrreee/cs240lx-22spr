// simple example of dynamically generating code: emit <ident>
// and <add_1> and call them.
#include "rpi.h"

//  from .list:
//  00008038 <ident>:
//      8038:   e12fff1e    bx  lr
unsigned ident(unsigned x) { 
    return x;
}

// from .list:
//  0000803c <add_1>:
//    803c:   e2800001    add r0, r0, #1
//    8040:   e12fff1e    bx  lr
unsigned add_1(unsigned x) { 
    return x+1; 
}
unsigned add_2(unsigned x) { 
    return x+2; 
}

void notmain() { 
	// emit code for <ident>
	unsigned ident_code[7];
	ident_code[0] =  0xe12fff1e; //   bx  lr

    // cast address of array to a function pointer.
	int (*fp)(int) = (typeof(fp))ident_code;

    // we can now call it like any other routine.
	printk("ident(10) = %d, fp(10) =%d\n", ident(10), fp(10));
	printk("ident(20) = %d, fp(20) =%d\n", ident(10), fp(20));

    // do some simple testing
    enum { N = 255 };
    for(unsigned x = 0; x < N; x++)
        assert(fp(x) == ident(x));
    output("ident passed %d tests\n", N);

    // -------------------------------------------------------------
    // emit code for <add_1>
	unsigned add1_code[7];
	add1_code[0] = 0xe2800001; // add r0, r0, #1
    add1_code[1] = 0xe12fff1e; //   bx  lr

    // cast to function pointer.
	fp = (typeof(fp))add1_code;

	printk("plus1(10) = %d, fp(10) =%d\n", add_1(10), fp(10));
	printk("plus1(20) = %d, fp(20) =%d\n", add_1(10), fp(20));

    // simple testing
    for(unsigned x = 0; x < N; x++)
        assert(fp(x) == add_1(x));
    output("add_1 passed %d tests\n", N);
}
