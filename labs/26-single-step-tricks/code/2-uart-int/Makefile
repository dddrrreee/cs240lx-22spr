# list out the driver program source 
TEST_SRC = test.c

# define this if you need to give the device for your pi
TTYUSB = 
BOOTLOADER = ./pi-echo-boot/my-install

# set RUN = 1 if you want the code to automatically run after building.
RUN = 1

DEPS = ./Makefile
SUPPORT_OBJS := $(SRC:.c=.o) uart-int.o

CFLAGS_EXTRA =  -fno-omit-frame-pointer -mpoke-function-name

include $(CS240LX_2022_PATH)/libpi/mk/Makefile.template

