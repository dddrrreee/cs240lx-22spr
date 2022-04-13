#ifndef __SAMPLES_H__
#define __SAMPLES_H__

#define CYCLE_PER_SEC (700*1000*1000)

typedef struct {
    unsigned v,ncycles;
} log_ent_t;

// dump out the log, calculating the error at each point,
// and the absolute cumulative error.
static inline unsigned 
dump_samples(log_ent_t *l, unsigned n, unsigned period) {
    unsigned tot = 0, tot_err = 0;

    unsigned prev = 0;
    for(int i = 0; i < n-1; i++) {
        log_ent_t *e = &l[i];

        unsigned ncyc = e->ncycles;
        ncyc -= prev;
        prev = tot = e->ncycles;

        unsigned exp = period * (i+1);
        unsigned err = tot > exp ? tot - exp : exp - tot;
        tot_err += err;

        printk(" %d: val=%d, time=%d, tot=%d: exp=%d (err=%d, toterr=%d)\n", i, e->v, ncyc, tot, exp, err, tot_err);
    }
    return tot_err;
}

// dump out the log, calculating the error at each point,
// and the absolute cumulative error.
static void dump_raw(log_ent_t *l, unsigned n) {
    unsigned tot = 0;

    unsigned prev = 0;
    for(int i = 0; i < n-1; i++) {
        log_ent_t *e = &l[i];

        unsigned ncyc = e->ncycles;
        ncyc -= prev;
        prev = tot = e->ncycles;
        unsigned v = 0;
        if(e->v)
            v = 1;
        printk(" %d: val=%d, time=%d, tot=%d\n", i, v, ncyc, tot);
    }
}


// trivial example of how to use the log: this
// writes in perfect values.
// and then prints.   it's not useful except to see
// how to use the interface
static void sample_ex(log_ent_t *l, unsigned n, unsigned cyc_per_flip) {
    printk("about to run a perfect sample: should have 0 error\n");

    unsigned v0 = 0;

    unsigned start = 1000;
    unsigned t = start;

    for(unsigned i = 0; i < n; i++) {
        t += cyc_per_flip;
        l[i] = (log_ent_t) { .v = v0, .ncycles =  t - start };
        v0 = 1 - v0;   // flip it.
    }
    unsigned err = dump_samples(l, n, cyc_per_flip);
    assert(!err);

    printk("done running example\n");
    clean_reboot();
}

#endif
