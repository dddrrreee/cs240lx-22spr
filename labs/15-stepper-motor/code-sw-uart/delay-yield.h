#ifndef __DELAY_YIELD_H__
#define __DELAY_YIELD_H__


#include "rpi.h"


// while waiting, do a yield so that we can run other stuff (e.g.
// other threads)
//
//  - XXX: should likely set a state variable and warn if they
//    do a raw delay_us() or busy wait. 
static inline void delay_us_yield(unsigned us) {
    unsigned s = timer_get_usec();
    while((timer_get_usec() - s) < us)
        rpi_wait();
}

#endif
