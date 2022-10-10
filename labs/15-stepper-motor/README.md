### Driving steppers with an A4988 driver.

Today we're going to use a `A4988` board to drive a nema 17 stepper.

  - [The datasheet for the A4988](docs/A4988.pdf) is kinda long but
    almost all is about dimensions and other stuff you can skip.

This is a pretty open-ended lab.   It's based on the lab written by Alex
Fu for the first cs340lx: [Alex README](README-Alex.md).  Feel free to
just use his lab and code (`alex-code`) --- it's much more thorough :).
Certainly read through it for extension ideas!

We'll first just get the stepper working in a simple way.  Then it's a
choose your adventure --- there's a ton of interesting things to do.

If you want further reading, in addition to Alex's lab, I thought these
were two reasonable stepper motor writeups; they are "for arduino"
but you can skip all that stuff:
  - [a4988 stepper tutorial 1](https://lastminuteengineers.com/a4988-stepper-motor-driver-arduino-tutorial/)
  - [a4988 stepper tutorial 2](https://howtomechatronics.com/tutorials/arduino/stepper-motors-and-arduino-the-ultimate-guide/)

-----------------------------------------------------------------------
### Part 0: wire up the stepper

Wire up the board and the stepper using the [PRELAB](PRELAB.md).
One non-essential difference is that the PRELAB uses a nema 23 motor
and we have nema 17.

You'll need a screwdriver and an electrical outlet. 

One difference from the PRELAB: Ideally you'll have a multimeter and set
the current limiter discussed in the tutorials above.  (I didn't do this,
but we should.)

After wiring everything up, you should be able to run the given test
program from Alex which plays the USA anthem (sardonically appropriate
given the current idiots pushing for nuclear war):

    % staff-binary/stepper_test.bin 

If this doesn't work, check with someone else!  There's a lot of miswiring
that can happen.   We also have cheap knock-off A4988 IC's, so it's not
a surprise if one is broken.

-----------------------------------------------------------------------
### Part 1: make the stepper do one revolution at a constant speed

We first do the "hello world" of steppers: making it do a single,
exact rotation.

If you look at the data sheet (especially p 6), you can see that we
tell the driver to move forward one step by raising a pin high for a
minimum of 2 microseconds then low for another 2usec (plus some hold
and transition time).

This method is obviously yet another another example of the common "set
pin high, low + time" low-complexity protocols we've done already: sonar,
the addressable light arrays, IR remote, etc.  However, unlike these other
examples, if you obey the timing requiremements it's possible nothing
will work!   The reason: The motor is a real object and has inertia
--- the speed it can run starting from rest is much much slower than a
step done in a total of roughly 5usec.  If you run the motor too fast,
its failure mode is that the stepper will not perform the step --- such
"missed steps" can lead to catastrophic failure when steppers are used
in real systems such as metal cutting machines.

So, our first project is to figure out roughly what the constant speed
we can run the stepper motor at without missing steps. Given the lack
of tools in our classroom environment we'll use our eyes rather than a
measurement system (though as we'll discuss below you have enough tools
to build such a measurement system already using a hall effect sensor).

   - The motor spindle has a flat surface.  Orientate this flat where
     you can easily check it's in a known position (e.g., pointing up
     or diagonally in line with one of the screw holes).

   - Pick large number of usec to wait between step commands
     (or the step hold times --- I think is equiv).   Verify that turning
     a full circle in one direction works.  If it doesn't you'll have
     to slow down even more (or fix your code :).

   - Verify that you can turn back and forth some number of complete
     rotations and the flat always returns to the same position.

   - Trim some more delay off (hint: binary search isn't just for code).

You'll notice that the value you get is *very* far away from what the 
datasheet limit is.

Great, we now have a known working system.  

Note that losing steps error doesn't come from going too slow but too
fast, so given a constant speed error that just makes you go more slowly
is ok.


-----------------------------------------------------------------------
### Part 2: build a stepper UART

Use the motor to transmit using a very low baud rate.  Use the adc+mic
from lab 8 to read the stepper noise.  You'll want to figure out the
midpoint for the level reading --- above this is a 1, below is a 0.

The easiest way to build seems to be using threads where you yield
from one to the other in the delay.  You may want to oversample (e.g.,
read multiple per period) and perhaps discard values that are "too high"
in case there is a burst of ambient noise.

There is some basic code in `code-sw-uart`.  Two main programs:

 - `interleave-example.c`: an example of how to interleave two routines
   using your threads package.   Since we have cooperative threads and
   one CPU we interleave by doing a yield whenever we are doing a 
   busy wait (as you immediately expect: its easy to forget a yield!
   So it's useful to write a checker).   We rely on the package being
   round robin to schedule everything appropriately.

   Alternatively we could interleave using interrupts.  Or, with less
   state explosion: by interleaving calls to run-to-completion routines
   rather than doing a thread yield.  (We may explore this later.)

 - `measure-motor.c`: we use similar logic to use the ADC to measure
   the motor while we are driving the stepper.  You'll need to rewrite
   your `a4988.h` header to call the `delay_us_yield` routine (in
   `delay-yield.h`) so that this works.

   I got around 13k for motor off and around 15k for motor on.  This
   isn't a huge difference but should be enough to transmit bits.

You can run these by changing the `Makefile` target.

Given these examples you should hopefully (I haven't finished mine!)
software uart that transmits bits using the motor.   

-----------------------------------------------------------------------
### Option: learn notes.

Use the stepper to make a note at a given frequency.  Pick this up with
the adc+mic and do an fft to get the frequency.   Similar to the stepper
UART except we need to be accurate about the frequency. Hopefully Parthiv
can show us how to do this accurately!

-----------------------------------------------------------------------
### Option: use interrupts to do part 1.

A common --- I would argue too-common --- approach to using devices
with real-time requirements such as our stepper driver is to immediately
switch to using interrupts.

The basic idea: set a timer interrupt trigger that periodically fires
at the smallest delay needed by the steppers (or one that divides all
needed delays) and in the interrupt handler toggle the appropriate pin.

Since this is so common, it makes sense to do as a frame of reference.
Basic approach:
 - Grab the timer interrupt code from 140e.
 - Set the timer interrupt to some value that allows you to 
   satisfy your timing requirements from part 1.  (Make sure you 
   check this by recording the time since the last interrupt
   and making sure its what you expect!)
 - Adapt the code from part 1 to run and set the right pin in the
   interrupt handler.

One nice thing w/ this setup is that you can easily do other tasks
in non-interrupt code.

A second, more generic approach:
  - Build a simple "timed-write" system where you record the `pin, value,
    deadline` for a series of writes and give this data structure to the
    interrupt code which is then responsible for issuing these writes
    after each deadline.

  - This lets you implement the stepper.  It also would let you do 
    general bit bangining.

  - You can extend this approach to do reads as well (so you could do
    sw-uart).  Or even more general, to run a set of given procedures at
    deadlines (including those above that just perform a write).  You can
    view these are run-to-completion realtime threads.  The period of
    the timer interrupt would be roughly the maximum error.

Note: if non-interrupt code shares data with interrupt code you will
have to avoid race conditions.   For example, where their simultanous
mutations corrupt data or one of them can read stale data.  Perhaps the
most common hack: simply disable interrupts when you do any mutation.
However disabling can easily lead to problems such as forgetting to
re-enable, or by disabling too-long, etc.  Another way is to use a
lock-free data structure such as a circular queue.

Note: Alex's README and code has a ton of information for this approach.

-----------------------------------------------------------------------
### Option: acceleration.

Once the stepper is up and running, it can go much faster than the speed
you came up with in Part 1 to go starting from rest. 

For machines that use steppers, time is money, so the common approach
to going faster: you accelerate at a constant rate (meaured in increases
in steps per second), go at a steady max velocity, and then decelerate
at a constant rate.  If you are close to the limit of the motor the
deceleration rate / time will obviously need to match the acceleration.
The shape of this acceleration-constant-deceleration looks like a
trapazoid, so that's the slang for this pattern.  (Optimal would look
not quite as clean, but it's unclear how to derive optimal analytically.)

Note that unlike with constant velocity, going "too slow" in terms of
stepper interactions is no longer safe on the deceleration end.

I don't have a good trick for this part.  So it'd be cool if you figure
one out.  There's tons of kinda-too-abstract web pages and pdf's
--- if you find a good one post to the newsgroup!  With that said,
a lot of people seem to use this TI note on stepper acceleration:
[stepper-accel](./docs/stepper-accel.pdf).

From Alex: Recommendations for highest speed: use your smallest
microstepping option, use delay_ncycles for finer granularity. Also,
be careful because the time which you delay between steps isn't exactly
equal to the psychical time between steps, since our step function takes
time. My fastest speed (on a nema 17 motor--haven't tested with the nema
23) was 8 usec (physical time, not delay time) per 16th step, or 2343
rpm. As a comparison, with neither accelerations nor microstepping,
my best rpm on that same motor was 336 rpm. For the nema 23: rpm 457
vs 10 rpm. Maybe you can do better! (Note: another cool thing about
steppers, *I think*, is that regardless of how fast they're spinning,
they consume the same amount of power. It's very likely that I'm wrong,
but based on steppers work, that's my impression).

-----------------------------------------------------------------------
### Extension: Accelerometer [Alex]

Make a 1D gimbal with your accel. If I'm not mistaken, the accel measures
forces that the chip feels. And since gravity is, well, gravity, we can
use it as a reference point for determining orientation. So if we put
things together like this:

![accel stepper](images/accel_stepper.JPG) 

Then we can program a self-balancing rig. My code is pretty simple:
1. I read from the accel to get a "zero" reference point
2. I begin a loop where I check the accel reading, compare it to the reference point, and then step in the appropriate direction to correct any movements. 
3. Then I delay for ~40ms, because when the stepper is stepping (and for a little time afterwards) our accel's readings aren't what we want, since there are other forces besides gravity now acting on our stepper. 
This way of doing things isn't the greatest, because there's a decent amount of lag in the position correcting, since we're not stepping fast enough due to the added delay (40ms between steps & 200 steps per rotation => 8 seconds per rev, very slow). Try to figure out something better than I did!

-----------------------------------------------------------------------
### Extension: Microstepping [Alex]

This one is very easy and pretty nice. Read up on how a4988 does
microstepping (or look at image below). Makes your stepper a lot
quieter and smoother. One challenge is that you lose speed, since your
steps sizes are smaller (everything will be N times slower if you do N
microsteps). You can counteract this by allowing your interrupt handler
to trigger N times more often than it previously did.

![microstepping](images/a4988_microstepping.png)

You might also notice that we ony get 1/16th microsteps with this
driver. Though that allows for pretty smooth control, theoretically we
can get way better. Some drivers give 1/256 microsteps. (I think) we can
"simulate" microsteps in software. The idea is that in a short period of
time, we switch between stepping forwards and backwards very quickly. And
if we're faster than the time it takes for the motor to complete a step,
then the motor will hang in the middle, "between" steps. You can use
this to get 1/2th microsteps and I think 1/32 microsteps if you set
the hardware to already be in 1/16th microsteps. I think you can also
use timings to get 1/Nth microsteps (if I step forwards for 75% of the
time and backwards for 25% of the time, then I'll be in a 3/4ths step
position?), but no clue if this is legit.


-----------------------------------------------------------------------
### Extension: Do something cool [Alex]

Ideas:
  - We have three hall effect sensors and a magnet--maybe you could
    build a basic encoder and use this in addition to running the motor at
    a fast speed over a long amount of time to check if / how many steps
    the motor skips?

  - sonar taped to shaft, to create point cloud map of your room. The
    resolution of your map is related to the size of your motor's
    steps/microsteps, and the amount of patience you have to generate a map.

  - Something with accelerometer. For example, have a laser pointer point
    in whatever direction you orient the accelerometer in. Alternatively,
    have laser pointer point in same direction no matter how you rotate
    the body of the stepper with your hand. Also: maybe replace laser
    pointer with your device of choice.

  - The light strip. Do something cool. 

  - Use the fact that we can generate tones with the stepper. Perhaps
    encode a secret message in the tones you generate, and see if we can
    decode it over zoom's audio and microphones connected to our Pi's.

  - Courtesy of Akshay, We've seen what the unix kernel *looks like*,
    now: what does it sound like?

  - Use your computer mouse to control stepper somehow?

  - Make your keyboard a *keyboard* piano. If you hold down a key, the
    stepper plays a note. If you lift a key, the stepper stops playing the
    note. How do deal with the situation where multiple keys are pressed?

