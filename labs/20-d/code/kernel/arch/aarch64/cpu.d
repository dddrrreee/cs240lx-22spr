module kernel.arch.aarch64.cpu;

void dsb() {
    asm {
        "dsb sy";
    }
}
