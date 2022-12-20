#ifndef RTOS_USB_H
#define RTOS_USB_H

#include <stdint.h>

// initializes USB process
void rtos_usb_init(uint32_t stack_base, uint32_t stack_size);
// safely detatches USB 
void rtos_usb_quit();

// starts a usb buffer send operation
void rtos_usb_start_send_buffer(uint8_t* buffer, int count);
// waits until the send operation is complete, returns the number of bytes sent
int rtos_usb_wait_until_send_complete();

// starts a usb buffer read operation
void rtos_usb_start_read_buffer(uint8_t* buffer, int count);
// waits until the read operation is complete, returns the number of bytes received
int rtos_usb_wait_until_read_complete();

#endif