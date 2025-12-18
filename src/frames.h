#ifndef FRAMES_H
#define FRAMES_H

#include "stdint.h"

#define FRAME_SIZE 4096   // 4 KB

void frames_init(uint32_t mem_size);
int  frame_alloc(void);
void frame_free(int frame);

#endif
