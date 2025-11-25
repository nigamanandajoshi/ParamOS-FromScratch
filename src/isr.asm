BITS 32
section .text

; -------------------------
; Software interrupt 0x21
; -------------------------
global isr21
extern isr21_handler

isr21:
    pusha
    call isr21_handler
    popa
    iretd


; -----------------------------------
; IRQ HANDLERS (IRQ0–IRQ15 → 32–47)
; -----------------------------------

global irq0
global irq1
global irq2
global irq3
global irq4
global irq5
global irq6
global irq7
global irq8
global irq9
global irq10
global irq11
global irq12
global irq13
global irq14
global irq15

; all IRQs call this C function:
;   void irq_handler(int irq);
extern irq_handler

%macro IRQ 1
irq%1:
    pusha
    push dword %1        ; push IRQ number (0–15)
    call irq_handler
    add esp, 4           ; pop argument
    popa
    iretd
%endmacro

IRQ 0
IRQ 1
IRQ 2
IRQ 3
IRQ 4
IRQ 5
IRQ 6
IRQ 7
IRQ 8
IRQ 9
IRQ 10
IRQ 11
IRQ 12
IRQ 13
IRQ 14
IRQ 15
