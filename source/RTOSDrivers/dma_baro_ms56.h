#ifndef DMA_BARO_MS56_H
#define DMA_BARO_MS56_H

#include <stdbool.h>
#include <sam.h>
#include "Drivers/baro_ms56.h"
#include "dma_spi.h"
#include "bus_manager.h"

typedef struct {
	BusManager_Transaction_Request request;
	uint32_t* data;
	uint8_t adc_command;
	bool command_sent;
} Barometer_Request;

bool dma_baro_process_request(Barometer_Descriptor* eepromdesc, DMA_Descriptor* dmadesc, Barometer_Request* request, BusManager_Transaction_Request* busrequest);

void baro_request(BusManager_Process* proc, Barometer_Request* request, Barometer_Descriptor* desc, int device_id, uint8_t adc_command, uint32_t* data);
void baro_wait_until_done(Barometer_Request* request);

#endif