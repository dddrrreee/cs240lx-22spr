#ifndef __CYCLE_H__
#define __CYCLE_H__
// utilities: you don't have to use these.

static inline unsigned usec_to_cycle(unsigned us) {
    return 700 * us;
}
static inline unsigned msec_to_cycle(unsigned ms) {
    return 1000 * usec_to_cycle(ms);
}
static unsigned sec_to_cycle(unsigned s) {
    assert(s < 2);
    return 1000 * msec_to_cycle(s);
}

static inline int cyc_is_timeout(unsigned start, unsigned n, unsigned t) {
    // printk("start=%u, end=%u, t=%u\n", start, end, t);
    return (t - start) >= n;
}

static inline int cyc_timeout(unsigned start, unsigned n) {
    return cyc_is_timeout(start, n, cycle_cnt_read());
}

static inline void delay_cyc(unsigned ncycles) {
    unsigned start =  cycle_cnt_read();
    while(!cyc_timeout(start, ncycles))
        ;
}
#endif
