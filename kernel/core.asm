; core.asm
[bits 32]
section .multiboot
mb_header:
    dd 0x1BADB002
    dd 0x00
    dd -(0x1BADB002 + 0x00)

section .text
extern main
global _start
extern timer_handler
extern keyboard_handler

_start:
    ; BSS-Clear
    mov edi, _bss_start
    mov ecx, _bss_end
    sub ecx, edi
    xor eax, eax
    rep stosb

    call main
    cli
.hang:
    hlt
    jmp .hang

global gdt_flush
extern gdt_ptr
gdt_flush:
    lgdt [gdt_ptr]
    jmp 0x08:.reload_cs
.reload_cs:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret

global timer_handler_asm
timer_handler_asm:
    pusha
    call timer_handler
    popa
    iretd

global keyboard_handler_asm
keyboard_handler_asm:
    pusha
    call keyboard_handler
    popa
    iretd

section .bss
_bss_start:
resb 4096
_bss_end:

section .note.GNU-stack noalloc noexec nowrite progbits
