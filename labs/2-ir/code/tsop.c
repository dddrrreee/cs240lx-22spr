// very dumb starter code.  you should rewrite and customize.
//
// when done i would suggest pulling it out into an device source/header 
// file and put in libpi/src so can use later.

#include "rpi.h"
#include "tsop.h"
enum { ir_eps = 200 };

// we should never get this.
enum { NOISE = 0 } ;
// const unsigned header_exp = 9000;
const unsigned header_exp = 4500;
const unsigned skip_exp = 600;
// const unsigned zero_exp = 600;
// const unsigned one_exp = 1600;
const unsigned zero_exp = 400;
const unsigned one_exp = 1400;
const unsigned power_btn = 0x01de237e2;

struct readings { unsigned usec, v; };

const char *key_to_str(unsigned x, unsigned remote_type) {
    output("convert an input value to a string\n");
	switch (remote_type) {
		case NEC_REMOTE:
			switch (x) {
				case NEC_PWR_BTN:
					return "-> Power\n";
				case NEC_VOL_UP_BTN:
					return "-> VOL +\n";
				case NEC_VOL_DOWN_BTN:
					return "-> VOL -\n";
				case NEC_O_BTN:
					return "-> O\n";
				case NEC_X_BTN:
					return "-> X\n";
			}
		case GEN_REMOTE:
			switch (x) {
				case GEN_PWR_BTN:
					return "-> Power\n";
				case GEN_SRC_BTN:
					return "-> Source\n";
				case GEN_VOL_UP_BTN:
					return "-> VOL +\n";
				case GEN_VOL_DOWN_BTN:
					return "-> VOL -\n";
				case GEN_MUTE_BTN:
					return "-> MUTE\n";
				case GEN_REPLAY_BTN:
					return "-> Replay\n";
				case GEN_REWIND_BTN:
					return "-> Rewind\n";
				case GEN_PLAY_PAUSE_BTN:
					return "-> PLAY/PAUSE\n";
				case GEN_FORWARD_BTN:
					return "-> Forward Skip\n";
			}
	}
	return NULL;
}

// adapt your read_while_equal: return 0 if timeout passed, otherwise
// the number of microseconds + 1 (to prevent 0).
static int read_while_eq(int pin, int v, unsigned timeout) {
	unsigned start = timer_get_usec();
	while (timer_get_usec() - start <= timeout) {
		if (gpio_read(pin) != v) 
			return (timer_get_usec() - start + 1);
	}
	return 0;
}

// integer absolute value.
static int abs(int x) {
    return x < 0 ? -x : x; 
}

//returns 1 if expected is within eps of actual
static int within(unsigned expected, unsigned actual, unsigned eps) {
	return abs(expected - actual) < eps;
}

// return 0 if e is closer to <lb>, 1 if its closer to <ub>
static int pick(struct readings *e, unsigned lb, unsigned ub) {
	if (within(lb, e->usec, ir_eps))
		return 0;
	if (within(ub, e->usec, ir_eps))
		return 1;
    panic("invalid time: <%d> expected %d or %d\n", e->usec, lb, ub);
}

// return 1 if is a skip: skip = delay of 550-/+eps
static int is_skip(struct readings *e) {
    return within(skip_exp, e->usec, ir_eps);
}

// header is a delay of 9000 and then a delay of 4500
int is_header(struct readings *r, unsigned n, unsigned remote_type) {
    if(n < 2) {
        return 0;
	}
	switch (remote_type) {
		case NEC_REMOTE:
			if (within(NEC_FIRST_HEADER, r[0].usec, ir_eps) && within(NEC_SECOND_HEADER, r[1].usec, ir_eps))
				return 1;
		case GEN_REMOTE:
			if (within(GEN_HEADER, r[0].usec, ir_eps) && within(GEN_HEADER, r[1].usec, ir_eps))
				return 1;
	}
	return 0;
}

// convert <r> into an integer by or'ing in 0 or 1 depending on the 
// time value.
//
// assert that they are seperated by skips!
unsigned convert_nec(struct readings *r, unsigned n) {
	if(!is_header(r, n, NEC_REMOTE))
		return NOISE;
	assert(n >  4);
	assert(is_skip(&r[2]));  //check skip comes after header

	unsigned v = 0;
	for (int i = 3; i < n; i++) {
		//should be skip
		if (i % 2 == 0)
			assert(is_skip(&r[i]));
		else if (pick(&r[i], NEC_ZERO_EXP, NEC_ONE_EXP))
			v |= 1 << i/2;
	}

	return v;
}

// convert <r> into an integer by or'ing in 0 or 1 depending on the 
// time value.
//
// assert that they are seperated by skips!
unsigned convert_generic(struct readings *r, unsigned n) {
	if(!is_header(r, n, GEN_REMOTE))
		return NOISE;
	assert(n >  4);
	assert(is_skip(&r[2]));  //check skip comes after header

	unsigned v = 0;
	unsigned curr_bit = 0;
	for (int i = 3; i < n; i++) {
		//skip extraneous header(?)
		if (i == 35)
			continue;
		//should be skip
		if (i % 2 == 0) {
			assert(is_skip(&r[i]));
			continue;
		} else {
			if (pick(&r[i], GEN_ZERO_EXP, GEN_ONE_EXP)) {
				v |= 1 << curr_bit;
			}
			curr_bit++;
		}
			
	}

	return v;
}

static void print_readings(struct readings *r, int n, unsigned remote_type) {
    assert(n);
    printk("-------------------------------------------------------\n");
    for(int i = 0; i < n; i++) {
        if(i) 
            assert(!is_header(r+i,n-i, remote_type));
        printk("\t%d: %d = %d usec\n", i, r[i].v, r[i].usec);
    }
    if(!is_header(r,n, remote_type))
        printk("NOISE\n");

}

// read in values until we get a timeout, return the number of readings.  
static int get_readings(int in, struct readings *r, unsigned N) {
	// get readings until timeout
	unsigned timeout = 15000;
	unsigned num_readings = 0;
	// value initially set to 0 to reverse with laterr
	unsigned expected_v = 0;
	for (int i = 0; i < N; i++) {
		r[i].v = expected_v;
		r[i].usec = read_while_eq(in, expected_v, timeout);
		if (r[i].usec == 0)
			return num_readings;
		num_readings++;
		expected_v = 1 - expected_v;
	}
	return num_readings;
}

// initialize the pin.
int tsop_init(int input) {
    // is open hi or lo?  have to set pullup or pulldown
	gpio_set_pullup(input);
    return 1;
}

void notmain(void) {
    int in = 21;
    tsop_init(in);
    output("about to start reading\n");

    // very dumb starter code
    while(1) {
        // wait until signal: or should we timeout?
        while(gpio_read(in))
            ;
#       define N 256
        static struct readings r[N];
        int n = get_readings(in, r, N);
        output("done getting readings\n");
    
        unsigned x = (n == GEN_NUM_READINGS) ? convert_generic(r,n) : convert_nec(r,n);
		unsigned remote_type = (n == GEN_NUM_READINGS) ? GEN_REMOTE : NEC_REMOTE;
        output("converted to %x\n", x);
		if (x == GEN_PWR_BTN || x == NEC_PWR_BTN)
			clean_reboot();
        const char *key = key_to_str(x, remote_type);
        if(key)
            printk("%s\n", key);
        else
            // failed: dump out the data so we can figure out what happened.
            print_readings(r,n, remote_type);
    }
	printk("stopping ir send/rec!\n");
    clean_reboot();
}
