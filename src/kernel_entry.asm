BITS 32
section .text.kernel_entry

global kernel_entry
extern kernel_main

kernel_entry:
    call kernel_main

hang:
    cli
    hlt
    jmp hang

