// shell.h - Simple command shell for ParamOS
#ifndef SHELL_H
#define SHELL_H

#include "stdint.h"

// Maximum command line length
#define SHELL_MAX_LINE 128

// Maximum number of arguments
#define SHELL_MAX_ARGS 8

// Shell entry point (runs as a task)
void shell_run(void);

// Parse and execute a command
void shell_execute(char* line);

#endif
