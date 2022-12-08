#include "include/gpio.h"
#include <stdint.h>

void mymain() {
    gpio_reset();
    
    gpio_set_output(25); 

    while (1) {
        gpio_set_on(25);
        DELAY(0x100000);
        gpio_set_off(25);
        DELAY(0x100000);
    }
}