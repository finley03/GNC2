#include "dma_eeprom_cat25.h"
#include "Drivers/spi.h"
#include "RTOS/rtos.h"
#include "rtos_dma.h"
#include "util.h"
#include TIME_DRIVER

#define EEPROM_WRITE_CYCLE_TIME 5

typedef struct __attribute__((packed, scalar_storage_order("big-endian"))) {
	uint8_t command;
	uint16_t address;
} DMA_EEPROM_Addressed_Request;

void dma_eeprom_read(EEPROM_Descriptor* eepromdesc, DMA_Descriptor* dmadesc, uint32_t address, int count, void* writeback) {
	// create request data
	DMA_EEPROM_Addressed_Request request = {
		.command = EEPROM_READ,
		.address = (uint16_t)address
	};
	register sercom_registers_t* sercom = eepromdesc->sercom;
	// create descriptor for tx channel
	dma_create_descriptor(&(dma_descriptor[dmadesc->txchannel]), true, false, DMA_BEATSIZE_BYTE, sizeof(request),
		&request, &(sercom->SPIM.SERCOM_DATA), &txdesc2);
	dma_create_descriptor(&txdesc2, false, false, DMA_BEATSIZE_BYTE, count,
		&zero, &(sercom->SPIM.SERCOM_DATA), DMA_NEXTDESCRIPTOR_NONE);
	// create descriptor for rx channel
	dma_create_descriptor(&(dma_descriptor[dmadesc->rxchannel]), false, false, DMA_BEATSIZE_BYTE, sizeof(request),
		&(sercom->SPIM.SERCOM_DATA), &datasink, &rxdesc2);
	dma_create_descriptor(&rxdesc2, false, true, DMA_BEATSIZE_BYTE, count,
		&(sercom->SPIM.SERCOM_DATA), writeback, DMA_NEXTDESCRIPTOR_NONE);
		
	dma_spi_transaction(sercom, &(eepromdesc->sspin), dmadesc);
}

void dma_eeprom_write(EEPROM_Descriptor* eepromdesc, DMA_Descriptor* dmadesc, uint32_t address, int count, void* data) {
	// create request data
	DMA_EEPROM_Addressed_Request request = {
		.command = EEPROM_WRITE,
		.address = (uint16_t)address
	};
	register sercom_registers_t* sercom = eepromdesc->sercom;
	// create descriptor for tx channel
	dma_create_descriptor(&(dma_descriptor[dmadesc->txchannel]), true, false, DMA_BEATSIZE_BYTE, sizeof(request),
		&request, &(sercom->SPIM.SERCOM_DATA), &txdesc2);
	dma_create_descriptor(&txdesc2, true, false, DMA_BEATSIZE_BYTE, count,
		data, &(sercom->SPIM.SERCOM_DATA), DMA_NEXTDESCRIPTOR_NONE);
	// create descriptor for rx channel
	dma_create_descriptor(&(dma_descriptor[dmadesc->rxchannel]), false, false, DMA_BEATSIZE_BYTE, sizeof(request),
		&(sercom->SPIM.SERCOM_DATA), &datasink, &rxdesc2);
	dma_create_descriptor(&rxdesc2, false, false, DMA_BEATSIZE_BYTE, count,
		&(sercom->SPIM.SERCOM_DATA), &datasink, DMA_NEXTDESCRIPTOR_NONE);
	
	eeprom_write_enable(eepromdesc);
	dma_spi_transaction(sercom, &(eepromdesc->sspin), dmadesc);
}

bool dma_eeprom_process_request(EEPROM_Descriptor* eepromdesc, DMA_Descriptor* dmadesc, EEPROM_Request* request, BusManager_Transaction_Request* busrequest) {
	switch (request->type) {
		case EEPROM_Read:
		{	
			dma_eeprom_read(eepromdesc, dmadesc, request->address, request->count, request->data);
		}
		break;
		case EEPROM_Write:
		{
			uint32_t top = request->address + request->count;
			uint32_t next_page = (request->address / EEPROM_PAGE_SIZE) * EEPROM_PAGE_SIZE + EEPROM_PAGE_SIZE;
			int count = (top < next_page) ? request->count : next_page - request->address;
			dma_eeprom_write(eepromdesc, dmadesc, request->address, count, request->data);
			busrequest->busy_until = time_read_ticks() + (uint32_t)(EEPROM_WRITE_CYCLE_TIME) * TIME_TICKS_MS_MULT;
			busrequest->busy = true;
			// prepare request to run again
			request->count -= count;
			if (request->count) {
				request->address += count;
				request->data = (uint8_t*)(request->data) + count;
				return false;
			}
			//rtos_delay_ms(EEPROM_WRITE_CYCLE_TIME);
		}
		break;
	}
	return true;
}

void eeprom_request(BusManager_Process* proc, EEPROM_Request* request, EEPROM_Descriptor* desc, int device_id, EEPROM_Transaction_Type type, uint32_t address, int count, void* data) {
	request->type = type;
	request->address = address;
	request->count = count;
	request->data = data;
	busmanager_request_transaction(proc, &(request->request), device_id, desc, request, dma_eeprom_process_request);
}

void eeprom_wait_until_done(EEPROM_Request* request) {
	wait_until(&(request->request.status), BusManager_Request_CompleteNotBusy, U8_MASK, Process_Wait_Until_Equal);
}