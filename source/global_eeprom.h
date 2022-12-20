#ifndef GLOBAL_EEPROM_H
#define GLOBAL_EEPROM_H

#include "RTOSDrivers/dma_eeprom_cat25.h"

bool init_eeprom();
bool check_eeprom();
void eeprom_read_request(EEPROM_Request* request, uint32_t address, void* buffer, int size);
void eeprom_write_request(EEPROM_Request* request, uint32_t address, void* buffer, int size);

#endif