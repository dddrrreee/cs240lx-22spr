.PHONY: clean all

CFLAGS += -g
CFLAGS += -O3
# CFLAGS += -fsanitize=address
# CFLAGS += -DUSE_MINISAT

main: main.c smt.o
	mkdir -p temp_files
	$(CC) $(CFLAGS) $^ -o $@

smt.o: smt.c

clean:
	rm -rf *.o main temp_files
