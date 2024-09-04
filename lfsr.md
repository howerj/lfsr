# LFSR

* Author: Richard James Howe
* License: 0BSD / Public Domain
* Email: <mailto:howe.r.j.89@gmail.com>
* Repo: <https://github.com/howerj/lfsr>

**PLACE HOLDER PROJECT**

**THIS PROJECT IS A WORK IN PROGRESS AND DOES NOT YET WORK**.

A virtual machine that uses a Linear Feedback Shift Register (LFSR) instead of a
normal program counter. The reason for doing this (historically) is that a normal 
program counter requires an adder which use more gates than a LFSR. The problem
with using an LFSR as a counter is that the program is scrambled.

From a software point of view a LFSR (both the VM and the tool-chain) appear
more complex, only in hardware are LFSR simpler.

Using a LFSR is quite a good choice when gates are at a premium and the counter
value does not have to be visible to a user of it (such as a delay counter, or
a counter for a stack).

# To Do / Notes

* Jump to zero would indicate a halt, alternatively it might need to contain a
  `jump-to-1` instruction, which would take care of the self starting problem.
* If we have 64 Kibi Words of memory, then instead of a LFSR that can address
  most of that memory (0xD008) we should use only one that can address the
  first KiBi Word or so (0x240). We would then build a virtual machine in those
  first thousand or so words and program normally from then on, targeting the
   VM instead.
* Port a Forth implementation to the machine.
* Port to an FPGA, make a bit-serial version?
* Implement in 7400 series logic?
* It might be cheaper (in terms of gates) to do the following:
  - Implement the Forth stacks using LFSR if the ADD instruction is not
  present, this will require an LFSR that can count up and down.
  - Always increment the Program Counter, even for a jump instruction. This
  *might* save on control logic
* Addition without the "+", in case we want to design a CPU without an adder at
all:

	int Add(int a, int b) {
		while (b) {
			int carry = a & b;
			a = a ^ b;
			b = carry << 1;
		}
		return a;
	}

* Is there a way of calculating the nth value of a LFSR without cycling through
  the intermediate states?

# References

* <https://en.wikipedia.org/wiki/Linear-feedback_shift_register>
* <https://github.com/howerj/subleq>
* <https://github.com/howerj/7400>
* <https://github.com/howerj/bit-serial>
* <https://en.wikipedia.org/wiki/Forth_(programming_language)>
* <https://stackoverflow.com/questions/1149929>
* <https://www.fpga4fun.com/Counters3.html>
* <https://groups.google.com/g/comp.lang.vhdl/c/GiFdsYfaeHA>
