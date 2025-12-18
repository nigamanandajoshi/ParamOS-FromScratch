// #ifndef KEYBOARD_H
// #define KEYBOARD_H

// #include "stdint.h"

// void keyboard_init();
// void keyboard_handler(uint8_t scancode);

// #endif
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "stdint.h"

void keyboard_init(void);
void keyboard_handler(uint8_t scancode);

/* NEW: buffered input API */
int  keyboard_available(void);
char keyboard_getchar(void);
char keyboard_getchar_nonblock(void);

#endif
