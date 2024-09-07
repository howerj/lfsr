CFLAGS=-Wall -Wextra -pedantic -std=c99 -O2
IMAGE=lfsr.hex
TARGET=lfsr

.PHONY: run clean

run: ${TARGET} ${IMAGE}
	./${TARGET} ${IMAGE}

${TARGET}.hex: ${TARGET}.fth
	gforth ${TARGET}.fth

${TARGET}: ${TARGET}.c

${TARGET}.tgz: ${TARGET}.md ${TARGET}.c ${TARGET}.hex
	tar zvcf $@ $^

clean:
	git clean -dffx
