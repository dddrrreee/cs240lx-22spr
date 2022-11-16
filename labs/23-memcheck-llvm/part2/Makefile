LLVM_BIN=
RTLIB=rtlib
# TEST_SRC=test-use-after-free
TEST_SRC=test-no-error
# TEST_SRC=test-unsafe-store
# TEST_SRC=test-unsafe-load
# TEST_SRC=test-mem-leak
BUILD_DIR=./objs

all: run

run: test
	./$(BUILD_DIR)/$(TEST_SRC)

test: $(BUILD_DIR)/$(TEST_SRC)-instrument.ll  $(BUILD_DIR)/$(RTLIB).o
	cc $(BUILD_DIR)/$(TEST_SRC)-instrument.ll $(BUILD_DIR)/$(RTLIB).o  -o $(BUILD_DIR)/$(TEST_SRC)

$(BUILD_DIR)/$(TEST_SRC).ll: $(TEST_SRC).c
	$(LLVM_BIN)/clang -S -emit-llvm -Xclang -disable-O0-optnone $(TEST_SRC).c -o $(BUILD_DIR)/$(TEST_SRC).ll

$(BUILD_DIR)/$(TEST_SRC)-instrument.ll: $(BUILD_DIR)/$(TEST_SRC).ll
	$(LLVM_BIN)/opt -load build/pass/libMemcheckPass.so -enable-new-pm=0 --memcheck -S  $(BUILD_DIR)/$(TEST_SRC).ll >  $(BUILD_DIR)/$(TEST_SRC)-instrument.ll

$(BUILD_DIR)/$(RTLIB).o: $(RTLIB).c
	$(CC) -c $(RTLIB).c  -o $(BUILD_DIR)/$(RTLIB).o

.PHONY: clean

clean: 
	rm $(BUILD_DIR)/*


