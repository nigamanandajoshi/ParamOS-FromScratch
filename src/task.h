// task.h - Process Control Block (PCB) and task management
#ifndef TASK_H
#define TASK_H

#include "stdint.h"

// Maximum number of concurrent tasks
#define MAX_TASKS 64

// Kernel stack size per task (4KB)
#define TASK_STACK_SIZE 4096

// Task states
typedef enum {
    TASK_STATE_UNUSED = 0,  // Slot is free
    TASK_STATE_READY,       // Ready to run
    TASK_STATE_RUNNING,     // Currently executing
    TASK_STATE_BLOCKED,     // Waiting for I/O or event
    TASK_STATE_DEAD         // Terminated, awaiting cleanup
} task_state_t;

// CPU register context (saved during context switch)
typedef struct {
    // Pushed by pusha (in reverse order)
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp_dummy;  // Ignored by popa
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    // Segment registers (optional, for future use)
    uint32_t ds;
    uint32_t es;
    uint32_t fs;
    uint32_t gs;

    // Pushed by CPU on interrupt
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
} __attribute__((packed)) cpu_context_t;

// Process Control Block (PCB)
typedef struct task {
    // Task identification
    uint32_t pid;               // Process ID
    char name[32];              // Task name for debugging

    // Execution state
    task_state_t state;         // Current state
    uint32_t esp;               // Stack pointer (saved on switch)
    uint32_t ebp;               // Base pointer
    uint32_t eip;               // Instruction pointer (entry point)

    // Stack management
    uint32_t* kernel_stack;     // Base of allocated kernel stack
    uint32_t kernel_stack_top;  // Top of stack (ESP starts here)

    // Scheduling
    uint32_t time_slice;        // Ticks remaining in current slice
    uint32_t priority;          // Task priority (future use)
    uint32_t total_ticks;       // Total ticks this task has run

    // Linked list for scheduler
    struct task* next;          // Next task in ready queue

} task_t;

// ========== Task Management API ==========

// Initialize the task subsystem
void tasks_init(void);

// Create a new task
// Returns PID on success, -1 on failure
int task_create(void (*entry_point)(void), const char* name);

// Exit the current task
void task_exit(void);

// Yield CPU to another task voluntarily
void task_yield(void);

// Get current running task
task_t* task_current(void);

// Get task by PID
task_t* task_get(uint32_t pid);

// ========== Scheduler API ==========

// Called from timer interrupt - may switch tasks
void scheduler_tick(void);

// Pick next task and switch to it
void schedule(void);

// Context switch (implemented in switch.asm)
extern void context_switch(uint32_t* old_esp, uint32_t new_esp);

#endif
