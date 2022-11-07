.section ".text.boot"

.globl _start
_start:
    // check processor ID is zero (executing on main core), else hang
    mrs     x1, mpidr_el1
    and     x1, x1, #3
    cbz     x1, 1f
    // we're not on the main core, so hang in an infinite wait loop
_hlt:
    wfe
    b       _hlt
1:

    // set stack to start below our code
    ldr     x1, =_start
    mov     sp, x1

	bl dstart
	b _hlt
