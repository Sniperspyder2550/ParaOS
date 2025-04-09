ENTRY(_start)

PHDRS {
    boot PT_LOAD FLAGS(5);    /* R-X for bootloader */
    text PT_LOAD FLAGS(5);    /* R-X for kernel */
    data PT_LOAD FLAGS(6);    /* RW for data */
}

SECTIONS {
    /* Bootloader loaded at 0x7C00 */
    .boot 0x7C00 : {
        *(.boot)
    } :boot

    /* Kernel loaded at 0x100000 */
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

    .bss : ALIGN(4) {
        _bss_start = .;
        *(.bss)
        *(COMMON)
        _bss_end = .;
    } :data

    /DISCARD/ : {
        *(.note.gnu.property)
    }
}
