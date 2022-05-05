.globl cycle_cnt_read_fast
cycle_cnt_read_fast:
    mrc p15, 0, r0, c15, c12, 1
    bx lr
