// simple program to demonstrate dynamic code generation
// on the x86 architecture (note: very very machine dependent)
// by dynamically generating code on for the identity function
//
// on linux, compile:
//  gcc -O2 -z execstack dcg.c
//
// how we do this depends alot on OS version.  its getting harder
// to generate code on heap/stack.  we use the 
//     gcc -z execstack
// option
//
// a nice discussion:
// https://stackoverflow.com/questions/68147140/which-feature-does-linux-kernel-update-that-lead-to-heap-not-executable-on-linux

#include <assert.h>
#include <stdio.h>

/*
  on my 64-bit machine, using 
    gcc -O2 -c dcg.c
	objdump --disassemble dcg.o
  gives:

    0000000000000000 <ident>:
    0:	f3 0f 1e fa          	endbr64 
    4:	89 f8                	mov    %edi,%eax
    6:	c3                   	retq   


  on a 32-bit machine:
    80483c4:       55                      push   %ebp
    80483c5:       89 e5                   mov    %esp,%ebp
    80483c7:       8b 45 08                mov    0x8(%ebp),%eax
    80483ca:       5d                      pop    %ebp
    80483cb:       c3                      ret    
*/
int ident(int x) {
	return x;
}

int main() {
    // Q: if you get rid of volatile?
    volatile unsigned char buf[1024] __attribute__((aligned (8)));
    assert((unsigned long)buf % 8 == 0);

    /* from <ident> above:
       0:	f3 0f 1e fa          	endbr64 
       4:	89 f8                	mov    %edi,%eax
       6:	c3                   	retq   
     */
    buf[0] = 0xf3; buf[1] = 0x0f; buf[2] = 0x1e; buf[3] = 0xfa;
    buf[4] = 0x89; buf[5] = 0xf8;
    buf[6] = 0xc3;

    /* cast buf's address to a function pointer. */
    int (*f)(int) = (typeof(f))buf;

    /* check that generated code returns right answer */
    printf("about to call ident\n");
    printf("%d = %d\n", 10, f(10));
    printf("%d = %d\n", 20, f(20));
    printf("done\n");
    return 0;
}
