[org 0x7C00]
[bits 16]
mov ax, 0x07C0      ; Segment statt absoluter Adresse
mov ds, ax

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

    ; Nachricht anzeigen
    mov si, msg
    call print_string

    ; Kernel laden (hier später erweitern)
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

msg db "Booting ParaOS...", 0

times 510-($-$$) db 0
dw 0xAA55