
# swap this if you want to use your cpyjmp.c
STAFF_OBJS := ../code/staff-cpyjmp.o

# list out the driver program source 
TEST_SRC = reloc-bootloader.c

# so gcc doesn't reorder.
CFLAGS_EXTRA  = -fno-toplevel-reorder

INC = -I../code -I../ptag-linker

SRC :=  ptag-cstart.c


# define this if you need to give the device for your pi
TTYUSB = 

# set RUN = 1 if you want the code to automatically run after building.
# RUN = 1


DEPS = ./Makefile
SUPPORT_OBJS := $(SRC:.c=.o)
include $(CS240LX_2022_PATH)/libpi/mk/Makefile.template

all:: $(RELOC) objs/hello-5.ptag run-ptag

libs::
	make -C ../ptag-linker

reloc-bootloader.ptag: hello/hello.bin reloc-bootloader.bin
	../ptag-linker/ptag-linker reloc-bootloader.bin hello/hello.bin reloc-bootloader.ptag
	my-install reloc-bootloader.ptag

clean::
	rm -f reloc-bootloader.ptag
	rm -f objs/*.ptag

PLINK = ../ptag-linker/ptag-linker
RELOC = reloc-bootloader.bin 

run-ptag: objs/hello-5.ptag
	my-install objs/hello-5.ptag

objs/hello-5.ptag: $(RELOC) objs/hello.bin
	$(PLINK) $(RELOC) objs/hello.bin objs/hello-1.ptag
	$(PLINK) $(RELOC) objs/hello-1.ptag objs/hello-2.ptag
	$(PLINK) $(RELOC) objs/hello-2.ptag objs/hello-3.ptag
	$(PLINK) $(RELOC) objs/hello-3.ptag objs/hello-4.ptag
	$(PLINK) $(RELOC) objs/hello-4.ptag objs/hello-5.ptag

objs/hello-10.ptag: $(RELOC) objs/hello-5.ptag
	$(PLINK) $(RELOC) objs/hello-5.ptag objs/hello-6.ptag
	$(PLINK) $(RELOC) objs/hello-6.ptag objs/hello-7.ptag
	$(PLINK) $(RELOC) objs/hello-7.ptag objs/hello-8.ptag
	$(PLINK) $(RELOC) objs/hello-8.ptag objs/hello-9.ptag
	$(PLINK) $(RELOC) objs/hello-9.ptag objs/hello-10.ptag
	my-install objs/hello-10.ptag


