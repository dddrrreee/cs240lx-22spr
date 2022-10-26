// extend so that it bundles a bunch of different
// binaries together.
//
// usage 
//  pitag-linker  A.bin B.bin out.ptag
//      A.bin = an "OS" pi program
//      B.bin = a "user-level" pi program
//      out.ptag = name of the output program.
//
// A should copy B where it wants to go.
//
// A holds:
//  BIN_TAG
//  total nbytes: strlen(string) + 1 + nbytes(B.bin)
//  0 terminated string
//  the binary

#include <string.h>
#include "libunix.h"
#include "pi-tag.h"

#ifndef PITAG_LINKER_PATH
#   define PITAG_LINKER_PATH "./"
#endif

int main(int argc, char *argv[]) {
    if(argc < 2)
        die("invalid number of arguments: expected one or more, have %d\n", argc);

    unsigned start = 1;
    const char *dev = "";
    if(prefix_cmp(argv[1], "/dev")) {
        dev = argv[1];
        output("device = <%s>\n", dev);
        start++;
    }


    char cmd[4096];
    sprintf(cmd, "%s/pitag-linker ", PITAG_LINKER_PATH);
    for(int i = start; i < argc; i++) {
        output("adding <%s> to <%s>\n", argv[i], cmd);
        strcatf(cmd, " %s ", argv[i]);
    }

    char out_file[1024];
    sprintf(out_file, "/tmp/tmp.pitag");
    // sprintf(out_file, "/tmp/%s.pitag", argv[argc-1]);
    strcat(cmd, out_file);
    // output("going to run command <%s>\n", cmd);
    run_system(cmd);

    // output("going to run command my-install %s %s <%s>\n", cmd);
    run_system("my-install %s %s", dev, out_file);

#if 0
    char *in_file = argv[1];
    if(!suffix_cmp(in_file, ".bin"))
        die("input file <%s> is not a .bin file?\n", in_file);


    char *out_file = argv[argc-1];

    output("input program =%s, out program = %s\n", in_file, out_file);

    pitag_t h = pitag_start_file(out_file, in_file);
    for(int i = 2; i < argc-1; i++) {
        const char *f = argv[i];

        if(!suffix_cmp(f, ".bin"))
            die("file <%s> is not a .bin file?\n", f);
        output("appending %s\n", f);
        pitag_add_bin_file(&h, f);
    }
    pitag_done(&h);
#endif

    return 0;
}
