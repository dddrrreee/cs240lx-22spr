# list out the driver program source
# TEST_SRC = $(wildcard tests/*.c)
TEST_SRC = tests/1-i2s-test.c
# TEST_SRC = tests/2-i2s-dump.c
# TEST_SRC = tests/3-i2s-audiovis.c
# TEST_SRC = tests/4-fft-test.c
# TEST_SRC = tests/5-fft-time.c
# TEST_SRC = tests/6-fft-freqout.c
# TEST_SRC = tests/7-fft-freqviz.c
# TEST_SRC = tests/8-fft-buckets.c

OPT_LEVEL = -O3

# your source, shared by driver code.
#   if you want to use our staff-hc-sr04.o,
#   comment SRC out and uncomment STAFF_OBJS
SRC = i2s.c fft.c

# define this if you need to give the device for your pi
TTYUSB = 

# set RUN = 1 if you want the code to automatically run after building.
RUN = 1

DEPS = ./Makefile
SUPPORT_OBJS := $(SRC:.c=.o)
include $(CS240LX_2022_PATH)/libpi/mk/Makefile.template
