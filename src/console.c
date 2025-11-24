#include "console.h"

#define VGA_MEMORY 0xB8000
#define VGA_COLS   80
#define VGA_ROWS   25

static uint16_t* const VGA = (uint16_t*)VGA_MEMORY;

static uint8_t cursor_row = 0;
static uint8_t cursor_col = 0;

static uint16_t vga_entry(char c, uint8_t color) {
    return ((uint16_t)color << 8) | (uint8_t)c;
}

void console_clear(uint8_t color) {
    for (int i = 0; i < VGA_COLS * VGA_ROWS; i++) {
        VGA[i] = vga_entry(' ', color);
    }
    cursor_row = 0;
    cursor_col = 0;
}

void console_putc(char c, uint8_t color) {
    if (c == '\n') {
        cursor_row++;
        cursor_col = 0;
    } else {
        int index = cursor_row * VGA_COLS + cursor_col;
        VGA[index] = vga_entry(c, color);
        cursor_col++;

        if (cursor_col >= VGA_COLS) {
            cursor_col = 0;
            cursor_row++;
        }
    }

    if (cursor_row >= VGA_ROWS) {
        cursor_row = 0;   // later we will replace with scrolling
    }
}

void console_write(const char* s, uint8_t color) {
    while (*s) {
        console_putc(*s++, color);
    }
}
