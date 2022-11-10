/*
   simple driver for a counterfeit texas inst ads1115 adc 
   datasheet is in docs.

   the quickstart guide on p35 describes how the bytes go 
   back and forth:
        - write high byte first.
        - read returns high byte first.

   given the lab setting, we jam everything into one file --- 
   after your code works you should pull the ads1115 code into
   its own file so you can add it easily to other projects.
 */
#include "rpi.h"
#include "ads1115.h"

void notmain(void) {
    unsigned dev_addr = ads1115_config();

    // 5. just loop and get readings.
    //  - vary your potentiometer
    //  - should get negative readings for low
    //  - around 20k for high.
    //
    // 
    // make sure: given we set gain to +/- 4v.
    // does the result make sense?
	for(int i = 0; i < 10; i++) {
        short v = ads1115_read16(dev_addr, conversion_reg);
        printk("out=%d\n", v);
		delay_ms(1000);
	}
	clean_reboot();
}

