NAME = stepper-test
SOURCES := $(NAME).c stepper.c stepper-int.c
# if you want to compile everything in a directory, do something like this.
# SOURCES := $(wildcard ./*.c)


include $(CS240LX_2022_PATH)/libpi/defs.mk

CFLAGS += -Wno-unused-function -Wno-unused-variable -falign-functions=4 

OBJS := $(patsubst %.c, %.o, $(SOURCES))

# add all the .h's as dependencies, libpi and the makefile.
DEPS := $(MEMMAP) $(LPI) $(wildcard ./*.h) ./Makefile 


# i *think* order is important.

all: libs $(OBJS) $(NAME).bin

$(OBJS): $(DEPS)
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@
%.o: %.S
	$(CC) -c $(CPP_ASFLAGS) $< -o $@

# rules to generate .elf->.list->.bin
%.elf: %.o $(OBJS) $(MEMMAP) $(DEPS) 
	$(LD) $(START) $(OBJS) -T $(LPP)/memmap -o $@  $(LIBS)

%.list: %.elf
	$(OD) -D   $*.elf > $*.list
%.bin: %.list
	$(OCP) $*.elf -O binary $*.bin

run:
	make
	my-install $(NAME).bin

tags: 
	ctags *.[chSs] $(LPP)/*.[ch] $(LPP)/*.[chSs]

clean:
	rm -rf $(BUILD_DIR) $(NAME) *~ tags *.o *.bin *.elf *.list *.img Makefile.bak  *.d

.PHONY: libs all tags clean run

# stop make from deleting these suffix'd files.
.PRECIOUS: %.list %.elf
