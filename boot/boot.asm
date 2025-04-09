; boot.asm
; A 16-bit bootloader that loads the kernel into 0x100000 and transitions to protected mode.
[org 0x7C00]
[BITS 16]

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ; Print bootloader message
    mov si, boot_msg
    call print_string

    ; Enable A20 line (needed to access addresses above 1MB)
    call enable_A20

    ; Load kernel into memory at 0x100000
    ; (This example loads 10 sectors from sector 2 onward; adjust if needed.)
    mov ax, 0x1000    ; ES:BX = 0x1000:0000 = physical address 0x100000
    mov es, ax
    xor bx, bx

    mov ah, 0x02      ; BIOS function: Read sectors
    mov al, 10        ; Number of sectors to read
    mov ch, 0         ; Cylinder 0
    mov cl, 2         ; Sector number (sector 1 holds the bootloader)
    mov dh, 0         ; Head 0
    mov dl, 0x80      ; Boot drive (usually 0x80)
    int 0x13
    jc disk_error

    ; Set up temporary GDT for protected-mode transition
    lgdt [gdt_descriptor]

    ; Enable protected mode by setting the PE bit in CR0
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; Do a far jump to flush the prefetch
