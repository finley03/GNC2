#include "util.h"

// Device Descriptors

FLASH_Descriptor flash_desc;
Barometer_Descriptor baro_desc;

// Processes

BusManager_Process spiproc;

// other values

const uint8_t zero = 0;
uint8_t datasink;