#include "frames.h"

#define MAX_FRAMES 32768   // supports up to 128 MB RAM
static uint32_t frame_bitmap[MAX_FRAMES / 32];
static uint32_t total_frames;

/* Bitmap helpers */
static inline void set_frame(uint32_t f) {
    frame_bitmap[f / 32] |= (1 << (f % 32));
}

static inline void clear_frame(uint32_t f) {
    frame_bitmap[f / 32] &= ~(1 << (f % 32));
}

static inline int test_frame(uint32_t f) {
    return frame_bitmap[f / 32] & (1 << (f % 32));
}

/* Initialize physical memory manager */
void frames_init(uint32_t mem_size) {
    total_frames = mem_size / FRAME_SIZE;

    // Clear bitmap
    for (uint32_t i = 0; i < total_frames / 32; i++)
        frame_bitmap[i] = 0;

    /*
     * Reserve first 1 MB:
     *  - BIOS
     *  - bootloader
     *  - kernel
     */
    for (uint32_t f = 0; f < 256; f++)
        set_frame(f);
}

/* Allocate a free frame */
int frame_alloc(void) {
    for (uint32_t i = 0; i < total_frames / 32; i++) {
        if (frame_bitmap[i] != 0xFFFFFFFF) {
            for (uint32_t j = 0; j < 32; j++) {
                uint32_t frame = i * 32 + j;
                if (frame < total_frames && !test_frame(frame)) {
                    set_frame(frame);
                    return frame;
                }
            }
        }
    }
    return -1; // out of memory
}

/* Free a frame */
void frame_free(int frame) {
    if (frame >= 0 && (uint32_t)frame < total_frames)
        clear_frame(frame);
}
