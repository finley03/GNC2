#ifndef DMA_EEPROM_CAT25_H
#define DMA_EEPROM_CAT25_H

#include <stdbool.h>
#include <sam.h>
#include "Drivers/eeprom_cat25.h"
#include "dma_spi.h"
#include "bus_manager.h"

//#define EEPROM_SERCOM_TX_TRIGGER SERCOM1_DMAC_ID_TX
//#define EEPROM_SERCOM_RX_TRIGGER SERCOM1_DMAC_ID_RX
//#define EEPROM_DMA_PRIORITY 1
//#define EEPROM_DMA_TX_CHANNEL 0
//#define EEPROM_DMA_RX_CHANNEL 1

typedef enum {
	EEPROM_Read,
	EEPROM_Write
} EEPROM_Transaction_Type;

typedef struct {
	BusManager_Transaction_Request request;
	uint32_t address;
	int count;
	void* data;
	EEPROM_Transaction_Type type;
} EEPROM_Request;

//// page safe
//void dma_eeprom_read(EEPROM_Descriptor* eepromdesc, DMA_SPI_Descriptor* dmadesc, uint32_t address, int count, void* writeback);
//// NOT page safe
//void dma_eeprom_write(EEPROM_Descriptor* eepromdesc, DMA_SPI_Descriptor* dmadesc, uint32_t address, int count, void* data);
// process request, should be called until request is marked as complete, taking into account busy_until
// is page safe
// returns true if request is completed
// false if not
bool dma_eeprom_process_request(EEPROM_Descriptor* eepromdesc, DMA_Descriptor* dmadesc, EEPROM_Request* request, BusManager_Transaction_Request* busrequest);

void eeprom_request(BusManager_Process* proc, EEPROM_Request* request, EEPROM_Descriptor* desc, int device_id, EEPROM_Transaction_Type type, uint32_t address, int count, void* data);
void eeprom_wait_until_done(EEPROM_Request* request);

#endif