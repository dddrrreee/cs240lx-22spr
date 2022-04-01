#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "libunix.h"

// read entire file into buffer.  return it.   zero pads to a 
// multiple of 4.
//
// make sure to cleanup!
void *read_file(unsigned *size, const char *name) {
    int fd = open(name, O_RDONLY);
    if(fd < 0)
        sys_die(open, "can't open pi program file: <%s>\n", name);

    // get the size (and other attributes)
    struct stat s;
    if(fstat(fd, &s) < 0)
        sys_die(fstat, "can't stat file <%s>\n", name);

    // allocate buffer and read the file contents into it.   allocate
    // worst case extra bytes.
    uint8_t *buf = calloc(1,s.st_size+4);
    assert(buf);

    int n = read(fd, buf, s.st_size);
    if(n != s.st_size)
        panic ("bad read file <%s>: got %d bytes expected %lu\n",
                        name, n, (unsigned long) s.st_size);
    close(fd);

    *size = n;
    return buf;
}

int read_file_noalloc(const char *name, void *buf, unsigned maxsize) {
    int fd = open(name, O_RDONLY);
    if(fd < 0)
        sys_die(open, "can't open pi program file: <%s>\n", name);

    int res = read(fd, buf, maxsize);
    if(res < 0)
        sys_die(read, read failed);
    if(res == maxsize)
        panic("buffer too small\n");
    close(fd);
    return res;
}
