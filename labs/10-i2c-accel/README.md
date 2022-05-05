## Using an accelerometer

Make sure you do the prelab!

Today we're going to communicate with an accelerometer (the  MPU6500)
using I2C.   These devices are useful for controlling stuff based
on motion.

The `docs` directory has a bunch of documents.

[A nice clear SPARKFUN tutorial on gyros](https://learn.sparkfun.com/tutorials/gyroscope/all).

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
