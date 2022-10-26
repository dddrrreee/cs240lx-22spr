// load a single binary using ptags and jump to it.
#include "rpi.h"
#include "ptag.h"
#include "cpyjmp.h"

void notmain(void) {
    ptag_t *t = ptag_start();

    // atm we are just handling a single binary; EXTENSION:
    // add ARGV and symtable options.
    assert(t->hdr->tag == PTAG_BIN_FILE_TAG);
    assert(!t->next);
    assert(!ptag_next(t));

    // we assume it's a dumb raw binary with no metadata we should
    // probe.
    ptag_prog_t *p = t->u.p;

    output("bootloader: about to run: <%s> at addr %x, nbytes=%d\n", 
        p->prog_name, p->code_addr, p->code_nbytes);

    // jump to it.
    cpyjmp_default(p->code_addr, p->code, p->code_nbytes);
    panic("IMPOSSIBLE: bootloaded code returned\n");
}
