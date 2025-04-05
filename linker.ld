ENTRY(_start)

PHDRS {
    text PT_LOAD FLAGS(0x5);  /* Read + Execute */
    data PT_LOAD FLAGS(0x6); /* Read + Write */
    bss PT_LOAD FLAGS(0x6);  /* Read + Write */
}

SECTIONS {
    . = 0x1000;

    .text : {
        *(.text)
    } :text

    .data : {
        *(.data)
    } :data

    .bss : {
        *(.bss)
    } :bss
}