# list out the driver program source 
TEST_SRC = tests/part2-test2.c tests/part2-test1.c

#   if you want to use our staff-hc-sr04.o,
#   comment SRC out and uncomment STAFF_OBJS
SUPPORT_OBJS := ckalloc.o ck-gc.o gc-asm.o

# define this if you need to give the device for your pi
TTYUSB = 

# set RUN = 1 if you want the code to automatically run after building.
RUN = 1

DEPS = ./Makefile
include $(CS240LX_2022_PATH)/libpi/mk/Makefile.template
