[bits 16]
org 0x7C00

entry:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ; VGA-Modus setzen
    mov ax, 0x0003
    int 0x10

    ; Boot-Nachricht
    mov si, boot_msg
    call print_string

    ; Kernel laden (später implementieren)
    hlt

print_string:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0E
    int 0x10
    jmp print_string
.done:
    ret

boot_msg db "Booting ParaOS...", 0
times 510-($-$$) db 0
dw 0xAA55