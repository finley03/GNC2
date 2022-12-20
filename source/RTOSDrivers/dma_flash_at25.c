#include "dma_flash_at25.h"
#include "Drivers/spi.h"
#include "RTOS/rtos.h"
#include "rtos_dma.h"
#include "util.h"
#include TIME_DRIVER

#define FLASH_ERASE_BUSY_POLL_TIMEOUT_US 10000 // time between polling flash chip during erase

uint8_t spi_flash_buffer[SPI_FLASH_PAGE_SIZE];

typedef struct __attribute__((packed, scalar_storage_order("big-endian"))) {
	uint8_t command;
	uint32_t address :24;
} DMA_FLASH_Addressed_Request;

void dma_flash_page_read(FLASH_Descriptor* flashdesc, DMA_Descriptor* dmadesc, uint32_t address) {
	// create request data
	DMA_FLASH_Addressed_Request request = {
		.command = NORMAL_READ_DATA,
		.address = address
	};
	register sercom_registers_t* sercom = flashdesc->sercom;
	// create descriptor for tx channel
	dma_create_descriptor(&(dma_descriptor[dmadesc->txchannel]), true, false, DMA_BEATSIZE_BYTE, sizeof(request),
	&request, &(sercom->SPIM.SERCOM_DATA), &txdesc2);
	dma_create_descriptor(&txdesc2, false, false, DMA_BEATSIZE_BYTE, SPI_FLASH_PAGE_SIZE,
	&zero, &(sercom->SPIM.SERCOM_DATA), DMA_NEXTDESCRIPTOR_NONE);
	// create descriptor for rx channel
	dma_create_descriptor(&(dma_descriptor[dmadesc->rxchannel]), false, false, DMA_BEATSIZE_BYTE, sizeof(request),
	&(sercom->SPIM.SERCOM_DATA), &datasink, &rxdesc2);
	dma_create_descriptor(&rxdesc2, false, true, DMA_BEATSIZE_BYTE, SPI_FLASH_PAGE_SIZE,
	&(sercom->SPIM.SERCOM_DATA), spi_flash_buffer, DMA_NEXTDESCRIPTOR_NONE);
	
	dma_spi_transaction(sercom, &(flashdesc->sspin), dmadesc);
}

void dma_flash_page_write(FLASH_Descriptor* flashdesc, DMA_Descriptor* dmadesc, uint32_t address) {
	// create request data
	DMA_FLASH_Addressed_Request request = {
		.command = PAGE_PROGRAM,
		.address = address
	};
	register sercom_registers_t* sercom = flashdesc->sercom;
	// create descriptor for tx channel
	dma_create_descriptor(&(dma_descriptor[dmadesc->txchannel]), true, false, DMA_BEATSIZE_BYTE, sizeof(request),
	&request, &(sercom->SPIM.SERCOM_DATA), &txdesc2);
	dma_create_descriptor(&txdesc2, true, false, DMA_BEATSIZE_BYTE, SPI_FLASH_PAGE_SIZE,
	spi_flash_buffer, &(sercom->SPIM.SERCOM_DATA), DMA_NEXTDESCRIPTOR_NONE);
	// create descriptor for rx channel
	dma_create_descriptor(&(dma_descriptor[dmadesc->rxchannel]), false, false, DMA_BEATSIZE_BYTE, sizeof(request),
	&(sercom->SPIM.SERCOM_DATA), &datasink, &rxdesc2);
	dma_create_descriptor(&rxdesc2, false, false, DMA_BEATSIZE_BYTE, SPI_FLASH_PAGE_SIZE,
	&(sercom->SPIM.SERCOM_DATA), &datasink, DMA_NEXTDESCRIPTOR_NONE);
	
	flash_write_enable(flashdesc);
	dma_spi_transaction(sercom, &(flashdesc->sspin), dmadesc);
}

void dma_flash_block_erase(FLASH_Descriptor* flashdesc, DMA_Descriptor* dmadesc, FLASH_Transaction_Type type, uint32_t address) {
	uint8_t command;
	switch (type) {
		case FLASH_Erase_4KB:
		command = BLOCK_ERASE_4K;
		break;
		case FLASH_Erase_32KB:
		command = BLOCK_ERASE_32K;
		break;
		case FLASH_Erase_64KB:
		command = BLOCK_ERASE_64K;
		break;
		default:
		return;
	}
	
	// create request data
	DMA_FLASH_Addressed_Request request = {
		.command = command,
		.address = address
	};
	register sercom_registers_t* sercom = flashdesc->sercom;
	// create descriptor for tx channel
	dma_create_descriptor(&(dma_descriptor[dmadesc->txchannel]), true, false, DMA_BEATSIZE_BYTE, sizeof(request),
	&request, &(sercom->SPIM.SERCOM_DATA), DMA_NEXTDESCRIPTOR_NONE);
	// create descriptor for rx channel
	dma_create_descriptor(&(dma_descriptor[dmadesc->rxchannel]), false, false, DMA_BEATSIZE_BYTE, sizeof(request),
	&(sercom->SPIM.SERCOM_DATA), &datasink, DMA_NEXTDESCRIPTOR_NONE);
	
	flash_write_enable(flashdesc);
	dma_spi_transaction(sercom, &(flashdesc->sspin), dmadesc);
}

void flash_chip_erase(FLASH_Descriptor* flashdesc) {
	flash_write_enable(flashdesc);
	port_clear(flashdesc->sspin.port, flashdesc->sspin.pin);
	spi_command(flashdesc->sercom, CHIP_ERASE);
	port_set(flashdesc->sspin.port, flashdesc->sspin.pin);
}

bool dma_flash_check_busy(FLASH_Descriptor* flashdesc, DMA_Descriptor* dmadesc) {
	uint8_t command = READ_STATUS_REGISTER_1;
	uint8_t status_register_1;
	register sercom_registers_t* sercom = flashdesc->sercom;
	
	// create descriptor for tx channel
	dma_create_descriptor(&(dma_descriptor[dmadesc->txchannel]), false, false, DMA_BEATSIZE_BYTE, 1,
	&command, &(sercom->SPIM.SERCOM_DATA), &txdesc2);
	dma_create_descriptor(&txdesc2, false, false, DMA_BEATSIZE_BYTE, 1,
	&zero, &(sercom->SPIM.SERCOM_DATA), DMA_NEXTDESCRIPTOR_NONE);
	// create descriptor for rx channel
	dma_create_descriptor(&(dma_descriptor[dmadesc->rxchannel]), false, false, DMA_BEATSIZE_BYTE, 1,
	&(sercom->SPIM.SERCOM_DATA), &datasink, &rxdesc2);
	dma_create_descriptor(&rxdesc2, false, false, DMA_BEATSIZE_BYTE, 1,
	&(sercom->SPIM.SERCOM_DATA), &status_register_1, DMA_NEXTDESCRIPTOR_NONE);
	
	dma_spi_transaction(sercom, &(flashdesc->sspin), dmadesc);
	
	return (status_register_1 & 0x01) ? true : false;
}

bool dma_flash_process_request(FLASH_Descriptor* flashdesc, DMA_Descriptor* dmadesc, FLASH_Request* request, BusManager_Transaction_Request* busrequest) {
	switch (request->type) {
		case FLASH_Page_Read:
		{
			dma_flash_page_read(flashdesc, dmadesc, request->address);
		}
		break;
		
		case FLASH_Page_Write:
		{
			dma_flash_page_write(flashdesc, dmadesc, request->address);
			busrequest->busy_until = time_read_ticks() + (uint32_t)(3400) * TIME_TICKS_US_MULT;
			busrequest->busy = true;
		}
		break;
		
		case FLASH_Erase_4KB:
		case FLASH_Erase_32KB:
		case FLASH_Erase_64KB:
		case FLASH_Chip_Erase:
		{
			// second time
			if (request->command_sent) {
				if (dma_flash_check_busy(flashdesc, dmadesc)) {
					busrequest->busy_until = time_read_ticks() + (uint32_t)(FLASH_ERASE_BUSY_POLL_TIMEOUT_US) * TIME_TICKS_US_MULT;
					busrequest->busy = true;
					return false;
				}
			}
			// first time
			else {
				if (request->type == FLASH_Chip_Erase) flash_chip_erase(flashdesc);
				else dma_flash_block_erase(flashdesc, dmadesc, request->type, request->address);
				request->command_sent = true;
				busrequest->busy_until = time_read_ticks() + (uint32_t)(FLASH_ERASE_BUSY_POLL_TIMEOUT_US) * TIME_TICKS_US_MULT;
				busrequest->busy = true;
				return false;
			}
			
		}
		break;
		
		default:
		break;
	}
	
	return true;
}

void flash_request(BusManager_Process* proc, FLASH_Request* request, FLASH_Descriptor* desc, int device_id, FLASH_Transaction_Type type, uint32_t address) {
	request->type = type;
	request->address = address;
	request->command_sent = false;
	busmanager_request_transaction(proc, &(request->request), device_id, desc, request, dma_flash_process_request);
}

void flash_wait_until_done(FLASH_Request* request) {
	wait_until(&(request->request.status), BusManager_Request_CompleteNotBusy, U8_MASK, Process_Wait_Until_Equal);
}