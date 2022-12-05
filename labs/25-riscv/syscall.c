#include <unistd.h>

#include "rvsim.h"

int sys_write(machine_t* m, int fd, uint32_t buf, uint32_t size) {
    (void) fd;
    // write everything to stdout
    return write(1, &m->mem.data[buf], size);
}

int sys_close(machine_t* m, int fd) {
    (void) m;
    (void) fd;
    return 0;
}

uint32_t sys_brk(machine_t* m, uint32_t addr) {
    if (addr) {
        m->brk = addr;
    }
    return m->brk;
}

int sys_fstat(machine_t* m, int fd, uint32_t statbuf) {
    // fill the statbuf with zeroes
    for (int i = 0; i < 112; i += 4) {
        mem_write32(&m->mem, statbuf + i, 0);
    }
    return 0;
}
