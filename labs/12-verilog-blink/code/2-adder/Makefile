ROOT=$(CS240LX_2022_PATH)/fpga
BOARD ?= upduino
BOARDROOT=$(ROOT)/$(BOARD)

SRC=adder.sv
TOP=adder
SIM=sim.cc
SIM_TOP=adder

include $(BOARDROOT)/rules.mk
include $(ROOT)/verilator.mk

clean:
	$(BOARDCLEAN)
	$(SIMCLEAN)
