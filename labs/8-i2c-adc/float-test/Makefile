# to switch to using float: set USE_FP=1
USE_FP=1

# this was a bad bug where we recompiled libpi, but not with 
# USE_FP set to 1 bc it didn't get propogated across the include
export USE_FP

TEST_SRC = float.c

# your source, shared by driver code.
SRC :=
# any staff objs
# STAFF_OBJS := 

# define this if you need to give the device for your pi
TTYUSB = 

# set RUN = 1 if you want the code to automatically run after building.
RUN = 1

LIBS += $(LPP)/libm/libm-pi.a

# DEPS = ./Makefile
SUPPORT_OBJS := $(SRC:.c=.o)
include $(CS240LX_2022_PATH)/libpi/mk/Makefile.template

# you'll need to have this too.
libs::
	make -C $(LPP) USE_FP=1
	make -C $(LPP)/libm USE_FP=1

# coolest hack ever from 
# https://www.cmcrossroads.com/article/tracing-rule-execution-gnu-make
# OLD_SHELL := $(SHELL)
# SHELL = $(warning Building $@$(if $<, (from $<))$(if $?, ($? newer)))$(OLD_SHELL) -x 

