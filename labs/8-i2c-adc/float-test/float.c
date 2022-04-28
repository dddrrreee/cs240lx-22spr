/* simple example using floating point.  */
#include "rpi.h"

// in libpi/libm
#include "rpi-math.h"

void notmain(void) {
    output("hello\n");

    // just in case float is messed up, give output time to
    // flush.
    delay_ms(500);  

    printk("hello: %f\n", M_PI);
    printk("cos(%f)=%f\n", M_PI, cos(M_PI));
    printk("sin(%f)=%f\n", M_PI, sin(M_PI));
}
