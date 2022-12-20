#ifndef DMA_SPI
#define DMA_SPI

#include "Drivers/dma.h"
#include "Drivers/port.h"

// typedef struct __attribute__((packed)) {
// 	uint8_t txchannel;
// 	uint8_t rxchannel;
// 	uint8_t rxtrig;
// 	uint8_t txtrig;
// 	uint8_t priority;
// } DMA_SPI_Descriptor;

void dma_spi_transaction(sercom_registers_t* sercom, Pin* sspin, DMA_Descriptor* desc);

// dma descriptors

extern DMA_Descriptor_Type txdesc2;
extern DMA_Descriptor_Type rxdesc2;

#endif