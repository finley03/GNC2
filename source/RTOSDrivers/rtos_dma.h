#ifndef RTOS_DMA_H
#define RTOS_DMA_H

#include <stdint.h>

void rtos_dma_wait_until_end(int channel);
void rtos_dma_wait_until_end_or_timeout(int channel, uint32_t timeout_ticks);

#endif