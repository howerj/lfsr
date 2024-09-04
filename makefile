CFLAGS=-Wall -Wextra -O3 -std=c99 -pedantic

.PHONY: all run clean

all: lfsr

run: lfsr lfsr.hex
	./lfsr lfsr.hex

lfsr.hex: lfsr.fth
	gforth $<

clean:
	git clean -dffx
