#include "keyboard.h"
#include "ports.h"
#include "console.h"

// Basic US keyboard scancode set 1
static char keymap[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t', 'q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,   // Left Control
    'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,   // Left Shift
    '\\','z','x','c','v','b','n','m',',','.','/',
    0,   // Right Shift
    '*',
    0,   // Alt
    ' ', // Spacebar
    // The rest are function keys or unused for now
};

void keyboard_handler(uint8_t scancode) {
    // Ignore key releases (high bit set)
    if (scancode & 0x80)
        return;

    char c = keymap[scancode];

    if (c)
        console_putc(c, 0x0F);
}

void keyboard_init() {
    // Enable IRQ1 on the PIC
    // PIC master mask port: 0x21
    uint8_t mask = inb(0x21);
    mask &= ~(1 << 1);   // enable line 1 (IRQ1)
    outb(0x21, mask);
}
