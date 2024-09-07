#include <stdio.h>
#include <stdint.h>

#ifndef COUNT
#define COUNT 2
#endif

#if COUNT == 0
#define POLY (0x240)
#define REV  (0x081) /* For each digit in POLY add 1 and MOD POLY bit-length (or ROTATE N-Bits left by one) */
#define PERIOD (1023)
#define BITS (10)
#elif COUNT == 1
#define POLY (0x110)
#define REV  (0x021)
#define PERIOD (511)
#define BITS (9)
#elif COUNT == 2
#define POLY (0xB8)
#define REV  (0x71)
#define PERIOD (255)
#define BITS (8)
#endif

static uint16_t lfsr(uint16_t lfsr, uint16_t polynomial_mask) {
	int feedback = lfsr & 1;
	lfsr >>= 1;
	if (feedback)
		lfsr ^= polynomial_mask;
	return lfsr;
}

static uint16_t rlfsr(uint16_t lfsr, uint16_t polynomial_mask) {
	int feedback = lfsr & (1 << (BITS - 1)); /* highest poly bit */
	lfsr <<= 1;
	if (feedback)
		lfsr ^= polynomial_mask;
	return lfsr % (PERIOD + 1); /* Mod LFSR length */
}

int main(void) {
	uint16_t s = 1, r = 1;
	for (int i = 0; i <= PERIOD; i++) {
		if (fprintf(stdout, "%x %x\n", s, r) < 0) return 1;
		s = lfsr(s, POLY);
		r = rlfsr(r, REV); 
	}
	return 0;
}

