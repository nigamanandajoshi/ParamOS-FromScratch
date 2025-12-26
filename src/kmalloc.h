#ifndef KMALLOC_H
#define KMALLOC_H

#include "stdint.h"

// Initialize the kernel heap allocator
void kmalloc_init(uint32_t heap_start, uint32_t heap_size);

// Allocate size bytes from the heap (4-byte aligned)
void* kmalloc(uint32_t size);

// Get remaining free heap space
uint32_t kmalloc_free_space(void);

#endif
