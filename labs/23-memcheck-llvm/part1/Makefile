LLVM_BIN= /home/engler/class/cs240lx-22spr/labs/23-memcheck-llvm/downloads/clang+llvm-15.0.2-x86_64-unknown-linux-gnu/bin
RTLIB = rtlib
TEST_SRC = test
BUILD_DIR=./objs

all: run

run: test
	./$(BUILD_DIR)/$(TEST_SRC)

test: $(BUILD_DIR)/$(TEST_SRC)-instrument.ll  $(BUILD_DIR)/$(RTLIB).o
	$(LLVM_BIN)/clang $(BUILD_DIR)/$(TEST_SRC)-instrument.ll $(BUILD_DIR)/$(RTLIB).o  -o $(BUILD_DIR)/$(TEST_SRC)

$(BUILD_DIR)/$(TEST_SRC).ll: $(TEST_SRC).c
	$(LLVM_BIN)/clang -S -emit-llvm -Xclang -disable-O0-optnone $(TEST_SRC).c -o $(BUILD_DIR)/$(TEST_SRC).ll

$(BUILD_DIR)/$(TEST_SRC)-instrument.ll: $(BUILD_DIR)/$(TEST_SRC).ll
	$(LLVM_BIN)/opt -load build/pass/libDummyPass.so -enable-new-pm=0 --dummy -S  $(BUILD_DIR)/$(TEST_SRC).ll >  $(BUILD_DIR)/$(TEST_SRC)-instrument.ll

$(BUILD_DIR)/$(RTLIB).o: $(RTLIB).c
	$(LLVM_BIN)/clang -c $(RTLIB).c  -o $(BUILD_DIR)/$(RTLIB).o

.PHONY: clean

clean: 
	rm $(BUILD_DIR)/*


