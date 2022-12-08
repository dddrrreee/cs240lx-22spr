#include "include/gpio.h"
#include "include/clock.h"
#include <stdint.h>
#define PIN 25

void mymain() {
    enable_clock();
    gpio_reset();

    gpio_set_output(PIN); 

    while(1) {
        gpio_set_on(PIN);

        delay_ms(1000);

        gpio_set_off(PIN);

        delay_ms(1000);
    }

}