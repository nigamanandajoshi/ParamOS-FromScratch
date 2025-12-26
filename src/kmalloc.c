// kmalloc.c - Simple bump allocator for ParamOS kernel heap

#include "kmalloc.h"
#include "stdint.h"

// Heap state
static uint32_t heap_base  = 0;
static uint32_t heap_end   = 0;
static uint32_t heap_ptr   = 0;

// Initialize the kernel heap
// heap_start: starting address of usable heap memory
// heap_size:  size of the heap in bytes
void kmalloc_init(uint32_t heap_start, uint32_t heap_size) {
    heap_base = heap_start;
    heap_end  = heap_start + heap_size;
    heap_ptr  = heap_start;
}

// Allocate 'size' bytes from the heap
// Returns pointer to allocated memory, or 0 (NULL) if out of memory
// Memory is aligned to 4 bytes for efficient access
void* kmalloc(uint32_t size) {
    // Align size to 4 bytes (word alignment)
    if (size & 0x3) {
        size = (size & ~0x3) + 4;
    }

    // Check if we have enough space
    if (heap_ptr + size > heap_end) {
        return (void*)0;  // Out of memory
    }

    // Bump allocate
    void* ptr = (void*)heap_ptr;
    heap_ptr += size;

    return ptr;
}

// Optional: Get remaining heap space
uint32_t kmalloc_free_space(void) {
    return heap_end - heap_ptr;
}
