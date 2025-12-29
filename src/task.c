// task.c - Task management implementation

#include "task.h"
#include "kmalloc.h"
#include "console.h"

// Task table - all tasks in the system
static task_t tasks[MAX_TASKS];

// Currently running task
static task_t* current_task = 0;

// Ready queue head (simple linked list)
static task_t* ready_queue = 0;

// Next available PID
static uint32_t next_pid = 1;

// Idle task (runs when nothing else can)
static task_t* idle_task = 0;

// ========== Internal Helpers ==========

static task_t* alloc_task_slot(void) {
    for (int i = 0; i < MAX_TASKS; i++) {
        if (tasks[i].state == TASK_STATE_UNUSED) {
            return &tasks[i];
        }
    }
    return 0;  // No free slots
}

static void add_to_ready_queue(task_t* task) {
    task->next = 0;
    task->state = TASK_STATE_READY;

    if (!ready_queue) {
        ready_queue = task;
        return;
    }

    // Add to end of queue
    task_t* t = ready_queue;
    while (t->next) {
        t = t->next;
    }
    t->next = task;
}

static task_t* pop_ready_queue(void) {
    if (!ready_queue) return 0;

    task_t* task = ready_queue;
    ready_queue = ready_queue->next;
    task->next = 0;
    return task;
}

// ========== Idle Task ==========

static void idle_task_func(void) {
    while (1) {
        __asm__ __volatile__("hlt");
    }
}

// ========== Public API ==========

void tasks_init(void) {
    // Clear all task slots
    for (int i = 0; i < MAX_TASKS; i++) {
        tasks[i].state = TASK_STATE_UNUSED;
        tasks[i].pid = 0;
        tasks[i].kernel_stack = 0;
    }

    ready_queue = 0;
    current_task = 0;
    next_pid = 1;

    // Create idle task (always runs when nothing else can)
    idle_task = alloc_task_slot();
    if (idle_task) {
        idle_task->pid = 0;  // PID 0 = idle
        idle_task->state = TASK_STATE_READY;
        
        // Copy name
        const char* name = "idle";
        for (int i = 0; i < 31 && name[i]; i++) {
            idle_task->name[i] = name[i];
            idle_task->name[i+1] = 0;
        }

        // Allocate stack
        idle_task->kernel_stack = (uint32_t*)kmalloc(TASK_STACK_SIZE);
        if (idle_task->kernel_stack) {
            idle_task->kernel_stack_top = (uint32_t)idle_task->kernel_stack + TASK_STACK_SIZE;
            
            // Set up initial stack frame for idle task
            uint32_t* sp = (uint32_t*)idle_task->kernel_stack_top;

            // Push fake return frame
            *(--sp) = 0x202;                    // EFLAGS (interrupts enabled)
            *(--sp) = 0x08;                     // CS
            *(--sp) = (uint32_t)idle_task_func; // EIP - entry point

            // Push registers (as if pusha was called)
            *(--sp) = 0;  // EAX
            *(--sp) = 0;  // ECX
            *(--sp) = 0;  // EDX
            *(--sp) = 0;  // EBX
            *(--sp) = 0;  // ESP (ignored by popa)
            *(--sp) = 0;  // EBP
            *(--sp) = 0;  // ESI
            *(--sp) = 0;  // EDI

            idle_task->esp = (uint32_t)sp;
        }
    }

    console_write("Task subsystem initialized.\n", 0x0A);
}

int task_create(void (*entry_point)(void), const char* name) {
    task_t* task = alloc_task_slot();
    if (!task) {
        console_write("task_create: no free slots\n", 0x0C);
        return -1;
    }

    // Assign PID
    task->pid = next_pid++;

    // Copy name
    for (int i = 0; i < 31 && name[i]; i++) {
        task->name[i] = name[i];
        task->name[i+1] = 0;
    }

    // Allocate kernel stack
    task->kernel_stack = (uint32_t*)kmalloc(TASK_STACK_SIZE);
    if (!task->kernel_stack) {
        console_write("task_create: out of memory\n", 0x0C);
        task->state = TASK_STATE_UNUSED;
        return -1;
    }

    task->kernel_stack_top = (uint32_t)task->kernel_stack + TASK_STACK_SIZE;

    // Set up initial stack frame
    // When this task is first scheduled, context_switch will pop these values
    uint32_t* sp = (uint32_t*)task->kernel_stack_top;

    // Push fake iret frame (for initial task startup)
    *(--sp) = 0x202;                // EFLAGS (IF=1, interrupts enabled)
    *(--sp) = 0x08;                 // CS (kernel code segment)
    *(--sp) = (uint32_t)entry_point; // EIP - where to start executing

    // Push registers (as if pusha was called)
    *(--sp) = 0;  // EAX
    *(--sp) = 0;  // ECX
    *(--sp) = 0;  // EDX
    *(--sp) = 0;  // EBX
    *(--sp) = 0;  // ESP (ignored by popa)
    *(--sp) = 0;  // EBP
    *(--sp) = 0;  // ESI
    *(--sp) = 0;  // EDI

    task->esp = (uint32_t)sp;
    task->ebp = 0;
    task->eip = (uint32_t)entry_point;
    task->time_slice = 10;  // 10 ticks = 100ms at 100Hz
    task->priority = 1;
    task->total_ticks = 0;

    // Add to ready queue
    add_to_ready_queue(task);

    console_write("Created task: ", 0x0F);
    console_write(name, 0x0E);
    console_write(" (PID ", 0x0F);
    console_putc('0' + (task->pid % 10), 0x0F);
    console_write(")\n", 0x0F);

    return task->pid;
}

void task_exit(void) {
    if (!current_task || current_task == idle_task) {
        return;  // Can't exit idle task or no task
    }

    current_task->state = TASK_STATE_DEAD;

    console_write("Task exited: ", 0x0C);
    console_write(current_task->name, 0x0E);
    console_putc('\n', 0x0C);

    // Switch to another task
    schedule();

    // Should never reach here
    while (1) {
        __asm__ __volatile__("hlt");
    }
}

void task_yield(void) {
    if (current_task && current_task->state == TASK_STATE_RUNNING) {
        add_to_ready_queue(current_task);
    }
    schedule();
}

task_t* task_current(void) {
    return current_task;
}

task_t* task_get(uint32_t pid) {
    for (int i = 0; i < MAX_TASKS; i++) {
        if (tasks[i].state != TASK_STATE_UNUSED && tasks[i].pid == pid) {
            return &tasks[i];
        }
    }
    return 0;
}

// ========== Scheduler ==========

void scheduler_tick(void) {
    if (!current_task) return;

    current_task->total_ticks++;

    // Decrement time slice
    if (current_task->time_slice > 0) {
        current_task->time_slice--;
    }

    // Time slice expired - preempt
    if (current_task->time_slice == 0) {
        current_task->time_slice = 10;  // Reset slice
        if (current_task != idle_task) {
            add_to_ready_queue(current_task);
        }
        schedule();
    }
}

void schedule(void) {
    task_t* next = pop_ready_queue();

    // If nothing ready, run idle task
    if (!next) {
        next = idle_task;
    }

    // Same task? No switch needed
    if (next == current_task) {
        return;
    }

    task_t* prev = current_task;
    current_task = next;
    current_task->state = TASK_STATE_RUNNING;

    // Perform context switch
    if (prev) {
        context_switch(&prev->esp, next->esp);
    } else {
        // First task ever - just jump to it
        context_switch(0, next->esp);
    }
}
