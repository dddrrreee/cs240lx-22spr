#ifndef __CODE_GEN_H__
#define __CODE_GEN_H__

#define CODE_MAX_LABELS 32
#define CODE_MAX_RELOC 32

typedef struct {
    uint32_t *code;
    uint32_t n;
    unsigned off;

    // offset of where each label is located in <code>
    uint32_t *labels[CODE_MAX_LABELS];
    unsigned nlabel;

    struct reloc { 
        uint32_t label;
        uint32_t *addr;
        // should have this be a patch routine function pointer.
        int type;
    } reloc[CODE_MAX_RELOC];
    unsigned nreloc;

} code_t;

void code_mk(code_t *c, void *code, unsigned n);

// given an address, associate it w/ a label: gives more flexibility.
// dunno if we need it.
uint32_t label_alloc_at(code_t *c, uint32_t *addr);

// alloc right where the code pointer is.
uint32_t label_alloc(code_t *c);

void reloc_mark_at(code_t *c, uint32_t *addr, int op, uint32_t l);

// mark current code location as jumping to <l>
void reloc_mark(code_t *c, int op, uint32_t l);

// walk through linking everything.
void code_link(code_t *c);

// add an instruction to <c>
uint32_t *code_push(code_t *c, uint32_t inst);

// load a uint32 as four or's in dumb way.
void load_imm32(code_t *c, uint8_t rd, uint32_t imm32);

#endif
