#include "global_eeprom.h"
#include "util.h"

EEPROM_Descriptor eeprom_desc;

bool init_eeprom() {
	port_set(EEPROM_SS_PORT, EEPROM_SS_PIN);
	port_set_output(EEPROM_SS_PORT, EEPROM_SS_PIN);

    Pin eeprom_ss = {.port = EEPROM_SS_PORT, .pin = EEPROM_SS_PIN};
	eeprom_init(&eeprom_desc, SPI_SERCOM, eeprom_ss, SPI_EEPROM_SIZE);

    return true;
}

bool check_eeprom() {
    return eeprom_check(&eeprom_desc);
}


void eeprom_read_request(EEPROM_Request* request, uint32_t address, void* buffer, int size) {
    eeprom_request(&spiproc, request, &eeprom_desc, SPI_DEVICE_EEPROM, EEPROM_Read, address, size, buffer);
}

void eeprom_write_request(EEPROM_Request* request, uint32_t address, void* buffer, int size) {
    eeprom_request(&spiproc, request, &eeprom_desc, SPI_DEVICE_EEPROM, EEPROM_Write, address, size, buffer);
}