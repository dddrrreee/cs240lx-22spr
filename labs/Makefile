# change these so you can do a "make test" and it verifies what you already
# have in there.  (probably need a pi in there).   also you will need to 
# do a better design.
# ah: for test, doesn't work for us b/c we don't have the code...
#
# also it is tricky if they swap with our code and their code.
SUBDIRS=  \
    1-dynamic-code-gen  \
    2-ir  \
    3-digital-analyzer  \
    4-ws2812b  \
    5-malloc+gc  \
    6-debug-alloc  \
    7-mem-protection  \
    8-i2c-adc  \
    9-memcheck-stat  \
    10-i2c-accel  \
    11-memcheck-trap  \
    14-reloc-bootloader  \
    15-stepper-motor  \
    16-eraser  \
    17-i2s  \
    18-ptag  \

#    12-verilog-blink  \
#    13-verilog-uart  \
#    19-pcb  \
#    20-d


.PHONY: all check clean
all check clean: $(SUBDIRS)

all: TARGET=all
check: TARGET=check
clean: TARGET=clean

# No, you can't do TARGET=$@, or at least I don't know how to.

# recursive call to make
$(SUBDIRS): force
	$(MAKE) -s -C $@ $(TARGET)

clean:
	rm -f *~ *.bak

.PHONY: force
	force :;
