#include "../include/cstart.h"
@ exit into normal arm mode .arm
@ Enter thumb mode from a arm function .thumb_func
@ Assembler directives
@ https://sourceware.org/binutils/docs/as/ARM-Directives.html

.cpu cortex-m0

@ Setup XIP Flash so we can read from it
.section ".text.boot"
.thumb
    @disable SSI
    ldr r0,=0x18000008
    ldr r1,=0x00000000
    str r1,[r0]

    @set baud rate to 8
    ldr r0,=0x18000014
    ldr r1,=0x00000008
    str r1,[r0]

    @setup SPI dataframe format and size
    ldr r0,=0x18000000
    ldr r1,=0x001F0300
    str r1,[r0]

    @enable spi register
    ldr r0,=0x180000F4
    ldr r1,=0x03000218
    str r1,[r0]

    @set number of data frames
    ldr r0,=0x18000004
    ldr r0,=0x00000000
    str r1, [r0]

    @enable SSI
    ldr r0,=0x18000008
    ldr r1,=0x00000001
    str r1,[r0]

    ldr r0, =0x10000000
    ldr r1, =0x20000000
    @ IMPORTANT!!!!!!: This how much code to copy over 0xf000 = 61KB
    @ Had a bug here for a while since I was only copying over 1KB
    ldr r2, =0x2000f000

    @TODO: Write loop to copy over contents from Flash to RAM
_boot_start_loop:
    ldr r3, [r0]
    str r3, [r1]
    add r0, #4
    add r1, #4
    @ is this correct?
    cmp r1, r2
    bne _boot_start_loop

    @ Tricky: Start at postion in ram after second stage boot which is 256 bytes
    ldr r0, =0x20000101
    bx r0

.section ".text.start"
@ _start is automatically run first
.globl _start
@ Tell machine this is where code will start in RAM
.thumb_func
_start:
   @ include constant in linker script
   ldr r0, = 0x20005000
   mov sp, r0
   bl cstart
   b .

.thumb_func
.globl PUT32
PUT32:
    str r1, [r0]
    bx lr

.thumb_func 
.globl GET32
GET32:
    ldr r0, [r0]
    bx lr

.globl DELAY
.thumb_func
DELAY:
    sub r0,#1
    bne DELAY
    bx lr
