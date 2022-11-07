module kernel.timer;

version (AArch64) import timer = kernel.arch.aarch64.timer;

version (ARM) import timer = kernel.arch.arm.timer;

version (RISCV64) import timer = kernel.arch.riscv64.timer;

version (RISCV32) import timer = kernel.arch.riscv32.timer;

alias delay_us = timer.delay_us;
alias init = timer.init;

void delay_ms(uint ms) {
    delay_us(ms * 1000);
}
