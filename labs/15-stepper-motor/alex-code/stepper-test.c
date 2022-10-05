#include "rpi.h"
#include "stepper.h"
#include "stepper-int.h"
#include "math-helpers.h"

// stepper pins
#define DIR 21
#define STEP 20
#define MS1 13
#define MS2 19
#define MS3 26

// play around with this!
#define DELAY_BETWEEN_STEPS_USEC 10000 

void test_stepper(){
    printk("testing stepper\n");
    
    stepper_t * stepper = stepper_init(DIR, STEP);

    for(int i = 0; i < 100; i++){
        assert(stepper_get_position_in_steps(stepper) == i);
        stepper_step_forward(stepper);
        delay_us(DELAY_BETWEEN_STEPS_USEC);
    }

    for(int i = 0; i < 100; i++){
        stepper_step_backward(stepper);
        delay_us(DELAY_BETWEEN_STEPS_USEC);
        assert(stepper_get_position_in_steps(stepper) == 99 - i);
    }

    printk("passed sw checks! How about the hardware?\n");
}

void test_stepper_with_interrupts(){
    printk("now testing stepper w/ interrupts\n");
    stepper_int_t * stepper = stepper_init_with_int(DIR, STEP);

    // test accuracy. Try changing goal and USEC_BETWEEN_STEPS to see how accuracy varies.
    int goal = 200;
    #define USEC_BETWEEN_STEPS 5000
    printk("enqueuing a goal of %d steps, at a rate of one step per %d usec\n", goal, USEC_BETWEEN_STEPS);
    unsigned start_time_usec = timer_get_usec();
    stepper_int_enqueue_pos(stepper, goal, USEC_BETWEEN_STEPS);

    while(!stepper_int_is_free(stepper)){/*wait*/}
    unsigned end_time_usec  = timer_get_usec();
    printk("stepper pos %d. Time it took: %d usec (expected %d, off by: %d)\n", stepper_int_get_position_in_steps(stepper), 
            (end_time_usec - start_time_usec), goal*USEC_BETWEEN_STEPS, goal*USEC_BETWEEN_STEPS - (end_time_usec - start_time_usec));

    // test different speeds
    unsigned num_jobs = 4;
    stepper_position_t * jobs[num_jobs];
    for(int i = 0; i < num_jobs; i++){
        jobs[i] = stepper_int_enqueue_pos(stepper, 100*(i+1) * (i % 2 ? 1 : -1), (num_jobs - i) * 2000);
    }

    for(int i = 0; i < num_jobs; i++){
        while(!stepper_int_position_is_complete(jobs[i])){/*wait*/}
        int pos = stepper_int_get_position_in_steps(stepper);
        printk("job %d done, curr pos %d\n", i, pos);
        assert(pos == 100*(i+1) * (i % 2 ? 1 : -1));
    }
    printk("success!");
}

void notmain(){
    uart_init();
    printk("Stepper: starting\n");

    test_stepper();

    test_stepper_with_interrupts();

    printk("Done!\n");
    clean_reboot();
}