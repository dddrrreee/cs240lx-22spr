.globl _start
_start:
	not x1, x0
	not x2, x0
	not x3, x0
	not x4, x0
	not x5, x0
	not x6, x0
	not x7, x0
	not x8, x0
	not x9, x0
	not x10, x0
	not x11, x0
	not x12, x0
	not x13, x0
	not x14, x0
	not x15, x0
	not x16, x0
	not x17, x0
	not x18, x0
	not x19, x0
	not x20, x0
	not x21, x0
	not x22, x0
	not x23, x0
	not x24, x0
	not x25, x0
	not x26, x0
	not x27, x0
	not x28, x0
	not x29, x0
	not x30, x0
	not x31, x0
	add x1, x30, x31
	sub x2, x30, x31
	sub x3, x2, x1
	sub x4, x2, x31
	or x5, x2, x4
	or x6, x5, x1
	and x7, x2, x31
	and x8, x5, x6
	xor x9, x5, x1
	xor x10, x31, x30
	xor x11, x5, x6
	slli x12, x5, 2
	slli x13, x12, 2
	add x14, x13, x5
	slli x15, x14, 6
	srai x16, x31, 5
	srai x17, x11, 1
	srai x18, x15, 2
	srli x19, x15, 4
	srli x20, x31, 10
	srli x21, x31, 31
	add x26, x15, x20
	sub x31, x20, x15
	slt x29, x31, x26
	slt x30, x26, x31

	ebreak
