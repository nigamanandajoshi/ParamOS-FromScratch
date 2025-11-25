BITS 32
section .text

global isr21
extern isr21_handler

; Simple ISR stub for interrupt 0x21
; Saves registers, calls a C handler, restores, returns.

isr21:
    pusha                   ; save all general-purpose registers
    call isr21_handler      ; call C handler
    popa                    ; restore registers
    iretd                   ; return from interrupt
