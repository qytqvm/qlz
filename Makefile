CC ?= cc
CFLAGS ?= -O3 -std=c99 -Wall -Wno-misleading-indentation

all: example qlz test bench

example: examples/example.c include/qlz.h
	$(CC) $(CFLAGS) -o $@ examples/example.c
qlz: examples/qlz-cli.c include/qlz.h
	$(CC) $(CFLAGS) -o $@ examples/qlz-cli.c
test_roundtrip: tests/test_roundtrip.c include/qlz.h
	$(CC) $(CFLAGS) -o $@ tests/test_roundtrip.c
bench: bench/bench.c include/qlz.h
	$(CC) $(CFLAGS) -D_POSIX_C_SOURCE=199309L -o qlz-bench bench/bench.c
test: test_roundtrip example
	./test_roundtrip && ./example
clean:
	rm -f example qlz test_roundtrip qlz-bench
.PHONY: all test bench clean
