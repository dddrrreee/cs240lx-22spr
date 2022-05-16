## Using an accelerometer

Today we're going to communicate with an accelerometer (the  MPU6500)
using I2C.   These devices are useful for controlling stuff based
on motion.

The `docs` directory has a bunch of documents.

[A nice clear SPARKFUN tutorial on gyros](https://learn.sparkfun.com/tutorials/gyroscope/all).

---------------------------------------------------------------------------
### Incomplete cheat sheet of page numbers.

The overall view of the device (`docs/MPU-6500-spec.annot.pdf`): 
  - p7: i2c rate (400khz)
  - p8: overview of gyro and accel specs.  accel can eat a 10000g impulse ok.
  - p9/p10: useful distilled values for settings.
  - p11: max voltage 3.45: use the 3v pin!
  - p12: i2c addresses.
  - p22: what accel measures at rest and the DMP.
  - p25: seems to have a self test.  we should use it!
  - p 29: wake on motion.  (not sure if DMP do other things -- good to find out).
  

The register document (`docs/MPU-6500-Register-Map2.annot.pdf`): 
what register to read/write with what values:

  - p6: entire register map.
  - p7: reset values 0 for everyhying but power management and `WHO_AM_I`
  - p14: gyro configuration (reg 27).
  - p14/p15: accel configuration 1 (reg 28: self test) 
        and and accel configuration 2 (reg 29: data rates).
  - p18: FIFO control for data output (reg 35).
  - p19: i2c clock rate: you can change the i2c clock speed.
  - p29: interrupt configure (reg 55) and interrupt enable (reg 56): I haven't
    used these but they are good because you can make sure you don't read
    stale data and don't miss anything.
  - p30: interrupt status (reg 58).
  - p31/32: accel readings (reg 59-64): you get a high byte and a low byte.
  - p33: temperature (65 and 66).

  - p33/p34: gyro output (67-72).
  - p40: user control over bunch of stuff.
  - p41: power managmement 1 (reg 107): use to reset the device, configure stuff.
  - p42: power management 2 (reg 108): turn on accel and gyro.  need this.
  - p43: FIFO count registers.
  - p44: `WHO_AM_I` (117): use this to validate you can read from the chip and its
    responsive.
    
  

---------------------------------------------------------------------------
### Part1: fill in the accelerometer code in the code directory.

Get your hardware hooked up and see that the staff binary worked.
Half (!) of the devices seem busted, so this is a crucial step.

Use the datasheet and application note from the docs directory.
There are notes in the code.

---------------------------------------------------------------------------
### Part2: fill in the gyroscope code in the code directory.

Use the datasheet and application note from the prelab.  Start with
the simple cookbook example they give and make sure your stuff looks
reasonable!

---------------------------------------------------------------------------
### Extension: display the readings using your light strip.

The nice thing about the light strip is that you can do high-dimensional displays easily.
One dumb way:
   - give the accel half and the gyro half (or do not!  just map their coordinate system
     to the entire thing).
   - map the 3-d point of each reading to a location.
   - map the velocity of the point (or the accel, no pun) to a color.
   - display!

---------------------------------------------------------------------------
### Some Legit Extensions

If you finish, there's lots of tricks to play.  A major one is doing correcton to 
the device errors.  The accel / gyro device is not that accurate without correction.  This 
article discusses the issues and what can be done (simply):
    - [Correct for hard-ion](https://www.fierceelectronics.com/components/compensating-for-tilt-hard-iron-and-soft-iron-effects)

Different writeups in `./docs` for different directions:
   1. [Make a legit compass](./docs/AN203_Compass_Heading_Using_Magnetometers.pdf)
   2. [Calibrate](./docs/AN4246.pdf)
   3. [Even more compass + Calibrate](./docs/AN4248.pdf)
   4. [Location](./docs/madgewick-estimate.pdf)
