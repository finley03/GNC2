#include "flash_at25.h"
#include "spi.h"

void flash_init(FLASH_Descriptor* desc, sercom_registers_t* sercom, Pin sspin, uint32_t size, uint8_t density_code) {
	desc->sercom = sercom;
	desc->sspin = sspin;
	desc->size = size;
	desc->density_code = density_code;
	port_set_output(desc->sspin.port, desc->sspin.pin);
	port_set(desc->sspin.port, desc->sspin.pin);
}

bool flash_check(FLASH_Descriptor* desc) {
	port_clear(desc->sspin.port, desc->sspin.pin);
	uint8_t jedecid[3];
	spi_command(desc->sercom, READ_JEDEC_ID);
	for (int i = 0; i < 3; ++i) jedecid[i] = spi_command(desc->sercom, 0);
	port_set(desc->sspin.port, desc->sspin.pin);
	
	if (jedecid[0] == MANUFACTURER_ID && jedecid[1] == (DEVICEID_1_FAMILY_CODE | desc->density_code) &&
		jedecid[2] == DEVICEID_2) return true;
	else return false;
}

void flash_write_enable(FLASH_Descriptor* desc) {
	port_clear(desc->sspin.port, desc->sspin.pin);
	spi_command(desc->sercom, WRITE_ENABLE);
	port_set(desc->sspin.port, desc->sspin.pin);
}

void flash_write_disable(FLASH_Descriptor* desc) {
	port_clear(desc->sspin.port, desc->sspin.pin);
	spi_command(desc->sercom, WRITE_DISABLE);
	port_set(desc->sspin.port, desc->sspin.pin);
}