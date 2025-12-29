// shell.c - Simple command shell for ParamOS

#include "shell.h"
#include "console.h"
#include "keyboard.h"
#include "kmalloc.h"
#include "task.h"
#include "stdint.h"

// ========== String Helpers ==========

static int str_len(const char* s) {
    int len = 0;
    while (s[len]) len++;
    return len;
}

static int str_equal(const char* a, const char* b) {
    while (*a && *b) {
        if (*a != *b) return 0;
        a++;
        b++;
    }
    return *a == *b;
}

static int str_starts_with(const char* str, const char* prefix) {
    while (*prefix) {
        if (*str != *prefix) return 0;
        str++;
        prefix++;
    }
    return 1;
}

// ========== Built-in Commands ==========

static void cmd_help(void) {
    console_write("\nAvailable commands:\n", 0x0F);
    console_write("  help   - Show this help\n", 0x07);
    console_write("  clear  - Clear the screen\n", 0x07);
    console_write("  echo   - Echo text back\n", 0x07);
    console_write("  mem    - Show memory info\n", 0x07);
    console_write("  ps     - List running tasks\n", 0x07);
    console_write("  ver    - Show version\n", 0x07);
    console_write("\n", 0x07);
}

static void cmd_clear(void) {
    console_clear(0x07);
}

static void cmd_echo(char* args) {
    // Skip "echo " prefix
    if (str_len(args) > 5) {
        console_write(args + 5, 0x0F);
    }
    console_putc('\n', 0x0F);
}

static void cmd_mem(void) {
    uint32_t free = kmalloc_free_space();
    
    console_write("\nMemory Info:\n", 0x0E);
    console_write("  Heap free: ", 0x07);
    
    // Simple number printing (up to 999999)
    char buf[12];
    int i = 0;
    uint32_t n = free;
    if (n == 0) {
        buf[i++] = '0';
    } else {
        char tmp[12];
        int j = 0;
        while (n > 0) {
            tmp[j++] = '0' + (n % 10);
            n /= 10;
        }
        while (j > 0) {
            buf[i++] = tmp[--j];
        }
    }
    buf[i] = 0;
    
    console_write(buf, 0x0A);
    console_write(" bytes\n\n", 0x07);
}

static void cmd_ps(void) {
    console_write("\nRunning Tasks:\n", 0x0E);
    console_write("  PID  State    Name\n", 0x07);
    console_write("  ---  -------  ----\n", 0x07);
    
    // Get current task
    task_t* cur = task_current();
    if (cur) {
        console_write("  ", 0x07);
        console_putc('0' + (cur->pid % 10), 0x0A);
        console_write("    RUNNING  ", 0x0A);
        console_write(cur->name, 0x0F);
        console_write(" (current)\n", 0x07);
    }
    
    console_write("\n", 0x07);
}

static void cmd_ver(void) {
    console_write("\nParamOS v0.1\n", 0x0E);
    console_write("A minimal 32-bit operating system\n", 0x07);
    console_write("Built by Nigam <3\n\n", 0x0D);
}

// ========== Command Parser ==========

void shell_execute(char* line) {
    // Skip empty lines
    if (line[0] == 0) return;

    // Match commands
    if (str_equal(line, "help") || str_equal(line, "?")) {
        cmd_help();
    }
    else if (str_equal(line, "clear") || str_equal(line, "cls")) {
        cmd_clear();
    }
    else if (str_starts_with(line, "echo ") || str_equal(line, "echo")) {
        cmd_echo(line);
    }
    else if (str_equal(line, "mem") || str_equal(line, "memory")) {
        cmd_mem();
    }
    else if (str_equal(line, "ps") || str_equal(line, "tasks")) {
        cmd_ps();
    }
    else if (str_equal(line, "ver") || str_equal(line, "version")) {
        cmd_ver();
    }
    else {
        console_write("Unknown command: ", 0x0C);
        console_write(line, 0x0C);
        console_write("\nType 'help' for available commands.\n", 0x07);
    }
}

// ========== Shell Main Loop ==========

void shell_run(void) {
    char line[SHELL_MAX_LINE];
    int pos = 0;

    console_write("\nWelcome to ParamOS Shell!\n", 0x0E);
    console_write("Type 'help' for available commands.\n\n", 0x07);

    while (1) {
        // Print prompt
        console_write("ParamOS> ", 0x0A);
        
        // Clear line buffer for new input
        for (int i = 0; i < SHELL_MAX_LINE; i++) {
            line[i] = 0;
        }
        pos = 0;

        // Read line character by character
        while (1) {
            char c = keyboard_getchar();  // Blocking read

            if (c == '\n') {
                // Enter pressed - execute command
                // Note: keyboard already echoed newline, but we need to be sure
                line[pos] = 0;
                shell_execute(line);
                break;
            }
            else if (c == '\b') {
                // Backspace - keyboard already handled display
                // Just update our buffer position
                if (pos > 0) {
                    pos--;
                    line[pos] = 0;  // Clear the character in buffer
                }
            }
            else if (c >= 32 && c < 127 && pos < SHELL_MAX_LINE - 1) {
                // Printable character - keyboard already echoed
                line[pos++] = c;
            }
        }
    }
}

