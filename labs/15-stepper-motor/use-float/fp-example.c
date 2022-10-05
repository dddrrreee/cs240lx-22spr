// trivial example of using floating point + our simple math
// library.
#include "rpi.h"
#include "rpi-math.h"
void notmain(void) {
    double x = 3.1415;
    printk("hello from pi=%f float!!\n", x);

    double v[] = { M_PI, 0, M_PI/2.0, M_PI/2.0*3.0 };
    for(int i = 0; i < 4; i++)  {
        printk("COS(%f) = %f\n", v[i], cos(v[i]));
        printk("sin(%f) = %f\n", v[i], sin(v[i]));
    }
    output("done\n");
    clean_reboot();
}
