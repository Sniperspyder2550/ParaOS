[bits 32]

[global timer_handler_asm]
[global keyboard_handler]

[extern timer_handler_c]
[extern keyboard_handler_c]

timer_handler:
    pusha
    call timer_handler_c
    popa
    iretd

keyboard_handler:
    pusha
    call keyboard_handler_c
    popa
    iretd