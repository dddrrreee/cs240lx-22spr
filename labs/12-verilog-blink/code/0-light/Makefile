ROOT=$(CS240LX_2022_PATH)/fpga
BOARD ?= upduino
BOARDROOT=$(ROOT)/$(BOARD)

SRC=led_top.sv
SIM_SRC=led_top.sv
TOP=led_top
SIM=sim.cc
SIM_TOP=led_top

include $(BOARDROOT)/rules.mk
include $(ROOT)/verilator.mk

clean:
	$(BOARDCLEAN)
	$(SIMCLEAN)
