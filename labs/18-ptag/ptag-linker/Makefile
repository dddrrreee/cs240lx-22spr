PROGS = ptag-linker reloc-install

CC = gcc

VERBOSE=1
#LIBUNIX_PATH := $(CS140E_STAFF_2022_PATH)/libunix
LIBUNIX_PATH := $(CS240LX_2022_PATH)/libunix
LIBUNIX := $(LIBUNIX_PATH)/libunix.a
CFLAGS := -Wall -Werror -g -Wno-unused-function -Wno-unused-variable -I. -I$(LIBUNIX_PATH) -g -DRPI_UNIX

# just make all .o's depend on everything.  simple.
SOURCES := $(wildcard ./*.c)
HDRS := $(wildcard ./*.h)
ALL_OBJS := $(patsubst %.c, %.o, $(SOURCES)) 

DEPS := ./Makefile $(LIBUNIX) $(LIBUNIX_PATH)/libunix.h $(HDRS)

all : libs $(PROGS) # install check

# make libunix quietly.  remove the '@' and the '-s' if you want to see what is 
# going on.
libs:
	@make -s -C $(LIBUNIX_PATH)

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -DPITAG_LINKER_PATH=\"$(PWD)\" $< -o $@

reloc-install: reloc-install.o $(DEPS)
	$(CC) $(CFLAGS) $< -o $@ $(LIBUNIX)

ptag-linker: ptag-linker.o $(DEPS)
	$(CC) $(CFLAGS) $< -o $@ $(LIBUNIX)

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),cleanall)
-include $(dep)  # include dep files
endif
endif

install: $(PROGS)
	cp $(PROGS) ../bin
.PHONY: install

%.d: %.c
	@$(CC) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$

clean :
	rm -f *.o *~ Makefile.bak $(NAME) *.ptag  $(PROGS)


.PHONY: clean tags libs run check
