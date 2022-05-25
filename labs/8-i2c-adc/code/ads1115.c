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
#include "i2c.h"
#include "ads1115.h"
#include "libc/bit-support.h"
#include "libc/math-helpers.h"

// for bitfield checking
#include "libc/helper-macros.h"

// bit masks for checking default reg values
const static uint16_t CONFIG_DEF = 0x8583;
const static uint16_t DESIRED_CONFIG_RD = 0x2e3;

// p27: register names
// enum { conversion_reg = 0, config_reg = 1 };


// p21 states device will reset itself when you do an
// i2c "general call" reset with 6h (6 hex = 0x6)
// we need to do this b/c  ---- and this is an easy 
// mistake to make --- even though the pi reboots,
// the adc (and all sensors) are still up and running,
// so have the previous configuration.
void ads1115_reset(void) {
    uint8_t rst = 0x6;
    i2c_write(0, &rst,1);
}

// write a 16 bit register: 
//  fill in a 3 byte array.
//    1. first byte is register name <reg>
//    2. next two bytes are the value <v> (in what order?)
//  send it:
//    3. call i2c_write with <dev_addr> and the array.
void ads1115_write16(uint8_t dev_addr, uint8_t reg, uint16_t v) {
	uint8_t send[3];
	send[0] = reg;
	send[1] = (uint8_t)(v >> 8);
	send[2] = (uint8_t)((v << 8) >> 8);
	i2c_write(dev_addr, send, 3);
}

// read a 16-bit register
// 1. write the register address (1 byte i2c_write)
// 2. read the two bytes that come back (a 2-byte i2c_read)
// 3. reconstruct the 16-bit value.
uint16_t ads1115_read16(uint8_t dev_addr, uint8_t reg) {
	uint8_t send[1];
	send[0] = reg;
	i2c_write(dev_addr, send, 1);

	uint8_t rec[2];
	i2c_read(dev_addr, rec, 2);

	uint16_t ret = ((uint16_t)rec[0] << 8 )| (uint16_t)rec[1];
	return ret;
}

// helper for _config that will set proper bits for our preffered configuration
uint16_t ads1115_setup(uint16_t conf_val) {
	//set p/g to 4v
	conf_val &= 0xF1FF; // strip val of default p/g 
	conf_val |= 0x200; // place desired p/g val into conf_val[9:11] 0b001

	//set mode to continuous
	conf_val &= 0xFEFF; // flip mode bit to 0b0 conf_val[8]

	//set data rate to 860sps
	conf_val |= 0xE0; // set DR bits to 0b111 conf_val[5:7]

	return conf_val;
}

// returns the device addr: hard-coded configuration atm.
uint8_t ads1115_config(void) {
    delay_ms(30);   // allow time for device to boot up.
    i2c_init();
    delay_ms(30);   // allow time to settle after init.

    // dev address: p23
    enum { dev_addr = 0x48 };
    // panic("set dev_addr!\n");

    // p28
    // one way to set fields in a register.
    //   note, these are not clearing them.
#   define PGA(x) ((x)<<(9))
#   define MODE(x) ((x)<<(8))
#   define DR(x) ((x)<<5)
#   define MUX(x) ((x)<<12)

#   define MODE_V(x) bits_get(x,8,8)    // ((x) >> 8)&1)
#   define DR_V(x)   bits_get(x,5,7)  // (((x)>>5)&0b111)
#   define PGA_V(x)  bits_get(x,9,11)  // (((x)>>9)&0b111)
#   define MUX_V(x)  bits_get(x,12,14) // (((x)>>12)&0b111)

    // reset device to known state.
    ads1115_reset();

    // we seem to have to wait for power up.
    delay_us(25);

    // 1. get the initial config
	uint16_t c = ads1115_read16(dev_addr, config_reg);
	printk("initial: config reg = =%x (lo=%b,hi=%b)\n", c,c&0xff,c>>8);

    // 2. sanity check the default default values from p29
    //   mode: 8 is 1
    //   dr: 7:5 = 0b100
    //   pg: 11:9 = 010
	demand((c & CONFIG_DEF) == CONFIG_DEF, "default config incorrect.\n");
	

    // 3. set the config to:
    //  - PGA gain to +/-4v 
    //  - MODE to continuous.
    //  - DR to 860sps
    // see page 28.
	uint16_t new_conf = c; //want other fields to prob stay the same?
	new_conf = ads1115_setup(new_conf);


	printk("writing config: %x\n", new_conf);
    ads1115_write16(dev_addr, config_reg, new_conf);

    // 4. read back the config and make sure the fields we set
    // are correct.
	uint16_t conf = ads1115_read16(dev_addr, config_reg);
	demand(DESIRED_CONFIG_RD == conf, "read config does not match what we wrote.\n");

    return dev_addr;
}
