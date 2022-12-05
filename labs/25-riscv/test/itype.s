.globl _start
_start:
	addi x1,   x0,    63  # x1 = 63
	andi x2,   x1,    67  # x2 = 3
	ori  x3,   x2,    64  # x3 = 67
	xori x4,   x3,    64  # x4 = 3
	ori  x5,   x0,    -1  # x5 = -1
	slti x6,   x0,    0   # x6 = 0
	slti x7,   x0,    10  # x7 = 1
	slli x8,   x7,    20  # x8 = 0x100000
	addi x8,   x8,    4   # x8 = 0x100004
	sw   x1,   0(x8)      # 0x4 = 63
	sw   x2,   -4(x8)     # 0x0 = 3
	sw   x3,   4(x8)      # 0x8 = 67
	lw   x9,   0(x8)      # x9 = 63
	lw   x10,  -4(x8)     # x10 = 3
	lw   x11,  4(x8)      # x11 = 67

	ebreak

