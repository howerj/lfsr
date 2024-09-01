CFLAGS=-Wall -Wextra -pedantic -std=c99 -O2
IMAGE=vm.hex

.PHONY: run clean

run: vm ${IMAGE}
	./vm ${IMAGE}

vm.hex: vm.fth
	gforth vm.fth

vm: vm.c

vm.tgz: vm.md vm.c vm.hex
	tar zvcf $@ $^

clean:
	git clean -dffx
