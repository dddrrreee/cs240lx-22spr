.section ".text.boot"

.globl _start
_start:
	la sp, 0x108000
	call dstart
_halt:
	j _halt
