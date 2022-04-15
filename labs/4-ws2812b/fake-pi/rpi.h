#ifndef __RPI_H__
#define __RPI_H__
// begin defining our r/pi library.

#include <stdint.h> // uint32_t

void put32(volatile void *addr, uint32_t v);
void PUT32(uint32_t addr, uint32_t v);

void delay_cycles(unsigned  ncyc);

// #include <stdio.h>

// // could also #define this if we included printf prototype
// void printk(const char *msg, ...);
unsigned cycle_cnt_get(void);

// this is sketch.
#include <stdio.h>

#define printk output

// print output
#define output(msg, args...) \
    do { printf(msg, ##args ); fflush(stdout); } while(0)

// print output with file/function/line
#define debug(msg...) do {                                           \
    output("%s:%s:%d:", __FILE__, __FUNCTION__, __LINE__);          \
    output(msg);                                                  \
} while(0)

// panic and die with an error message.
#define panic(msg, args...) \
    do { debug("PANIC:" msg, ##args); exit(1); } while(0)



#endif
