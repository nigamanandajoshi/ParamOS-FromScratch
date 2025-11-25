#ifndef PARAMOS_IDT_H
#define PARAMOS_IDT_H

#include "stdint.h"

struct idt_entry {
    uint16_t base_low;   // lower 16 bits of handler address
    uint16_t sel;        // code segment selector in GDT
    uint8_t  always0;    // must be 0
    uint8_t  flags;      // type and attributes
    uint16_t base_high;  // upper 16 bits of handler address
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;      // size of IDT - 1
    uint32_t base;       // address of first idt_entry
} __attribute__((packed));

void idt_init(void);

#endif
