// trivial little fake r/pi system that will allow you to debug your
// ws8212b code.

// can compile on your laptop and then run the code there
// too:
//
// some nice things:
//  1. use a debugger;
//  2. by comparing the put/get values can check your code against other
//     people (lab 3)
//  3. you have memory protection, so null pointer writes get detected.
//  4. can run with tools (e.g., valgrind) to look for other errors.
//
// while the fake-pi is laughably simple and ignores more than it handles,
// it's still useful b/c:
//  1. most of your code is just straight C code, which runs the same on
//     your laptop and the pi.
//  2. the main pi specific thing is what happens when you read/write
//     gpio addresses.   given how simple these are, we can get awa
//     with just treating them as memory, where the next read returns
//     the value of the last write.  for fancier hardware, or for 
//     a complete set of possible GPIO behaviors we would have to do
//     something fancier.   
//
//     NOTE: this problem of how to model devices accurately is a 
//     big challenge both for virtual machines and machine simulators.
//
// it's good to understand what is going on here.  both why it works,
// and when you can use this kind of trick in other places.
//      - one interesting thing: we can transparently take code that
//      you wrote explicitly to run bare-metal on the pi ARM cpu,
//      and interact with the weird broadcom chip and run it on
//      your laptop, which has neither, *without making any change!*
//
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "libunix.h"


// there is no assembly or other weird things in rpi.h so we can
// include and use it on your laptop even tho it is intended for
// the pi.
// #include "rpi.h"
#include "fake-pi.h"

/********************************************************************
 * dumb fake cycle counter: increments by 1 each time its read.
 */

static unsigned fake_cycle_cnt_v;

// don't advance: used to tag things with the current cycle count.
unsigned cycle_cnt_get(void) {
    return fake_cycle_cnt_v;
}

// set initial fake value.
void cycle_cnt_init(void) {
    fake_cycle_cnt_v = fake_random();
}

static unsigned cycle_diff(void) {
    static unsigned last;

    unsigned diff = 0;

    unsigned cur = cycle_cnt_get();
    if(last) 
        diff = cur - last;
    
    last = cur;
    return diff;
}

// return current fake and advance by 1 each time.
//   in real sim would control how fast.
unsigned cycle_cnt_read(void) {
    unsigned v = cycle_cnt_get();
    fake_cycle_cnt_v++;
    return v;
}




// emit a trace statement -- these are the outputs that you compare
// run to run to see that the code doesn't change.

// some programs run forever -- truncate after <max_trace> statements
enum { max_trace = 1024 };
unsigned ntrace = 0;

static unsigned trace_on_p = 0, trace_ops = 0;
static inline void trace_on(void) { trace_on_p = 1; }
static inline void trace_off(void) { trace_on_p = 0; }

#define trace(msg, args...) do {                                        \
    if(trace_on_p) {                                                    \
        output("TRACE:%d: " msg, trace_ops++, ##args);                                   \
        if(ntrace++ >= max_trace) {                                     \
            output("TRACE: exiting after %d trace statements\n", ntrace);\
            fake_pi_exit();                                             \
        }                                                               \
    }                                                                   \
} while(0)


/***********************************************************************
 * a tiny fake simulator that traces all reads and writes to device
 * memory.
 *
 * main pi-specific thing is a tiny model of device
 * memory: for each device address, what happens when you
 * read or write it?   in real life you would build this
 * model more succinctly with a map, but we write everything 
 * out here for maximum obviousness.
 */

#define GPIO_BASE 0x20200000

// the locations we track.
enum {
    gpio_set0  = (GPIO_BASE + 0x1C),
    gpio_clr0  = (GPIO_BASE + 0x28),
};

// the value for each location.
static unsigned 
        gpio_set0_v,
        gpio_clr0_v;

// same, but takes <addr> as a uint32_t
void PUT32(uint32_t addr, uint32_t v) {
    enum { raise_op_p = 1 };

#if 0
    if(!trace_on_p)
        output("initializing PUT32(0x%x) = 0x%x\n", addr, v);
#endif
    switch(addr) {
    case gpio_set0:  
        gpio_set0_v  = v;  
        if(!raise_op_p) 
            trace("PUT32(0x%x) = 0x%x\n", addr, v);
        else {
            // atm: make sure just one bit is set.
            assert((v & -v) == v);
            trace("set(pin=%d)=1, cycles=%d\n", ffs(v), cycle_diff());
        } 
        break;
    case gpio_clr0:  
        gpio_clr0_v  = v;  
        if(!raise_op_p) 
            trace("PUT32(0x%x) = 0x%x\n", addr, v);
        else {
            // atm: make sure just one bit is set.
            assert((v & -v) == v);
            trace("set(pin=%d)=0, cycles=%d\n", ffs(v), cycle_diff());
        } 
        break;
    default: panic("write to illegal address: %x\n", addr);
    }
}
// same as PUT32 but takes a pointer.
void put32(volatile void *addr, uint32_t v) {
    PUT32((uint32_t)(uint64_t)addr, v);
}

void fake_pi_exit(void) {
    output("TRACE: pi exited cleanly: %d calls to random\n", fake_random_calls());
    exit(0);
}

void delay_cycles(unsigned ncycles) {
    trace("delaying %d cycles\n", ncycles);
}


int exit_code(int pid) {
    int status;
    if(waitpid(pid, &status, 0) < 0)
        sys_die("waitpid", waitpid failed?);
    if(!WIFEXITED(status))
        panic("%d: unexpected crash\n", pid);
    return WEXITSTATUS(status);
}

// give this as a prelab?
void fancy_check(int N) {
    void notmain(void);

    trace("running %d trials\n", N);
    for(int i = 0; i < N; i++) {
        output("----------------running iteration %d----------------\n", i);
        fake_random_seed(i);
        cycle_cnt_init();

        int pid = fork();
        if(pid < 0)
            sys_die(fork, cannot fork process!);

        // child
        if(!pid) {
            notmain();
            fflush(stdout);
            fake_pi_exit();
        } else {
            int ret = exit_code(pid);
            if(ret != 0)
                panic("unexpected exit value=%d\n", ret);
            fflush(stdout);
        }
    }
    trace("DONE: %d trials!\n", N);
}

// initialize "device memory" and then call the pi program
int main(int argc, char *argv[]) {

    unsigned n = 1;
    if(argc == 2) {
        n = atoi(argv[1]);
        if(n == 0)
            panic("n was <%s>: should be a non-zero number\n", argv[1]);
    } else if(n != 1)
        panic("invalid number of args: %d\n", argc);

    fancy_check(n);
    return 0;
}
