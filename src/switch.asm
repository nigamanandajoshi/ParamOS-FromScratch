; switch.asm - Context switching for ParamOS
; Saves current task's registers and loads next task's registers

BITS 32
section .text

; void context_switch(uint32_t* old_esp, uint32_t new_esp)
; Arguments:
;   [esp+4] = pointer to save current ESP (can be NULL for first switch)
;   [esp+8] = new ESP to load
;
; This function:
; 1. Saves all registers to current stack
; 2. Saves current ESP to old_esp pointer
; 3. Loads new ESP
; 4. Restores all registers from new stack
; 5. Returns (which jumps to new task's EIP)

global context_switch

context_switch:
    ; Save current registers
    pusha               ; Push EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI

    ; Get arguments
    mov eax, [esp + 36] ; old_esp pointer (32 bytes from pusha + 4 bytes return addr)
    mov ecx, [esp + 40] ; new_esp value

    ; Save current ESP if old_esp is not NULL
    cmp eax, 0
    je .load_new_stack
    mov [eax], esp      ; Save current stack pointer

.load_new_stack:
    ; Switch to new stack
    mov esp, ecx

    ; Restore new task's registers
    popa                ; Pop EDI, ESI, EBP, (skip ESP), EBX, EDX, ECX, EAX

    ; Return to new task (pops EIP from new stack and jumps there)
    ret
