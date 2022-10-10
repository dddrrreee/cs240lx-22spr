// simple example of using interleaving threads for a given amount of time
//
// basic idea for how you can run multiple pi programs simulatneously on 
// a single pi so you can do loop back experiments (e.g., send to two NRFs
// attached to the same pi, run a motor and listen to it at the same time
// etc)
//
// fragile in that we depend on threads doing RR.
//
// Note: you can do the same thing by using interrupts or --- much less
// state space exploding --- but interleaving run to completion routines
// instead of threads.
//
#include "rpi.h"
#include "rpi-thread.h"
#include "delay-yield.h"
#include "pretty-time.h"

// print out thread annotation 
#define th_debug(msg, args...) do {             \
    debug(" thread=<%s>:", rpi_cur_thread()->annot);     \
    output(msg, ##args);                             \
} while(0)

struct delay_state {
    volatile int done;
    int how_long_us;
    int verbose_p;      // unused: can control printing.
};

// override the empty libpi rpi_wait implementation to thread yield
void rpi_wait(void) {
//    th_debug("rpi_wait: <%s> thread yielding\n", rpi_cur_thread()->annot);
    rpi_yield();
}

static void test_interleave(void *input) {
    struct delay_state *s = input;
    
    th_debug("start running: done=%d\n", s->done);

    // can call the client routine here if they don't want to add
    // the loop.
    while(!s->done) {
        th_debug("running loop: done =%d\n", s->done);
        delay_us_yield(1000);
    }
    th_debug("exiting: done=%d\n", s->done);
}


void run_for(void *input) {
    volatile struct delay_state *s = input;

    th_debug("about to run the routines for %d usec\n", 
            s->how_long_us);

    delay_us_yield(s->how_long_us);
    s->done = 1;

    // doing this last print adds many ms delay to the end time.
    // if you comment out they should come down alot.
    th_debug("done running routines: setting done=%d\n", 
            s->done); 
}

// run two threads simulatenously: should have a way to kill them
// rather than this hack of having a global variable.
//
// one way; interface where you have an 
// asynchronous join and then a kill to remove a thread.
//
// is there a cleaner way?  have a kill when done fork?
void run_two(void (*A)(void*), void (*B)(void*)) {
    struct delay_state s = { 
        .done = 0,
        .verbose_p = 0,
        // 1 sec about
        .how_long_us = 1000 * 1000, 
    };

    // this is actually fine conceptually, we just don't do atm.
    assert(!rpi_thread_on());

    // XXX: 
    //  - should pass in the real routine names
    //  - could also do thread creation in the interleave routine for
    //    more control.
    rpi_fork(A, &s)->annot = "A";
    rpi_fork(B, &s)->annot = "B";
    rpi_fork(run_for, &s)->annot = "delay_routine";

    unsigned start = timer_get_usec();
    rpi_thread_start();
    unsigned end = timer_get_usec();
    panic("done running: %s\n", ptime_tot(end-start).str);
    assert(!rpi_thread_on());
}

void notmain(void) {
    output("about to run interleave example\n");
    run_two(test_interleave, test_interleave);
}
