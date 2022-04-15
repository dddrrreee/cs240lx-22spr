# list out the driver program source 
TEST_SRC = 2-neopix.c 1-blink.c  0-timing-check.c # simple.c
TEST_SRC = 0-timing-check.c 

OPT_LEVEL = -O3

# your source, shared by driver code.
#   if you want to use our staff-hc-sr04.o,
#   comment SRC out and uncomment STAFF_OBJS
SRC := neopixel.c

# define this if you need to give the device for your pi
TTYUSB = 

# set RUN = 1 if you want the code to automatically run after building.
# RUN = 1

DEPS = ./Makefile
SUPPORT_OBJS := $(SRC:.c=.o)
include $(CS49N_2021_PATH)/libpi/mk/Makefile.template
