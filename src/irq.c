#include "irq.h"
#include "pic.h"
#include "console.h"
#include "stdint.h"

static uint32_t timer_ticks = 0;

void irq_handler(int irq) {
    if (irq == 0) {
        // Timer interrupt (IRQ0)
        timer_ticks++;
        if (timer_ticks % 100 == 0) {
            console_write("Tick\n", 0x0E);
        }
    }

    // other IRQs can be handled here later (keyboard, etc.)

    pic_send_eoi(irq);
}
