.section ".text.boot"
.globl _start
_start:
    // force the mode to be SUPER.
    mov r0,  #0b10011
    orr r0,r0,#(1<<7)    
    msr cpsr, r0
    // prefetch flush 
    mov r1, #0
    mcr p15, 0, r1, c7, c5, 4

    mov sp, 0x8000000   // initialize stack pointer
    mov fp, #0          // clear frame pointer reg.  don't think needed.
    //@ bl notmain      // we could jump right to notmain

    bl zigMain          // call our code to do initialization.
    wfe               // if we return, wait for interrupts (simple way to sleep for now)


.globl put32
.globl PUT32
put32:
PUT32:
    str r1,[r0]     // store r1 into address held in r0
    bx lr           // return

.globl get32
.globl GET32
get32:
GET32:
    ldr r0,[r0]     // load address held in r0 into r0
    bx lr           // return



// do nothing: put the call here so the compiler cannot optimize.
.globl nop
nop:
    bx lr
