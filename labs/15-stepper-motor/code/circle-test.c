// turn the motor in a circle in both directions.
// if it's plugged in, it's hard to turn the shaft so unplug and rotate
// to an easily verified position.
#include "rpi.h"
#include "a4988.h"

// you need to fill these in.
enum { dir_delay = 0, step_delay = 0 };

// rotate shaft 360 degrees.
static void run_circle(step_t *s, int direction) {
    for(int n = 0; n < 200; n++) 
        step(s,direction);
}

void notmain(void) {
    demand(dir_delay && step_delay, "must set these up");

    enum { dir = 21, step = 20 };

    step_t s = step_mk(dir, dir_delay, step, step_delay);

    enum { N = 4 };
    output("about to run forward and back %d times\n", N);
    for(int i = 0; i < N; i++) {
        run_circle(&s, forward);
        run_circle(&s, backward);
    }
    clean_reboot();
}

