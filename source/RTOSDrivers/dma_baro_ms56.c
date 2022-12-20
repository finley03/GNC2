#include "dma_baro_ms56.h"
#include "Drivers/spi.h"
#include "RTOS/rtos.h"
#include "rtos_dma.h"
#include "util.h"
#include TIME_DRIVER

void dma_baro_read_adc(Barometer_Descriptor* barodesc, DMA_Descriptor* dmadesc, uint32_t* data) {
	// writeback location for DMA
	uint8_t rxdata[3];
	uint8_t command = BARO_ADC_READ;
	register sercom_registers_t* sercom = barodesc->sercom;
	// create descriptor for tx channel
	dma_create_descriptor(&(dma_descriptor[dmadesc->txchannel]), false, false, DMA_BEATSIZE_BYTE, 1,
		&command, &(sercom->SPIM.SERCOM_DATA), &txdesc2);
	dma_create_descriptor(&txdesc2, false, false, DMA_BEATSIZE_BYTE, 3,
		&zero, &(sercom->SPIM.SERCOM_DATA), DMA_NEXTDESCRIPTOR_NONE);
	// create descriptor for rx channel
	dma_create_descriptor(&(dma_descriptor[dmadesc->rxchannel]), false, false, DMA_BEATSIZE_BYTE, 1,
		&(sercom->SPIM.SERCOM_DATA), &datasink, &rxdesc2);
	dma_create_descriptor(&rxdesc2, false, true, DMA_BEATSIZE_BYTE, 3,
		&(sercom->SPIM.SERCOM_DATA), rxdata, DMA_NEXTDESCRIPTOR_NONE);
	
	dma_spi_transaction(sercom, &(barodesc->sspin), dmadesc);
	
	*data = ((uint32_t)rxdata[0] << 16) | ((uint32_t)rxdata[1] << 8) | ((uint32_t)rxdata[2]);
}

bool dma_baro_process_request(Barometer_Descriptor* barodesc, DMA_Descriptor* dmadesc, Barometer_Request* request, BusManager_Transaction_Request* busrequest) {
	// second time
	if (request->command_sent) {
		dma_baro_read_adc(barodesc, dmadesc, request->data);
		return true;
	}
	// first time
	else {
		uint32_t conversion_time = baro_get_conversion_time(request->adc_command);
		if (!conversion_time) return false;
		// send command
		// set ss low
		port_clear(barodesc->sspin.port, barodesc->sspin.pin);
		// send command
		spi_command(barodesc->sercom, request->adc_command);
		// set ss high
		port_set(barodesc->sspin.port, barodesc->sspin.pin);
		// indicate command sent
		request->command_sent = true;
		// wait
		busrequest->busy_until = time_read_ticks() + conversion_time * TIME_TICKS_US_MULT;
		busrequest->busy = true;
		return false;
	}
}

void baro_request(BusManager_Process* proc, Barometer_Request* request, Barometer_Descriptor* desc, int device_id, uint8_t adc_command, uint32_t* data) {
	request->data = data;
	request->adc_command = adc_command;
	request->command_sent = false;
	busmanager_request_transaction(proc, &(request->request), device_id, desc, request, dma_baro_process_request);
}

void baro_wait_until_done(Barometer_Request* request) {
	wait_until(&(request->request.status), BusManager_Request_CompleteNotBusy, U8_MASK, Process_Wait_Until_Equal);
}