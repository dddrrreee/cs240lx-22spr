# list out the driver program source 
TEST_SRC = 0-test-reloc.c

# so gcc doesn't reorder.
CFLAGS_EXTRA  = -fno-toplevel-reorder


# swap these if you want to use your cpyjmp.c
# SRC := cpyjmp.c
STAFF_OBJS := staff-cpyjmp.o

# define this if you need to give the device for your pi
TTYUSB = 

# set RUN = 1 if you want the code to automatically run after building.
RUN = 1

DEPS = ./Makefile
SUPPORT_OBJS := $(SRC:.c=.o)
include $(CS240LX_2022_PATH)/libpi/mk/Makefile.template
