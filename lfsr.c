/* 16-bit Accumulator based VM designed using a LFSR instead of a normal
 * Program Counter, See <https://github.com/howerj/lfsr>  */
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LFSR_SZ (0x1000)

enum { LFSR_OLFSR = 1 << 0 /* Use LFSR or ADD PC */, LFSR_OADD = 1 << 1 /* ADD or use LSHIFT */, LFSR_OFIRST = 1 << 2, };

typedef struct {
	uint16_t m[LFSR_SZ], pc, a, opts, polynomial, pcmask;
	int (*get)(void *in);
	int (*put)(void *out, const int ch);
	void *in, *out;
	FILE *debug;
} lfsr_t;

static inline uint16_t lfsr_next(uint16_t n, const uint16_t polynomial_mask, const uint16_t pcmask, const int add) {
	if (add) return (n + 1) & pcmask;
	const int feedback = n & 1;
	n >>= 1;
	return (feedback ? n ^ polynomial_mask : n) & pcmask;
}

static inline uint16_t lfsr_load(lfsr_t *v, const uint16_t addr, const int io) { /* more peripherals could be added if needed */
	assert(v);
	return io && addr & 0x8000 ? v->get(v->in) : v->m[addr % LFSR_SZ];
}

static inline void lfsr_store(lfsr_t *v, const uint16_t addr, const uint16_t val, const long cycles) {
	assert(v);
	if (addr & 0x8000) {
		if (v->opts & LFSR_OFIRST) { /* Useful to know when simulating the VHDL test-bench */
			v->opts &= ~LFSR_OFIRST;
			if (v->debug)
				(void)fprintf(v->debug, "Cycles until first output: %ld\n", cycles);
		}
		(void)v->put(v->out, val);
		return;
	}
	v->m[addr % LFSR_SZ] = val;
}

static int lfsr_run(lfsr_t *v) {
	assert(v);
	uint16_t pc = v->pc, a = v->a, *m = v->m, opts = v->opts, polynomial = v->polynomial, pcmask = v->pcmask;
	static const char *names[] = { "xor", "and", "lsl1", "lsr1", "load", "store", "jmp", "jmpz", };
	for (long cycles = 0;;cycles++) { /* An `ADD` instruction things up greatly, `OR` not so much */
		const uint16_t ins = m[pc % LFSR_SZ];
		const uint16_t imm = ins & 0xFFF;
		const uint16_t alu = (ins >> 12) & 0x7;
		const uint16_t _pc = lfsr_next(pc, polynomial, pcmask, !!(opts & LFSR_OLFSR));
		const uint16_t arg = ins & 0x8000 ? lfsr_load(v, imm, 0) : imm;
		if (v->debug && fprintf(v->debug, "%d: %c a_%s %d\n", (unsigned)pc, ins & 0x8000 ? 'i' : '-', names[alu], (unsigned)a) < 0) return -1;
		switch (alu) {
		case 0: a ^= arg; pc = _pc; break;
		case 1: a &= arg; pc = _pc; break;
		case 2: a = opts & LFSR_OADD ? a + arg : arg << 1; pc = _pc; break; /* optional ADD/shift left by 1*/
		case 3: a = arg >> 1; pc = _pc; break;
		case 4: a = lfsr_load(v, arg, 1); pc = _pc; break;
		case 5: lfsr_store(v, arg, a, cycles); pc = _pc; break;
		case 6: if (pc == arg) goto end; pc = arg; break; /* `goto end` for testing only */
		case 7: pc = _pc; if (!a) pc = arg; break;
		}
	}
end:
	v->pc = pc; /* save machine state, program counter */
	v->a = a; /* save machine state, accumulator */
	return 0;
}

static int lfsr_put(void *out, const int ch) { 
	const int nch = fputc(ch, (FILE*)out); 
	return fflush((FILE*)out) < 0 ? -1 : nch; 
}

static int lfsr_get(void *in) { 
	return fgetc((FILE*)in); 
}

static int option(const char *opt, const int def /*default*/) { /* very lazy options */
	const char *r = getenv(opt);
	if (!r) return def; /* Never indicate failure, never show weakness in option processing */
	return atoi(r); /* We could do case insensitive check for "yes"/"on" = 1, and "no"/"off" = 0 as well */
}

int main(int argc, char **argv) {
	lfsr_t vm = { 
		.put = lfsr_put, .get = lfsr_get, .in = stdin, .out = stdout, 
		.debug      = option("DEBUG", 0) ? stderr : NULL,
		.polynomial = option("LFSR_POLY", 0xB8), /* 0x84 gives period 217 instead of 255 but uses 2 taps */
		.pcmask     = option("LFSR_MASK", 0xFF), /* Sets bit width of program counter */
		.opts       = (LFSR_OLFSR * !!option("LFSR_INC", 0)) | (LFSR_OADD * !!option("LFSR_ADD", 0)),
	};
	if (argc < 2) {
		(void)fprintf(stderr, "Usage: %s prog.hex\n", argv[0]);
		return 1;
	}
	errno = 0;
	FILE *prog = fopen(argv[1], "rb");
	if (!prog) {
		(void)fprintf(stderr, "Unable to open file `%s` for reading: %s\n", argv[1], strerror(errno));
		return 2;
	}
	for (size_t i = 0; i < LFSR_SZ; i++) {
		unsigned long d = 0;
		if (fscanf(prog, "%lx,", &d) != 1) /* optional comma */
			break;
		vm.m[i] = d;
	}
	if (fclose(prog) < 0) return 3;
	return lfsr_run(&vm) < 0 ? 4 : 0;
}
