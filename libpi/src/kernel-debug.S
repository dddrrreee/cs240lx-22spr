#include "rpi-asm.h"

#define USER_STACK 0x70000000
#define SUPER_REG_STR 0x400000

reset_asm_debug:
  sub   lr, lr, #4
  bl    reset_vector

prefetch_abort_asm_debug:
  mov   sp, #INT_STACK_ADDR
  sub   lr, lr, #4
  push  {r0-r12,lr}
  mov   r0, lr
  bl    prefetch_abort_vector
  pop   {r0-r12,lr} 	   
  movs  pc, lr        
                 
data_abort_asm_debug:
  mov   sp, #INT_STACK_ADDR   
  sub   lr, lr, #8
  push  {r0-r12,lr}         
  bl    data_abort_vector
  pop   {r0-r12,lr} 	   
  movs  pc, lr


undefined_instruction_asm_debug:
  mov   sp, #INT_STACK_ADDR  
  sub   lr, lr, #4
  push  {r0-r12,lr} 
  mov   r0, lr
  mov   r1, sp
  bl    undefined_instruction_vector_debug
  pop   {r0-r12,lr} 
  add   lr, lr, #4      @ continue AFTER the undefined instruction.
  movs  pc, lr 

@ right now, this is a stupid 
@ swi handler that only handles one 
@ cases and does not branch to a function.
@ Fine for today, but you will need to change
@ this to add functionality.
software_interrupt_asm_debug:
    mov  sp, #INT_STACK_ADDR 
    mov  r1, #SUPER_REG_STR
    ldm  r1, {sp}
    pop  {r0-r12, lr}
    bx   lr

interrupt_asm_debug:
  mov   sp, #INT_STACK_ADDR   
  sub   lr, lr, #4
  push  {r0-r12,lr} 
  mov   r0, lr
  bl    interrupt_vector
  pop   {r0-r12,lr} 
  movs  pc, lr   

start_function:
.globl start_function
  push  {r0-r12, lr}
  mov   r1, #SUPER_REG_STR
  str   sp, [r1]
  msr   cpsr_c, #USER_MODE
  mov   sp, #USER_STACK
  blx   r0
  swi   #0 

.align 5; 
.globl  kernel_debug
kernel_debug: 
    b reset_asm_debug
    b undefined_instruction_asm_debug
    b software_interrupt_asm_debug
    b prefetch_abort_asm_debug
    b data_abort_asm_debug
    b reset_asm_debug
    b interrupt_asm_debug

