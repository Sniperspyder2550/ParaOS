ENTRY(_start)
MEMORY {
    CODE (rx) : ORIGIN = 0x1000, LENGTH = 1M
    DATA (rw) : ORIGIN = 0x200000, LENGTH = 1M
    STACK (rw) : ORIGIN = 0x300000, LENGTH = 16K
}

SECTIONS {
    . = 0x00100000; /* Kernel load address */

    .text : {
        *(.text)
    } > CODE

    .rodata : {
        *(.rodata)
    } > CODE

    .data : {
        *(.data)
    } > DATA

    .bss : {
        *(.bss)
        *(COMMON)
    } > DATA
}