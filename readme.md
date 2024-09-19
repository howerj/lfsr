# LFSR CPU/VM running Forth

* Author: Richard James Howe
* License: 0BSD / Public Domain
* Email: <mailto:howe.r.j.89@gmail.com>
* Repo: <https://github.com/howerj/lfsr>

This project contains a Virtual Machine (VM), which will be referred
to as a CPU from now one to avoid confusion with a second VM,
and a program image containing an implementation of the programming
language Forth for this CPU that uses a Linear Feedback Shift Register
(LFSR) instead of a normal program counter. The reason for doing this
(historically) is that a normal program counter requires an adder
which use more gates than a LFSR. The problem with using an LFSR as
a counter is that the program is scrambled.

To see an implementation of this system on an FPGA visit
<https://github.com/howerj/lfsr-vhdl>. It is written in VHDL.

From a software point of view a LFSR (both the CPU and the tool-chain)
appear more complex, only in hardware are LFSR simpler.

Using a LFSR is quite a good choice when gates are at a premium and
the counter value does not have to be visible to a user of it (such
as a delay counter, or a counter for a stack).

Note that because of the pedigree of this Forth (which assumes you are
talking to it over a UART) that it repeats every character typed into
it, it is also does not halt on EOF (as EOF/-1 is used to indicate
that there is no character currently available in the hardware). This
could be changed if needed.

Using a LFSR register instead of a normal counter creates a level of
obfuscation, if the LFSR polynomial had to be keyed in at startup it
would form a key. This is more of an idea than anything practical.

# Use cases

* Just for fun. 
* As a proof of concept.
* Extremely small CPUs in constrained environments, it has a similar
set of possible uses as <https://github.com/howerj/bit-serial>.

# Usage

Type `make run` (requires `make` and a C compiler), an examples session:

	2 2 + . cr
	: ahoy cr ." HELLO, WORLD" ;
	ahoy
	bye

Type `gforth lfsr.fth` to rebuild `lfsr.hex`, a pre-built hex file
containing an implementation of Forth is already provided.

This `readme.md` will not contain a Forth tutorial, look elsewhere
for one.

For a list of all defined functions in Forth type `words`.

Hitting `CTRL-D` **Will not** cause the system to exit.

# Instruction Set and System Design

A short summary of the instruction set and design:

* The machine is an accumulator based machine.
* All instructions are 16-bits in length.
* The top four bits determine the instruction.
* The lowest four bits are the operand parameter.
* Addresses are in number of 16-bit cells, not bytes.
* The program counter is 8-bits in size and is advanced with a LFSR.
* The top most bit determined whether the operand is used directly or 
whether or it loaded first.

The instruction set has been carefully chosen so that it
should be very simple to implement in a traditional manner,
or in a bit-serial fashion (much like my other CPU project at
<https://github.com/howerj/bit-serial> that runs on an FPGA). It has
also been designed so that it should be possible to implement in 7400
series logic ICs (excluding ROM and RAM) as well as on an FPGA. There
may be no savings in logic on an FPGA due to the fact that slices
have a built in carry chain (usually), but a comparison could be made
when the system is implemented on an FPGA (This has been done, there
is very little difference).

As the Program Counter does not use addition, it was decided that
addition should be removed from the instruction set. It would be a
bit weird to worry about all the gates the Program Counter is using and
then just include an adder elsewhere. The ADD instruction is sorely
missed and makes the Forth code more complex and slows it down. The
only other instruction that feels like it should be present (to me)
is bit-wise OR, in practice it is not used that much within the
interpreter so it not missed, it is reimplemented using bitwise AND
and INVERT (XOR against all bits set).

The instruction layout is:

	+----------+---------------+-------------------+
	|  BIT 15  | BITS 12 to 14 |    BITS 0 to 11   |
	+----------+---------------+-------------------+
	| INDIRECT | INSTRUCTION   | VALUE   / ADDRESS |
	+----------+---------------+-------------------+

The `INDIRECT` flag determined whether bits 0 to 11 are treated as
a value (not set) or an address (`INDIRECT` is set).

The `INSTRUCTION` field is 3-bits in size, the instructions are:

	0 : AND   : ACC = ACC & ARG
	1 : XOR   : ACC = ACC ^ ARG
	2 : LLS1  : ACC = ACC << 1
	3 : LRS1  : ACC = ACC >> 1
	4 : LOAD  : ACC = MEM[ARG]
	5 : STORE : MEM[ARG] = ACC
	6 : JUMP  : PC = ARG
	7 : JUMPZ : IF (ACC == 0) { PC = ARG }

All instructions advance the program counter except `JUMP`, and
(conditionally) `JUMPZ`. All instruction affect or use the accumulator
except the jump instructions, and all arguments use the `ARG` value
except the shift instructions `LLS1`/`LRS1`. `MEM` consists of a
linear array of 16-bit values.

There is one special address, address 0. This address is never
incremented after an instruction is run as this is a lock up state for
the LFSR, the current instruction will be executed indefinitely. A way
to exit this condition is for the first instruction to be a jump to
address `1`, however any non-zero address will do. The system starts
up executing from address zero. Conditional jumps could be used to
determine whether to reset or halt the system if needed. Alternatively
a LFSR that used XNOR could have been used (the lockup state for which 
is all ones) but it was not.

`ACC` is the 16-bit accumulator.

`ARG` is either the zero extended 12-bit operand (the lowest 12-bits
of the instruction) as is, or the 16-bit value `MEM[operand]` if the
topmost bit of the instruction is set.

Input and Output is memory mapped, reading from a negative address
(high bit set) causes a byte to be read or output. This is triggered
from reading or writing to any negative address, if multiple
peripherals are to be added the address will have to be decoded
correctly.

For the purposes of simulation `JUMP` will cause the CPU to halt if
the jump address is the same as the program counter. This will not
be implemented in hardware.

The program counter uses a 8-bit LFSR to advance, that means only 256
16-bit values can be directly addressed by this CPU, this is not a
limitation that matters for the purpose of the software running on this
system, a complete Forth Programming Language image, as the Virtual
Machine that supports Forth can be written in under 256 instructions
for this system. That Forth Virtual Machine can address more memory
by using LOAD/STORE to access values outside the 256 instruction range.

This instruction set might change depending on the implementation,
or when it comes to implementation, to make things easier and smaller
still.

# To Do

* [x] Get basic implementation working
* [x] Port a Forth implementation to the machine.
  - [ ]  Make a build target for a VM that is exactly the same except
  it uses normal program counter. (optional)
* [x] Optimize machine (remove Add instruction, put in indirect bit)
* [x] Port to an FPGA
  - [x] See <https://github.com/howerj/lfsr-vhdl>
  - [ ] make a bit-serial version? (optional)
* [ ] Implement in 7400 series logic?
  - [ ] Separate the eForth image into `ROM` and `RAM` sections. We
  should able to make the first X KiB of the image read-only. This might
  mean cutting the image down to get it under a 4KiB limit and moving
  variables.

# References

* <https://en.wikipedia.org/wiki/Linear-feedback_shift_register>
* <https://github.com/howerj/subleq>
* <https://github.com/howerj/7400>
* <https://github.com/howerj/bit-serial>
* <https://en.wikipedia.org/wiki/Forth_(programming_language)>
* <https://stackoverflow.com/questions/1149929>
* <https://www.fpga4fun.com/Counters3.html>
* <https://groups.google.com/g/comp.lang.vhdl/c/GiFdsYfaeHA>
