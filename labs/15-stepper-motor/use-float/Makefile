# we give you  .o's to start with: swap yours in.
TEST_SRC = fp-example.c
USE_FP=1

# is there a way to do this so we don't have to export?
export USE_FP


# your source, shared by driver code.
#   if you want to use our staff-hc-sr04.o,
#   comment SRC out and uncomment STAFF_OBJS
# SRC := ads11115.c
# STAFF_OBJS := staff-ads1115.o staff-i2c.o

# define this if you need to give the device for your pi
TTYUSB = 

# set RUN = 1 if you want the code to automatically run after building.
RUN = 1

LIBS += $(LIBM)

DEPS = ./Makefile
SUPPORT_OBJS := $(SRC:.c=.o)
include $(CS240LX_2022_PATH)/libpi/mk/Makefile.template

cleanall:
	make -C $(LPP) clean
	make -C $(LPP)/libm clean
