BITS 16
ORG 0x7C00

; -------------------------
; Variables (must be before code!)
; -------------------------
BOOT_DRIVE db 0

start:
    cli
    mov [BOOT_DRIVE], dl      ; Save BIOS boot drive

    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

; -------------------------
; ENABLE A20
; -------------------------
a20_wait1:
    in al, 0x64
    test al, 2
    jnz a20_wait1

    mov al, 0xD1
    out 0x64, al

a20_wait2:
    in al, 0x64
    test al, 2
    jnz a20_wait2

    mov al, 0xDF
    out 0x60, al

; -------------------------
; LOAD KERNEL AT 0x00010000
; -------------------------
    mov ax, 0x1000      ; ES = 0x1000 → physical 0x10000
    mov es, ax
    xor bx, bx

    mov ah, 0x02        ; BIOS read sector
    mov al, 10          ; kernel = 10 sectors (~5KB)
    mov ch, 0
    mov cl, 2           ; start at sector #2
    mov dh, 0
    mov dl, [BOOT_DRIVE]

    int 0x13
    jc disk_error

; -------------------------
; LOAD GDT
; -------------------------
    lgdt [gdt_descriptor]

; -------------------------
; ENTER PROTECTED MODE
; -------------------------
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp 0x08:protected_mode   ; far jump into 32-bit PM


; -------------------------
; PROTECTED MODE
; -------------------------
[BITS 32]
protected_mode:
    cli
    mov ax, 0x10           ; data segment selector
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov esp, 0x9FB00       ; stack in low memory

    ; Jump to kernel entry at 0x00010000
    jmp 0x08:0x00010000


; -------------------------
; DISK ERROR
; -------------------------
[BITS 16]
disk_error:
    mov si, diskmsg
.err_loop:
    lodsb
    or al, al
    jz $
    mov ah, 0x0E
    int 0x10
    jmp .err_loop

diskmsg db "Disk error loading kernel!",0


; -------------------------
; GDT
; -------------------------
[BITS 32]
gdt_start:
    dq 0x0000000000000000        ; null descriptor
    dq 0x00CF9A000000FFFF        ; code segment
    dq 0x00CF92000000FFFF        ; data segment
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start


; -------------------------
; BOOT SIGNATURE
; -------------------------
times 510-($-$$) db 0
dw 0xAA55
