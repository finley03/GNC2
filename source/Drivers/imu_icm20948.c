#include "imu_icm20948.h"
#include "sercom.h"
#include "spi.h"
#include "time.h"


void imu_icm20948_write_byte(IMU_ICM20948_Descriptor* desc, uint8_t address, uint8_t data) {
	// set ss low
	port_clear(desc->sspin.port, desc->sspin.pin);
	// select address
	spi_command(desc->sercom, address & IMU_WRITE_MASK);
	// send data
	spi_command(desc->sercom, data);
	// set ss high
	port_set(desc->sspin.port, desc->sspin.pin);
}

uint8_t imu_icm20948_read_byte(IMU_ICM20948_Descriptor* desc, uint8_t address) {
	// set ss low
	port_clear(desc->sspin.port, desc->sspin.pin);
	// select address
	spi_command(desc->sercom, address | IMU_READ_MASK);
	// send data
	uint8_t out = spi_command(desc->sercom, 0);
	// set ss high
	port_set(desc->sspin.port, desc->sspin.pin);
	return out;
}

void imu_icm20948_write(IMU_ICM20948_Descriptor* desc, uint8_t address, int count, uint8_t* data) {
	// set ss low
	port_clear(desc->sspin.port, desc->sspin.pin);
	// select address
	spi_command(desc->sercom, address & IMU_WRITE_MASK);
	// send data
	for (int i = 0; i < count; ++i) {
		spi_command(desc->sercom, data[i]);
	}
	// set ss high
	port_set(desc->sspin.port, desc->sspin.pin);
}

void imu_icm20948_read(IMU_ICM20948_Descriptor* desc, uint8_t address, int count, uint8_t* data) {
	// set ss low
	port_clear(desc->sspin.port, desc->sspin.pin);
	// select address
	spi_command(desc->sercom, address | IMU_READ_MASK);
	// send data
	for (int i = 0; i < count; ++i) {
		data[i] = spi_command(desc->sercom, 0);
	}
	// set ss high
	port_set(desc->sspin.port, desc->sspin.pin);
}

void imu_icm20948_user_bank(IMU_ICM20948_Descriptor* desc, int bank) {
	if (bank < 4 && bank >= 0) {
		imu_icm20948_write_byte(desc, REG_BANK_SEL, (bank << 4));
	} 
}

bool imu_icm20948_init(IMU_ICM20948_Descriptor* desc, sercom_registers_t* sercom, Pin sspin) {
	// check sercom
	if (!sercom_check(sercom)) return false;
	
	desc->sercom = sercom;
	desc->sspin = sspin;
	
	// software reset
	imu_icm20948_write_byte(desc, PWR_MGMT_1, 0b10000000);
	
	delay_ms(11);
	
	// set i2c disable bit
	imu_icm20948_write_byte(desc, USER_CTRL, 0b00010000);
	
	// exit sleep mode
	imu_icm20948_write_byte(desc, PWR_MGMT_1, 0b00000001);
	
	// wait to exit sleep mode (unspecified in datasheet)
	delay_us(80);
	
	return true;
}

bool imu_icm20948_check(IMU_ICM20948_Descriptor* desc) {
	// set user bank
	imu_icm20948_user_bank(desc, 0);
	uint8_t who_am_i = imu_icm20948_read_byte(desc, WHO_AM_I);
	
	if (who_am_i == WHO_AM_I_VAL) return true;
	else return false;
}

void mag_icm20948_write_byte(IMU_ICM20948_Descriptor* desc, uint8_t address, uint8_t data) {
	// set user bank to 3
	imu_icm20948_user_bank(desc, 3);
	
	// set slave address
	imu_icm20948_write_byte(desc, I2C_SLV0_ADDR, 0x0c);
	// set address to write in magnetometer
	imu_icm20948_write_byte(desc, I2C_SLV0_REG, address);
	// store data to write
	imu_icm20948_write_byte(desc, I2C_SLV0_DO, data);
	// enable i2c and send byte
	imu_icm20948_write_byte(desc, I2C_SLV0_CTRL, 0x80 | 0x01);
	
	// wait for transaction to complete
	delay_ms(10);
}

uint8_t mag_icm20948_read_byte(IMU_ICM20948_Descriptor* desc, uint8_t address) {
	// set user bank to 3
	imu_icm20948_user_bank(desc, 3);
	
	// set slave address
	// set address to read in magnetometer
	// enable i2c and request byte
	uint8_t request[3] = {0x0c | 0x80, address, 0x80 | 0x01};
	imu_icm20948_write(desc, I2C_SLV0_ADDR, 3, request);
	
	// wait for transfer to complete
	delay_us(1000); // random failiures at 800us
	
	// set user bank to 0
	imu_icm20948_user_bank(desc, 0);
	
	return imu_icm20948_read_byte(desc, EXT_SLV_SENS_DATA_00);
}

void mag_icm20948_read(IMU_ICM20948_Descriptor* desc, uint8_t address, uint8_t count, uint8_t* data) {
	// set user bank to 3
	imu_icm20948_user_bank(desc, 3);
	
	// set slave address
	// set address to read in magnetometer
	// enable i2c and request byte
	uint8_t request[3] = {0x0c | 0x80, address, 0x80 | count};
	imu_icm20948_write(desc, I2C_SLV0_ADDR, 3, request);
	
	// wait for transfer to complete
	delay_us(1000); // random failiures at 800us
	
	// set user bank to 0
	imu_icm20948_user_bank(desc, 0);
	
	// collect data
	imu_icm20948_read(desc, EXT_SLV_SENS_DATA_00, count, data);
}

bool mag_icm20948_init(IMU_ICM20948_Descriptor* desc) {
	// set user bank to 0
	imu_icm20948_user_bank(desc, 0);
	
	// enable i2c master
	imu_icm20948_write_byte(desc, USER_CTRL, 0b00100000);
	
	// set user bank to 3
	imu_icm20948_user_bank(desc, 3);
	
	// set i2c master clock frequency to type 7 (recommended mode)
	imu_icm20948_write_byte(desc, I2C_MST_CTRL, 0x07);
	// enable delay odr for i2c slave 0
	imu_icm20948_write_byte(desc, I2C_MST_DELAY_CTRL, 0x01);
	
	// wait for transaction to process
	delay_ms(1);
	
	// reset
	mag_icm20948_write_byte(desc, MAG_CNTL3, 0x01);
	// set magnetometer to continuous measurement at 100hz
	mag_icm20948_write_byte(desc, MAG_CNTL2, 0x08);
	
	return true;
}

bool mag_icm20948_check(IMU_ICM20948_Descriptor* desc) {
	uint8_t who_am_i[2];
	//uint8_t who_am_i = imu_mag_read_byte(desc, MAG_WAI1);
	mag_icm20948_read(desc, MAG_WAI1, 2, who_am_i);
	
	if (who_am_i[0] == MAG_WAI1_VAL && who_am_i[1] == MAG_WAI2_VAL) return true;
	else return false;
}