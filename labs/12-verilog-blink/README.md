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

## Part 2

Go to [`code/2-adder`](code/2-adder) and implement a 32-bit ripple-carry adder
(do not use the `+` operator).

## Part 3

Go to [`code/3-counter`](code/3-blink) and implement a counter and use it to
blink your LED.

# Extensions

* Implement a fancier adder: (e.g., carry-propagate, carry-lookahead, or
  carry-save).
* Turn your adder into an adder/subtractor. You should be able to re-use the
  adder circuitry for subtraction.
* Implement a shifter.

If you finish early you can also move on to the UART lab.
