#include "rpi.h"
#include "../unix-side/armv6-insts.h"

void notmain(void) {
    // print out the string in the header.

    // figure out where it points to!
	const unsigned header_string_addr = 0x00008004;
    const char *header_string = (char*)header_string_addr; 
	const unsigned fill = *(unsigned *)0x00008020;

    assert(header_string);
    printk("<%s>\n", header_string);
	printk("<%x>\n", fill);
    printk("success!\n");
    clean_reboot();
}
