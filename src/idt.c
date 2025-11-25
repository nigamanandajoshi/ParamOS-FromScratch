#include "idt.h"
#include "stdint.h"

static struct idt_entry idt[256];
static struct idt_ptr   idtp;

// from isr.asm
extern void isr21(void);

static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low  = base & 0xFFFF;
    idt[num].sel       = sel;
    idt[num].always0   = 0;
    idt[num].flags     = flags;
    idt[num].base_high = (base >> 16) & 0xFFFF;
}

static void idt_load(struct idt_ptr* idtp) {
    __asm__ __volatile__("lidtl (%0)" : : "r"(idtp));
}

void idt_init(void) {
    // Clear the IDT
    for (int i = 0; i < 256; i++) {
        idt[i].base_low  = 0;
        idt[i].sel       = 0;
        idt[i].always0   = 0;
        idt[i].flags     = 0;
        idt[i].base_high = 0;
    }

    // Set a demo handler on interrupt vector 0x21
    // Code segment selector = 0x08 (your GDT code segment)
    // Flags 0x8E: present, ring 0, 32-bit interrupt gate
    idt_set_gate(0x21, (uint32_t)isr21, 0x08, 0x8E);

    idtp.limit = sizeof(idt) - 1;
    idtp.base  = (uint32_t)&idt;

    idt_load(&idtp);
}
