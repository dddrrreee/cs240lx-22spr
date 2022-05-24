ROOT=$(CS240LX_2022_PATH)/fpga

SRC=mux.sv
SIM_SRC=mux.sv
SIM=sim.cc
SIM_TOP=mux

include $(ROOT)/verilator.mk

test: sim
	@./sim > sim.out
	@cmp --silent sim.out sim.expected && echo "test passed" || echo "test failed"

.PHONY: test

clean:
	$(SIMCLEAN)
