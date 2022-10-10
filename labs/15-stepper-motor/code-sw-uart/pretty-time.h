#ifndef __PRETTY_TIME_H__
#define __PRETTY_TIME_H__

typedef struct pretty_time {
    unsigned sec, msec, usec;
    char str[64];
} pretty_time_t;

static inline unsigned time_to_sec(unsigned tot) {
    return tot / (1000*1000);
}
static inline unsigned time_to_msec(unsigned tot) {
    unsigned ms =  tot % (1000*1000);
    return ms / 1000;
}
static inline unsigned time_to_usec(unsigned tot) {
    return tot % 1000;
}

static inline pretty_time_t ptime_tot(unsigned total_usec) {
    pretty_time_t p = (pretty_time_t) {
        .sec = time_to_sec(total_usec),
        .msec = time_to_msec(total_usec),
        .usec = time_to_usec(total_usec)
    };
    // lame way to strip off different parts [fix strcatf]
    if(p.sec)
        snprintk(p.str, 64, "%dsec:%dmsec:%dusec", p.sec, p.msec, p.usec);
    else if(p.msec)
        snprintk(p.str, 64, "%dmsec:%dusec", p.msec, p.usec);
    else
        snprintk(p.str, 64, "%dusec", p.usec);

    return p;
}

// given a usecond start, return the pretty time to now.
static inline pretty_time_t ptime_end(unsigned start) {
    return ptime_tot(timer_get_usec() - start);
}

#endif
