### Overview: reverse engineering a infrared remote control.

Today we're going to use an IR sensor --- [the Vishay TSOP4838](https://www.newark.com/webapp/wcs/stores/servlet/ProductDisplay?catalogId=15003&productSeoURL=vishay&partNumber=60K6999) to reverse engineer the
key presses from a remote so that you can control your pi using one.

Where stuff is:

  - The datasheet is in `4-ir/docs/tsop4838.pdf`.  You should read
    this to make sure (1) you know which pins to connect to and (2)
    what min and max power they expect.

  - The code is in `4-ir/code/ir-tsop.c`.

  - We have remotes.  However, if
    you have your phone, you should be able to download a "universal
    remote" app and use that.  A cool side-effect is having the
    infrastructure to control your pi from your phone.

    Note: many (most?) remote apps suck, so it makes sense to poke around
    on reddit or stackoverflow for a simple one.

*Avoid this major, common mistake*:

  - Do not print values *while* recording times, since that messes up
	the timing.  Instead when the IR has been low for "a long time",
	emit all the timings you've seen.


First steps:
  1. Write the code to read from the input pin.  

  2. Remotes communicate by sending values for a speicific amount of
     time.  So to start off record the value sent and the time it was sent.

  3. A key press transition is typically signaled with a "very long"
     on-value.	Then a set of timed off/on pulses.    You should be
     able to see this pattern in the values you print and it should be
     "somewhat" stable.

### Checkoff

Checkoff:
  1. You should pass the few tests.
  2. You should show that pressing four keys on your remote causes your
     pi to print the key value.

  3. Ideally you can either (1) send and receive wirelessly or (2) use
     GPIO interrupts to grab a remote signal.

--------------------------------------------------------------------
### Background: how an IR remote sends values

On the remotes I used for class (but not this year: we have different
remotes) it looks like they send a 0 or 1 bit by always sending 1 but
for differing amounts of time.   They seperate the bits by sending 0
for a set amount of time.  (You might read up on Manchester encoding.)

For these remotes:

    - skip: send 0 for about 600usec.
    - '0': send '1' for about 600usec.
    - '1': send '1' for about 1600 usec.

So to send 01 they would:

    1. Send a skip (0 for 600 usec).
    2. Send a 0 (1 for 600 usec);
    3. Send a skip (0 for 600 usec);
    4. Send a 1 (1 for 1600usec).
    5. (maybe) send a skip (0 for 600usec).

It also looks like they indicate the start of a transmission by sending
1 for about 40,000 usec and then an initial value.

So given this information you should be able to reverse engineer the
value for each key.
   1. pick up the header.
   2. read the values.
   3. convert the values to 0 and 1, shifting them into an unsigned
  
The main issue is handling error.  We'll just do something simple:
   1. When deciding if a value is a skip, return true if it's within
   a given number of usec (e.g., if its between 400 to 800usec).

   2. When deciding if transmission was a 1 or a 0, pick the value by 
   deciding if its above or below the halfway point between the two 
   values (e.g., if the timing is above (1600+600)/2 then its a 1 and
   is a 0 otherwise).

   3. If you are more clever than this somewhat brain-dead approach,
   be more clever!

--------------------------------------------------------------------
### What to do:

Go through `tsop322.c` and start implementing the code.  It's all in one file
to make it easier to see what is going on.
  1. If you scan through for `unimplemented` you will see the parts to implement.
  2. Start with `get_readings` which will need a modified version of your timeout
     from last time.
  3. Then implement `convert` to produce a 32-bit value from a set of readings.
  4. Then figure out what value each key on your remote gives and implement
     `key_to_str`.
  5. There are some helper functions that hopefully are self explanatory.



----------------------------------------------------------------
#  Part 1: reverse engineering key presses.

Given the protocol described above, you should be able to reverse engineer the
value `v` for each key.

   0. Set at 32-bit unsigned variable `v` to 0.
   1. Wait until you see a start bit (sort of similar to the UART protocol you built).
   2. For each on pulse, record its bit position, and convert it to a 0 or 1
      (depending on its length, as above).
   3. For any 1 bit in (2) set the bit at that position in `v`.
   4. When your code gets a timeout, print `v` at the end.
   5. If you hit the same remote key over and over, you should get the same value.

The main issue is handling error.  The easiest thing is to decide based
on whether its above or below the midpoint value between the two.

NOTES:
   1. Use a `gpio_pullup` to pull the input pin high when nothing is happening.
   2. As we found in the early timing labs, if you print something
      while you are waiting for a time-based signal, the print will
      likely take long enough that you miss it or corrupt the timing.

   3. As mentioned before, since the timer on the pi is a different
      device than GPIO, you'd have to use a memory barrier to synchronize
      them.  To avoid this, you might want to use the pi's cycle counter
      (which doesn't need a memory barrier).  Of course, this can overflow
      pretty quickly, so make sure your code does not get confused.

----------------------------------------------------------------
#  Part 2: interrupts.

In general, if we use remotes its to very-occasionally control a device
that is pretty busy doing something else.  In this case, it's natural
to use interrupts (note: this is hard if we care about very fine timing;
there are other approaches).

If you need a refresher:
  - lab 8 in cs140e goes over GPIO interrupts.
  - I checked in a staff version `staff-objs/gpio-int.o` that you can link in
    (by modifying `put-your-src-here.mk`) and use.
  - If you have it, you can also use your own code.


The relevant functions:

    // include/gpio.h

    // p97 set to detect rising edge (0->1) on <pin>.
    // as the broadcom doc states, it  detects by sampling based on the clock.
    // it looks for "011" (low, hi, hi) to suppress noise.  i.e., its triggered only
    // *after* a 1 reading has been sampled twice, so there will be delay.
    // if you want lower latency, you should us async rising edge (p99)
    void gpio_int_rising_edge(unsigned pin);

    // p98: detect falling edge (1->0).  sampled using the system clock.
    // similarly to rising edge detection, it suppresses noise by looking for
    // "100" --- i.e., is triggered after two readings of "0" and so the
    // interrupt is delayed two clock cycles.   if you want  lower latency,
    // you should use async falling edge. (p99)
    void gpio_int_falling_edge(unsigned pin);

    // p96: a 1<<pin is set in EVENT_DETECT if <pin> triggered an interrupt.
    // if you configure multiple events to lead to interrupts, you will have to
    // read the pin to determine which caused it.
    int gpio_event_detected(unsigned pin);

    // p96: have to write a 1 to the pin to clear the event.
    void gpio_event_clear(unsigned pin);


Simple strategy:
   1. enable rising edge (from 0 to 1).
   2. In the interrupt handler, just process the entire remote transmission.
   3. This locks up the pi during this, but in the case that you are going to
      do a major phase shift, this is ok.

A possibly better strategy:
   1. Detect each edge and put the result in a queue.
   2. When you detect a stop, convert the edges to a value.
   3. This is more complicated, but lets you spend not-much time in the handler.

----------------------------------------------------------------
###  Part 3: simple networking

For this lab you're going to use a TSOP3223 infrared (IR) receiver and
a IR led to communicate with your second pi.

#### Simple transmit for LED

Using the IR led (blue, translucent) you will send a signal to communicate
with the TSOP.
 - You don't just turn the LED on and off.  The TSOP looks for a specific signal
   of off-on sent at a specific Khz and rejects everything else.
 - So look in the datasheet for the frequency it expects.
 - Compute the microseconds for on-off.

Compute it as:

    usec_period = 1./freq * 1000. * 1000.
    usec on-off = usec_period / 2.

Alternate sending this signal and show you can blink your second pi's LED.

#### Simple transmit for UART

Re-purpose your UART protocol so that you can send arbitrary bytes using
the TSOP.  Show you can print `hello world!`   Have the two pi's send a
counter back and forth.
