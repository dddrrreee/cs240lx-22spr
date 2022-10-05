#ifndef __STEPPER_H__
#define __STEPPER_H__

// for microstepping extension
typedef enum {
    FULL_STEP,
    HALF_STEP,
    QUARTER_STEP,
    EIGHTH_STEP,
    SIXTEENTH_STEP
} stepper_microstep_mode_t;

// see images/a4988_pinout.png
typedef struct {
    int step_count;
    unsigned dir;
    unsigned step;
    unsigned MS1;
    unsigned MS2;
    unsigned MS3;
} stepper_t;

stepper_t * stepper_init(unsigned dir, unsigned step);

// for microstepping extension
stepper_t * stepper_init_with_microsteps(unsigned dir, unsigned step, unsigned MS1, unsigned MS2, unsigned MS3, stepper_microstep_mode_t microstep_mode);

// for microstepping extension
void stepper_set_microsteps(stepper_t * stepper, stepper_microstep_mode_t microstep_mode);

void stepper_step_forward(stepper_t * stepper);

void stepper_step_backward(stepper_t * stepper);

int stepper_get_position_in_steps(stepper_t * stepper);

#endif 