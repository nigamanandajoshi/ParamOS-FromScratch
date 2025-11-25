// #include "ports.h"
// #include "console.h"
// #include "ports.h"      


// void kernel_main() {
//     console_clear(0x07);

//     console_write("Param OS with PORTS loaded!\n", 0x0F);

//     uint8_t status = inb(0x64); // read keyboard controller status
    
//     console_write("Port read OK!\n", 0x0A);

//     (void)status; // avoid unused warning for now
// }
#include "console.h"
#include "ports.h"
#include "idt.h"
#include "stdint.h"

// C-side handler called from isr.asm
void isr21_handler(void) {
    console_write("IDT test: interrupt 0x21 handled.\n", 0x0B);
}

void kernel_main(void) {
    console_clear(0x07);

    console_write("Welcome to Param OS C Kernel!\n", 0x0F);
    console_write("Running in 32-bit Protected Mode.\n", 0x0A);
    console_write("Powered by Nigam <3\n\n", 0x0E);

    console_write("Initializing IDT...\n", 0x07);
    idt_init();
    console_write("IDT installed.\n", 0x07);

    console_write("Triggering software interrupt 0x21...\n", 0x07);
    __asm__ __volatile__("int $0x21");

    console_write("Returned from interrupt 0x21.\n", 0x07);

    // hang – kernel_entry.asm will handle halt loop
}
