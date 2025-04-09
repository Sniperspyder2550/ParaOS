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
    mov ax, 0x1000    ; ES:BX = 0x1000:0000 (physical address 0x100000)
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

    ; Do a far jump to flush the prefetch queue and switch to our 32-bit code segment.
    jmp CODE_SEG:protected_mode_entry

; ---------------------------
; 16-bit routines below
; ---------------------------
print_string:
    lodsb
    or al, al
    jz .print_done
    mov ah, 0x0E
    int 0x10
    jmp print_string
.print_done:
    ret

enable_A20:
    in al, 0x92
    or al, 0x02
    out 0x92, al
    ret

disk_error:
    mov si, disk_err_msg
    call print_string
    jmp $

; ---------------------------
; Data & GDT Setup
; ---------------------------
boot_msg      db "Bootloader: Loading ParaOS Kernel...", 0
disk_err_msg  db "Disk read error!", 0

; Simple GDT: null descriptor, then 32-bit code and data segments.
align 8
gdt_start:
    dd 0x0
    dd 0x0

    ; Code Segment Descriptor (base=0, limit=4GB, executable & readable)
    dw 0xFFFF         ; Limit Low
    dw 0x0000         ; Base Low
    db 0x00           ; Base Middle
    db 10011010b      ; Access: present, ring 0, code, executable, readable
    db 11001111b      ; Granularity: 4K granularity, 32-bit, limit high bits
    db 0x00           ; Base High

    ; Data Segment Descriptor (base=0, limit=4GB, writable)
    dw 0xFFFF         ; Limit Low
    dw 0x0000         ; Base Low
    db 0x00           ; Base Middle
    db 10010010b      ; Access: present, ring 0, data, writable
    db 11001111b      ; Granularity: 4K granularity, 32-bit, limit high bits
    db 0x00           ; Base High
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

; Define selectors (must match the descriptors above)
%define CODE_SEG 0x08
%define DATA_SEG 0x10

; ---------------------------
; Switch to 32-bit Protected Mode
; ---------------------------
[BITS 32]
protected_mode_entry:
    ; Set up segment registers for protected mode
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Jump to kernel entry point (kernel is linked to run at 0x100000)
    jmp 0x100000

halt:
    cli
    hlt
    jmp halt

; Pad boot sector and add boot signature
times 510 - ($ - $$) db 0
dw 0xAA55
