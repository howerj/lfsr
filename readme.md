# LFSR

* Author: Richard James Howe
* License: 0BSD / Public Domain
* Email: <mailto:howe.r.j.89@gmail.com>
* Repo: <https://github.com/howerj/lfsr>

**This project is working, but is still a work in progress**.

A virtual machine that uses a Linear Feedback Shift Register (LFSR) instead of a
normal program counter. The reason for doing this (historically) is that a normal 
program counter requires an adder which use more gates than a LFSR. The problem
with using an LFSR as a counter is that the program is scrambled.

From a software point of view a LFSR (both the VM and the tool-chain) appear
more complex, only in hardware are LFSR simpler.

Using a LFSR is quite a good choice when gates are at a premium and the counter
value does not have to be visible to a user of it (such as a delay counter, or
a counter for a stack).

Note that because of the pedigree of this Forth (which assumes you are talking
to it over a UART) that it repeats every character typed into it, it is also
does not halt on EOF (as EOF/-1 is used to indicate that there is no character
currently available in the hardware). This could be changed if needed.

# To Do / Notes

* [x] Get basic implementation working
* [x] Port a Forth implementation to the machine.
* [ ] Optimize machine (remove Add instruction, indirect bit)
* [ ] Port to an FPGA, make a bit-serial version?
* [ ] Implement in 7400 series logic?

# References

* <https://en.wikipedia.org/wiki/Linear-feedback_shift_register>
* <https://github.com/howerj/subleq>
* <https://github.com/howerj/7400>
* <https://github.com/howerj/bit-serial>
* <https://en.wikipedia.org/wiki/Forth_(programming_language)>
* <https://stackoverflow.com/questions/1149929>
* <https://www.fpga4fun.com/Counters3.html>
* <https://groups.google.com/g/comp.lang.vhdl/c/GiFdsYfaeHA>
