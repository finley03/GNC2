#include "global_ports.h"
#include "RTOSDrivers/rtos_usb.h"
#include "RTOSDrivers/rtos_uart.h"
#include "Drivers/uart.h"
#include "util.h"


const DMA_Descriptor uart1_descriptor = {
    .txchannel = UART1_DMA_TX_CHANNEL,
    .rxchannel = UART1_DMA_RX_CHANNEL,
    .txtrig = UART1_DMA_TX_TRIGGER,
    .rxtrig = UART1_DMA_RX_TRIGGER,
    .priority = UART1_DMA_PRIORITY
};

const DMA_Descriptor uart2_descriptor = {
    .txchannel = UART2_DMA_TX_CHANNEL,
    .rxchannel = UART2_DMA_RX_CHANNEL,
    .txtrig = UART2_DMA_TX_TRIGGER,
    .rxtrig = UART2_DMA_RX_TRIGGER,
    .priority = UART2_DMA_PRIORITY
};

const DMA_Descriptor uart3_descriptor = {
    .txchannel = UART3_DMA_TX_CHANNEL,
    .rxchannel = UART3_DMA_RX_CHANNEL,
    .txtrig = UART3_DMA_TX_TRIGGER,
    .rxtrig = UART3_DMA_RX_TRIGGER,
    .priority = UART3_DMA_PRIORITY
};

const DMA_Descriptor uart4_descriptor = {
    .txchannel = UART4_DMA_TX_CHANNEL,
    .rxchannel = UART4_DMA_RX_CHANNEL,
    .txtrig = UART4_DMA_TX_TRIGGER,
    .rxtrig = UART4_DMA_RX_TRIGGER,
    .priority = UART4_DMA_PRIORITY
};


bool serial_init() {
    bool ret = true;

	// init uart
	port_wrconfig(UART1_PORT, UART1_PMUX, UART1_PINS);
	port_wrconfig(UART2_PORT, UART2_PMUX, UART2_PINS);
	port_wrconfig(UART3_PORT, UART3_PMUX, UART3_PINS);
	port_wrconfig(UART4_PORT, UART4_PMUX, UART4_PINS);
	if (!uart_init(UART1_SERCOM, UART1_RXPO, UART1_TXPO, UART1_BAUD)) ret = false;
	if (!uart_init(UART2_SERCOM, UART2_RXPO, UART2_TXPO, UART2_BAUD)) ret = false;
	if (!uart_init(UART3_SERCOM, UART3_RXPO, UART3_TXPO, UART3_BAUD)) ret = false;
	if (!uart_init(UART4_SERCOM, UART4_RXPO, UART4_TXPO, UART4_BAUD)) ret = false;
    
	// init usb
	rtos_usb_init(USB_STACK_BASE, USB_STACK_SIZE);
	usb_attach();
}

bool serial_quit() {
    // will kill usb process which would remain running
    rtos_usb_quit();
}

bool serial_set_baud(Serial_Port_IDs port, uint32_t baud) {
    switch (port) {
        case PORT0:
        break;

        case PORT1:
        uart_set_baud(UART1_SERCOM, baud);
        break;

        case PORT2:
        uart_set_baud(UART2_SERCOM, baud);
        break;
        
        case PORT3:
        uart_set_baud(UART3_SERCOM, baud);
        break;

        case PORT4:
        uart_set_baud(UART4_SERCOM, baud);
        break;

        default:
        return false;
        break;
    }

    return true;
}

bool serial_enable_interrupt(Serial_Port_IDs port, void (*callback)(void)) {
        switch (port) {
        case PORT0:
        break;

        case PORT1:
        uart_set_interrupt(UART1_SERCOM, callback);
        break;

        case PORT2:
        uart_set_interrupt(UART2_SERCOM, callback);
        break;
        
        case PORT3:
        uart_set_interrupt(UART3_SERCOM, callback);
        break;

        case PORT4:
        uart_set_interrupt(UART4_SERCOM, callback);
        break;

        default:
        return false;
        break;
    }

    return true;
}


bool serial_write_start(Serial_Port_IDs port, uint8_t* buffer, int count) {
    switch (port) {
        case PORT0:
        rtos_usb_start_send_buffer(buffer, count);
        break;

        case PORT1:
        rtos_uart_start_send_buffer(UART1_SERCOM, &uart1_descriptor, buffer, count);
        break;

        case PORT2:
        rtos_uart_start_send_buffer(UART2_SERCOM, &uart2_descriptor, buffer, count);
        break;
        
        case PORT3:
        rtos_uart_start_send_buffer(UART3_SERCOM, &uart3_descriptor, buffer, count);
        break;

        case PORT4:
        rtos_uart_start_send_buffer(UART4_SERCOM, &uart4_descriptor, buffer, count);
        break;

        default:
        return false;
        break;
    }

    return true;
}

bool serial_write_wait_until_complete(Serial_Port_IDs port) {
    switch (port) {
        case PORT0:
        rtos_usb_wait_until_send_complete();
        break;

        case PORT1:
        rtos_uart_wait_until_send_complete(&uart1_descriptor);
        break;

        case PORT2:
        rtos_uart_wait_until_send_complete(&uart2_descriptor);
        break;
        
        case PORT3:
        rtos_uart_wait_until_send_complete(&uart3_descriptor);
        break;

        case PORT4:
        rtos_uart_wait_until_send_complete(&uart4_descriptor);
        break;

        default:
        return false;
        break;
    }

    return true;
}


bool serial_read_start(Serial_Port_IDs port, uint8_t* buffer, int count) {
    switch (port) {
        case PORT0:
        rtos_usb_start_read_buffer(buffer, count);
        break;

        case PORT1:
        rtos_uart_start_read_buffer(UART1_SERCOM, &uart1_descriptor, buffer, count);
        break;

        case PORT2:
        rtos_uart_start_read_buffer(UART2_SERCOM, &uart2_descriptor, buffer, count);
        break;
        
        case PORT3:
        rtos_uart_start_read_buffer(UART3_SERCOM, &uart3_descriptor, buffer, count);
        break;

        case PORT4:
        rtos_uart_start_read_buffer(UART4_SERCOM, &uart4_descriptor, buffer, count);
        break;

        default:
        return false;
        break;
    }

    return true;
}

bool serial_read_wait_until_complete(Serial_Port_IDs port) {
    switch (port) {
        case PORT0:
        rtos_usb_wait_until_read_complete();
        break;

        case PORT1:
        rtos_uart_wait_until_read_complete(&uart1_descriptor);
        break;

        case PORT2:
        rtos_uart_wait_until_read_complete(&uart2_descriptor);
        break;
        
        case PORT3:
        rtos_uart_wait_until_read_complete(&uart3_descriptor);
        break;

        case PORT4:
        rtos_uart_wait_until_read_complete(&uart4_descriptor);
        break;

        default:
        return false;
        break;
    }

    return true;
}

bool serial_read_start_infinite(Serial_Port_IDs port, uint8_t* buffer, int count) {
    switch (port) {
        case PORT0:
        return false;
        break;

        case PORT1:
        rtos_uart_start_read_buffer_infinite(UART1_SERCOM, &uart1_descriptor, buffer, count);
        break;

        case PORT2:
        rtos_uart_start_read_buffer_infinite(UART2_SERCOM, &uart2_descriptor, buffer, count);
        break;
        
        case PORT3:
        rtos_uart_start_read_buffer_infinite(UART3_SERCOM, &uart3_descriptor, buffer, count);
        break;

        case PORT4:
        rtos_uart_start_read_buffer_infinite(UART4_SERCOM, &uart4_descriptor, buffer, count);
        break;

        default:
        return false;
        break;
    }

    return true;
}

bool serial_read_wait_until_complete_or_timeout(Serial_Port_IDs port, uint32_t timeout_ms) {
    switch (port) {
        case PORT0:
        rtos_usb_wait_until_read_complete(); // USB doesn't time out
        break;

        case PORT1:
        rtos_uart_wait_until_read_complete_or_timeout(&uart1_descriptor, timeout_ms * time_ticks_ms_mult);
        break;

        case PORT2:
        rtos_uart_wait_until_read_complete_or_timeout(&uart2_descriptor, timeout_ms * time_ticks_ms_mult);
        break;
        
        case PORT3:
        rtos_uart_wait_until_read_complete_or_timeout(&uart3_descriptor, timeout_ms * time_ticks_ms_mult);
        break;

        case PORT4:
        rtos_uart_wait_until_read_complete_or_timeout(&uart4_descriptor, timeout_ms * time_ticks_ms_mult);
        break;

        default:
        return false;
        break;
    }

    return true;
}

bool serial_flush(Serial_Port_IDs port) {
    switch (port) {
        case PORT0:
        break;

        case PORT1:
        uart_flush(UART1_SERCOM);
        break;

        case PORT2:
        uart_flush(UART2_SERCOM);
        break;
        
        case PORT3:
        uart_flush(UART3_SERCOM);
        break;

        case PORT4:
        uart_flush(UART4_SERCOM);
        break;

        default:
        return false;
        break;
    }

    return true;
}