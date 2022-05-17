ROOT=$(CS240LX_2022_PATH)/fpga
BOARD ?= upduino
BOARDROOT=$(ROOT)/$(BOARD)

SRC=$(wildcard *.sv)
TOP=blink_top
SIM=sim.cc
SIM_TOP=blink_top

include $(BOARDROOT)/rules.mk
include $(ROOT)/verilator.mk

clean:
	$(BOARDCLEAN)
	$(SIMCLEAN)
