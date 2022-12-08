## 0-blinker
If your shared library code works you should be able to build this directory with meson and install a mymain.bin file. You can copy this file to tools/bin-uf2-converter and running  the following 3 commands should give you a working .uf2 file.

1. `meson setup buildir`
2. `cd buildir`
3. `meson install`

Checkout the meson.build file to get a better of how things work. First we load in constants from the cross file we created and then proceed to build the c_files in this directory. After adding these to our list of object files (PROCESSED_O_FILES) we build our .elf file. Which gets converted into our binary file. Notice that by adding our header files (H_FILES) to our list of dependencies we will automatically track changes in header files.