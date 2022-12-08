# Run: meson setup buildir --cross-file arm-cross-comp.txt 
project('lib', 'c')

ARMGNU = meson.get_external_property('ARMGNU')
CFLAGS = meson.get_external_property('CFLAGS')
ASMFLAGS = meson.get_external_property('ASMFLAGS')
LFLAGS = meson.get_external_property('LFLAGS')

#TODO: Add gpio.h and gpio.c to these lists
C_DIR = 'libc'
C_FILES = ['cstart.c', 'start.S', 'clock.c']
H_DIR = 'include'
H_FILES = ['cstart.h', 'clock.h', 'integer-math.h']
ASM_FILES = ['start.S']

PROCESSED_H_FILES = []
foreach h_file: H_FILES
    PROCESSED_H_FILES += H_DIR / h_file
endforeach
# meson will only run this if an executable depends on it so must set build_by_default
# Also must set depend_files or else meson will not track changes to headers
OBJS = []
foreach c_file: C_FILES
    OBJS += custom_target('c-object-gen-' + c_file,
                            input : C_DIR / c_file,
                            output : '@BASENAME@.o',
                            command : [ARMGNU + '-gcc', CFLAGS, '-c', '@INPUT@',
                                    '-o', '@OUTPUT@'],
                            install: true, 
                            install_dir: meson.current_source_dir() / 'objects',
                            depend_files: PROCESSED_H_FILES)
endforeach