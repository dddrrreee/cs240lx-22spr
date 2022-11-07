// how to interpret the binaries produced by our different memmaps.

typedef struct {
    uint32_t code_addr;
    const void *code;
    uint32_t nbytes;
} memmap_info_t;

// given an executable <code> check what type of binary it is.
static inline memmap_info_t memmap_info(const void *prog, unsigned nbytes) {
    uint32_t code_addr = 0x8000;
    const uint32_t *code = prog;

    // new style binary.
    if(code[0] == 0x12345678) {
        // address to copy to is at offset 2.
        code_addr = code[2];

        // sizeof of header
        uint32_t hdr_nbytes = code[1];

        // atm we only have 16 byte headers.
        assert(hdr_nbytes == 16);

        // nbytes is at offset 3
        uint32_t n = code[3] + hdr_nbytes;
        if(n != nbytes)
            panic("code claims %d bytes, pitag claimed %d bytes\n", n, nbytes);

        // skip the header so later code doesn't have to know about it.
        code_addr += hdr_nbytes;
        code += hdr_nbytes/4;
        nbytes -= hdr_nbytes;
    }

    return (memmap_info_t){ 
            .code_addr = code_addr,
            .code = code,
            .nbytes = nbytes
    };
}
