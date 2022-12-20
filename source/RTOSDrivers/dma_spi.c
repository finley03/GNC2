#include "dma_spi.h"
#include "rtos_dma.h"
#include "Drivers/spi.h"
#include "Drivers/eeprom_cat25.h"
#include "dma_eeprom_cat25.h"

DMA_Descriptor_Type txdesc2;
DMA_Descriptor_Type rxdesc2;

void dma_spi_transaction(sercom_registers_t* sercom, Pin* sspin, DMA_Descriptor* desc) {
	// disable channels
	dma_disable_channel(desc->txchannel);
	dma_disable_channel(desc->rxchannel);
	// flush spi
	spi_flush(sercom);
	// init for correct parameters
	dma_init_channel(desc->txchannel, DMA_TRIGACT_BEAT, desc->txtrig, desc->priority);
	dma_init_channel(desc->rxchannel, DMA_TRIGACT_BEAT, desc->rxtrig, desc->priority);
	// set ss low
	port_clear(sspin->port, sspin->pin);
	// start transfer
	dma_enable_channel(desc->txchannel);
	dma_enable_channel(desc->rxchannel);
	// wait until transfer done
	rtos_dma_wait_until_end(desc->rxchannel);
	// disable channels
	dma_disable_channel(desc->txchannel);
	dma_disable_channel(desc->rxchannel);
	// set ss high
	port_set(sspin->port, sspin->pin);
}