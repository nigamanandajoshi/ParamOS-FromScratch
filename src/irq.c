#include "irq.h"
#include "pic.h"
#include "console.h"
#include "stdint.h"
#include "keyboard.h"
#include "ports.h"
#include "task.h"

static uint32_t timer_ticks = 0;

void irq_handler(int irq) {
    if (irq == 0) {
        // Timer interrupt (IRQ0)
        timer_ticks++;
        
        // Call scheduler tick for preemptive multitasking
        scheduler_tick();
    }

    // Keyboard IRQ
    else if (irq == 1) {
        uint8_t sc = inb(0x60);
        keyboard_handler(sc);
    }

    pic_send_eoi(irq);
}

