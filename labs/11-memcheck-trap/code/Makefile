TEST_SRC =
TEST_SRC += tests/00-test.c
TEST_SRC += tests/01-test.c
TEST_SRC += tests/02-test.c
TEST_SRC += tests/03-test.c
TEST_SRC += tests/04-test.c
TEST_SRC += tests/05-test.c

TEST_SRC += tests/1-purify-bug.c
TEST_SRC += tests/2-purify-bug.c
TEST_SRC += tests/3-purify-free-bug.c
TEST_SRC += tests/4-purify-bug.c

TEST_SRC += tests/5-purify-no-bug.c
TEST_SRC += tests/6-purify-no-bug-slow.c
TEST_SRC += tests/7-purify-no-bug.c

SUPPORT_OBJS = memtrace.o
SUPPORT_OBJS  += checker-purify.o 
SUPPORT_OBJS  += full-except.o

STAFF_OBJS += staff-switchto-asm.o
STAFF_OBJS += staff-full-except-asm.o
STAFF_OBJS += staff-breakpoint.o
STAFF_OBJS += staff-memtrace-util.o

LPI_STAFF_OBJS = $(CS240LX_2022_PATH)/libpi/staff-objs/

# you should be able to drop in your versions of these
# [*except* that pinned-vm needs the new header file]
STAFF_OBJS += $(LPI_STAFF_OBJS)/kmalloc.o
STAFF_OBJS += $(LPI_STAFF_OBJS)/interrupts-vec-asm.o
STAFF_OBJS += $(LPI_STAFF_OBJS)/staff-pinned-vm.o
STAFF_OBJS += $(LPI_STAFF_OBJS)/staff-mmu-asm.o
STAFF_OBJS += $(LPI_STAFF_OBJS)/staff-mmu.o
STAFF_OBJS += $(LPI_STAFF_OBJS)/ckalloc.o  
STAFF_OBJS += $(LPI_STAFF_OBJS)/kr-malloc.o  

# define this if you need to give the device for your pi
TTYUSB = 

# set RUN = 1 if you want the code to automatically run after building.
RUN = 1

# use the local memmap
# MEMMAP = ./memmap

DEPS = ./Makefile
include $(CS240LX_2022_PATH)/libpi/mk/Makefile.template

clean::
	make -C tests clean
