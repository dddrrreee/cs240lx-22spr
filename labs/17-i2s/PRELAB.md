# I2S Prelab

For today's lab, we will be working with audio; specifically, an [Adafruit I2S microphone](https://www.adafruit.com/product/3421) and the I2S hardware peripheral in the Raspberry Pi.

## Basic terminology

- I2S: Inter-IC Sound, a serial bus standard for communication between digital audio devices.
- PCM: The format in which audio samples are transmitted on an I2S bus. The docs seem to use PCM and I2S interchangeably. Think of a PCM sample as the "packet" of the I2S "network".

## I2S General Background

I2S is a pretty simple protocol that enables transmitting audio samples with at an arbitrary bit width and sample rate. You can read more about it here:

[Wikipedia Page](https://en.wikipedia.org/wiki/I%C2%B2S)

In particular, take a look at the timing diagram 

![I2S Timing Diagram](images/wikipedia_i2s_timing.png "I2S Timing Diagram (Wikipedia)")

The TL;DR is that there are two clocks
- The serial/bit clock (SCLK/BCLK). Generally, the bit is changed by the transmitting device on falling edge and read by the receiving device on the rising edge. From a digital design perspective it makes sense to read on a rising edge as we can trigger our flipflops on the BCLK (if that doesn't make sense ignore it, or ask Parthiv).
- The word select clock (WS), left/right clock (LRCLK), or frame sync (FS), which synchronizes frames (samples). This is low while transmitting the left sample and high while transmitting the right samples, but "off by one" bit as shown in the image above. The off by one is also to give "warning" to the target device that it should store the current sample or prepare the next sample. And data can then be latched on the rising edge of the BCLK as the LSB is read.

The I2S protocol was recently updated to use the more reasonable terms "controller" and "target" for the device controlling the clocks and the device responding to the clocks, vs. historically questionable names.

One interesting thing about I2S is that it is typically a unidirectional protocol, but that direction could be either direction. In other words, the controller can read samples from the target (e.g. pi reading from microphone) or the controller can send samples to the target (e.g. pi sending samples to a DAC to play audio). We will be implementing the first behavior, a pi (controller) reading samples from the target (microphone).

Note that the samples are signed, but by transmitting MSB first, it allows for arbitrary bit widths. If the controller only wants 12 bits, it could just change the LRCLK every 12 cycles of BCLK, and the target would just discard any additional bits of precision and begin transmitting the next sample. Or if a target can only output 12 bits of precision but the controller is requesting 16, it can just set the 4 LSB to 0. 

## Sound and DSP General Background

This is going to be a very incomplete high-level overview of the Digital Signal Processing (DSP) techniques you'll need for the lab. 

We can represent signals either in the time domain or in the frequency domain. In the time domain, which is how we usually record signals, we have a bunch of samples that represent the amplitude of our signal at various steps in time. For example, each sample may be a 16-bit integer and we may get these at 44.1 kHz, meaning that each sample lives for 1/(44.1kHz) = ~22.68 us. We usually perceive sound in the frequency domain though, hearing specific pitches and notes that live at a certain frequency, rather than hearing the amplitude of the sound changing quickly over time.

The Fast Fourier Transform (FFT) is an algorithm used to extract the frequency-domain components of a time-domain signal (or reconstruct a time-domain signal from it's frequency-domain components aka inverse FFT). It's an implementation of the Discrete Fourier Transform (DFT), which is in essence a [complex matrix multiplication](https://ccrma.stanford.edu/~jos/st/Matrix_Formulation_DFT.html). We take the N-vector of time domain samples, multiply by a complex NxN DFT matrix, and get as a result a complex N-vector of frequency domain components. Below is said matrix for a length 4 signal.

![DFT Matrix 4x4](images/dft_matrix.png "DFT Matrix 4x4")

This produces N outputs (N frequency components), each of which require N multiply/adds. So we have an O(N^2) algorithm for the matrix multiplication DFT. The value computed by the FFT for each frequency bin is a complex value. The magnitude of this complex value (|a+bi| = a^2 + b^2) is the amplitude of that signal, and the phase of the complex value (arctan(b/a)) is the phase. We don't hear phase so for the purposes of today we will just use the magnitude (also it's a lot easier to compute!).

The FFT calculates the exact same matrix multiplication, but exploits symmetry in the DFT matrix to compute in in O(NlogN) time. There are several ways to approach this but all known algorithms are O(NlogN). Though there is no proof that it cannot be done with lower complexity. How the most common (butterfly) FFT works is a bit beyond what is needed for the lab, but if you're interested, [here's a decent overview](https://www.dspguide.com/ch12/2.htm). 

The most important thing to know is that the DFT computes the amplitude of frequency components from [-fs/2, fs/2), in steps of fs/N (if you have N samples). These are ordered slightly weirdly (ask Parthiv if you want to know why), as follows:

0, fs/N, 2fs/N, 3fs/N, ..., (N-1)fs/2N, -fs/2, -(N-1)fs/2N, ..., -3fs/N, -2fs/N, -fs/N.

What you need to know about these frequency indices:
- We can only detect frequencies up to 1/2 of the sampling rate. See [Nyquist-Shannon Sampling Theorem](https://en.wikipedia.org/wiki/Nyquist%E2%80%93Shannon_sampling_theorem).
- Half of the FFT outputs are "useless" (and could be optimized out). For real time domain signals (which we deal with), the amplitude of each negative frequency is exactly -1 * the amplitude of the corresponding positive frequency. This is not necessarily the case for complex time domain signals, but we only have real samples from the microphone :)

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
