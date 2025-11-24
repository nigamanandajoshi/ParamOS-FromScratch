#include "console.h"
#include "stdint.h"

void kernel_main() {
    console_clear(0x07);

    console_write("Welcome to Param OS C Kernel!\n", 0x0F);
    console_write("Running in 32-bit Protected Mode.\n", 0x0A);
    console_write("Powered by Nigam <3\n", 0x0E);
}
