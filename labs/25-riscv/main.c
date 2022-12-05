#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>

#include "rvsim.h"

int main(int argc, char** argv) {
    if (argc <= 1) {
        printf("usage: rvsim ELF\n");
        return 1;
    }
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        printf("rvsim: file %s not found\n", argv[1]);
        return 1;
    }

    struct stat s;
    int status = fstat(fd, &s);
    assert(status == 0);

    char* fdata = mmap(NULL, s.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

    machine_t m;
    machine_new(&m, 0, 0x1000000);
    machine_load(&m, fdata);

    bool halt = false;

    int count = 0;

    while (!halt) {
        halt = machine_exec(&m);
        count++;
    }

    printf("executed instructions: %d\n", count);

#if DUMPREG
    printf("registers:\n");
    for (int i = 0; i < 32; i++) {
        printf("%d: %d\n", i, m.regs[i]);
    }
#endif

#if DUMPMEM
    printf("memory (0x100000-0x100100):\n");
    for (int i = 0x100000; i < 0x100100; i += 4) {
        uint32_t val = mem_read32(&m.mem, i);
        printf("%x: %d\n", i, val);
    }
#endif

    machine_free(&m);
    munmap(fdata, s.st_size);
}
