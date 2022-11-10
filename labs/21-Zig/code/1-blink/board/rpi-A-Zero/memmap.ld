SECTIONS {
    .text 0x8000: {
        __code_start__ = .;
        KEEP(*(.text.boot))
        *(.text*)
        __code_end__ = .;
        . = ALIGN(8);
    }
    .rodata : { *(.rodata*) }
    .data : {
        __data_start__ = .;
        *(data*)
        . = ALIGN(4);
        __data_end__ = .;
    }
 
    .bss : {
        . = ALIGN(4);
        FILL(0x00); // fills our BSS with 0's
        __bss_start__ = .;
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(8);
        __bss_end__ = .;
        . = ALIGN(8);
        __prog_end__ = .;
        __heap_start__ = .;
    }

    /DISCARD/ :
    {
        *(.comment .comment.*)
        // these sections are for unwinding stacks and backtraces and such, we dont need them for now. It inflates binary size a ton.
        *(.ARM* .ARM.*)
        *(.debug* .debug.*)
        //*(.strtab* .strtab.*)
        //*(.symtab* .symtab.*)
        //*(.shstrtab* .shstrtab.*)
    }
}
