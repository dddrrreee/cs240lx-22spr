module kernel.arch.arm.cpu;

version (LDC) {
    import ldc.llvmasm;
    void dsb() {
        __asm (
            "mcr p15, 0, r0, c7, c10, 4", ""
        );
    }
} else {
    void dsb() {
        asm {
            "mcr p15, 0, r0, c7, c10, 4";
        }
    }
}
