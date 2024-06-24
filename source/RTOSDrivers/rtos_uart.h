#ifndef RTOS_UART_H
#define RTOS_UART_H

#include <stdint.h>
#include <stdbool.h>
#include "Drivers/dma.h"

// starts a uart buffer send operation
// returns false if error is encountered
bool rtos_uart_start_send_buffer(sercom_registers_t* sercom, DMA_Descriptor* dmadesc, uint8_t* buffer, int count);
// waits until the send operation is complete, returns the number of bytes sent
void rtos_uart_wait_until_send_complete(DMA_Descriptor* dmadesc);

// starts a uart buffer read operation
// returns false if error is encountered
bool rtos_uart_start_read_buffer(sercom_registers_t* sercom, DMA_Descriptor* dmadesc, uint8_t* buffer, int count);
// waits until the read operation is complete, returns the number of bytes received
void rtos_uart_wait_until_read_complete(DMA_Descriptor* dmadesc);
// waits until the read operation is complete or times out, returns the number of bytes received
void rtos_uart_wait_until_read_complete_or_timeout(DMA_Descriptor* dmadesc, uint32_t timeout_ticks);

#endif