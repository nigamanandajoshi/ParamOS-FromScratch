#include "ports.h"
#include "pic.h"
#include "stdint.h"

#define PIC1_CMD  0x20
#define PIC1_DATA 0x21
#define PIC2_CMD  0xA0
#define PIC2_DATA 0xA1
#define PIC_EOI   0x20

void pic_send_eoi(int irq) {
    if (irq >= 8) {
        outb(PIC2_CMD, PIC_EOI);
    }
    outb(PIC1_CMD, PIC_EOI);
}

void pic_remap(void) {
    uint8_t a1 = inb(PIC1_DATA);
    uint8_t a2 = inb(PIC2_DATA);

    // Start initialization sequence
    outb(PIC1_CMD, 0x11);
    outb(PIC2_CMD, 0x11);

    // Vector offsets
    outb(PIC1_DATA, 0x20); // Master PIC: 32
    outb(PIC2_DATA, 0x28); // Slave PIC: 40

    // Wiring info
    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);

    // Environment info
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);

    // Restore masks as a baseline…
    outb(PIC1_DATA, a1);
    outb(PIC2_DATA, a2);

    // …then explicitly unmask only IRQ0 (timer) and mask others
    outb(PIC1_DATA, 0xFE); // 1111 1110 : only IRQ0 = 0 (enabled)
    outb(PIC2_DATA, 0xFF); // all slave IRQs masked for now
}
