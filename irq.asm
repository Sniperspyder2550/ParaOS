; korrigierte irq.asm - ohne Divisionen
[bits 32]
[global keyboard_handler]
[extern keyboard_handler_c]

keyboard_handler:
    pusha
    call keyboard_handler_c  ; Direkter Aufruf ohne Berechnung
    popa
    iretd