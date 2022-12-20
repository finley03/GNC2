#include "baro_ms56.h"
#include "sercom.h"
#include "time.h"
#include "spi.h"

// takes delay in us
void baro_send_command(Barometer_Descriptor* desc, uint8_t command, uint16_t delay) {
	// set ss low
	port_clear(desc->sspin.port, desc->sspin.pin);
	// send command
	spi_command(desc->sercom, command);
	// set ss high
	port_set(desc->sspin.port, desc->sspin.pin);
	// wait
	delay_us(delay);
}

void baro_read_prom(Barometer_Descriptor* desc) {
	// loop through baro prom values
	for (uint8_t addr = BARO_PROM_SETUP; addr <= BARO_PROM_CRC; addr += 2) {
		// set ss low
		port_clear(desc->sspin.port, desc->sspin.pin);
		// send address
		spi_command(desc->sercom, addr);
		// receive data
		uint8_t data0 = spi_command(desc->sercom, 0);
		uint8_t data1 = spi_command(desc->sercom, 0);
		// set ss high
		port_set(desc->sspin.port, desc->sspin.pin);
		// set value
		uint16_t* promvalptr = (uint16_t*)((uint8_t*)(&(desc->prom)) + addr - BARO_PROM_SETUP);
		*promvalptr = ((uint16_t)data0 << 8) | ((uint16_t)data1);
	}
}

uint32_t baro_read_adc(Barometer_Descriptor* desc) {
	// set ss low
	port_clear(desc->sspin.port, desc->sspin.pin);
	// send adc read command
	spi_command(desc->sercom, BARO_ADC_READ);
	// receive data
	uint8_t data0 = spi_command(desc->sercom, 0);
	uint8_t data1 = spi_command(desc->sercom, 0);
	uint8_t data2 = spi_command(desc->sercom, 0);
	// set ss high
	port_set(desc->sspin.port, desc->sspin.pin);
	
	return ((uint32_t)data0 << 16) | ((uint32_t)data1 << 8) | ((uint32_t)data2);
}

bool baro_check(Barometer_Descriptor* desc) {
	// check CRC
	// remainder
	uint16_t n_rem = 0;
	// save value of crc in prom
	uint16_t prom_crc = desc->prom.crc;
	// zero final 8 bits
	desc->prom.crc &= 0xFF00;
	// create indexable pointer
	uint16_t* prom = (uint16_t*)&(desc->prom);
	// zero crc byte
	desc->prom.crc &= 0xFF00;
	// loop through bytes
	for (int i = 0; i < 16; ++i) {
		uint16_t byte;
		// select LSB or MSB
		byte = (i & 1) ? prom[i >> 1] & 0xFF : prom[i >> 1] >> 8;
		n_rem ^= byte;
		// loop through bits
		for (int j = 0; j < 8; ++j) {
			n_rem = (n_rem & 0x8000) ? (n_rem << 1) ^ 0x3000 : (n_rem << 1);
		}
	}
	n_rem = (0xF & (n_rem >> 12));
	desc->prom.crc = prom_crc;
	return n_rem == (prom_crc & 0xF);
}

bool baro_init(Barometer_Descriptor* desc, sercom_registers_t* sercom, Pin sspin) {
	// check sercom
	if (!sercom_check(sercom)) return false;
	
	desc->sercom = sercom;
	desc->sspin = sspin;
	
	// reset
	baro_send_command(desc, BARO_RESET, 3000);
	// read prom
	baro_read_prom(desc);
	
	return true;
}

uint32_t baro_get_conversion_time(uint8_t adc_command) {
	switch (adc_command) {
		case BARO_D1_256:
		case BARO_D2_256:
		return 600;
		
		case BARO_D1_512:
		case BARO_D2_512:
		return 1170;
		
		case BARO_D1_1024:
		case BARO_D2_1024:
		return 2280;
		
		case BARO_D1_2048:
		case BARO_D2_2048:
		return 4540;
		
		case BARO_D1_4096:
		case BARO_D2_4096:
		return 9040;
		
		default:
		return 0;
	}
}