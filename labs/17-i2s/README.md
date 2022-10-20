# I2S Lab by Parthiv

The r/pi has a hardware I2S peripheral. The docs for this are a bit of a mess, but it may be useful for you to be able to use audio transmit/receive in your projects. The key section of the BCM manual is chapter 8. I got single-channel receive working and I think that having multi-channel receive should not be too much extra work. Transmitting should also be pretty doable as you just do an equivalent setup for the TX and then write to the FIFO register instead of reading from it. I think you can have both TX and RX going at the same time. 

If you have questions at any point please ask me! This is my first time making a lab so let me know if there's anything unclear that can be updated for the future.

## Prelab
The prelab contains some background info on the protocol and some basic useful signal processing stuff. Take a look at [PRELAB.md](PRELAB.md).

### Basic terminology

- I2S: Inter-IC Sound, a serial bus standard for communication between digital audio devices.
- PCM: The format in which audio samples are transmitted on an I2S bus. The docs seem to use PCM and I2S interchangeably. Think of a PCM sample as the "packet" of the I2S "network".

## Docs

It's important that we use the hardware I2S peripheral because it allows us to free up CPU time. A very common ("CD-quality") audio format is 16 bits @ 44.1 kHz sampling rate, which corresponds to roughly 705.6 thousand GPIO reads per second. It's a lot nicer if we can let the hardware do this for us, then read data from a FIFO when we need it. 

I would recommend at least skimming these docs. They aren't quite sufficient to cover everything we need, but they're a solid start.

[BCM2835-ARM-Peripherals.pdf](https://datasheets.raspberrypi.com/bcm2835/bcm2835-peripherals.pdf)
- Section 8: PCM/I2S
- Section 6.3: General Purpose GPIO Clocks (sorta... it's missing the parts that we need)

[BCM2835 Peripherals Errata](https://elinux.org/BCM2835_datasheet_errata)
- p107-108 table 6-35: explanation of PCM/I2S clock
- p105 table: explanation of clock divider

[I2S Mic Datasheet](docs/i2S%2BDatasheet.PDF)


## Step 0: Hardware Setup

Step 0 is to wire the I2S mic to the Pi. I like [pinout.xyz](https://pinout.xyz/pinout/pcm#) to find the pinout of different peripherals. 
- 18, CLK (aka BCLK). The main clock of the I2S bus (usually 2-4 MHz). The I2S transmitter will provide a new bit on the rising edge of the clock and the I2S receiver will read the bit on the falling edge of the clock. 
- 19, FS (aka WS aka LRCLK). This signal goes low/right when requesting the left/right channels of audio data respectively. FS -> frame sync, WS -> word select, LRCLK -> left right clock.
- 20, DIN. This is the input for the pi's I2S receiver. If you have a sample-producing device (e.g. microphone) you'd plug its DOUT into here. 
- 21, DOUT. This is the output for the pi's I2S transmitter. If you have a sample-consuming device (e.g. audio amplifier) you'd plug its DIN into here. 

You just need pins 18, 19, and 20 for the mic. Also, connect the 3V and GND pins to 3.3V and GND respectively.

From here, you can 
```
pi-install prebuilt-binaries/1-i2s-test.bin
```
and you will see a bunch of zero samples! That's not what we expected...

Plug the SEL pin to 3.3V and this will tell the microphone that it's assigned to the right channel, so it will actually start outputting samples

Note that this program just runs for a very long time so I would just recommend power cycling the pi if you Ctrl-C'd out of pi-install. Either re-install the program (if you Ctrl-C + power cycle) or have it keep running. Now, you should hopefully see nonzero samples spit out to your terminal. If not, then something is wrong with the hardware (ask Parthiv for help).

## Step 1: Implementing I2S access

Now, we're going to implement the software to talk to the I2S peripheral. I provided a whole bunch of constants in `i2s.h` so you can just use those if you want.

### Initializing I2S

For this, you'll want to implement
```
void i2s_init(void);
```
in `i2s.c`. I used `volatile` pointers with no issues (see top of `i2s.c` for some declarations) but if you don't like that then you can use `PUT32` and `GET32`.

Here is the process that worked for me:
1. Set the I2S pins (above) all to mode `ALT0` (`0b100`)
2. dev_barrier. I haven't tested without this but we're changing from GPIO to Clock Manager peripheral so it's better to be safe.
3. Configure the Clock Manager to manage the clock for I2S. This is pretty much completely undocumented in the Peripherals manual. Every time you write to a Clock Manager register, you must set the MS byte of the word you write to `0x5A`. Why? It's the password for the clock manager. Not a great password if it's published in all the docs... (actually sort of makes sense as it reduces the likelihood of accidentally writing there and messing up the chip clock).  
    - Write `0b0001` to lowest 4 bits of the PCM_CTRL register (`0x20101098`). This uses the highest resolution clock we have available, the 19.2 MHz XTAL clock. We'll also write `0b11` to bits 9 and 10 of this register to enable the 3-stage MASH clock divider.
    - Setup clock divider in the PCM_DIV register (`0x2010109C`). The meaning of this register is explained in the two errata sections listed in Docs, as well as Section 6.3 of the Peripherals manual. The crystal outputs 19.2 MHz so we need a 6.8027 divider to reach 19.2 MHz / 6.8027 / 64 = 44.1001 KHz. Close enough.  
    - Enable I2S clock. Write 1 to bit 4 of PCM_CTRL. Make sure not to clear anything you already set. This will tell the Clock Manager to start outputting a 19.2 MHz / 6.8027 = 2.822 MHz clock. 
4. dev_barrier. Done with clock manager peripheral, now time for I2S.
5. Configure the I2S peripheral. This is reasonably well-documented in the Peripherals manual. It can be configured in either polling, interrupt, or DMA mode. I just did polling but in a "real" project you'd probably want to use DMA (or interrupt).
    - Mode register (`0x20203008`). Set FLEN field to 63 and FSLEN field to 32. This means that each frame is 63 + 1 = 64 bits, with each channel being 32 bits. This gives us 32 bits for each channel.
    - Receiver config register (`0x2020300C`). Set CH1EN bit to enable channel 1, set CH1WID bits to 8 and CH1WEX to set channel 1 to 32 bits. 
    - Control and status register (`0x20203000`). Set the EN bit to enable the I2S peripheralset STBY bit to disable standby, set RXCLR bit to clear the receive FIFO, and set RXON bit to enable receiver. 
6. dev_barrier. I2S should now be constantly sending the two clocks out to any peripherals connected to it, and loading samples into the FIFO located at `0x20203004`. 

### Reading a sample

Next we'll want to be able to read a sample. 

```
int32_t i2s_read_sample(void);
```

This isn't too difficult as the I2S FIFO is just a memory-mapped IO at `0x20203004` aka `i2s_regs->fifo`. 
1. dev_barrier. I haven't tested without this but you may be switching from another peripheral, and you want a dev_barrier before the first read.
2. Wait for the `RXD` bit in the `CS` register to go high. If it's high there is at least 1 sample available in the FIFO. Of course this is sort of a waste as you're just burning CPU cycles.
3. Read the sample from the `FIFO` register.
4. No dev_barrier needed, we didn't write anything.

### Testing
- `1-i2s-test.c`
    - This will just dump i2s samples over UART. It's a good way to see that your code is working, as you should see samples that are hopefully changing over time. It's not timing accurate or anything since the UART writes are very slow.
- `2-i2s-dump.c` 
    - This will record 5 seconds of audio to a buffer, then dump them out to the UART at the end. 
    - Recommended usage: `make 2>&1 | grep DUMP | tr -d DUMP > ../py/dump.txt` (one of you Unix pros can tell me a better way to do this). This puts all of the output from `pi-install`, filters it to only have the lines starting with `DUMP`, then trims off `DUMP` and puts all those lines into the file `dump.txt` in the `py` directory.
    - Then, we can `cd py` and then `python3 text_to_wav.py` (you will need numpy and scipy). This is a quick script that reads the samples from `dump.txt` and generates a wav file `dump.wav`. You will hopefully hear something!
    - This is super slow because it's writing 44100 * 5 = 220500 lines to the file. This could definitely be improved if we were able to read the samples from the pi as actual values rather than ASCII.
- `3-i2s-audiovis.c`
    - This is a really basic sound visualization which reads samples via I2S and moves the LEDs on the neopixel ring as the samples change amplitude. Make sure your neopixel data in is on pin 2, or change the test file to the pin you're using.

## Step 2: Integrating the FFT

As mentioned in the prelab, over short periods of time we perceive sounds more in the frequency domain (pitch) rather than in the time domain (change in amplitude). We'd like our pi to be able to do the same thing. We achieve this with the Fast Fourier Transform (FFT), as described in the PRELAB. 

I've provided an implementation of the FFT in `fft.c`. If you're curious, you can take a look. However, there are a couple of helper functions that you need to fill in in `fft.h`.

### Converting uint32 to Q.15

```
inline int16_t to_q15(uint32_t x);
```

A quick explanation of what and why we're doing this. For the FFT, we want to treat the samples as ranging from [-1, 1). The problem is that the Adafruit microphone seems to output samples from 0 to 2^18-1 (i.e. unsigned 18 bit samples), which are then zero padded on the right side to become 32 bits.

However, the FFT implementation here uses Q.15. The Q.15 format is a fixed-point decimal where the MSB is a sign bit and the remaining 15 bits represent 1/2, 1/4, 1/8, etc. If you add up this series you will see that you can represent [-1, 1) as we'd like. With this format, we can use integer operations to operate on fractional values. 

So for the function, we want to:
1. Shift the range of the samples from [0, 2^32 - 1] to [-2^31, 2^31 - 1].
2. Bitshift the result so that we can fit into 16 bits (we will lose a bit of precision). You may want to experiment with different bitshift amounts to find one that works well for your application, as it takes some VERY loud sounds to get the MSBs of the microphone output to change.

### Fixed point multiplication

Now that we have fixed point data, we can do math with it. We represent the Q.15 data as `int16_t`, which means we can use halfword (16 bit) integer operations. We want to implement multiplication (16 bit x 16 bit = 32 bit).

```
inline int32_t fft_fixed_mul(int16_t a, int16_t b);
```

I used inline assembly to ensure that the compiler is using the correct multiplication. Check out [this page](https://developer.arm.com/documentation/ddi0406/c/Application-Level-Architecture/ARM-Instruction-Set-Encoding/Data-processing-and-miscellaneous-instructions/Halfword-multiply-and-multiply-accumulate) to help you determine what the correct instruction is.

### Testing

- `4-fft-test.c`
    - This does a quick test of the FFT alone. Compare your output with that of the binary in `prebuilt-binaries/4-fft-test.c`. The input signal is an alternating signal which is the maximum frequency that can be represented by the sample rate (i.e. fs/2). So when we take the FFT, we expect that the only frequency present is at N/2 (index 8), with amplitude 1024 (since the samples are 1024, -1024, etc.). And when we do the inverse FFT, we should reconstruct exactly the original signal. If yours doesn't do this then something's wrong, ask Parthiv!
- `5-fft-time.c`
    - This just tests the speed of the FFT by running it a bunch of times.
- `6-fft-freqout.c`
    - This attempts to extract the dominant (fundamental) frequency that is heard b the microphone. It's not super complicated and is a pretty good starting point if you want to do your own analysis. 
        - Step 1: gather a bunch of samples
        - Step 2: compute the FFT of those samples
        - Step 3: process the FFT result. In this case, try to find the dominant frequency by looking for the largest amplitude. To reject harmonics, make sure that a given amplitude is some factor larger than the previous max in order to be selected as the max. This is
        not a very effective approach in noisy environments, though.
- `7-fft-freqviz.c`
    - Same as `6-fft-freqout.c`, but also draws a bar on the neopixel ring that lengthens with higher frequency.
- `8-fft-buckets.c`
    -  Displays the intensity of different frequency buckets on the neopixel ring. Each neopixel represents a "bucket" (a group of neighboring frequencies) and its brightness represents the current intensity of that bucket.

For the FFT tests I'd recommend using a [tone generator](https://www.szynalski.com/tone-generator/) either on your computer or phone. As you change the frequency you should see the UART output and/or neopixels change!

## Extensions / Projects
Some ideas:
- Figure out interrupt and/or DMA operation, which would be a huge improvement over busy waiting.
- Figure out audio transmit on I2S. Parthiv has an I2S DAC target which reads samples from the bus and outputs them to a headphone jack, so you could use this to test.
- Lighting effects based on sound from microphone.
- Audio-based UART. E.g. play notes using your stepper motor and then listen using the FFT to extract the frequency. You could, for example, encode a 1 as a specific frequency and a 0 as a different one, or even try and encode multiple bits (e.g. 4 frequencies to encode 2 bit pairs, 8 for 3 bit pairs, etc, depending on your accuracy). I haven't tried this but would be curious to see if it would work. If you do go for this, I would recommend picking frequencies that aren't multiples of each other to avoid confusion around harmonics.
