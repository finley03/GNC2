#include "util.h"
#include PORT_DRIVER
#include "Drivers/pwm.h"
#include "RTOSDrivers/dma_eeprom_cat25.h"
#include "RTOSDrivers/dma_baro_ms56.h"
#include "RTOSDrivers/dma_flash_at25.h"

#include "global_imu.h"
#include "global_ports.h"
#include "global_eeprom.h"
#include "navigation.h"
#include TIME_DRIVER

bool init();
void shutdown();
void buzzer();

uint8_t usbbuffer[128];
const char lipsum[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Fusce maximus dapibus nisi, nec erat curae.";

int main(void) {
	if (!init()) SOS();
	
	Process bz;
	init_process(&bz, buzzer, BUZZER_STACK_BASE, BUZZER_STACK_SIZE);
	dispatch_process(&bz);

	start_navigation();

	// for (int i = 0; i < 64; ++i) {
	// 	EEPROM_Request r;
	// 	eeprom_write_request(&r, i, &i, 1);
	// 	eeprom_wait_until_done(&r);
	// }

	uint32_t time = time_read_ticks();
	for (int i = 0; i < 256; ++i) {
		uint8_t ff = 0xFF;
		EEPROM_Request r;
		uint8_t value = 0;
		eeprom_read_request(&r, i, &value, 1);
		eeprom_wait_until_done(&r);
		if (value != 0xFF) {
			EEPROM_Request r2;
			eeprom_write_request(&r2, i, &ff, 1);
			eeprom_wait_until_done(&r2);
		}
	}
	time = time_read_ticks() - time;
	volatile uint32_t timeus = TIME_US_MULT * time;


	EEPROM_Request read_request;
	eeprom_read_request(&read_request, 0, usbbuffer, ARRLEN(usbbuffer));
	eeprom_wait_until_done(&read_request);

	while (1) {
		rtos_delay_ms(1);
	}
	
	shutdown();
}

bool selftest() {
	bool pass = true;
	
	if (!check_imu()) pass = false;
	
	// check barometer
	if (!baro_check(&baro_desc)) pass = false;
	
	// check eeprom
	if (!check_eeprom()) pass = false;
	
	// check flash
	if (!flash_check(&flash_desc)) pass = false;
	
	return pass;
}

const DMA_Descriptor dma_spi_desc = {
	.txchannel = SPI_DMA_TX_CHANNEL,
	.rxchannel = SPI_DMA_RX_CHANNEL,
	.txtrig = SPI_DMA_TX_TRIGGER,
	.rxtrig = SPI_DMA_RX_TRIGGER,
	.priority = SPI_DMA_PRIORITY
};

bool init() {
	// init spi
	port_wrconfig(SPI_PORT_A, SPI_PMUX_A, SPI_PINS_A);
	port_wrconfig(SPI_PORT_B, SPI_PMUX_B, SPI_PINS_B);
	port_set(BARO_SS_PORT, BARO_SS_PIN);
	port_set(FLASH_SS_PORT, FLASH_SS_PIN);
	port_set_output(BARO_SS_PORT, BARO_SS_PIN);
	port_set_output(FLASH_SS_PORT, FLASH_SS_PIN);
	if (!spi_init(SPI_SERCOM, 0, 0, SPI_DIPO, SPI_DOPO, SPI_BAUD)) SOS();

	
	// init dma
	dma_init();

	if (!serial_init()) SOS();
	
	// init imu and magnetometer
	// {
	// 	IMU_ON_BUS();
	// 	Pin imu_ss = {.port = IMU_SS_PORT, .pin = IMU_SS_PIN};
	// 	if (!imu_icm20948_init(&imu_desc, SPI_SERCOM, imu_ss)) SOS();
	// 	if (!mag_icm20948_init(&imu_desc)) SOS();
	// 	IMU_OFF_BUS();
	// }
	if (!imu_init()) SOS();

	// init barometer
	{
		Pin baro_ss = {.port = BARO_SS_PORT, .pin = BARO_SS_PIN};
		if (!baro_init(&baro_desc, SPI_SERCOM, baro_ss)) SOS();
	}
	
	// init eeprom
	// {
	// 	Pin eeprom_ss = {.port = EEPROM_SS_PORT, .pin = EEPROM_SS_PIN};
	// 	eeprom_init(&eeprom_desc, SPI_SERCOM, eeprom_ss, SPI_EEPROM_SIZE);
	// }
	init_eeprom();
	
	// init flash
	{
		Pin flash_ss = {.port = FLASH_SS_PORT, .pin = FLASH_SS_PIN};
		flash_init(&flash_desc, SPI_SERCOM, flash_ss, SPI_FLASH_SIZE, SPI_FLASH_DENSITY_CODE);
	}
	
	// start spi bus manager
	// busmanager_new_process(&spiproc, SPIMGR_STACK_BASE, SPIMGR_STACK_SIZE, SPI_SERCOM, &dma_spi_desc, spi_process_exec_function);
	busmanager_new_process(&spiproc, SPIMGR_STACK_BASE, SPIMGR_STACK_SIZE, SPI_SERCOM, &dma_spi_desc);
	
	
	return selftest();
}

void shutdown() {
	serial_quit();
}

void buzzer() {
	if (!pwm_init_tc(TC7_REGS, PWM_PRESCALER_DIV256)) SOS();
	if (!pwm_set_frequency_tc(TC7_REGS, 2825)) SOS();
	if (!pwm_set_duty_tc(TC7_REGS, 1, 0.0f)) SOS();
	if (!pwm_enable_tc(TC7_REGS)) SOS();
	
	port_wrconfig(PORT_PORTB, PORT_PMUX_E, PORT_PB23);
	
	for (int i = 0; i < 2; ++i) {
		pwm_set_duty_tc(TC7_REGS, 1, 0.5f);
		rtos_delay_ms(75);
		pwm_set_duty_tc(TC7_REGS, 1, 0.0f);
		rtos_delay_ms(75);
	}
}