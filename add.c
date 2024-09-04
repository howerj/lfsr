#include <stdlib.h>
#include <stdio.h>

static int Add(int a, int b) {
	while (b) {
		/*(void)fprintf(stderr, "a, b = %d %d\n", a, b);*/
		int carry = a & b;
		a = a ^ b;
		b = carry << 1;
	}
	return a;
}

int main(int argc, char **argv) {
	if (argc != 3) {
		(void)fprintf(stderr, "Usage: %s number number\n", argv[0]);
		return 1;
	}
	const int a = atoi(argv[1]);
	const int b = atoi(argv[2]);
	const int c = Add(a, b);
	return fprintf(stdout, "%d + %d = %d\n", a, b, c) < 0 ? 1 : 0;
}
