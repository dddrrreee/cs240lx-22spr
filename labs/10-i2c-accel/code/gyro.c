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

/*********************************************************************
 * gyro code
 */
typedef struct {
    uint8_t addr;
    unsigned hz;

    // scale: for accel is in g, for gyro is in dps.
    unsigned dps;
} gyro_t;

enum {
    // p13, p6
    CONFIG = 28, 
    // p14, p6
    GYRO_CONFIG = 29, 

    // p6, p14
    gyro_config_reg = 0x1b,
    gyro_250dps  = 0b00,
    gyro_500dps  = 0b01,
    gyro_1000dps = 0b10,
    gyro_2000dps = 0b11,

    // p7
    GYRO_XOUT_H = 67,
};


// returns the raw value from the sensor.
static short mg_raw(uint8_t lo, uint8_t hi) {
    return (short)(hi<<8|lo);
}



gyro_t mpu6500_gyro_init(uint8_t addr, unsigned gyro_dps) { 
    unsigned dps = 0;
    switch(gyro_dps) {
    case gyro_250dps: dps = 250; break;
    case gyro_500dps: dps = 500; break;
    case gyro_1000dps: dps = 1000; break;
    case gyro_2000dps: dps = 2000; break;
    default: panic("invalid dps: %b\n", dps);
    }

    // gyro config
    // set 20hz (p13)
    // set dps (p14)
    unimplemented();
    return (gyro_t) { .addr = addr, .dps = dps, .hz = 20 };
}

// use int or fifo to tell when data.
int gyro_has_data(const gyro_t *h) {
    return 1;
}

static int dps_to_scale(unsigned dps) {
    switch(dps) {
    // we do this just for testing.
    case 245:
    case 250:  return  8750;
    case 500:  return 17500;
    case 1000: return 35000;
    case 2000: return 70000;
    default: panic("invalid dps: %d\n", dps);
    }
}

static int mdps_scale_deg2(int deg, int dps) {
    // hack to get around no div
    if(dps == 250) 
        return (deg * 250) / SHRT_MAX;
    else if(dps == 500) 
        return (deg * 500) / SHRT_MAX;
    else
        panic("bad dps=%d\n", dps);
}

static int mdps_scale_deg(int deg, int dps) {
    return (deg * dps_to_scale(dps)) /1000;
}

static inline int within(int exp, int have, int tol) {
    int diff = exp - have;
    if(diff < 0)
        diff = -diff;
    return diff <= tol;
}

static void test_dps(int expected_i, uint8_t h, uint8_t l, int dps) {
    int s_i = mdps_scale_deg(mg_raw(l, h), dps);
    expected_i *= 1000;
    int tol = 5;
    if(!within(expected_i, s_i, tol))
        panic("expected %d, got = %d, scale=%d\n", expected_i, s_i, dps);
    else
        output("expected %d, got = %d, (within +/-%d) scale=%d\n", expected_i, s_i, tol, dps);
}

imu_xyz_t gyro_rd(const gyro_t *h) {
    // not sure if we have to drain the queue if there are more readings?

    unsigned dps_scale = h->dps;
    uint8_t addr = h->addr;

    while(!gyro_has_data(h))
        ;

    int x = 0, y = 0, z = 0;
    unimplemented();

    return xyz_mk(x,y,z);
}

// milli dps: i don't think is correct?
imu_xyz_t gyro_scale(gyro_t *h, imu_xyz_t xyz) {
    int dps = h->dps;
    int x = mdps_scale_deg2(xyz.x, dps);
    int y = mdps_scale_deg2(xyz.y, dps);
    int z = mdps_scale_deg2(xyz.z, dps);
    return xyz_mk(x,y,z);
}

// hard rset.
void mpu6500_reset(uint8_t addr) {
    // reset

    // just from accel.c
    unimplemented();
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
        WHO_AM_I_VAL2 = 0x71,
        WHO_AM_I_VAL3 = 0b1110100
    };

    uint8_t v = imu_rd(dev_addr, WHO_AM_I_REG);
    if(v != WHO_AM_I_VAL1 
    && v != WHO_AM_I_VAL2
    && v != WHO_AM_I_VAL3)
        panic("Initial probe failed: expected %b or %b or %b, got %b\n", 
            WHO_AM_I_VAL1, 
            WHO_AM_I_VAL2, 
            WHO_AM_I_VAL3, 
            v);
    else
        printk("SUCCESS: mpu-6500 acknowledged our ping: WHO_AM_I=%b!!\n", v);

    // hard reset: it won't be when your pi reboots.
    mpu6500_reset(dev_addr);

    // device independent testing.
    int dps = 245;
    test_dps(0, 0x00, 0x00, dps);
    test_dps(100, 0x2c, 0xa4, dps);
    test_dps(200, 0x59, 0x49, dps);
    test_dps(-100, 0xd3, 0x5c, dps);

    // part 2: get the gyro working.
    gyro_t g = mpu6500_gyro_init(dev_addr, gyro_250dps);
    assert(g.dps==250);
    for(int i = 0; i < 100; i++) {
        imu_xyz_t xyz_raw = gyro_rd(&g);
        output("reading gyro %d\n", i);
        xyz_print("\traw", xyz_raw);
        xyz_print("\tscaled (milli dps)", gyro_scale(&g, xyz_raw));
        delay_ms(1000);
    }
}
