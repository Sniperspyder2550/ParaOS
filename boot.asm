[bits 16]

global _start
extern kernel_main

start:
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

disk_error:
    mov si, disk_err_msg    ; Load the error message address into SI
    ; Add code here to handle the error (e.g., display the message or halt)
    hlt                     ; Halt the CPU

[bits 32]

pm_start:
    ; Protected mode code here

lgdt [gdt_descriptor]
mov esp, 0x90000
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
gdt_data:
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

section .text
global _start
extern kernel_main
_start:
    cli                     ; Clear interrupts
    call kernel_main        ; Call the kernel's main function
    hlt                     ; Halt the CPU

times 510-($-$$) db 0
dw 0xAA55

section .note.GNU-stack noalloc noexec nowrite progbits