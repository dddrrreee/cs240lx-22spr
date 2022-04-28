## Using the I2C protocol to use an analog-to-digital converter (ADC)

So far this quarter we've interacted with digital devices that give a
0 or a 1 as output, possibly with a time delay (e.g., ir, ws2812b).
Many devices (especially the cheaper, simpler ones) convey information
by outputing a varying voltage instead.  In addition to cost, often an
analog device has a significantly simpler datasheet and is easier to
get working than a digital one.

The downside of the R/PI is that it has no way
to read these directly.  Instead we have to use an
[ADC](https://en.wikipedia.org/wiki/Analog-to-digital_converter) to
convert the voltage to a number.

You're going to do three things:
  1. Write a quick driver for the `ads1115` analog-to-digital converter.
     This will give us a way to get a known signal and see that you 
     are correct.

  2. Use it to get the reading from a potentiometer.  At the lowest setting
     it should be around 1, at the highest around 16k.

  3. Add a microphone and see that you get reasonable readings.

  4. Extension: make a pretty equalizer using the ws2812b
     by using the adc to read from a microphone, doing an FFT on the
     data and display the results in a nice way (e.g., seperating the
     bass, mid and treble; changing color based on magnitude and rate
     of change, etc.)

  5. Extension: write your own i2c driver.   The easiest approach would
     be to bit-bang it.  More fancy is to use the broadcom hardware.

---------------------------------------------------------------------------
### Floating point: there is now a floating point target.

The `float-test` directory has a simple test for floating point.  Note: we
compile libpi in two ways: both with and without floating point support.

Currently for your staff objects you'll have to do a gross hack to
select whether it's the floating point version or not: if you look in
`manifest.mk` you can see how we get the object in `staff-objs/fp`
(if `USE_FLOAT` is defined) or from the original `staff-objs/` (if not).
There's likely a better way but this was expedient before lab.

---------------------------------------------------------------------------
### Part0: hook up your potentiometer and ADC.


First off to make sure your basic hardware is working, use the breadboard
to hook up the potentiometer given in class (the little knob) to the
power and ground from the pi and a middle leg controlling an LED.
The LED should go bright and low as you turn the knob.  You'll keep
this setup even when you're using the ADC so that you can quickly check
visually that the ADC readings make sense.

<table><tr><td>
  <img src="images/pot-back.jpg"/>
</td></tr></table>


Next hook up your ADC to the breadboard as well, with:
  - `a0` connected to the middle of the potentiometer (and LED).
  - `g` to ground
  - `v` to 3v.
  - the two i2c pins: `sda` and `scl`.


<p float="left">
  <img src="images/pot-adc.jpg" width="300" />
  <img src="images/rpi-adc.jpg" width="300" />
</p>

You should be able to run the `staff-binary/ads1115.bin` and get output.

---------------------------------------------------------------------------
### Part 1: write the ADC driver (using the `staff-i2c.o` i2c)

NOTE: 
   - If you run `make` in the `code` directory, it will compile a binary
     using the staff `.o` files.  You should be able to run this binary.

We want a known signal to check your ADC implementation.  Using the
potentiometer makes this relatively easy.  It also re-enforces that
after awhile, datasheets look very similar and they often aren't that bad.

This code will closely mirror your last lab.  To save time, the starter
code has some (but not all) page numbers.  There's an annotated datasheet
in `./docs/ads1114.annoted.pdf`).  It's 56 pages(!)  but you only need
a few parts (they should all be highlighted in red).

For this part:
  1. Implement `adc_write16` and `adc_read16` to read and write the 
     16-bit registers on the ads1115 using `i2c_write` and `i2c_read`.
  2. Figure out how to set the configuration register to have 860 samples per
     second and a +/- 4v range.
  3. Make sure your readings make sense.

The I2C model has similarities to our use of `GET` (`i2c_read`) and `PUT`
(`i2c_write`) --- each call takes an 8-bit `address` (i2c calls these
register addreses, but they don't have to literally be registers on
the device) and  either sets it to a given value (`PUT`) or gets the
existing value (as with `GET`).  There is an I2C driver checked in,
so you don't have to write your own --- however, if you're looking for
an interesting extension, doing so is fun!

So your workflow should be:
  1. Look up the different register names in the document (p 27).
  2. Look up their format.
  3. Use i2c calls to set and get them.  (see `code/i2c.h` for the routines you
     can call).  Just as with GPIO: pay attention to when you need to preserve
     old values or not.  

     The i2c routines take an array of unsigned bytes, where the first
     element is the 8-bit register number and (if you are doing a write)
     the next two bytes are the high 8-bits and then the low 8-bits of
     the 16-bit value being set.  

     The main weird aspect: as you can see on page 24, rather than include
     the register number in the `i2c_read` command as you would expect,
     you instead first *write* the register number using `i2c_write`
     (e.g., `i2c_write(dev_addr, &reg, 1)`,  where `reg` holds the
     8-bit register name and `dev_addr` the device address) and *then*
     do an I2c read of two bytes (eg., `i2c_read(dev_addr, data, 2)`
     where data is a two byte array).

     A bit confusingly: for an `i2c_read`, the high byte of the result
     is returned in the low byte (`data[0]`), and the high byte in the
     high byte (`data[1]`).  You can figure this out by (1) reading the
     configuration register and (2) then print out its values and (3)
     comparing these to the default values ("reset") in table 8 on page
     28 and 29.

     The "quickstart" guide on p 35 gives an example.

This shouldn't take that long.  And having an ADC is super useful ---
there's a huge number of cheap analogue sensors and devices you can
now use.

If you need help debugging, here's a useful routine that will 
pretty print your configuration register:

        // print config
        void ads1115_print(const char *msg, uint32_t c) {
            printk("<%s> config:\n", msg);
            printk("\tOS[15]=%b\n", bit_get(c,15));
            printk("\tMUX[12:14]=%b\n", bits_get(c,12,14));
            printk("\tPGA[9:11]=%b\n", bits_get(c,9,11));
            printk("\tMODE[8]=%b\n", bit_get(c,8));
            printk("\tDR[5:7]=%b\n", bits_get(c,5,7));
            printk("\tCOMP_MODE[4]=%b\n", bit_get(c,4));
            printk("\tCOMP_POL[3]=%b\n", bit_get(c,3));
            printk("\tCOMP_LAT[2]=%b\n", bit_get(c,2));
            printk("\tCOMP_Q[0:1]=%b\n", bits_get(c,0,1));
        }

---------------------------------------------------------------------------
### Part 2: use your adc to decode readings from the mic

We gave out microphones (`docs/max4466-ebay.pdf`) in class.  You should
download the datasheet, and get make sure you get some sensible data.

---------------------------------------------------------------------------
### Exension: use the alert pin to detect when a reading is available.

There is an alert pin on the adc that indicates when there is data (see page
19 and related of the datasheet).  Use this to detect you can read, and verify
that the rate you observe makes sense.

---------------------------------------------------------------------------
### Exension: hook your light strip up to the microphone output.

It's pretty easy to use the ADC to hook it up to your light strip and
display the results.  The easiest way is to just go by the level of the
mic.  You can also use the floating point support in libpi to do a simple
FFT and display that.

---------------------------------------------------------------------------
### Exension: implement your own i2c driver.

There are annotated documents in the `docs` directory that decribe

You can flip between our implementation and yours by modifying the `Makefile`.
The annotated broadcom has the information you'l need.

Note:
  1. You'll have to set the pins 2  (`SDA`) and 3 (`SCL`) pins to be used
     for i2c using `gpio_set_function` and the information on page 98.
  2. You'll want to use `dev_barriers()` here.
  3. Use the flags in the i2c fields!  Make sure don't start sending
     when the i2c hardware is still busy or when there is no space on
     the fifo.  Make sure if you read there is data there.  Make sure
     you check if there are errors.

As a first step, do the structure layout so that the field fields pass the
assertions.

Use the `get32_T` and `put32_T` macros as discussed in the prelab.
So your code will look something like:

    c_register c = get32_T(&i2c->control);
        c.clear = 0b11; // shouldn't need, but hygenic(?)
        c.st = 1;
        c.read = read_p;
    put32_T(i2c->control, c);

`i2c_init`:
    1. Setup the pins
    2. Enable the i2c device.
    3. Reset the `done` flag and any errors.
    4. At the end: Assert anything useful.  For example, that a transfer is not active.

On both `i2c_read` and `i2c_write` make sure you start by reseting the
`done` flag and clearing any errors.
