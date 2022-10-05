#ifndef __STEPPER_INT_H__
#define __STEPPER_INT_H__

#include "stepper.h"

typedef enum  {
    IN_JOB,
    NOT_IN_JOB
} stepper_status_t;

typedef enum {
    NOT_STARTED,
    STARTED,
    FINISHED,
    ERROR
} stepper_position_status_t;

typedef struct stepper_position_t {
    struct stepper_position_t * next;
    int goal_steps;
    unsigned usec_between_steps;
    unsigned usec_at_prev_step;
    stepper_position_status_t status;
} stepper_position_t;

#define E stepper_position_t
#include "Q.h"

typedef struct {
    stepper_t * stepper;
    stepper_status_t status;
    Q_t positions_Q;
} stepper_int_t;

stepper_int_t * stepper_init_with_int(unsigned dir, unsigned step);

// for extension
stepper_int_t * stepper_init_with_int_with_microsteps(unsigned dir, unsigned step, unsigned MS1, unsigned MS2, unsigned MS3, stepper_microstep_mode_t microstep_mode);

/* retuns the enqueued position. perhaps return the queue of positions instead? */
stepper_position_t * stepper_int_enqueue_pos(stepper_int_t * stepper, int goal_steps, unsigned usec_between_steps);

int stepper_int_get_position_in_steps(stepper_int_t * stepper);

// returns 0 if in a job, 1 if not in job
int stepper_int_is_free(stepper_int_t * stepper);

// returns 0 if status is NOT_STARTED or STARTED and 1 otherwise.
int stepper_int_position_is_complete(stepper_position_t * pos);

#endif