// engler, cs49n: simplistic mpu6500 driver code.
//
// everything is put in here so it's easy to find.  when it works,
// seperate it out.
//
// KEY: document why you are doing what you are doing.
//  **** put page numbers for any device-specific things you do ***
//  **** put page numbers for any device-specific things you do ***
//  **** put page numbers for any device-specific things you do ***
//  **** put page numbers for any device-specific things you do ***
//  **** put page numbers for any device-specific things you do ***
//  **** put page numbers for any device-specific things you do ***
//  **** put page numbers for any device-specific things you do ***
// 
// also: a sentence or two will go a long way in a year when you want 
// to re-use the code.
#include "rpi.h"
#include "i2c.h"
#include <limits.h>

// it's easier to bundle these together.
typedef struct { int x,y,z; } imu_xyz_t;

static inline imu_xyz_t 
xyz_mk(int x, int y, int z) {
    return (imu_xyz_t){.x = x, .y = y, .z = z};
}
void xyz_print(const char *msg, imu_xyz_t xyz) {
    output("%s (x=%d,y=%d,z=%d)\n", msg, xyz.x,xyz.y,xyz.z);
}

// i2c helpers.

// read register <reg> from i2c device <addr>
uint8_t imu_rd(uint8_t addr, uint8_t reg) {
    i2c_write(addr, &reg, 1);
        
    uint8_t v;
    i2c_read(addr,  &v, 1);
    return v;
}

// write register <reg> with value <v> 
void imu_wr(uint8_t addr, uint8_t reg, uint8_t v) {
    uint8_t data[2];
    data[0] = reg;
    data[1] = v;
    i2c_write(addr, data, 2);
    // printk("writeReg: %x=%x\n", reg, v);
}


// <base_reg> = lowest reg in sequence. --- hw will auto-increment 
int imu_rd_n(uint8_t addr, uint8_t base_reg, uint8_t *v, uint32_t n) {
        i2c_write(addr, (void*) &base_reg, 1);
        return i2c_read(addr, v, n);
}

/**********************************************************************
 * simple accel setup and use
 */

// note: the names are short and overly generic: this saves typing, but 
// can cause problems later.
typedef struct {
    uint8_t addr;
    unsigned hz;
    unsigned g;
} accel_t;

// returns the raw value from the sensor: combine
// the low and the hi and sign extend (cast to short)
static short mg_raw(uint8_t lo, uint8_t hi) {
    unimplemented();
}

// returns milligauss, integer
static int mg_scaled(int v, int mg_scale) {
    return (v*1000 * mg_scale) / SHRT_MAX;
}

// takes in raw data and scales it.
imu_xyz_t accel_scale(accel_t *h, imu_xyz_t xyz) {
    int g = h->g;
    int x = mg_scaled(h->g, xyz.x);
    int y = mg_scaled(h->g, xyz.y);
    int z = mg_scaled(h->g, xyz.z);
    return xyz_mk(x,y,z);
}

static void test_mg(int expected, uint8_t l, uint8_t h, unsigned g) {
    int s_i = mg_scaled(mg_raw(h,l),g);
    printk("expect = %d, got %d\n", expected, s_i);
    assert(s_i == expected);
}

enum {
    // p6, p14
    accel_config_reg = 0x1c,
    accel_off = 3,
    accel_2g = 0b00,
    accel_4g = 0b01,
    accel_8g = 0b10,
    accel_16g = 0b11,

    // p6, p15: set bandwidth / rate
    accel_config_reg2 = 0x1d,
    accel_fchoice_b_off = 3, 
    a_dlpf_cfg_off = 0,

    // p31,32
    ACCEL_XOUT_H = 0x3b,
    accel_xout_l = 0x3c,
    accel_yout_h = 0x3d,
    accel_yout_l = 0x3e,
    accel_zout_h = 0x3f,
    accel_zout_l = 0x40,

    // p 41
    PWR_MGMT_1 = 107,
    // p 42
    PWR_MGMT_2 = 108
};

// extension: do this using interrupts or fifo.
int accel_has_data(const accel_t *h) {
    return 1;
}
    
// set to accel 2g (p14), bandwidth 20hz (p15)
accel_t mpu6500_accel_init(uint8_t addr, unsigned accel_g) {
    unsigned g = 0;
    switch(accel_g) {
    case accel_2g: g = 2; break;
    case accel_4g: g = 4; break;
    case accel_8g: g = 8; break;
    case accel_16g: g = 16; break;
    default: panic("invalid g=%b\n", g);
    }

    // set scaling to <accel_g> and 20hz sample rate
    unimplemented();

    output("accel_config_reg=%b\n", imu_rd(addr, accel_config_reg));
    output("accel_config_reg2=%b\n", imu_rd(addr, accel_config_reg2));
    return (accel_t) { .addr = addr, .g = g, .hz = 20 };
}

// do a hard reset
void mpu6500_reset(uint8_t addr) {
    // reset: p41
    unimplemented();

    // they don't give a delay; but it's typical you need one.
    delay_ms(100);
}


// block until there is data and then return it (raw)
//
// p26 interprets the data.
// if high bit is set, then accel is negative.
//
// read them all at once for consistent
// readings using autoincrement.
// these are blocking.  perhaps make non-block?
// returns raw, unscaled values.
imu_xyz_t accel_rd(const accel_t *h) {
    // not sure if we have to drain the queue if there are more readings?

    unsigned mg_scale = h->g;
    uint8_t addr = h->addr;

    // right now this doesn't do anything.
    while(!accel_has_data(h))
        ;


    // read in the x,y,z from the accel using imu_rd_n
    int x = 0, y = 0, z = 0;

    unimplemented();

    return xyz_mk(x,y,z);
}


/**********************************************************************
 * trivial driver.
 */
void notmain(void) {
    delay_ms(100);   // allow time for device to boot up.
    i2c_init();
    delay_ms(100);   // allow time to settle after init.

    // from application note.
    uint8_t dev_addr = 0b1101000;

    enum { 
        WHO_AM_I_REG      = 0x75, 
        // this is default: but seems we can get 0x71 too
        WHO_AM_I_VAL1 = 0x70,       
        WHO_AM_I_VAL2 = 0x71 
    };

    uint8_t v = imu_rd(dev_addr, WHO_AM_I_REG);
    if(v != WHO_AM_I_VAL1 && v != WHO_AM_I_VAL2)
        panic("Initial probe failed: expected %b or %b, got %b\n", 
            WHO_AM_I_VAL1, WHO_AM_I_VAL2, v);
    else
        printk("SUCCESS: mpu-6500 acknowledged our ping: WHO_AM_I=%b!!\n", v);

    // hard reset: it won't be when your pi reboots.
    mpu6500_reset(dev_addr);

    // first test that your scaling works.
    // this is device independent.
    test_mg(0, 0x00, 0x00, 2);
    test_mg(350, 0x16, 0x69, 2);
    test_mg(1000, 0x40, 0x09, 2);
    test_mg(-350, 0xe9, 0x97, 2);
    test_mg(-1000, 0xbf, 0xf7, 2);

    // part 1: get the accel working.
    accel_t h = mpu6500_accel_init(dev_addr, accel_2g);
    assert(h.g==2);
    for(int i = 0; i < 100; i++) {
        imu_xyz_t xyz_raw = accel_rd(&h);
        output("reading %d\n", i);
        xyz_print("\traw", xyz_raw);
        xyz_print("\tscaled (milligaus: 1000=1g)", accel_scale(&h, xyz_raw));

        delay_ms(1000);
    }
}
