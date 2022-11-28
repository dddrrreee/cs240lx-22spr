.PHONY: clean all

CFLAGS += -g
CFLAGS += -O3
# CFLAGS += -fsanitize=address

all: basic fast
fast: fast.c
basic: basic.c

clean:
	rm -f basic fast xcheck_*.output perf.*
