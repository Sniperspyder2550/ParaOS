[bits 32]
section .text
global _start

_start:
    ; Initialize the stack, call kernel main, or perform other setup tasks
    call idt_load       ; Load the IDT
    call enable_interrupts ; Enable interrupts
    hlt                 ; Halt the CPU (replace with your kernel's main loop)
