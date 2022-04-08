# list out the driver program source 
TEST_SRC = tsop.c

# your source, shared by driver code.
#   if you want to use our staff-hc-sr04.o,
#   comment SRC out and uncomment STAFF_OBJS
# SRC := hc-sr04.c
# STAFF_OBJS = staff-hc-sr04.o

# define this if you need to give the device for your pi
TTYUSB = 

# set RUN = 1 if you want the code to automatically run after building.
RUN = 1

DEPS = ./Makefile
SUPPORT_OBJS := $(SRC:.c=.o)
# include $(CS49N_2021_PATH)/libpi/mk/Makefile.template

include $(CS240LX_2022_PATH)/libpi/mk/Makefile.template

