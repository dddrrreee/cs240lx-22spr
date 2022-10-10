// simplistic example of measuring motor while reading it.
// the adc difference between on and off is pretty small!  likely
// need to handle this (esp in presence of ambient noise).
#include "rpi.h"
#include "rpi-thread.h"
#include "delay-yield.h"
#include "pretty-time.h"
#include "a4988.h"
#include "ads1115.h"

// print out thread annotation 
#define th_debug(msg, args...) do {             \
    debug(" thread=<%s>:", rpi_cur_thread()->annot);     \
    output(msg, ##args);                             \
} while(0)

// override the empty libpi rpi_wait implementation to thread yield
void rpi_wait(void) {
//    th_debug("rpi_wait: <%s> thread yielding\n", rpi_cur_thread()->annot);
    rpi_yield();
}

void rpi_run_async(const char *annot, void (*fp)(void*), void *arg) {
    rpi_fork(fp,arg)->annot = annot;
    // force a context switch
    rpi_wait();
}

typedef struct motor_context {
    step_t s;   // describes motor
    uint8_t     adc_addr;
    volatile int done;
    volatile int motor_done;
    unsigned nsamples;
} motor_ctx_t;

// brute force prevent readings faster than 1200usec (mic
// can do 860 samples / second).
short read_ads1115_safe(uint8_t dev_addr) {
    short s = ads1115_get_level(dev_addr);
    delay_us_yield(1200);
    return s;
}

// turn motor on: will stop when done is set.  perhaps
// make an interface that will run it for a given number
// of usec.
void motor_on(void *arg) {
    motor_ctx_t *ctx = arg;

    // race if the parent reads before.
    ctx->motor_done = 0;

    while(!ctx->done) {
        // note: step() can take a long time --- it won't 
        // bail halfway if done changes, only at the end.
        //
        // if needed: not that hard to make a step_abort routine that
        // takes a pointer.
        step(&ctx->s, forward);
    }
    th_debug("done!\n");
    ctx->motor_done = 1;
}

// threaded main so we can easily fork off
// additional threads. 
//
// XXX: our rpi thread interface is lame in that we didn't build
// a join() in class so can't actually guaratee the motor thread
// is done!  add this.
void th_notmain(void) {
    enum { step_delay = 3000 };
    enum { dir_pin = 21, step_pin = 20 };

    motor_ctx_t ctx = (motor_ctx_t) { 
        .adc_addr = ads1115_config(),
        .s = step_mk(dir_pin, step_pin, step_delay),
        .done = 0,
        .nsamples = 0,
        .motor_done = 0
    };

    enum { N = 400 };

    // get the max and average level with 
    int tot_lvl = 0, max_lvl = 0;
    for(int i = 0; i < N; i++) {
        short s = read_ads1115_safe(ctx.adc_addr);
        tot_lvl += s;
    
        if(s > max_lvl)
            max_lvl = s;
    }
    int ave_lvl = tot_lvl / N;

    output("motor off: samples=%d ave level=%d, max level=%d\n", 
            N, ave_lvl, max_lvl);
        
    assert(ctx.s.step_delay);

    // replicate code so it's obvious
    tot_lvl = 0; max_lvl = 0;

    rpi_run_async("motor on", motor_on, &ctx);
    for(int i = 0; i < N; i++) {
        short s = read_ads1115_safe(ctx.adc_addr);
        tot_lvl += s;
    
        if(s > max_lvl)
            max_lvl = s;
    }
    ave_lvl = tot_lvl / N;
    ctx.done = 1;

    // ugly hack since we don't have a join()
    // 
    // NOTE: ctx is stack allocated: if we return the motor
    // thread can still have a pointer to ctx and read garbage.
    while(!ctx.motor_done)
        rpi_wait();

    output("motor on: samples=%d ave level=%d, max level=%d\n", 
            N, ave_lvl, max_lvl);

}

void rpi_start_async(void (*fn)(void)) {
    void (*fp)(void*) = (void*)fn;
    rpi_fork(fp, 0)->annot = "first thread";
    rpi_thread_start();
}

// this would go in a library: run a main as a threaded routine.
// could also modify the threads library.
void notmain(void) {
    rpi_start_async(th_notmain);
    output("done with threaded main!\n");
}
