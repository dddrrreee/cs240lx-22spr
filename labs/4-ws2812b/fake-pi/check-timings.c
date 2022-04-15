#include "rpi.h"
#include "WS2812B.h"


#define STRINGIFY(x) #x

#if 0
void run(const char *msg, unsigned expect, void (*fp)(unsigned pin), unsigned pin) {
    unsigned s = cycle_cnt_get();
    fp(pin);
    printk("%s: got = %d cycles, should be about = %d cycles\n", 
        msg,
        cycle_cnt_get() - s + compensation,
        expect);
}
#endif


#define RUN(stmt, expect) ({                                     \
    unsigned s = cycle_cnt_get();                                       \
    stmt;                                                               \
    unsigned got = cycle_cnt_get() - s + compensation;                         \
    printk("%s: got = %d cycles, should be about = %d cycles\n",    \
        STRINGIFY(stmt),                                             \
        got, \
        expect);                                                    \
    got;\
})

void notmain(void) { 
    enum { pin = 21 };

    RUN(t1h(pin), T1H);
    RUN(t1l(pin), T1L);
    RUN(t0h(pin), T0H);
    RUN(t0l(pin), T0L);

    // 8 1s should be (T1H+T1L)*8
    RUN(pix_sendbyte(pin, ~0), 8 * (T1H+T1L));
    // 8 1s should be (T0H+T0L)*8
    RUN(pix_sendbyte(pin, 0), 8 * (T0H+T0L));

    // alternativing 1s and 0s: should all be the same
    unsigned sum = 0;

    sum += RUN(pix_sendbyte(pin, 0b01010101), 4 * (T0H+T0L) + 4 * (T1H+T1L));
    sum += RUN(pix_sendbyte(pin, 0b1111), 4 * (T0H+T0L) + 4 * (T1H+T1L));
    sum += RUN(pix_sendbyte(pin, 0b11110000), 4 * (T0H+T0L) + 4 * (T1H+T1L));

    RUN(pix_sendpixel(pin, 0b11110000, 0b1111, 0b10101010), sum);
    

    
#if 0
    unsigned s = cycle_cnt_get();
    t1h(pin);
    printk("timing = %d\n", cycle_cnt_get() - s + compensation);

// implement T0H from the datasheet.
static inline void t0h(unsigned pin) { write_1(pin, T0H); }
// implement T1L from the datasheet.
static inline void t1l(unsigned pin) { write_0(pin, T1L); }
// implement T0L from the datasheed.
static inline void t0l(unsigned pin) { write_0(pin, T0L); }
// implement RESET from the datasheet.
static inline void treset(unsigned pin) { write_0(pin, FLUSH);  }



    pix_sendbyte(21, ~0);
    pix_sendbyte(21, 0);
    pix_sendbyte(21, ~0);
    pix_sendbyte(21, 0);
#endif
}
