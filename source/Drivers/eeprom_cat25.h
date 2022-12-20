#ifndef EEPROM_CAT25_H
#define EEPROM_CAT25_H

#include <stdbool.h>
#include <sam.h>
#include "port.h"

#define EEPROM_WREN 0b00000110
#define EEPROM_WRDI 0b00000100
#define EEPROM_RDSR 0b00000101
#define EEPROM_WRSR 0b00000001
#define EEPROM_READ 0b00000011
#define EEPROM_WRITE 0b00000010

#define EEPROM_PAGE_SIZE 64

typedef struct __attribute__((packed)) {
	sercom_registers_t* sercom; // serial communication port
	Pin sspin; // SS pin
	uint16_t size; // size of eeprom
} EEPROM_Descriptor;

void eeprom_init(EEPROM_Descriptor* desc, sercom_registers_t* sercom, Pin sspin, uint16_t size);
void eeprom_write_enable(EEPROM_Descriptor* desc);
void eeprom_write_disable(EEPROM_Descriptor* desc);
uint8_t eeprom_read_byte(EEPROM_Descriptor* desc, uint32_t address);
void eeprom_write_byte(EEPROM_Descriptor* desc, uint32_t address, uint8_t data);
void eeprom_read(EEPROM_Descriptor* desc, uint32_t address, int count, void* writeback);
void eeprom_write(EEPROM_Descriptor* desc, uint32_t address, int count, void* data);
bool eeprom_check(EEPROM_Descriptor* desc);

#endif