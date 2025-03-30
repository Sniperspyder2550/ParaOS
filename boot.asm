[org 0x7C00]
[bits 16]

start.
cli
mov [boot_drive], dl
mov sp, 0x7C00

mov ah, 0x02
mov al, 1
mov ch, 0
mov dh, 0
mov cl, 2
mov dl, [boot_drive]
mov bx, 0x1000
int 0x13

jc disk_error

lgdt [gdt_descriptor]
mov eax, cr0
or eax, 0x1
mov cr0, eax
jmp 0x08:pm_start

[bits 32]

move esp, 0x90000
jmp 0x1000

boot_drive db 0
disk_err_msg db "Disk error", 0

gdt_start:
dq 0x0
gdt_code:
dw 0xFFFF
dw 0x0
db 0x0
db 10011010b
db 11001111b
db 0x0
gdt:data
dw 0xFFFF
dw 0x0 
db 0x0
db 10010010b
db 11001111b
db 0x0
gdt_end:

gdt_descriptor:
dw gdt_end - gdt_start -1
dd gdt_start 


times 510-($-$$) db 0
dw 0xAA55