#ifndef FLASH_AT25_H
#define FLASH_AT25_H

#include <stdbool.h>
#include <sam.h>
#include "port.h"

// commands

#define ENABLE_RESET 0x66
#define RESET_DEVICE 0x99
#define DEEP_POWER_DOWN 0xb9
#define RELEASE_POWER_DOWN 0xab
#define NORMAL_READ_DATA 0x03
#define FAST_READ 0x0b
#define DUAL_OUTPUT_FAST_READ 0x3b
#define DUAL_IO_FAST_READ 0xbb
#define QUAD_OUTPUT_FAST_READ 0x6b
#define QUAD_IO_FAST_READ 0xeb
#define WORD_READ_QUAD_IO 0xe7
#define SET_BURST_WITH_WRAP 0x77
#define WRITE_ENABLE 0x06
#define VOLATILE_SR_WRITE_ENABLE 0x50
#define WRITE_DISABLE 0x04
#define PAGE_PROGRAM 0x02
#define QUAD_PAGE_PROGRAM 0x32
#define BLOCK_ERASE_4K 0x20
#define BLOCK_ERASE_32K 0x52
#define BLOCK_ERASE_64K 0xd8
#define CHIP_ERASE 0xc7
#define PROG_ERASE_SUSPEND 0x75
#define PROG_ERASE_RESUME 0x7a
#define READ_STATUS_REGISTER_1 0x05
#define READ_STATUS_REGISTER_2 0x35
#define WRITE_STATUS_REGISTER_1 0x01
#define WRITE_STATUS_REGISTER_2 0x31
#define READ_DEVICE_ID 0x90
#define READ_DEVICE_ID_DUAL_IO 0x92
#define READ_DEVICE_ID_QUAD_IO 0x94
#define READ_JEDEC_ID 0x9f
#define READ_SERIAL_FLASH_DISCOVERABLE_PARAMETER 0x5a
#define ERASE_SECURITY_REGISTERS 0x44
#define PROGRAM_SECURITY_REGISTERS 0x42
#define READ_SECURITY_REGISTERS 0x48
#define READ_UNIQUE_ID_NUMBER 0x4b

// expected values
#define MANUFACTURER_ID 0x1F
#define DEVICEID_1_FAMILY_CODE 0x80
#define DEVICEID_2 0x01

#define SPI_FLASH_PAGE_SIZE 256

typedef struct __attribute__((packed)) {
	sercom_registers_t* sercom; // serial communication port
	uint32_t size; // size of flash
	Pin sspin; // SS pin
	uint8_t density_code :5;
} FLASH_Descriptor;

void flash_init(FLASH_Descriptor* desc, sercom_registers_t* sercom, Pin sspin, uint32_t size, uint8_t density_code);
bool flash_check(FLASH_Descriptor* desc);

#endif