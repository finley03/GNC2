#include "util.h"
#include "Drivers/pwm.h"
#include "RTOSDrivers/dma_eeprom_cat25.h"
#include "RTOSDrivers/dma_baro_ms56.h"
#include "RTOSDrivers/dma_flash_at25.h"

#include "global_imu.h"
#include "global_ports.h"
#include "global_eeprom.h"
#include "navigation.h"
#include "comms.h"
#include "rc_receiver.h"

#include "fixedpoint.h"

Process bz;

bool init();
void shutdown();
void buzzer();


int main(void) {
	if (!init()) SOS();
	
	init_process(&bz, buzzer, BUZZER_STACK_BASE, BUZZER_STACK_SIZE);
	dispatch_process(&bz);

	start_navigation();

	volatile fp32_t testvalue1 = FP32_FROM_MILLI(10);
	volatile fp32_t testvalue2 = FP32_FROM_INT(14);
	testvalue1 = fp_multiply(testvalue1, testvalue2);

	// run comms loop
	while (comms_loop()) {
		// Execute non-critical periodic code here
		globals.TotalCpuUtilization = (int8_t)get_cpu_utiliztion();
		globals.ProcessCpuUtilization = (int8_t)get_process_cpu_utiliztion();
		globals.OSCpuUtilization = globals.TotalCpuUtilization - globals.ProcessCpuUtilization;
	}

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
	if (!imu_init()) SOS();

	// init barometer
	{
		Pin baro_ss = {.port = BARO_SS_PORT, .pin = BARO_SS_PIN};
		if (!baro_init(&baro_desc, SPI_SERCOM, baro_ss)) SOS();
	}
	
	// init eeprom
	init_eeprom();
	
	// init flash
	{
		Pin flash_ss = {.port = FLASH_SS_PORT, .pin = FLASH_SS_PIN};
		flash_init(&flash_desc, SPI_SERCOM, flash_ss, SPI_FLASH_SIZE, SPI_FLASH_DENSITY_CODE);
	}
	
	// start spi bus manager
	busmanager_new_process(&spiproc, SPIMGR_STACK_BASE, SPIMGR_STACK_SIZE, SPI_SERCOM, &dma_spi_desc);
	
	init_globals();

#ifdef COMMS_TEST
	uint32_t seed; // get seed from startup count
	get_global(Global_ID_StartupCount, &seed);
	srand(seed);
#endif

	if (!receiver_init(PORT2)) SOS();

	if (!init_comms(PORT0)) SOS();
	
	return selftest();
}

void shutdown() {
	serial_quit();
}

volatile int usb_int_counter = 0;

void buzzer() {
	if (!pwm_init_tc(TC7_REGS, PWM_PRESCALER_DIV256)) SOS();
	if (!pwm_set_frequency_tc(TC7_REGS, 2825)) SOS();
	if (!pwm_set_duty_tc(TC7_REGS, 1, 0.0f)) SOS();
	if (!pwm_enable_tc(TC7_REGS)) SOS();
	
	port_wrconfig(PORT_PORTB, PORT_PMUX_E, PORT_PB23);
	
	for (int i = 0; i < 2; ++i) {
		pwm_set_duty_tc(TC7_REGS, 1, 0.5f);
		led_on();
		rtos_delay_ms(75);
		pwm_set_duty_tc(TC7_REGS, 1, 0.0f);
		led_off();
		rtos_delay_ms(75);
	}

	// while (1) {
	// 	rtos_delay_ms(1);
	// 	globals.DebugInt1 = (int)*testbuffer;
	// }

	// while (1) {
	// 	led_on();
	// 	rtos_delay_ms(200);
	// 	led_off();
	// 	rtos_delay_ms(200);
	// }
}