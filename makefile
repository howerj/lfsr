CFLAGS=-Wall -Wextra -pedantic -std=c99 -O2
IMAGE=lfsr.hex
TARGET=lfsr

.PHONY: run test clean

run: ${TARGET} ${IMAGE}
	./${TARGET} ${IMAGE}

test: ${TARGET} ${IMAGE} test.fth
	./${TARGET} ${IMAGE} < test.fth

${TARGET}.hex: ${TARGET}.fth
	gforth ${TARGET}.fth

${TARGET}: ${TARGET}.c

${TARGET}.tgz: readme.md ${TARGET}.c ${TARGET}.hex
	tar zvcf $@ $^

clean:
	git clean -dffx
