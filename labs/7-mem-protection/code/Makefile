#TEST_SRC := $(wildcard ./tests-2.0/part1-test[0-9]*.c)

TEST_SRC = tests/0-test3.c

# make a local copy of cstart so you can modify it.
SUPPORT_OBJS = cstart.o  pinned-vm.o

# you will swap in yours
STAFF_OBJS = staff-mmu-asm.o staff-mmu.o   staff-pinned-vm.o staff-mmu-except.o 

# staff-pinned-vm.o

# define this if you need to give the device for your pi
TTYUSB = 

# set RUN = 1 if you want the code to automatically run after building.
RUN = 1

# use the local memmap
MEMMAP = ./memmap

DEPS = ./Makefile
include $(CS240LX_2022_PATH)/libpi/mk/Makefile.template
