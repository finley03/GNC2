#include "uart.h"
#include "sercom.h"
#include "time.h"

void (*sercom0_callback)(void);
void (*sercom1_callback)(void);
void (*sercom4_callback)(void);
void (*sercom5_callback)(void);

bool uart_init(sercom_registers_t* sercom, uint8_t rxpo, uint8_t txpo, uint32_t baud) {
    if (!sercom_init(sercom)) return false;

    // configure CTRLA register
    sercom->USART_INT.SERCOM_CTRLA = SERCOM_USART_INT_CTRLA_DORD(1) | SERCOM_USART_INT_CTRLA_CMODE_ASYNC |
        SERCOM_USART_INT_CTRLA_FORM_USART_FRAME_NO_PARITY | SERCOM_USART_INT_CTRLA_RXPO(rxpo) |
        SERCOM_USART_INT_CTRLA_TXPO(txpo) | SERCOM_USART_INT_CTRLA_MODE_USART_INT_CLK;

    // configure CTRLB register
    sercom->USART_INT.SERCOM_CTRLB = SERCOM_USART_INT_CTRLB_RXEN(1) | SERCOM_USART_INT_CTRLB_TXEN(1);

    // wait for sync on CTRLB
    while (sercom->USART_INT.SERCOM_SYNCBUSY & SERCOM_USART_INT_SYNCBUSY_CTRLB_Msk);

    if (!uart_set_baud(sercom, baud)) return false;

    // enable USART
    sercom->USART_INT.SERCOM_CTRLA |= SERCOM_USART_INT_CTRLA_ENABLE(1);

    // wait for sync
    while (sercom->USART_INT.SERCOM_SYNCBUSY & SERCOM_USART_INT_SYNCBUSY_ENABLE_Msk);

    return true;
}

bool uart_set_baud(sercom_registers_t* sercom, uint32_t baud) {
    if (!sercom_check(sercom)) return false;
    // check baud is in range
    if (baud > 3E6) return false;

    float baudval = 65536.0f * (1.0f - ((float)baud / F_CPU * 16));
    sercom->USART_INT.SERCOM_BAUD = (uint16_t)baudval;
    return true;
}

bool uart_set_interrupt(sercom_registers_t* sercom, void (*callback)(void)) {
    if (!sercom_check(sercom)) return false;
    
    sercom->USART_INT.SERCOM_INTENSET = SERCOM_USART_INT_INTFLAG_RXC(1);

    switch ((uint32_t)sercom) {
        case (uint32_t)SERCOM0_REGS:
        sercom0_callback = callback;
        NVIC_EnableIRQ(SERCOM0_IRQn);
        break;

        case (uint32_t)SERCOM1_REGS:
        sercom1_callback = callback;
        NVIC_EnableIRQ(SERCOM1_IRQn);
        break;

        case (uint32_t)SERCOM4_REGS:
        sercom4_callback = callback;
        NVIC_EnableIRQ(SERCOM4_IRQn);
        break;

        case (uint32_t)SERCOM5_REGS:
        sercom5_callback = callback;
        NVIC_EnableIRQ(SERCOM5_IRQn);
        break;
    }
}

void uart_flush(sercom_registers_t* sercom) {
    while (sercom->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_RXC_Msk) sercom->USART_INT.SERCOM_DATA;
}

void uart_send_buffer(sercom_registers_t* sercom, uint8_t* buffer, int count) {
    for (int i = 0; i < count; ++i) {
        while (!(sercom->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_DRE_Msk));
        sercom->USART_INT.SERCOM_DATA = buffer[i];
    }
}

int uart_read_buffer(sercom_registers_t* sercom, uint8_t* buffer, int count, int wait_microseconds) {
    int i;
    for (i = 0; i < count; ++i) {
        uint32_t current_time = read_timer_20ns();
        // wait until data available
        while (!(sercom->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_RXC_Msk)) {
            if (read_timer_20ns() - current_time > wait_microseconds * TIMER_TICK_US_MULTIPLIER) return i;
        }

        buffer[i] = (uint8_t)(sercom->USART_INT.SERCOM_DATA);
    }
    return i;
}

void SERCOM0_Handler() {
    sercom0_callback();
    SERCOM0_REGS->USART_INT.SERCOM_INTFLAG = SERCOM0_REGS->USART_INT.SERCOM_INTFLAG;
}

void SERCOM1_Handler() {
    sercom1_callback();
    SERCOM1_REGS->USART_INT.SERCOM_INTFLAG = SERCOM1_REGS->USART_INT.SERCOM_INTFLAG;
}

void SERCOM4_Handler() {
    sercom4_callback();
    SERCOM4_REGS->USART_INT.SERCOM_INTFLAG = SERCOM4_REGS->USART_INT.SERCOM_INTFLAG;
}

void SERCOM5_Handler() {
    sercom5_callback();
    SERCOM5_REGS->USART_INT.SERCOM_INTFLAG = SERCOM5_REGS->USART_INT.SERCOM_INTFLAG;
}