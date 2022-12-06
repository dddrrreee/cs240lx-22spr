SUPPORT_OBJS := debug-fault.o 

# define this if you need to give the device for your pi
TTYUSB = 
BOOTLOADER = my-install

# uncomment if you want it to automatically run.
RUN=1

TEST_SRC := test.c


include $(CS240LX_2022_PATH)/libpi/mk/Makefile.template

# override the default rules.  should have a cleaner solution
test.elf:  test.o $(DEPS) $(START) $(LIBS)
	$(LD) $(START)  $< -T $(MEMMAP) -o $@  $(LIBS)


