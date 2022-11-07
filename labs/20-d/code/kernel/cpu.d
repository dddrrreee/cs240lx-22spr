module kernel.cpu;

version (AArch64) import cpu = kernel.arch.aarch64.cpu;

version (ARM) import cpu = kernel.arch.arm.cpu;

version (RISCV32) import cpu = kernel.arch.riscv32.cpu;

alias dsb = cpu.dsb;
