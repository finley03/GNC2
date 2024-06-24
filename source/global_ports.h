#ifndef GLOBAL_PORTS_H
#define GLOBAL_PORTS_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    PORT0,
    PORT1,
    PORT2,
    PORT3,
    PORT4,
    __PORT_ID_END
} Serial_Port_IDs;

bool serial_init();
bool serial_quit();

bool serial_write_start(Serial_Port_IDs port, uint8_t* buffer, int count);
bool serial_write_wait_until_complete(Serial_Port_IDs port);

bool serial_read_start(Serial_Port_IDs port, uint8_t* buffer, int count);
bool serial_read_wait_until_complete(Serial_Port_IDs port);
bool serial_read_wait_until_complete_or_timeout(Serial_Port_IDs port, uint32_t timeout_ms);

bool serial_flush(Serial_Port_IDs port);

#endif