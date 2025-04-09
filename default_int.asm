; default_int.asm
[BITS 32]
global default_interrupt_handler_asm
default_interrupt_handler_asm:
    pusha
    ; Acknowledge the PIC interrupt (send EOI)
    mov al, 0x20
    out 0x20, al
    popa
    iret
