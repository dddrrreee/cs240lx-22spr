#ifndef __CPYJMP_H__
#define __CPYJMP_H__

void cpyjmp_check(void);

// pointer to a routine that will copy <code> to <code_dst> and jump to it.
typedef 
   void (*cpyjmp_t)(uint32_t code_dst, const void *code, unsigned nbytes) 
        /* __attribute__((noreturn)) */;

unsigned cpyjmp_nbytes(void);


// relocate the copy jump code to <addr>
//      returns a point to the relocated copy routine.
cpyjmp_t cpyjmp_relocate(uint32_t addr);

// will never relocate below this: your original code/data must be 
// below here!
// enum { cpyjmp_lowest_reloc_addr = HIGHEST_USED_ADDR };

// default implementation / example.
//
// copies it above <cpyjmp_lowest_reloc_addr> and jumps to it
__attribute__((noreturn)) 
static inline void 
cpyjmp_default(uint32_t code_dst, const void *code, unsigned nbytes) {
    
    // uint32_t safe_addr = cpyjmp_lowest_reloc_addr + 8;
    uint32_t safe_addr = (uint32_t)kmalloc(cpyjmp_nbytes());

#   define MB(x) ((x) * 1024 * 1024)
    // make sure code is not suspiciously large.
    assert(nbytes < MB(1));
    // make sure code not suspiciualy high.
    assert(code_dst < MB(128));
    // no overlap w/ copy code.
    assert((code_dst+nbytes) < safe_addr || code_dst > (safe_addr+1024));

    cpyjmp_t cj = cpyjmp_relocate(safe_addr);

     // give any output a chance to flush.
    delay_ms(1);   
    cj(code_dst, code, nbytes);
    panic("should not return\n");
    // cj(code_dst, code, nbytes);

    // note: the code for this could get trashed so this is very iffy.
    clean_reboot();

#   undef MB
}
#endif
