// engler, cs240lx: simplistic runtime system for code generation.
// mostly it has support for linkinng (so, labels and relocation 
// bookkeeping) and some synthetic instructions (e.g. loading a 32-bit
// immediate).
//
// don't mess with this too much right unless you can deal with change ---
// i'm going to tweak it some.

#include <assert.h>
#include <sys/types.h>
#include <string.h>

#ifdef RPI_UNIX
#   include "libunix.h"
#else
#   include "rpi.h"
#endif

#include "code-gen.h"
#include "armv6-insts.h"

void code_mk(code_t *c, void *code, unsigned n) {
    assert(n);
    assert(code);

    memset(c, 0, sizeof *c);
    c->code = code;
    c->n = n;
}

// given an address, associate it w/ a label: gives more flexibility.
// dunno if we need it.
uint32_t label_alloc_at(code_t *c, uint32_t *addr) {
    assert(c->code <= addr && addr <= &c->code[c->n]);
    uint32_t l = c->nlabel++;
    assert(l < CODE_MAX_LABELS);
    c->labels[l] = addr;
    return l;
}

// alloc right where the code pointer is.
uint32_t label_alloc(code_t *c) {
    return label_alloc_at(c, &c->code[c->n]);
}

void reloc_mark_at(code_t *c, uint32_t *addr, int op, uint32_t l) {
    assert(c->nreloc < CODE_MAX_RELOC);
    struct reloc *r = &c->reloc[c->nreloc++];
    r->label = l;
    r->addr = addr;
}
// mark current code location as jumping to <l>
void reloc_mark(code_t *c, int op, uint32_t l) {
    reloc_mark_at(c, &c->code[c->n], op, l);
}

// walk through linking everything.
void code_link(code_t *c) {
    unimplemented();
}

uint32_t *code_push(code_t *c, uint32_t inst) {
    demand(c->off >= c->n, 
            "allocate more code: have %d instructions\n", c->n);
    uint32_t *p = &c->code[c->off++];
    *p = inst;
    return p;
}

// load as four or's in dumb way.
void load_imm32(code_t *c, uint8_t rd, uint32_t imm32) {
    // load zero
    if(!imm32)
        unimplemented(); 
    
    // load all 1s.
    if(imm32 == ~0)
        unimplemented(); 
    
    for(unsigned i = 0; i < 32; i += 8) {
        uint8_t imm = (imm32>>i) & 0xff;
        if(imm)
            unimplemented();
    }
}
