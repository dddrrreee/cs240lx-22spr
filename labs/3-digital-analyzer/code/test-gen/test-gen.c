// simple example test generator: a lot of error.  you should reduce it.
#include "rpi.h"
#include "test-gen.h"
#include "cycle-count.h"
#include "cycle-util.h"

static inline unsigned cycle_cnt_read_fast() {
	unsigned _out;							                    
  	asm volatile ("MRC p15, 0, %0, c15, c12, 1" : "=r"(_out));	
	return _out;								                        

}
//helper for writing to gpio pin faster
static volatile unsigned *SET0 = (unsigned *)0x2020001c;
static volatile unsigned *CLR0 = (unsigned *)0x20200028;
static inline void fast_gpio_write(unsigned pin, unsigned val) {
    if (val)
        *SET0 = 1 << 21;
    else
        *CLR0 = 1 << 21;
}

// send N samples at <ncycle> cycles each in a simple way.
// a bunch of error sources here.
void test_gen(unsigned pin, unsigned N, unsigned ncycle) {
	
    unsigned ndelay = 0;
    unsigned start = cycle_cnt_read_fast();

    unsigned v = 1;
    for(unsigned i = 0; i < N; i++) {
        fast_gpio_write(pin, v);
        v = 1-v;
        ndelay += ncycle;
        // we are not sure: are we delaying too much or too little?
        delay_ncycles(start, ndelay);
    }
    unsigned end = cycle_cnt_read_fast();
    printk("expected %d cycles, have %d, v=%d\n", ncycle*N, end-start,v);
}


void notmain(void) {
    int pin = 21;
    gpio_set_output(pin);
    cycle_cnt_init();
	flush_all_caches();
	// enable_cache();

    // keep it seperate so easy to look at assembly.
    test_gen(pin, 20, CYCLE_PER_FLIP);

    clean_reboot();
}
