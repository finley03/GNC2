#include "globals.h"
#include "global_eeprom.h"
#include <string.h>
#include "util.h"

__EEPROM_Header __eeprom_header;
Global_Variables globals;
const char* global_variable_names[] = __GLOBAL_VARIABLE_NAMES;

bool get_address_from_id(__Global_Variable_Types type, Global_Variable_IDs id, void** address, int* size) {
    int index;

    switch (type) {
        #ifdef __GLOBAL_I64_START_VARIABLE
        case __I64:
        if (id < __GLOBAL_I64_ID_START || id >= __GLOBAL_I64_ID_END) return false;
        *size = 8;
        index = id - __GLOBAL_I64_ID_START;
        *address = &(globals.__GLOBAL_I64_START_VARIABLE) + index;
        break;
        #endif

        #ifdef __GLOBAL_U64_START_VARIABLE
        case __U64:
        if (id < __GLOBAL_U64_ID_START || id >= __GLOBAL_U64_ID_END) return false;
        *size = 8;
        index = id - __GLOBAL_U64_ID_START;
        *address = &(globals.__GLOBAL_U64_START_VARIABLE) + index;
        break;
        #endif

        #ifdef __GLOBAL_F64_START_VARIABLE
        case __F64:
        if (id < __GLOBAL_F64_ID_START || id >= __GLOBAL_F64_ID_END) return false;
        *size = 8;
        index = id - __GLOBAL_F64_ID_START;
        *address = &(globals.__GLOBAL_F64_START_VARIABLE) + index;
        break;
        #endif

        #ifdef __GLOBAL_I32_START_VARIABLE
        case __I32:
        if (id < __GLOBAL_I32_ID_START || id >= __GLOBAL_I32_ID_END) return false;
        *size = 4;
        index = id - __GLOBAL_I32_ID_START;
        *address = &(globals.__GLOBAL_I32_START_VARIABLE) + index;
        break;
        #endif

        #ifdef __GLOBAL_U32_START_VARIABLE
        case __U32:
        if (id < __GLOBAL_U32_ID_START || id >= __GLOBAL_U32_ID_END) return false;
        *size = 4;
        index = id - __GLOBAL_U32_ID_START;
        *address = &(globals.__GLOBAL_U32_START_VARIABLE) + index;
        break;
        #endif

        #ifdef __GLOBAL_FP32_START_VARIABLE
        case __FP32:
        if (id < __GLOBAL_FP32_ID_START || id >= __GLOBAL_FP32_ID_END) return false;
        *size = 4;
        index = id - __GLOBAL_FP32_ID_START;
        *address = &(globals.__GLOBAL_FP32_START_VARIABLE) + index;
        break;
        #endif

        #ifdef __GLOBAL_F32_START_VARIABLE
        case __F32:
        if (id < __GLOBAL_F32_ID_START || id >= __GLOBAL_F32_ID_END) return false;
        *size = 4;
        index = id - __GLOBAL_F32_ID_START;
        *address = &(globals.__GLOBAL_F32_START_VARIABLE) + index;
        break;
        #endif

        #ifdef __GLOBAL_I16_START_VARIABLE
        case __I16:
        if (id < __GLOBAL_I16_ID_START || id >= __GLOBAL_I16_ID_END) return false;
        *size = 2;
        index = id - __GLOBAL_I16_ID_START;
        *address = &(globals.__GLOBAL_I16_START_VARIABLE) + index;
        break;
        #endif

        #ifdef __GLOBAL_U16_START_VARIABLE
        case __U16:
        if (id < __GLOBAL_U16_ID_START || id >= __GLOBAL_U16_ID_END) return false;
        *size = 2;
        index = id - __GLOBAL_U16_ID_START;
        *address = &(globals.__GLOBAL_U16_START_VARIABLE) + index;
        break;
        #endif

        #ifdef __GLOBAL_I8_START_VARIABLE
        case __I8:
        if (id < __GLOBAL_I8_ID_START || id >= __GLOBAL_I8_ID_END) return false;
        *size = 1;
        index = id - __GLOBAL_I8_ID_START;
        *address = &(globals.__GLOBAL_I8_START_VARIABLE) + index;
        break;
        #endif

        #ifdef __GLOBAL_U8_START_VARIABLE
        case __U8:
        if (id < __GLOBAL_U8_ID_START || id >= __GLOBAL_U8_ID_END) return false;
        *size = 1;
        index = id - __GLOBAL_U8_ID_START;
        *address = &(globals.__GLOBAL_U8_START_VARIABLE) + index;
        break;
        #endif

        #ifdef __GLOBAL_BOOL_START_VARIABLE
        case __BOOL:
        if (id < __GLOBAL_BOOL_ID_START || id >= __GLOBAL_BOOL_ID_END) return false;
        *size = 1;
        index = id - __GLOBAL_BOOL_ID_START;
        *address = &(globals.__GLOBAL_BOOL_START_VARIABLE) + index;
        break;
        #endif

        // #ifdef __GLOBAL_BOOL_PACKED_START_VARIABLE
        // case __BOOL_PACKED:
        // if (id < __GLOBAL_BOOL_PACKED_ID_START || id >= __GLOBAL_BOOL_PACKED_ID_END) return false;
        // *size = (id - __GLOBAL_BOOL_PACKED_ID_START) % 8; // HERE SIZE STORES THE BIT POSITION
        // index = id - __GLOBAL_BOOL_PACKED_ID_START;
        // address = ((uint8_t*)&(globals.__GLOBAL_BOOL_PACKED_START_VARIABLE)) + index / 8;
        // break;
        // #endif

        default:
        return false;
        break;
    }
}

bool get_global_type_from_id(Global_Variable_IDs id, __Global_Variable_Types* type) {
    if (id - __GLOBAL_ID_START < 0 || id >= __GLOBAL_ID_END) return false;
    if (id - __GLOBAL_I64_ID_END < 0) {*type = __I64; return true;}
    if (id - __GLOBAL_U64_ID_END < 0) {*type = __U64; return true;}
    if (id - __GLOBAL_F64_ID_END < 0) {*type = __F64; return true;}
    if (id - __GLOBAL_I32_ID_END < 0) {*type = __I32; return true;}
    if (id - __GLOBAL_U32_ID_END < 0) {*type = __U32; return true;}
    if (id - __GLOBAL_FP32_ID_END < 0) {*type = __FP32; return true;}
    if (id - __GLOBAL_F32_ID_END < 0) {*type = __F32; return true;}
    if (id - __GLOBAL_I16_ID_END < 0) {*type = __I16; return true;}
    if (id - __GLOBAL_U16_ID_END < 0) {*type = __U16; return true;}
    if (id - __GLOBAL_I8_ID_END < 0) {*type = __I8; return true;}
    if (id - __GLOBAL_U8_ID_END < 0) {*type = __U8; return true;}
    if (id - __GLOBAL_BOOL_ID_END < 0) {*type = __BOOL; return true;}
}

bool get_global_address_size(Global_Variable_IDs id, void** address, int* size) {
    __Global_Variable_Types type;
    if (!get_global_type_from_id(id, &type)) return false;
    if (!get_address_from_id(type, id, address, size)) return false;
}

bool set_global(Global_Variable_IDs id, void* value) {
    __Global_Variable_Types type;
    void* address;
    int size;
    if (!get_global_type_from_id(id, &type)) return false;
    if (!get_address_from_id(type, id, &address, &size)) return false;
    memcpy(address, value, size);
    return true;
}

bool get_global(Global_Variable_IDs id, void* value) {
    __Global_Variable_Types type;
    void* address;
    int size;
    if (!get_global_type_from_id(id, &type)) return false;
    if (!get_address_from_id(type, id, &address, &size)) return false;
    memcpy(value, address, size);
    return true;
}

bool load_global(Global_Variable_IDs id) {
    __Global_Variable_Types type;
    void* address;
    int size;
    if (!get_global_type_from_id(id, &type)) return false;
    if (!get_address_from_id(type, id, &address, &size)) return false;
    volatile uint32_t eeprom_address = __EEPROM_GLOBAL_VARIABLES_START_ADDRESS + (uint32_t)address - (uint32_t)(&globals);
    EEPROM_Request request;
    eeprom_read_request(&request, eeprom_address, address, size);
    eeprom_wait_until_done(&request);
    return true;
}

bool save_global(Global_Variable_IDs id) {
    __Global_Variable_Types type;
    void* address;
    int size;
    if (!get_global_type_from_id(id, &type)) return false;
    if (!get_address_from_id(type, id, &address, &size)) return false;
    volatile uint32_t eeprom_address = __EEPROM_GLOBAL_VARIABLES_START_ADDRESS + (uint32_t)address - (uint32_t)(&globals);
    EEPROM_Request request;
    eeprom_write_request(&request, eeprom_address, address, size);
    eeprom_wait_until_done(&request);
    return true;
}

bool get_global_name(Global_Variable_IDs id, uint8_t* buffer, int* length, int maxLength) {
    bool ret = true;
    if (id >= __GLOBAL_ID_END) {
        id = 0;
        ret = false;
    }

    *length = MIN_2(maxLength, strlen(global_variable_names[id]) + 1);

    memcpy(buffer, global_variable_names[id], *length);

    return ret;
}

bool init_globals() {
    // load header
    EEPROM_Request request;
    eeprom_read_request(&request, 0, &__eeprom_header, sizeof(__eeprom_header));
    eeprom_wait_until_done(&request);
    
    // check and modify header values
    if (__eeprom_header.GlobalHash != __GLOBAL_HASH) {
        __eeprom_header.GlobalHash = __GLOBAL_HASH;
        eeprom_write_request(&request, 0, &(__eeprom_header.GlobalHash), 4);
        eeprom_wait_until_done(&request);
    }
    
    ++__eeprom_header.StartupCount;
    eeprom_write_request(&request, 4, &(__eeprom_header.StartupCount), 4);
    eeprom_wait_until_done(&request);

    // load globals
    eeprom_read_request(&request, __EEPROM_GLOBAL_VARIABLES_START_ADDRESS, &globals, sizeof(globals));
    eeprom_wait_until_done(&request);

    globals.GlobalHash = __eeprom_header.GlobalHash;
    globals.StartupCount = __eeprom_header.StartupCount;

    return true;
}