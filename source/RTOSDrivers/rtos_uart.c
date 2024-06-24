#include "rtos_uart.h"
#include "Drivers/sercom.h"
#include "rtos_dma.h"

bool rtos_uart_start_send_buffer(sercom_registers_t* sercom, DMA_Descriptor* dmadesc, uint8_t* buffer, int count) {
    if (!sercom_check(sercom)) return false;

    dma_create_descriptor(&(dma_descriptor[dmadesc->txchannel]), true, false, DMA_BEATSIZE_BYTE, count,
    buffer, &(sercom->USART_INT.SERCOM_DATA), DMA_NEXTDESCRIPTOR_NONE);

    // disable channels
	dma_disable_channel(dmadesc->txchannel);
    // init for correct parameters
	dma_init_channel(dmadesc->txchannel, DMA_TRIGACT_BEAT, dmadesc->txtrig, dmadesc->priority);
    // start transfer
	dma_enable_channel(dmadesc->txchannel);

    return true;
}

void rtos_uart_wait_until_send_complete(DMA_Descriptor* dmadesc) {
    rtos_dma_wait_until_end(dmadesc->txchannel);
}


bool rtos_uart_start_read_buffer(sercom_registers_t* sercom, DMA_Descriptor* dmadesc, uint8_t* buffer, int count) {
    if (!sercom_check(sercom)) return false;

    dma_create_descriptor(&(dma_descriptor[dmadesc->rxchannel]), false, true, DMA_BEATSIZE_BYTE, count,
    &(sercom->USART_INT.SERCOM_DATA), buffer, DMA_NEXTDESCRIPTOR_NONE);

    // disable channels
	dma_disable_channel(dmadesc->rxchannel);
    // init for correct parameters
	dma_init_channel(dmadesc->rxchannel, DMA_TRIGACT_BEAT, dmadesc->rxtrig, dmadesc->priority);
    // start transfer
	dma_enable_channel(dmadesc->rxchannel);

    return true;
}

void rtos_uart_wait_until_read_complete(DMA_Descriptor* dmadesc) {
    rtos_dma_wait_until_end(dmadesc->rxchannel);
}

void rtos_uart_wait_until_read_complete_or_timeout(DMA_Descriptor* dmadesc, uint32_t timeout_ticks) {
    rtos_dma_wait_until_end_or_timeout(dmadesc->rxchannel, timeout_ticks);

    dma_enable_channel(dmadesc->rxchannel);
}