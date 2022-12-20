#include "eeprom_cat25.h"
#include "port.h"
#include "spi.h"

#define EEPROM_RDY_MASK 0b00000001
#define EEPROM_WEL_MASK 0b00000010

void eeprom_init(EEPROM_Descriptor* desc, sercom_registers_t* sercom, Pin sspin, uint16_t size) {
	desc->sercom = sercom;
	desc->sspin = sspin;
	desc->size = size;
	port_set_output(desc->sspin.port, desc->sspin.pin);
	port_set(desc->sspin.port, desc->sspin.pin);
}

void eeprom_write_enable(EEPROM_Descriptor* desc) {
	port_clear(desc->sspin.port, desc->sspin.pin);
	spi_command(desc->sercom, EEPROM_WREN);
	port_set(desc->sspin.port, desc->sspin.pin);
}

void eeprom_write_disable(EEPROM_Descriptor* desc) {
	port_clear(desc->sspin.port, desc->sspin.pin);
	spi_command(desc->sercom, EEPROM_WRDI);
	port_set(desc->sspin.port, desc->sspin.pin);
}

uint8_t eeprom_read_status(EEPROM_Descriptor* desc) {
	port_clear(desc->sspin.port, desc->sspin.pin);
	spi_command(desc->sercom, EEPROM_RDSR);
	uint8_t out = spi_command(desc->sercom, 0);
	port_set(desc->sspin.port, desc->sspin.pin);
	return out;
}

void eeprom_write_status(EEPROM_Descriptor* desc, uint8_t data) {
	port_clear(desc->sspin.port, desc->sspin.pin);
	spi_command(desc->sercom, EEPROM_WRSR);
	spi_command(desc->sercom, data);
	port_set(desc->sspin.port, desc->sspin.pin);
}

uint8_t eeprom_read_byte(EEPROM_Descriptor* desc, uint32_t address) {
	port_clear(desc->sspin.port, desc->sspin.pin);
	spi_command(desc->sercom, EEPROM_READ);
	spi_command(desc->sercom, (uint8_t)(address >> 8));
	spi_command(desc->sercom, (uint8_t)address);
	uint8_t out = spi_command(desc->sercom, 0);
	port_set(desc->sspin.port, desc->sspin.pin);
	return out;
}

void eeprom_write_byte(EEPROM_Descriptor* desc, uint32_t address, uint8_t data) {
	eeprom_write_enable(desc);
	port_clear(desc->sspin.port, desc->sspin.pin);
	spi_command(desc->sercom, EEPROM_WRITE);
	spi_command(desc->sercom, (uint8_t)(address >> 8));
	spi_command(desc->sercom, (uint8_t)address);
	spi_command(desc->sercom, data);
	port_set(desc->sspin.port, desc->sspin.pin);
	eeprom_write_disable(desc);
	while (eeprom_read_status(desc) & EEPROM_RDY_MASK);
}

void eeprom_read(EEPROM_Descriptor* desc, uint32_t address, int count, void* writeback) {
	port_clear(desc->sspin.port, desc->sspin.pin);
	spi_command(desc->sercom, EEPROM_READ);
	spi_command(desc->sercom, (uint8_t)(address >> 8));
	spi_command(desc->sercom, (uint8_t)address);
	for (int i = 0; i < count; ++i) ((uint8_t*)writeback)[i] = spi_command(desc->sercom, 0);
	port_set(desc->sspin.port, desc->sspin.pin);
}

void eeprom_write(EEPROM_Descriptor* desc, uint32_t address, int count, void* data) {
	uint32_t top = address + count - 1;
	uint32_t nr_pages = (top / EEPROM_PAGE_SIZE) - (address / EEPROM_PAGE_SIZE) + 1;
	uint32_t data_index = 0;
	eeprom_write_enable(desc);
	for (int i = 0; i < nr_pages; ++i) {
		port_clear(desc->sspin.port, desc->sspin.pin);
		spi_command(desc->sercom, EEPROM_WRITE);
		spi_command(desc->sercom, (uint8_t)(address >> 8));
		spi_command(desc->sercom, (uint8_t)address);
		for (int j = address % EEPROM_PAGE_SIZE; address <= top && j < EEPROM_PAGE_SIZE; ++j) {
			spi_command(desc->sercom, ((uint8_t*)data)[data_index++]);
			++address;
		}
		port_set(desc->sspin.port, desc->sspin.pin);
		// wait until write cycle is done
		while (eeprom_read_status(desc) & EEPROM_RDY_MASK);
	}
	eeprom_write_disable(desc);
}

// checks eeprom by toggling write enable bit
bool eeprom_check(EEPROM_Descriptor* desc) {
	eeprom_write_enable(desc);
	if (!(eeprom_read_status(desc) & EEPROM_WEL_MASK)) return false;
	eeprom_write_disable(desc);
	if (eeprom_read_status(desc) & EEPROM_WEL_MASK) return false;
	return true;
}