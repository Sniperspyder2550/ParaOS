ENTRY(_start)
MEMORY {
    CODE (rx) : ORIGIN = 0x1000, LENGTH = 1M
    DATA (rw) : ORIGIN = 0x200000, LENGTH = 1M
    STACK (rw) : ORIGIN = 0x300000, LENGTH = 16K
}

SECTIONS {
    . = 0x00100000; /* Example load address */

    /* Code section (read-only, executable) */
    .text : {
        KEEP(*(.text._start)) /* Ensure _start is included */
        *(.text)
        *(.text.*)
        KEEP(*(.init))
        KEEP(*(.fini))
    } > CODE

    /* Read-only data section (read-only, non-executable) */
    .rodata : {
        *(.rodata)
        *(.rodata.*)
        *(.eh_frame)
    } > CODE

    /* Data section (read-write, non-executable) */
    .data : {
        *(.data)
        *(.data.*)
    } > DATA

    /* BSS section (uninitialized data, read-write, non-executable) */
    .bss : {
        *(.bss)
        *(COMMON)
    } > DATA

    /* Stack section (read-write, non-executable) */
    .stack : {
        . = ORIGIN(STACK);
        . = . + LENGTH(STACK);
    } > STACK
}