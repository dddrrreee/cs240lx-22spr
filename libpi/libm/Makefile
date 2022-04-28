# our math library.
TARGET= libm-pi.a

# note: we are only compiling a small number of the possible routines in this 
# directory.   it's usually just a matter of adding the .o here (possibly pulling
# in additional .o dependencies).   we do it conservatively to attempt to minimize
# bad interactions with libpi.
LIB_OBJS = s_cos.o k_cos.o k_sin.o e_rem_pio2.o k_rem_pio2.o s_scalbn.o s_floor.o s_copysign.o s_fabs.o s_sin.o s_atan.o e_atan2.o s_round.o

# k_rem_pio2.o
SRC = $(LIB_OBJS:.o=.c)

USE_FP=1

# add all the .h's as dependencies, libpi and the makefile.
DEPS := $(wildcard ./*.h) ./Makefile $(LPI)


include $(CS49N_2021_PATH)/libpi/defs.mk

# include $(CS240LX_2021_PATH)/libpi/includes.mk

#ARM = arm-none-eabi
#CC = $(ARM)-gcc
#LD  = $(ARM)-ld
#AS  = $(ARM)-as
#OCP = $(ARM)-objcopy

# CFLAGS=  -fno-builtin -mhard-float -Og -nostdlib -nostartfiles -ffreestanding  -mcpu=arm1176jzf-s -mtune=arm1176jzf-s -mhard-float -mfpu=vfp -march=armv6zk -std=c99 -Wall -Iinclude -I. -D__BSD_VISIBLE -Wno-implicit-function-declaration

CFLAGS += -Iinclude

all : $(TARGET)

protos: 
	cproto -I. -Iinclude $(SRC) > math-protos.h

.c.o: $(DEPS)
	$(CC) $(CFLAGS) -c $< 

$(TARGET): $(LIB_OBJS)  $(DEPS)
	$(AR) crf $(TARGET) $^ 

clean :
	rm -f $(LIB_OBJS) *~ $(TARGET)

.PHONY: clean protos 

