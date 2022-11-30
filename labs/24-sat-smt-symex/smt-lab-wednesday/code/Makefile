.PHONY: clean all

CFLAGS += -g
CFLAGS += -O3
# CFLAGS += -fsanitize=address

main: main.c smt.o
	mkdir -p temp_files
	$(CC) $(CFLAGS) $^ -o $@

test_basic_bv: test_cases/test_basic_bv.c smt.o
	$(CC) $(CFLAGS) $^ -o $@ -I.

test_bv_add: test_cases/test_bv_add.c smt.o
	$(CC) $(CFLAGS) $^ -o $@ -I.

test_arrays: test_cases/test_arrays.c smt.o
	$(CC) $(CFLAGS) $^ -o $@ -I.

do_tests: test_basic_bv test_bv_add test_arrays
	./test_basic_bv
	./test_bv_add
	./test_arrays

smt.o: smt.c

clean:
	rm -rf *.o main temp_files
