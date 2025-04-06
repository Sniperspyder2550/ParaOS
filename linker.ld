ENTRY(_start)

PHDRS {
    boot PT_LOAD FLAGS(5);    /* R-X für Bootloader */
    text PT_LOAD FLAGS(5);    /* R-X für Kernel */
    data PT_LOAD FLAGS(6);    /* RW- für Daten */
}

SECTIONS {
    /* Bootloader bei 0x7C00 */
    .boot 0x7C00 : {
        *(.boot)
    } :boot

    /* Kernel ab 1MB (0x100000) */
    . = 0x100000;

    .text : {
        *(.multiboot)
        *(.text .text.*)
    } :text

    .rodata : {
        *(.rodata .rodata.*)
    } :text

    .data : {
        *(.data .data.*)
    } :data

    .bss : {
        _bss_start = .;
        *(.bss)
        *(COMMON)
        _bss_end = .;
    } :data

    /DISCARD/ : {
        *(.note.gnu.property)
    }
}