.global gdt_flush
gdt_flush:
    lgdt [eax]       # Load the GDT descriptor from the address in EAX
    mov ax, 0x10     # Load the data segment selector (offset 0x10 in GDT)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ljmp $0x08, $next # Far jump to reload the code segment (offset 0x08 in GDT)
next:
    ret
