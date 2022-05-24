ROOT=$(CS240LX_2022_PATH)/fpga
BOARD ?= upduino
BOARDROOT=$(ROOT)/$(BOARD)

SRC=empty.sv
TOP=empty
SIM=sim.cc
SIM_TOP=empty

include $(BOARDROOT)/rules.mk
include $(ROOT)/verilator.mk

clean:
	$(BOARDCLEAN)
	$(SIMCLEAN)
