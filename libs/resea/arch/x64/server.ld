OUTPUT_FORMAT("elf64-x86-64");
ENTRY(start);

SECTIONS {

    . = 0x01000000;

    .text :{
        *(.text);
    }

    . = ALIGN(0x1000);

    .data :{
        *(.data);
    }

    .rodata :{
        *(.rodata*);
    }

    . = ALIGN(0x1000);

    .bss :{
        *(.bss);
    }

    /DISCARD/ :{
        *(*.eh_frame);
    }
}
