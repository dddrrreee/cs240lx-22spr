TEST_SRC := $(wildcard ./tests/*.c)


# make a local copy of cstart so you can modify it.
SUPPORT_OBJS = cstart.o  pinned-vm.o mmu.o mmu-asm.o

# you will swap in yours
STAFF_OBJS = staff-mmu-asm.o  staff-mmu.o staff-pinned-vm.o staff-mmu-except.o 

# staff-pinned-vm.o

# define this if you need to give the device for your pi
TTYUSB = 

# set RUN = 1 if you want the code to automatically run after building.
RUN = 1

# use the local memmap
MEMMAP = ./memmap

DEPS = ./Makefile
# rules to generate .elf->.list->.bin

# %.bin: %.list
# 	$(OCP) $*.elf -O binary $*.bin

include $(CS240LX_2022_PATH)/libpi/mk/Makefile.template

%.elf: %.o $(MEMMAP) $(LINK_OBJS) $(DEPS) 
	$(LD) $(START) $*.o $(LINK_OBJS) -T $(LPP)/memmap -o $*.elf  $(LPI)
%.list: %.elf
	$(OD) -D   $*.elf > $*.list
