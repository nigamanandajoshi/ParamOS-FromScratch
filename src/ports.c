#include "ports.h"

// Read byte from port
uint8_t inb(uint16_t port) {
    uint8_t result;
    __asm__ __volatile__("inb %1, %0"
                         : "=a"(result)
                         : "dN"(port));
    return result;
}

// Write byte to port
void outb(uint16_t port, uint8_t value) {
    __asm__ __volatile__("outb %0, %1"
                         :
                         : "a"(value), "dN"(port));
}

// Read word (16-bit)
uint16_t inw(uint16_t port) {
    uint16_t result;
    __asm__ __volatile__("inw %1, %0"
                         : "=a"(result)
                         : "dN"(port));
    return result;
}

// Write word (16-bit)
void outw(uint16_t port, uint16_t value) {
    __asm__ __volatile__("outw %0, %1"
                         :
                         : "a"(value), "dN"(port));
}
