.globl _start
_start:
	lui x1, 0x20
	auipc x2, 0x10
	auipc x3, 42
	lui x4, 0xbeef
	auipc x5, 0xbeef
	ebreak
