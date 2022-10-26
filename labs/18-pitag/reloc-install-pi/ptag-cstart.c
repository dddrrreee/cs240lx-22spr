// grab our home grown PTAG
#include "rpi.h"
#include "cycle-count.h"
#include "libc/crc.h"
#include "ptag.h"
#include "libc/helper-macros.h"

#define E ptag_t 
#include "libc/Q.h"
static Q_t tagQ;

// compute size of ptag region and move it out of the
// way of the bss.
static void *ptag_move(unsigned *out_nbytes, unsigned *out_ntags) {
    extern char __data_end__;
    extern char __heap_start__;

    void *dst = &__heap_start__;
    void *src = &__data_end__;

    // compute the size of the ptag list
    ptag_hdr_t *h = src;
    unsigned ntags = 0;
    unsigned nbytes = 0;
    for(; h->tag == PTAG_BIN_FILE_TAG; ntags++) {
        nbytes += h->tot_nbytes;
        h = (void*)((char*)h + h->tot_nbytes);
    }

    // for today we always expect a tag: you should remove this.
    assert(ntags > 0);
    assert(nbytes > 0);

    // move it out of the way.  this is very sleazy.
    mb();
    memmove(dst, src, nbytes);
    mb();

    *out_nbytes = nbytes;
    *out_ntags = ntags;
    return dst;
}

// have to do this before you do anything else.
// currently is pretty iffy.
static void ptag_make_list(void *addr, unsigned nbytes, unsigned ntags) {
    unsigned onemb = 1024 * 1024;

    // oh: this trashes it.
    Q_init(&tagQ);

    // start heap after
    void *end = (char*)addr + nbytes;
    // this is for today: not an inherent limitation.
    if(end >= (void*)onemb)
        panic("ptags %p should be below heap location %p\n", addr,onemb);

    kmalloc_init_set_start(onemb, onemb);

    ptag_hdr_t *h = addr;
    for(int i = 0; i < ntags; i++) {
        ptag_prog_t *p = (void*)&h->data[0];

        // could this get swapped?
        output("found record for:\n\tprogname=<%s>,\n\taddr=%x nbytes=%d,\n\tcrc=%x code=[%x,%x...]\n", 
                p->prog_name,
                p->code_addr,
                p->code_nbytes,
                p->code_crc,
                p->code[0], p->code[1]);

        uint32_t crc = our_crc32(p->code, p->code_nbytes);
        if(crc != p->code_crc)
            panic("expected crc=%x, computed=%x [nbytes=%d]\n", 
                    p->code_crc, crc, p->code_nbytes);

        ptag_t *t = kmalloc(sizeof *t);
        t->hdr = h;
        t->u.p = p;
        Q_append(&tagQ, t);

        h = (void*)((char*)h + h->tot_nbytes);
    }
}

// get next tag in the list.
ptag_t *ptag_next(ptag_t *p) {
    return Q_next(p);
}

ptag_t *ptag_start(void) {
    return Q_start(&tagQ);
}

void _cstart() {
    extern int __bss_start__, __bss_end__;
	void notmain();

    // move out of the way so bss initialization does not kill
    // it.  we do it in a dangerous way.
    unsigned nbytes,ntags;
    void *addr = ptag_move(&nbytes,&ntags);

    int* bss = &__bss_start__;
    int* bss_end = &__bss_end__;
 
    while( bss < bss_end )
        *bss++ = 0;

    // wait: we don't call uart init?

    ptag_make_list(addr,nbytes,ntags);
    
    custom_loader();

    // is there a downside?
    cycle_cnt_init();
    notmain(); 
	clean_reboot();
}
