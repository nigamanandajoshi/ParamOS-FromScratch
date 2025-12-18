#include "keyboard.h"
#include "ports.h"
#include "console.h"

#define KBD_BUF_SIZE 128

/* ---------- Ring buffer ---------- */
static char kbd_buf[KBD_BUF_SIZE];
static int  kbd_head = 0;
static int  kbd_tail = 0;

static inline void kbd_push(char c) {
    int next = (kbd_head + 1) % KBD_BUF_SIZE;
    if (next != kbd_tail) {
        kbd_buf[kbd_head] = c;
        kbd_head = next;
    }
}

/* ---------- Keyboard state ---------- */
static uint8_t shift_pressed = 0;
static uint8_t ctrl_pressed = 0;
static uint8_t alt_pressed = 0;
static uint8_t capslock_on = 0;

/* US keyboard scancode set 1 - lowercase/unshifted 
 * Each row is 16 scancodes for easier counting */
static char keymap[128] = {
/*       0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F  */
/* 0 */  0,   27,  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=','\b','\t',
/* 1 */ 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']','\n',  0, 'a', 's',
/* 2 */ 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';','\'', '`',  0,'\\', 'z', 'x', 'c', 'v',
/* 3 */ 'b', 'n', 'm', ',', '.', '/',  0, '*',  0, ' ',  0,  0,  0,  0,  0,  0,
/* 4 */  0,   0,   0,   0,   0,   0,   0,   0,   0,   0, '-',  0,   0,   0, '+',  0,
/* 5 */  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
/* 6 */  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
/* 7 */  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};

/* Shifted characters - uppercase/symbols */
static char keymap_shift[128] = {
/*       0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F  */
/* 0 */  0,   27,  '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+','\b','\t',
/* 1 */ 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}','\n',  0, 'A', 'S',
/* 2 */ 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',  0, '|', 'Z', 'X', 'C', 'V',
/* 3 */ 'B', 'N', 'M', '<', '>', '?',  0, '*',  0, ' ',  0,  0,  0,  0,  0,  0,
/* 4 */  0,   0,   0,   0,   0,   0,   0,   0,   0,   0, '-',  0,   0,   0, '+',  0,
/* 5 */  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
/* 6 */  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
/* 7 */  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};

/* Scancode constants */
#define SC_LSHIFT   0x2A
#define SC_RSHIFT   0x36
#define SC_LCTRL    0x1D
#define SC_LALT     0x38
#define SC_CAPSLOCK 0x3A

/* ---------- IRQ1 handler ---------- */
void keyboard_handler(uint8_t scancode) {
    /* Key release (high bit set) */
    if (scancode & 0x80) {
        uint8_t key = scancode & 0x7F;
        if (key == SC_LSHIFT || key == SC_RSHIFT)
            shift_pressed = 0;
        else if (key == SC_LCTRL)
            ctrl_pressed = 0;
        else if (key == SC_LALT)
            alt_pressed = 0;
        return;
    }

    /* Shift pressed */
    if (scancode == SC_LSHIFT || scancode == SC_RSHIFT) {
        shift_pressed = 1;
        return;
    }
    
    /* Ctrl pressed */
    if (scancode == SC_LCTRL) {
        ctrl_pressed = 1;
        return;
    }
    
    /* Alt pressed */
    if (scancode == SC_LALT) {
        alt_pressed = 1;
        return;
    }
    
    /* Caps Lock toggle */
    if (scancode == SC_CAPSLOCK) {
        capslock_on = !capslock_on;
        return;
    }

    /* Get character from appropriate keymap */
    char c = shift_pressed ? keymap_shift[scancode] : keymap[scancode];
    
    /* Handle Caps Lock for letters */
    if (capslock_on && c) {
        if (c >= 'a' && c <= 'z')
            c = c - 32;  /* to uppercase */
        else if (c >= 'A' && c <= 'Z')
            c = c + 32;  /* to lowercase (shift+caps = lowercase) */
    }
    
    /* Handle Ctrl combinations */
    if (ctrl_pressed && c) {
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
            char base = (c >= 'a') ? c - 'a' : c - 'A';
            c = base + 1;  /* Ctrl+A=1, Ctrl+B=2, etc. */
            kbd_push(c);
            console_putc('^', 0x0F);
            console_putc(base + 'A', 0x0F);
            return;
        }
    }

    /* Normal character - echo and buffer */
    if (c) {
        kbd_push(c);
        console_putc(c, 0x0F);
    }
}

/* ---------- Initialization ---------- */
void keyboard_init(void) {
    uint8_t mask = inb(0x21);
    mask &= ~(1 << 1);
    outb(0x21, mask);
}

/* ---------- Buffered input API ---------- */
int keyboard_available(void) {
    return kbd_head != kbd_tail;
}

char keyboard_getchar_nonblock(void) {
    if (!keyboard_available())
        return 0;
    char c = kbd_buf[kbd_tail];
    kbd_tail = (kbd_tail + 1) % KBD_BUF_SIZE;
    return c;
}

char keyboard_getchar(void) {
    while (!keyboard_available()) {
        __asm__ __volatile__("hlt");
    }
    return keyboard_getchar_nonblock();
}
