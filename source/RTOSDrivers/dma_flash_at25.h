#ifndef DMA_FLASH_AT25_H
#define DMA_FLASH_AT25_H

#include <stdbool.h>
#include <sam.h>
#include "Drivers/flash_at25.h"
#include "dma_spi.h"
#include "bus_manager.h"

extern uint8_t spi_flash_buffer[SPI_FLASH_PAGE_SIZE];

typedef enum {
	FLASH_Page_Read,
	FLASH_Page_Write,
	FLASH_Erase_4KB,
	FLASH_Erase_32KB,
	FLASH_Erase_64KB,
	FLASH_Chip_Erase,
	__FLASH_Check_Busy // for internal use
} FLASH_Transaction_Type;

typedef struct {
	BusManager_Transaction_Request request;
	uint32_t address;
	FLASH_Transaction_Type type;
	bool command_sent;
} FLASH_Request;

bool dma_flash_process_request(FLASH_Descriptor* flashdesc, DMA_Descriptor* dmadesc, FLASH_Request* request, BusManager_Transaction_Request* busrequest);

void flash_request(BusManager_Process* proc, FLASH_Request* request, FLASH_Descriptor* desc, int device_id, FLASH_Transaction_Type type, uint32_t address);
void flash_wait_until_done(FLASH_Request* request);

#endif