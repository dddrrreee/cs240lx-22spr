Goal: implement a ripple-carry adder in `adder.sv`.

The starter code is setup for the following approach (you can change it if you
want):

Start by making a half adder. A half adder adds two bits `a` and `b` and
outputs the sum and the carry. Then implement a full adder which adds three
bits and outputs the sum and carry. Then you can chain a half adder and 31 full
adders together to make a 32-bit adder.

**TIP**: use the `generate` statement to avoid copy-pasting.

Run `make sim` to generate a simulator. Run `./sim` to run it.

Do not use the Verilog `+` operator or any other arithmetic operators (use only
bitwise operators and module instantiation).
