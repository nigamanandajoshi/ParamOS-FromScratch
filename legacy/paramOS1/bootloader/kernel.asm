; Param OS - Stage 2 "Kernel" (real-mode)
; Loaded by the bootloader at 0x0000:0x8000

BITS 16
ORG 0x8000

kernel_start:
    ; Set up segments
    xor ax, ax
    mov ds, ax
    mov es, ax

    ; Clear screen again so we see that kernel took over
    mov ax, 0x0600
    mov bh, 0x07
    mov cx, 0x0000
    mov dx, 0x184F
    int 0x10

    ; Print kernel banner
    mov ah, 0x02
    mov bh, 0x00
    mov dh, 5
    mov dl, 5
    int 0x10

    mov si, kmsg1
    call print_string

    mov ah, 0x02
    mov bh, 0x00
    mov dh, 7
    mov dl, 5
    int 0x10

    mov si, kmsg2
    call print_string

    ; Hang here
.halt:
    jmp .halt

print_string:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0E
    int 0x10
    jmp print_string
.done:
    ret

kmsg1 db "Welcome to Param OS Kernel (stage 2)!", 0
kmsg2 db "Running in 16-bit real mode, loaded by our bootloader.", 0
