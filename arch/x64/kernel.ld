OUTPUT_FORMAT("elf64-x86-64");
ENTRY(x64_startup);

MEMORY {
  startup(rx): ORIGIN = 0x00010000, LENGTH = 1K
  kernel(rx): ORIGIN = 0xffff800000011000, LENGTH = 60K
  ram(rx): ORIGIN = 0xffff800000020000, LENGTH = 256K
}

SECTIONS {
  .startup : {
    arch/x64/startup.o(.text);
  } > startup

  .text : AT(0x00011000) {
    *(.text);
  } > kernel

  . = ALIGN(8);

  .data :{
    *(.data);
  } > kernel

  .rodata :{
    *(.rodata*);
  } > kernel

  . = ALIGN(8);

  __bss_start = .;
  .bss :{
    *(.bss);

    /* The kernel stack used in boot sequence and idle thread. */
    __boot_stack = .;
    . = ALIGN(8);
    . += 8192;
    __boot_stack_end = .;
  } > ram
  __bss_end = .;

  /DISCARD/ :{
    *(*.eh_frame);
  }
}
