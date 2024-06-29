#ifndef UART_H
#define UART_C

#include <stdbool.h>
#include <sam.h>

// the given SERCOM is validated
bool uart_init(sercom_registers_t* sercom, uint8_t rxpo, uint8_t txpo, uint32_t baud);
bool uart_set_baud(sercom_registers_t* sercom, uint32_t baud);
bool uart_set_interrupt(sercom_registers_t* sercom, void (*callback)(void));
// for these the SERCOM is not validated
void uart_flush(sercom_registers_t* sercom);
void uart_send_buffer(sercom_registers_t* sercom, uint8_t* buffer, int count);

// returns number of bytes read
// wait_microseconds is the number of microseconds permitted between two bytes
int uart_read_buffer(sercom_registers_t* sercom, uint8_t* buffer, int count, int wait_microseconds);

#endif