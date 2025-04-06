global timer_handler_asm
global keyboard_handler_asm

section .text

timer_handler_asm:
    pusha                       ; Push all general-purpose registers
    call timer_handler          ; Call the C timer handler
    popa                        ; Pop all general-purpose registers
    iret                        ; Return from interrupt

keyboard_handler_asm:
    pusha                       ; Push all general-purpose registers
    call keyboard_handler       ; Call the C keyboard handler
    popa                        ; Pop all general-purpose registers
    iret                        ; Return from interrupt
