#include "rpi.h"
#include "../unix-side/armv6-insts.h"

void notmain(void) {
    // print out the string in the header.

    // figure out where it points to!
    unimplemented();
    const char *header_string = 0;

    assert(header_string);
    printk("<%s>\n", header_string);
    printk("success!\n");
    clean_reboot();
}
