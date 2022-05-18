# Getting started with Verilog

First check out the [Prelab](PRELAB.md) for instructions for installation and
the SystemVerilog document.

## Part 0

To check that your toolchain is installed correctly and to write your first
line of Verilog, go to [`code/0-light`](code/0-light) and fill in the code in `led_top.sv` to turn
the red (or other color of your choosing) LED on.

Run `make synth` to synthesize the design, and `make prog` to upload it to the
FPGA. Run `make sim` to run it in a simulator.

## Part 1

Go to [`code/1-mux`](code/1-mux) and implement a 2-1 multiplexer.

Run `make sim` to generate a simulator. Run `./sim` to run it.

Run `make test` to auto-check.

## Part 2

Go to [`code/2-adder`](code/2-adder) and implement a 32-bit ripple-carry adder.

The starter code is setup for the following approach (you can change it if you
want):

Start by making a half adder. A half adder adds two bits `a` and `b` and
outputs the sum and the carry. Then implement a full adder which adds three
bits and outputs the sum and carry. Then you can chain a half adder and 31 full
adders together to make a 32-bit adder.

**TIP**: use the `generate` statement to avoid copy-pasting.

Run `make sim` to generate a simulator. Run `./sim` to run it (it will perform
some tests).

Do not use the Verilog `+` operator or any other arithmetic operators (use only
bitwise operators and module instantiation).

## Part 3

Go to [`code/3-blink`](code/3-blink) and implement a counter and use it to
blink your LED.

Goal: write a counter that counts up to a constant and then resets. Then
connect the top bit of the counter value to the red LED, and pick a constant
such that the LED blinks at a suitable rate.

This time you have a clock so you will have to use an `always_ff` block. Use a
D FF connected to an adder (you can use the `+` operator, or your adder from
part 2). I recommend you split your logic into two/three parts for maximum
simplicity: 

* An `always_ff` block for the D FF.
* Some combinational logic to compute the next state for the D FF.
* Some combinational logic to compute the output (not necessary in this case)

In general it is a good idea to always use this separation and avoid putting
logic directly in the `always_ff` block. This will become more evident when
making the UART module.

# Extensions

* Implement a fancier adder: (e.g., carry-propagate, carry-lookahead, or
  carry-save).
* Turn your adder into an adder/subtractor. You should be able to re-use the
  adder circuitry for subtraction.
* Implement a shifter.

If you finish early you can also move on to the UART lab.

# Extra info

Check out the `fpga/upduino` directory in the repo root to see how your module
is being wrapped. The `upduino.pcf` file (pin constraint file) defines how the
top-level variables (`clk`, `led_red`, etc...) get assigned to pins on the
board. This file was distributed by the manufacturer. In `top.v` your module is
wrapped by a top-level module that instantiates custom UPduino cells for
generating a clock and for driving the RGB LED. Note: many FPGAs define the
clock directly in the constraint file so something like `SB_HFOSC` is not
necessary. Also check out `rules.mk` to see how the toolchain is invoked.
