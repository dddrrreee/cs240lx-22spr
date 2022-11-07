#define SYS_MODE   0b11111

#define prefetch_flush(reg) \
    mov reg, #0;            \
    mcr p15, 0, reg, c7, c5, 4

.section ".text.boot"

.globl _start
_start:
	# force the mode to be system
	mov r0, #SYS_MODE
	# disable interrupts
	orr r0, r0, #(1 << 7)
	msr cpsr, r0
	prefetch_flush(r1)

	ldr r0, =_start
	mov sp, r0
	# clear frame pointer
	mov fp, #0
	bl dstart
_hlt:
	wfe
	b _hlt

