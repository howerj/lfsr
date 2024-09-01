/* 16-bit Accumulator based VM designed to be built in 7400 series ICs */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#define SZ (0x2000)

typedef struct {
	uint16_t m[SZ], pc, a;
	int (*get)(void *in);
	int (*put)(void *out, int ch);
	void *in, *out;
	FILE *debug;
} vm_t;

static inline uint16_t load(vm_t *v, uint16_t addr) {
	assert(v);
	if (addr & 0x8000)
		return v->get(v->in);
	return v->m[addr % SZ];
}

static inline void store(vm_t *v, uint16_t addr, uint16_t val) {
	assert(v);
	if (addr & 0x8000)
		(void)v->put(v->out, val);
	v->m[addr % SZ] = val;
}

static int run(vm_t *v) {
	assert(v);
	uint16_t pc = v->pc, a = v->pc, *m = v->m; /* load machine state */
	for (int running = 1; running;) {
		const uint16_t ins = m[pc % SZ];
		const uint16_t imm = ins & 0xFFF;
		const uint16_t alu = (ins >> 12) & 0xF;
		if (v->debug && fprintf(v->debug, "%04x:%04X %04X\n", (unsigned)pc, (unsigned)ins, (unsigned)a) < 0) return -1;
		switch (alu) {
		case 0: a = load(v, imm); pc++; break;
		case 1: a = load(v, imm); a = load(v, a); pc++; break;
		case 2: store(v, load(v, imm), a); pc++; break;
		case 3: a = imm; pc++; break;
		case 4: store(v, imm, a); pc++; break;
		case 5: a += load(v, imm); pc++; break;
		case 6: pc++; break;
		case 7: a &= load(v, imm); pc++; break;
		case 8: a |= load(v, imm); pc++; break;
		case 9: a ^= load(v, imm); pc++; break;
		case 10: a >>= 1; pc++; break;
		case 11: if (pc == imm) running = 0; pc = imm; break;
		case 12: pc++; if (!a) pc = imm; break;
		case 13: pc++; break;
		case 14: store(v, imm, pc); pc = imm + 1; break;
		case 15: pc = load(v, imm); break;
		}
	}
	v->pc = pc; /* save machine state */
	v->a = a;
	return 0;
}

static int put(void *out, int ch) { 
	ch = fputc(ch, (FILE*)out); 
	return fflush((FILE*)out) < 0 ? -1 : ch; 
}

static int get(void *in) { 
	return fgetc((FILE*)in); 
}

static int option(const char *opt) {
	char *r = getenv(opt);
	if (!r) return 0;
	return atoi(r); /* could do case insensitive check for "yes"/"on" = 1, and "no"/"off" = 0 as well */
}

int main(int argc, char **argv) {
	vm_t vm = { .put = put, .get = get, .in = stdin, .out = stdout, };
	vm.debug = option("DEBUG") ? stderr : NULL; /* lazy options */
	if (argc < 2) {
		(void)fprintf(stderr, "Usage: %s prog.hex\n", argv[0]);
		return 1;
	}
	FILE *prog = fopen(argv[1], "rb");
	if (!prog) {
		(void)fprintf(stderr, "Unable to open file `%s` for reading\n", argv[1]);
		return 2;
	}
	for (size_t i = 0; i < SZ; i++) {
		unsigned long d = 0;
		if (fscanf(prog, "%lx,", &d) != 1) /* optional comma */
			break;
		vm.m[i] = d;
	}
	if (fclose(prog) < 0) return 3;
	return run(&vm) < 0;
}
