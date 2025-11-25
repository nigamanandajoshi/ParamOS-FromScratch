#include "timer.h"
#include "ports.h"

// PIT runs at ~1.19318 MHz
void timer_init(uint32_t frequency) {
    uint32_t divisor = 1193180 / frequency;

    // Command byte: channel 0, lobyte/hibyte, mode 3, binary
    outb(0x43, 0x36);

    // Divisor low and high bytes
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
}
