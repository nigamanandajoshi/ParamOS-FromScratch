#ifndef PARAMOS_PORTS_H
#define PARAMOS_PORTS_H

#include "stdint.h"

// Read 8-bit from port
uint8_t inb(uint16_t port);

// Write 8-bit to port
void outb(uint16_t port, uint8_t value);

// Read 16-bit from port
uint16_t inw(uint16_t port);

// Write 16-bit to port
void outw(uint16_t port, uint16_t value);

#endif
