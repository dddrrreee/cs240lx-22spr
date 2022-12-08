# Run: meson setup buildir --cross-file ../arm-cross-comp.txt 
# cd buildir; meson compile; meson install
project('0-blink', 'c')

ARMGNU = meson.get_external_property('ARMGNU')
CFLAGS = meson.get_external_property('CFLAGS')
ASMFLAGS = meson.get_external_property('ASMFLAGS')
LFLAGS = meson.get_external_property('LFLAGS')
OBJS_DIR = meson.get_external_property('OBJS_DIR')
MEMMAP = meson.get_external_property('MEMMAP')

H_FILES = ['include/cstart.h', 'include/gpio.h']
C_FILES = ['blink.c']
OBJS = ['gpio.o', 'start.o', 'cstart.o']

PROCESSED_O_FILES = []
foreach o_file: OBJS
    PROCESSED_O_FILES += OBJS_DIR / o_file
endforeach

# Tried getting something like this to work but too many assumptions are made by Meson
#executable('mymain.bin', C_FILES, c_args: CFLAGS, link_args:LFLAGS, native:false, link_with:lib_dep)

OBJS = []
foreach c_file: C_FILES
    # Building C Files in this directory
    PROCESSED_O_FILES += custom_target('c-object-gen-' + c_file,
                            input : c_file,
                            output : '@BASENAME@.o',
                            command : [ARMGNU + '-gcc', CFLAGS, '-c', '@INPUT@',
                                    '-o', '@OUTPUT@'],
                            build_by_default: true,
                            depend_files: H_FILES)
endforeach

#Building .elf file
main_elf = custom_target('main-elf-gen',
                        input : MEMMAP,
                        output : 'mymain.elf',
                        command : [ARMGNU + '-ld', LFLAGS, '-T', '@INPUT@', PROCESSED_O_FILES,
                                   '-o', '@OUTPUT@'],
                        build_by_default: true,
                        depend_files: H_FILES)

#Convert .elf file into binary
main_binary = custom_target('main-binary-gen',
                        output : 'mymain.bin',
                        command : [ARMGNU + '-objcopy', '-O', 'binary', main_elf,
                                   '@OUTPUT@'],
                        build_by_default: true, 
                        install: true,
                        install_dir: meson.current_source_dir())

