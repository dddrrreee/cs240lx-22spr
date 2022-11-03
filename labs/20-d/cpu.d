module cpu;

void dsb() {
    asm {
        "mcr p15, 0, r0, c7, c10, 4";
    }
}
