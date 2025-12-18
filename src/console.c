#include "console.h"

#define VGA_COLS 80
#define VGA_ROWS 25
#define VGA_MEMORY 0xB8000

static uint16_t* const VGA = (uint16_t*)VGA_MEMORY;

static int cursor_row = 0;
static int cursor_col = 0;

static inline uint16_t vga_entry(char c, uint8_t color) {
    return ((uint16_t)color << 8) | (uint8_t)c;
}

void console_clear(uint8_t color) {
    for (int i = 0; i < VGA_COLS * VGA_ROWS; i++) {
        VGA[i] = vga_entry(' ', color);
    }
    cursor_row = 0;
    cursor_col = 0;
}

static void scroll(uint8_t color) {
    // Move each row up by 1
    for (int row = 1; row < VGA_ROWS; row++) {
        for (int col = 0; col < VGA_COLS; col++) {
            VGA[(row - 1) * VGA_COLS + col] =
                VGA[row * VGA_COLS + col];
        }
    }

    // Clear the last row
    for (int col = 0; col < VGA_COLS; col++) {
        VGA[(VGA_ROWS - 1) * VGA_COLS + col] = vga_entry(' ', color);
    }

    cursor_row = VGA_ROWS - 1;
    cursor_col = 0;
}

static void console_newline() {
    cursor_row++;
    cursor_col = 0;

    if (cursor_row >= VGA_ROWS) {
        scroll(0x07);  // default terminal color
    }
}

void console_putc(char c, uint8_t color) {
    if (c == '\n') {
        console_newline();
        return;
    }
    
    /* Handle backspace */
    if (c == '\b') {
        if (cursor_col > 0) {
            cursor_col--;
        } else if (cursor_row > 0) {
            /* Wrap to end of previous line */
            cursor_row--;
            cursor_col = VGA_COLS - 1;
        }
        /* Erase the character at current position */
        VGA[cursor_row * VGA_COLS + cursor_col] = vga_entry(' ', color);
        return;
    }
    
    /* Handle tab */
    if (c == '\t') {
        /* Move to next 4-column boundary */
        int spaces = 4 - (cursor_col % 4);
        for (int i = 0; i < spaces && cursor_col < VGA_COLS; i++) {
            VGA[cursor_row * VGA_COLS + cursor_col] = vga_entry(' ', color);
            cursor_col++;
        }
        if (cursor_col >= VGA_COLS) {
            console_newline();
        }
        return;
    }

    VGA[cursor_row * VGA_COLS + cursor_col] = vga_entry(c, color);
    cursor_col++;

    if (cursor_col >= VGA_COLS) {
        console_newline();
    }
}

void console_write(const char* s, uint8_t color) {
    while (*s) {
        console_putc(*s, color);
        s++;
    }
}
