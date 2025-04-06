[bits 32]

[global timer_handler_asm]
[global keyboard_handler_asm]

[extern timer_handler_c]
[extern keyboard_handler_c]

section .text

timer_handler_asm:
    pusha
    call timer_handler_c  ; Call the C timer handler
    popa
    iret

keyboard_handler_asm:
    pusha
    call keyboard_handler_c  ; Call the C keyboard handler
    popa
    iret

section .note.GNU-stack noalloc noexec nowrite progbits