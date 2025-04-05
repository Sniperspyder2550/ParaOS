global timer_handler_asm
extern timer_handler

section .text
timer_handler_asm:
    pusha
    call timer_handler
    popa
    iret

section .note.GNU-stack noalloc noexec nowrite progbits