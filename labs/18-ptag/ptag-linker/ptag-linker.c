// trivial ptag linker that appends binaries and tags them.
//
// usage 
//  pitag-linker  A.bin B.bin out.ptag
//      A.bin = an "OS" pi program
//      B.bin = a "user-level" pi program
//      out.ptag = name of the output program.
//
// A should copy B where it wants to go.
#include <string.h>
#include "libunix.h"

// where all the structures are: shared with pi code
// so they argree on what each byte means.
#include "ptag-structs.h"

// no-op atm: for lab: EXTENSION: take a colon (':') seperated
// path string from your env and look for the binary in all of those
// locations.  saves a lot of extra typing.
const char *find_pi_binary(const char *s) { return s; }

// if you want to reduce output, just redefine
#if 0
#   define ldebug(args...) do { } while(0)
#else
#   define ldebug(args...) debug(args)
#endif

// EXTENSION: compute a checksum over the 
// entire output file (can use incremental hashes)
// and put this and the size at the start (use lseek
// to put it after you write everything).
typedef struct {
    int fd;
    unsigned tot_nbytes;
} file_t;


static inline ptag_hdr_t 
ptag_hdr_mk(file_t *f, uint32_t tag, uint32_t nbytes) {
    return (ptag_hdr_t){ 
        .tag = tag, 
        .tot_nbytes = nbytes + sizeof(ptag_hdr_t), 
        .expected_off = f->tot_nbytes 
    };
}

file_t file_mk(const char *file_name) {
    // create new file.
    int fd = create_file(file_name);
    assert(fd>0);
    return (file_t){ .fd = fd, };
}

// append a raw byte stream to <f>
void file_append_bytes(file_t *f, const void *data, unsigned nbytes) {
    write_exact(f->fd, data, nbytes);
    f->tot_nbytes += nbytes;
}

// append <file_name> to <f>
void file_append(file_t *f, const char *file_name) {
    unsigned nbytes;

    uint32_t *data = (void*)read_file(&nbytes, file_name);
        file_append_bytes(f, data, nbytes);
    free(data);
}

// read in <file_name> append to <f>
void ptag_append_prog(file_t *f, const char *file_name) {
    unsigned code_nbytes;
    uint32_t *code = (void*)read_file(&code_nbytes, file_name);

    // round up to 4byte alignment so crc doesn't spuriously differ
    // b/c of garbage (your read_file should be padding to this in any
    // case)
    code_nbytes = pi_roundup(code_nbytes,4);

    // total ptag entry size is header + all the code.
    uint32_t tot_nbytes = code_nbytes 
            + sizeof (ptag_prog_t);

    ptag_hdr_t hdr = ptag_hdr_mk(f, PTAG_BIN_FILE_TAG, tot_nbytes);

    ptag_prog_t p = { 
            .code_nbytes = code_nbytes, 
            // currently hardwired.  extension: get from the 
            // .bin if it has the right header.
            .code_addr = 0x8000,
            .code_crc = our_crc32(code, code_nbytes)
    };
    // prog_name[] is 0 initialized by compiler.
    strncpy(p.prog_name, file_name, sizeof p.prog_name-1);

    // if this is not true: you need to round up and handle so
    // that the checksum doesn't spuriously mismatch.
    assert(code_nbytes % 4 == 0);

    ldebug("append: progname=<%s>, addr=%x nbytes=%d,crc=%x code=[%x,%x...]\n",
                p.prog_name,
                p.code_addr,
                p.code_nbytes,
                p.code_crc,
                code[0], code[1]);


    file_append_bytes(f, &hdr, sizeof hdr);
    file_append_bytes(f, &p, sizeof p);
    file_append_bytes(f, code, code_nbytes);

    free(code);
}

int main(int argc, char *argv[]) {
    if(argc < 4)
        die("invalid number of arguments: expected three or more, have %d\n", argc);

    const char *in_file = argv[1];
    if(!suffix_cmp(in_file, ".bin") 
    && !suffix_cmp(in_file, ".ptag"))
        panic("file <%s> is not a .bin or .ptag file?\n", in_file);

    const char *out_file = argv[argc-1];

    const char *o = find_pi_binary(in_file);
    output("input program =<%s> out program = %s\n", in_file, out_file);
    in_file = o;

    // append original.
    file_t f = file_mk(out_file);
    file_append(&f, in_file);

    for(int i = 2; i < argc-1; i++) {
        const char *file = argv[i];

        if(!suffix_cmp(file, ".bin")
        && !suffix_cmp(file, ".ptag"))
            panic("file <%s> is not a .bin or .ptag file?\n", file);

        // output("mapping <%s>\n", f);
        const char *o = find_pi_binary(file);
        // output("mapped <%s> to <%s>\n", f, o);
        output("appending %s\n", o);
        ptag_append_prog(&f, o);
    }
    // at this point would emit the crc if we checksum'd the entire
    // output file.
    close(f.fd);

    return 0;
}
