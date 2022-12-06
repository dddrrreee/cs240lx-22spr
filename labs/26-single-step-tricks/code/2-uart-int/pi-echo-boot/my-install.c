// engler, cs140e: driver for "bootloader" for an r/pi connected via 
// a tty-USB device.
//
// To make grading easier:
//              DO NOT MODIFY THIS CODE!
//              DO NOT MODIFY THIS CODE!
//              DO NOT MODIFY THIS CODE!
//              DO NOT MODIFY THIS CODE!
//              DO NOT MODIFY THIS CODE!
//
// You shouldn't have to modify any code in this file.  Though, if you find
// a bug or improve it, let me know!
//
#include <ctype.h>
#include <string.h>
#include <sys/stat.h>
#include <termios.h>
#include <ctype.h>

#define _SVID_SOURCE
#include <dirent.h>
#include <sys/stat.h>


#include "libunix.h"
#include "simple-boot.h"

static const char *ttyusb_prefixes[] = {
	"ttyUSB",	// linux
	"cu.SLAB_USB", // mac os
    // if your system uses another name, add it.
	0
};

static int filter(const struct dirent *d) {
    if (strncmp(d->d_name,ttyusb_prefixes[0], strlen(ttyusb_prefixes[0]))==0) return 1;
    if (strncmp(d->d_name,ttyusb_prefixes[1], strlen(ttyusb_prefixes[1]))==0) return 1;
    return 0;
}

// find the TTY-usb device (if any) by using <scandir> to search for
// a device with a prefix given by <ttyusb_prefixes> in /dev
// returns:
//  - device name.
// error: panic's if 0 or more than 1 devices.
char *find_ttyusb(void) {
    struct dirent **namelist;
    int n = scandir("/dev", &namelist, NULL, alphasort);
    int count = 0;
    char* c = malloc(32*sizeof(char));
    strcpy(c, "/dev/");

    while (n--) {
        if (filter(namelist[n])) {
            count++;
            strcat(c, namelist[n]->d_name);
        }
        free(namelist[n]);
    }
    free(namelist);

    if (count == 0) panic("0 devices found\n");
    if (count > 1)  panic("More than one device found\n");

    return c;
}


int main(int argc, char *argv[]) { 
    char *progname = argv[0];

    // we don't use the -exec option til lab 11.  we just put it in now to save
    // time later.
    char **exec_argv = 0;
    for(int i = 1; i < argc; i++) {
        if(strcmp(argv[i], "-exec") == 0) {
            argv[i++] = 0;
            if(i == argc)
                die("%s: bad use of -exec: need a program name\n", progname);
            argc = i-1;
            exec_argv = &argv[i];
            output("exec: <%s>\n", exec_argv[0]);
        }
    }

    // 1. get the name of the ttyUSB.
    char *dev_name;
    if(argc == 2)
        dev_name = find_ttyusb();
    else if(argc == 3)
        dev_name = argv[1];
    else
        die("%s: wrong number (n=%d) of arguments: expected 1 or 2\n", progname, argc-1);

    // 2. open the ttyUSB in 115200, 8n1 mode
    int fd = set_tty_to_8n1(open_tty(dev_name), B115200, 1);

    // 3. read in program
    char *pi_prog = argv[argc-1];
	unsigned nbytes;
    uint8_t *code = read_file(&nbytes, pi_prog);

    // 4. let's send it!
	output("%s: tty-usb=<%s> program=<%s> about to boot\n", progname, dev_name, pi_prog);
    simple_boot(fd, code, nbytes);

    // 5. echo output from pi
    if(!exec_argv)
        pi_echo(STDIN_FILENO, fd, dev_name);
    else
        panic("should not have an -exec yet\n");
	return 0;
}
