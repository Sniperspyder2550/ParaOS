[bits 32]
section .text

extern timer_handler

global timer_handler_asm
global idt_load
global enable_interrupts
global default_handler

extern idt_descriptor

default_handler:
    mov edi, 0xB8000 + 160
    mov word [edi], 0x4F21
    iret

idt_load:
    lidt [idt_descriptor]
    ret

enable_interrupts:
    sti 
    ret

timer_handler_asm:
    pusha                   ; Save all general-purpose registers
    call timer_handler      ; Call the C timer handler
    popa                    ; Restore all general-purpose registers
    iret                    ; Return from the interrupt
    

section .note.GNU-stack noalloc noexec nowrite progbits