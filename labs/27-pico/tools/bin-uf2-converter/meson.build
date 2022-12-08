project('converter', 'c')
# Flags are just a list of strings
CFLAGS = ['-Wall', '-O2']
# Can be used to convert a .bin file to a .uf2 file compatible with the pico
# ex: ./bin-uf2 mymain.bin mymain.uf2
uf2conv = executable('conv', 'uf2conv.c', c_args: CFLAGS)

main_elf = custom_target('uf2-file-conv',
                        input : 'mymain.bin',
                        output : 'firmware.uf2',
                        command : [uf2conv, '@INPUT@', '@OUTPUT@'],
                        build_by_default: true,
                        install:true, 
                        install_dir: meson.current_source_dir())