#include "irq.h"
#include "pic.h"
#include "console.h"
#include "stdint.h"
#include "keyboard.h"
#include "ports.h"

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
    else if (irq == 1) {
        uint8_t sc = inb(0x60);
        keyboard_handler(sc);
    }


    pic_send_eoi(irq);
}
