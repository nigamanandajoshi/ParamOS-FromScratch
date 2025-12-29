// // #include "ports.h"
// // #include "console.h"
// // #include "ports.h"      


// // void kernel_main() {
// //     console_clear(0x07);

// //     console_write("Param OS with PORTS loaded!\n", 0x0F);

// //     uint8_t status = inb(0x64); // read keyboard controller status
    
// //     console_write("Port read OK!\n", 0x0A);

// //     (void)status; // avoid unused warning for now
// // }
// #include "console.h"
// #include "ports.h"
// #include "idt.h"
// #include "stdint.h"

// // C-side handler called from isr.asm
// void isr21_handler(void) {
//     console_write("IDT test: interrupt 0x21 handled.\n", 0x0B);
// }

// void kernel_main(void) {
//     console_clear(0x07);

//     console_write("Welcome to Param OS C Kernel!\n", 0x0F);
//     console_write("Running in 32-bit Protected Mode.\n", 0x0A);
//     console_write("Powered by Nigam <3\n\n", 0x0E);

//     console_write("Initializing IDT...\n", 0x07);
//     idt_init();
//     console_write("IDT installed.\n", 0x07);

//     console_write("Triggering software interrupt 0x21...\n", 0x07);
//     __asm__ __volatile__("int $0x21");

//     console_write("Returned from interrupt 0x21.\n", 0x07);

//     // hang – kernel_entry.asm will handle halt loop
// }

#include "console.h"
#include "ports.h"
#include "idt.h"
#include "pic.h"
#include "timer.h"
#include "stdint.h"
#include "keyboard.h"
#include "frames.h"
#include "kmalloc.h"
#include "task.h"
#include "shell.h"


// called from isr21 stub
void isr21_handler(void) {
    console_write("Software interrupt 0x21 handled.\n", 0x0B);
}

void kernel_main(void) {
    console_clear(0x07);

    console_write("ParamOS C Kernel is running.\n", 0x0F);
    console_write("Setting up interrupts...\n", 0x07);

    idt_init();
    console_write("IDT installed.\n", 0x07);

    pic_remap();
    console_write("PIC remapped.\n", 0x07);

    timer_init(100); // 100 Hz => 10ms tick
    console_write("Timer initialized at 100Hz.\n", 0x07);

    keyboard_init();
    console_write("Keyboard initialized.\n", 0x07);

    frames_init(64 * 1024 * 1024); // assume 64MB RAM
    console_write("Physical memory manager initialized.\n", 0x0A);

    // Initialize kernel heap (1MB starting at 2MB mark)
    kmalloc_init(0x200000, 0x100000);
    console_write("Kernel heap initialized.\n", 0x0A);

    // Initialize task subsystem
    tasks_init();

    // Create shell as main task
    task_create(shell_run, "shell");

    console_write("Enabling interrupts...\n", 0x07);
    __asm__ __volatile__("sti");

    // Start the scheduler - this will switch to shell
    schedule();

    // Should never reach here
    console_write("ERROR: Returned from schedule!\n", 0x0C);
    for (;;) {
        __asm__ __volatile__("hlt");
    }
}


