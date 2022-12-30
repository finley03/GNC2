#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h>
#include "global_variables.h"

typedef struct {
    uint32_t GlobalHash;
    uint32_t StartupCount;
} __EEPROM_Header;

extern Global_Variables globals;

#define __EEPROM_GLOBAL_VARIABLES_START_ADDRESS 32

// returns true when global map has changed
bool init_globals();

bool get_global_address_size(Global_Variable_IDs id, void** address, int* size);
bool set_global(Global_Variable_IDs id, void* value);
bool get_global(Global_Variable_IDs id, void* value);
bool load_global(Global_Variable_IDs id);
bool save_global(Global_Variable_IDs id);

#endif