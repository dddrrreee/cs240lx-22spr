module kernel.arch.riscv32.cpu;

void dsb() {
    asm {
        "fence";
    }
}
