#include "ports.h"
#include "console.h"
#include "ports.h"      


void kernel_main() {
    console_clear(0x07);

    console_write("Param OS with PORTS loaded!\n", 0x0F);

    uint8_t status = inb(0x64); // read keyboard controller status
    
    console_write("Port read OK!\n", 0x0A);

    (void)status; // avoid unused warning for now
}
