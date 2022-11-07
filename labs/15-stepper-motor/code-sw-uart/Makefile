# two example programs: measuring a motor level, and a simple 
# thread interleaving example.
TEST_SRC = measure-motor.c 
TEST_SRC = interleave-example.c

# if you want to use your own, replace the file in STAFF_OBJS with
# your file in SRC (or just add to your libpi)
#SRC := fft.c
# this should be in your libpi
STAFF_OBJS := $(CS240LX_2022_PATH)/libpi/staff-objs/rpi-thread.o
STAFF_OBJS += $(CS240LX_2022_PATH)/libpi/staff-objs/rpi-thread-asm.o
STAFF_OBJS += staff-ads1115.o 
STAFF_OBJS += staff-i2c.o

# define this if you need to give the device for your pi
TTYUSB = 

# set RUN = 1 if you want the code to automatically run after building.
RUN = 1

LIBS += $(CS240LX_2022_PATH)/lib/libgcc.a

DEPS = ./Makefile
SUPPORT_OBJS := $(SRC:.c=.o)
include $(CS240LX_2022_PATH)/libpi/mk/Makefile.template
