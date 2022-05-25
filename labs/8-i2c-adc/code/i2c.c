/*
 * engler: simple starter code for cs240lx i2c driver.
 *
 * simplified i2c implementation --- no dma, no interrupts.  the latter
 * probably should get added.  the pi's we use can only access i2c1
 * so we hardwire everything in.
 *
 * note: our code differs from cs107e code b/c it makes the strong 
 * iffy assumption that
 *		*addr = val 
 * 	or 
 *		struct->field |= val
 *	is *guaranteed* to result in a single 32-bit store/load rather than 
 * 	potentially having a single 8 bit (or 16 bit) load / store
 *	(or even multiple stores).  this might be a reasonable assumption.
 *	however, it's actually a big ask to guarantee that gcc under any
 *	optimization level you give it (esp no -O) with lots of weird
 *	addresses and volatile will always do single 32-bit stores even
 *	when you are modifying sub 32-bit quantities in the word.  
 *
 *	related: if you do structure assignment:
 *		foo(struct foo *dst, struct foo *src) {
 *			*dst = *src;
 *		}
 *	gcc may call memcpy and the current implementation is a byte copy.
 *	broadcom says nothing (afaik) that reassures that writing a byte
 *	at a time to a 32 bit broadcom register is gonna necessarily be ok.
 * 
 * Starts at p28.
 *
 * There are three BSC masters inside BCM. The register addresses starts from
 *	 BSC0: 0x7E20_5000
 *	 BSC1: 0x7E80_4000
 *	 BSC2 : 0x7E80_5000
 * the PI only has BSC1.
 */
#include "rpi.h"
#include "libc/helper-macros.h"
#include "i2c.h"

/* broadcom p30 */
typedef struct {
	uint32_t read:1,	// :0 READ transfer.  
				// 0 = write packet, 1 = read packet.
		__unused0:3,		// 1:3, reserved: write as 0.
		clear:2,	// 4:5 fifo clear
				// 0b00 = no action
				// 0bx1 = clear fifo, one shot
				// 0b1x = clear fifo, one shot
				// read back as 0.
				// 2-bits b/c back-compat.
		__unused1:1,		// :6 write as 0
		st:1,		// :7, start xfer
				// 0 = no action
				// 1 = start a new xfer.  
				// one shot, read as 0.
		intd:1,		// :8, intd interrupt done
				// 0 = don't generate interrupts on done.
				// 1 = generate interrupt DONE = 1.
		intt:1,		// :9 interrupt on tx
				// 0 = don't generate ints on txw condition
				// 1 = generate interupt while TXW = 1
		intr:1,		// :10 interrupt on rx.
				// 0 = don't interrupt
				// 1 = generate interrupt while RXR = 1.
		__unused2:4,		// 11:14 - reserved, write 0.
		i2cen:1,	// 15 i2c enable
				// 0 = disabled
				// 1 = enabled.
		__unused3:16;		// reserved: write as 0.
} c_register;

static c_register read_c(volatile c_register *c) {
	return get32_T(c);
}

// p31 status register: recover status, errors, int requests.
// thorough description.
typedef struct {
	unsigned ta:1,	// :0
				// 0 = transfer not active,
				// 1 = xfer active.
		done:1, 	// :1
				// 0 = xfer not completed
				// 1 = xfer complete.  
				// *cleared by writing 1 to field.
		txw:1,		// :2 fifo needs writing (full)
				// 0 = FIFO is at least full and a write 
				// is underway.
				// 1 = FIFO is less than full and a write is
				// underway.
				// cleared by writing enough data to FIFO(?)
		rxr:1,		// :3 fifo needs reading.
				// 0 = FIFO < full and read is underway(?)
				// 1 = FIFO is more full and a read is 
				// underway(?)
				// cleared by reading sufficient data from 
				// FIFO
		txd:1,		// :4 fifo can accept data.
				// 0 = fifo is full.
				// 1 = fifo has space for at least 1 byte.
		rxd:1,		// :5 fifo contains data
				// 0 = fifo is empty
				// 1 = fifo has at least 1 byte.
				// cleared by reading.
		txe:1,		// :6 fifo empty
				// 0 = fifo not empty
				// 1 = fifo is empty.
				// if write underway no further serial 
				// data can be xmitted(?)
		rxf:1,		// :7 fifo full
				// 0 = fifo not full
				// 1 = fifo full.  must read
		err:1,		// :8 acck error
				// 0 = no errors
				// 1 = slave not ack address
				// *cleared by writing 1.
		clktk:1,	// :9 clock stretch timeout
				// 0 = no errors detect
				// 1 = slave held scl signal low
				// for longer than specified in icclktk
				// * cleared by writing 1.
		__unused0:22;
} s_register;


static s_register read_s(volatile s_register *s) {
	return get32_T(s);
}

typedef struct RPI_i2c {
	c_register control;
	s_register status;

#	define check_dlen(x) assert(((x) >> 16) == 0)
	uint32_t dlen; 			// number of bytes to xmit, recv
					// reading from dlen when TA=1
					// or DONE=1 returns bytes still
					// to recv/xmit.  
					// reading when TA=0 and DONE=0
					// returns the last DLEN written.
					// can be left over multiple pkts.

#	define check_dev_addr(x) assert(((x) >> 7) == 0)
	uint32_t 	dev_addr;
	// only use the lower 8 bits.
	uint32_t fifo;
#	define check_clock_div(x) assert(((x) >> 16) == 0)
	uint32_t clock_div;
	// we aren't going to use this.
	uint32_t clock_delay;
	// we aren't going to use this.
	uint32_t clock_stretch_timeout;
} RPI_i2c;

// broadcom: 
// 0x20205000 	// BSC0
// 0x20804000; 	// BSC1
// 0x20805000 	// BSC2
static volatile RPI_i2c *i2c = (void*)0x20804000; 	// BSC1

// this should be able to fail, right??
int i2c_write(unsigned addr, uint8_t data[], unsigned nbytes) {
    // unimplemented();
    dev_barrier(); 

	// clear out done flag and any errors
	volatile s_register *i2c_s = &i2c->status;
    s_register i2c_status = read_s(i2c_s);
    i2c_status.done = 1;
    i2c_status.clktk = 1;
    i2c_status.err = 1;

	put32_T(i2c->status, i2c_status);

	
	// i2c_status = read_s(i2c_s);
	// wait until any previous xfer over.
	while (i2c->status.ta != 0);
	// while(1) {
	// 	i2c_status = read_s(i2c);
	// 	// printk("status.ta is %d\n", i2c_status.ta);
	// 	if (i2c_status.ta == 0) break;
	// } 

	volatile c_register *i2c_c = &i2c->control;
    c_register i2c_control = read_c(i2c_c);

	i2c_control.clear = 0b10;
	i2c_control.read = 0;
	i2c_control.st = 1;

	put32_T(i2c->control, i2c_control);

	i2c->dlen = nbytes;
	i2c->dev_addr = addr;

	for (unsigned i = 0; i < nbytes; i++) {
		while (i2c->status.txd != 1);
		i2c->fifo = data[i];
	}

    // wait for current xfer to complete
	while (i2c->status.done != 1); // uncomment

	i2c_status = read_s(i2c_s);
	i2c_status.done = 1;
	put32_T(i2c->status, i2c_status);

    dev_barrier(); 
	return 1;
}

// should be able to fail, right?
int i2c_read(unsigned addr, uint8_t data[], unsigned nbytes) {
    // unimplemented();
	dev_barrier();

	// clear out done flag and any errors
	volatile s_register *i2c_s = &i2c->status;
    s_register i2c_status = read_s(i2c_s);
    i2c_status.done = 1;
    i2c_status.clktk = 1;
    i2c_status.err = 1;

	put32_T(i2c->status, i2c_status);

    // wait until any previous xfer over.
	while(i2c->status.ta != 0);

	volatile c_register *i2c_c = &i2c->control;
    c_register i2c_control = read_c(i2c_c);

	i2c_control.clear = 0b10;
	i2c_control.st = 1;
	i2c_control.read = 1;
	i2c->dev_addr = addr;
	i2c->dlen = nbytes;
	put32_T(i2c->control, i2c_control);

	for(unsigned i = 0; i < nbytes; i++)  {
		while (i2c->status.rxd != 1);
		data[i] = i2c->fifo;
	}	

	// printk("entering final");
    // wait for current xfer to complete
	while (i2c->status.done != 1); // uncomment

	i2c_status = read_s(i2c_s);
	i2c_status.done = 1;
	put32_T(i2c->status, i2c_status);
		
    dev_barrier();
	return 1;
}

static void check_layout(void); 

// could bind addr here, or could redo?
// is the i2c dev different than gpio?  yes?  btgpio_set_functionut
// can only talk to it via gpio so maybe we don't
// need dev barriers.
void i2c_init(void) {
	check_layout();

    // we don't know what was happening before.
	dev_barrier();

    // unimplemented();
    
    // set up pins 2 and 3 to SDA and SCL (see BCM 102)
    gpio_set_function(2, GPIO_FUNC_ALT0);
    gpio_set_function(3, GPIO_FUNC_ALT0);

    // dev_barrier because we are going from gpio to i2c hardware
    dev_barrier();

    // Enable i2c device
    // RPI_i2c i2c_mem = get32_T(i2c);
	volatile s_register *i2c_s = &i2c->status;
    s_register i2c_status = read_s(i2c_s);
	volatile c_register *i2c_c = &i2c->control;
    c_register i2c_control = read_c(i2c_c);

    i2c_control.i2cen = 1;
    i2c_status.done = 1;
    i2c_status.clktk = 1;
    i2c_status.err = 1;

    put32_T(i2c->status, i2c_status);
    put32_T(i2c->control, i2c_control);

    // i2c_status = read_s(i2c_s);
    // assert(i2c_status.ta == 0);

    // don't know what device is going to get used next.
	dev_barrier();
}

// sanity check that sizes and offsets are correct.
// would be nice if the bitchecks could be compile time.
static void check_layout(void) { 
    AssertNow(offsetof(RPI_i2c, control) == 0x0);
    AssertNow(offsetof(RPI_i2c, status) == 0x4);
    AssertNow(offsetof(RPI_i2c, dlen) == 0x8);
    AssertNow(offsetof(RPI_i2c, dev_addr) == 0xc);
    AssertNow(offsetof(RPI_i2c, fifo) == 0x10);
    AssertNow(offsetof(RPI_i2c, clock_div) == 0x14);
    AssertNow(offsetof(RPI_i2c, clock_delay) == 0x18);
    AssertNow(offsetof(RPI_i2c, clock_stretch_timeout) == 0x1c);

    AssertNow(sizeof(c_register) == 4);
	check_bitfield(c_register, read, 0, 1);
	check_bitfield(c_register, clear, 4, 2);
	check_bitfield(c_register, st, 7, 1);
	check_bitfield(c_register, intd, 8, 1);
	check_bitfield(c_register, intt, 9, 1);
	check_bitfield(c_register, intr, 10, 1);
	check_bitfield(c_register, i2cen, 15, 1);

    AssertNow(sizeof(s_register) == 4);
	check_bitfield(s_register, ta, 0, 1);
	check_bitfield(s_register, done, 1, 1);
	check_bitfield(s_register, txw, 2, 1);
	check_bitfield(s_register, rxr, 3, 1);
	check_bitfield(s_register, txd, 4, 1);
	check_bitfield(s_register, rxd, 5, 1);
	check_bitfield(s_register, txe, 6, 1);
	check_bitfield(s_register, rxf, 7, 1);
	check_bitfield(s_register, err, 8, 1);
	check_bitfield(s_register, clktk, 9, 1);
}