; Param OS - Stage 1 Bootloader (fixed)
; Loads a second-stage kernel from disk and jumps to it.
; Assembled with: nasm -f bin boot.asm -o boot.bin

BITS 16
ORG 0x7C00

start:
    ; Set up segments and stack
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ; Save boot drive
    ; Force to 0x80 to avoid QEMU DL bugs
    mov byte [boot_drive], 0x80

    ; Clear screen
    mov ax, 0x0600
    mov bh, 0x07
    mov cx, 0x0000
    mov dx, 0x184F
    int 0x10

    ; Print main message
    mov ah, 0x02
    mov bh, 0x00
    mov dh, 5
    mov dl, 10
    int 0x10

    mov si, msg_boot
    call print_string

    ; Move cursor and print loading text
    mov ah, 0x02
    mov bh, 0x00
    mov dh, 7
    mov dl, 10
    int 0x10

    mov si, msg_load
    call print_string

    ; ---------------------------
    ; Load second-stage kernel
    ; ---------------------------
    xor ax, ax
    mov es, ax           ; ES = 0000
    mov bx, 0x8000       ; target offset

    mov ah, 0x02         ; BIOS read sectors
    mov al, NUM_SECTORS  ; number of sectors to read
    mov ch, 0            ; cylinder
    mov cl, 2            ; sector 2 (after bootloader)
    mov dh, 0            ; head
    mov dl, [boot_drive] ; drive
    int 0x13
    jc disk_error        ; if CF = 1 → error

    ; Jump to loaded kernel at 0000:8000
    jmp 0x0000:0x8000

disk_error:
    mov ah, 0x02
    mov bh, 0x00
    mov dh, 9
    mov dl, 10
    int 0x10

    mov si, msg_err
    call print_string
    jmp $

; --------------------------------
; Print zero-terminated string at DS:SI
; --------------------------------
print_string:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0E
    int 0x10
    jmp print_string
.done:
    ret

; --------------------------------
; Data
; --------------------------------
msg_boot db "Param OS: Bootloader is running!", 0
msg_load db "Loading Param OS kernel...", 0
msg_err  db "Disk read error while loading kernel.", 0

boot_drive db 0
NUM_SECTORS equ 1          ; Kernel is exactly 1 sector (512B)

; Boot sector signature
times 510-($-$$) db 0
dw 0xAA55
