#ifndef PARAMOS_CONSOLE_H
#define PARAMOS_CONSOLE_H

#include "stdint.h"

void console_clear(uint8_t color);
void console_putc(char c, uint8_t color);
void console_write(const char* s, uint8_t color);

#endif
