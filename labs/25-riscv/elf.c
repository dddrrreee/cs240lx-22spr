#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "elf.h"
#include "rvsim.h"

// Loads an elf file stored in 'data' into the machine's memory, and
// initializes the PC (elf entrypoint) and BRK (heap start).
void machine_load(machine_t* m, char* data) {
    elfhdr_t* elf = (elfhdr_t*) data;

    assert(elf->magic == ELF_MAGIC);

    uint32_t max = 0;

    for (int i = 0, off = elf->phoff; i < elf->phnum; i++, off += sizeof(proghdr_t)) {
        proghdr_t* ph = (proghdr_t*) (data + off);
        if (ph->type != ELF_PROG_LOAD) {
            continue;
        }
        assert(ph->memsz >= ph->filesz);
        assert(ph->vaddr + ph->memsz >= ph->vaddr);

        if (ph->vaddr + ph->memsz > max) {
            max = ph->vaddr + ph->memsz;
        }

        memcpy(m->mem.data + ph->vaddr - m->mem.base, data + ph->off, ph->filesz);
        memset(m->mem.data + ph->vaddr - m->mem.base + ph->filesz, 0, ph->memsz - ph->filesz);
    }

    m->pc = elf->entry;
    m->brk = max;
}
