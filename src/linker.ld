ENTRY(kernel_entry)

SECTIONS {
    . = 0x10000;

    .text : ALIGN(4) {
        KEEP(*(.text.kernel_entry))
        KEEP(*(.text*))
        *(.text)
    }

    .rodata : ALIGN(4) {
        *(.rodata*)
    }

    .data : ALIGN(4) {
        *(.data*)
    }

    .bss : ALIGN(4) {
        *(COMMON)
        *(.bss*)
    }
}
