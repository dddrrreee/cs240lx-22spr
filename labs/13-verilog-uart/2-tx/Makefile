ROOT=$(CS240LX_2022_PATH)/fpga
BOARD ?= upduino
BOARDROOT=$(ROOT)/$(BOARD)

SRC=$(wildcard *.sv)
TOP=uart_top
SIM=sim.cc
SIM_TOP=uart_top
SIM_SRC=$(SRC)
VFLAGS += -DCLK_MHZ=48 --public

include $(BOARDROOT)/rules.mk
include $(ROOT)/verilator.mk

clean:
	$(BOARDCLEAN)
	$(SIMCLEAN)
