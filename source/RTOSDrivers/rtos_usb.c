#include "rtos_usb.h"
#include "RTOS/rtos.h"
#include "Drivers/usb/usb.h"
#include "Drivers/usb/usb_serial.h"
#include PORT_DRIVER

Process usbproc;
void usb_proc_loop();
extern void usb_handle_function();

// true if an interrupt has occured, will unblock usb_proc_loop
bool usb_interrupt;
// set to true to cause usb_proc_loop to quit next interrupt
bool usb_quit;

// true if usb send operation is in progress
bool usb_send_status;
// buffer to be sent over usb
uint8_t* usb_send_buffer;
// number of bytes to be sent
int usb_send_buffer_count;
// number of bytes actually sent
int usb_send_bytes_sent;
// // number of microseconds to wait
// int usb_send_wait_microseconds;

// true if usb read operation is in progress
bool usb_read_status;
// buffer to be received over usb
uint8_t* usb_read_buffer;
// number of bytes to be read
int usb_read_buffer_count;
// number of bytes actually read
int usb_read_bytes_read;
// // number of microseconds to wait
// int usb_read_wait_microseconds;


void rtos_usb_init(uint32_t stack_base, uint32_t stack_size) {
	NVIC_DisableIRQ(USB_IRQn);

	port_wrconfig(PORT_PORTA, PORT_PMUX_G, PORT_PA24 | PORT_PA25);
	usb_init();
	usb_attach();

	init_process(&usbproc, usb_proc_loop, stack_base, stack_size);
	dispatch_process(&usbproc);
	wait_until_started(&usbproc);
	NVIC_EnableIRQ(USB_IRQn);
}

void usb_proc_loop() {
	usb_interrupt = false;
	usb_quit = false;
	usb_send_status = false;
	usb_read_status = false;
	
	// contained in loop
	while (1) {
		// wait_until(&usb_interrupt, true, BOOL_MASK, Process_Wait_Until_Equal);
		wait_until_true(&usb_interrupt);
		usb_interrupt = false;
		if (usb_quit) break;
		
		usb_handle_function();

		// if send operation is in progress and new data must be sent
		if (usb_send_status && !usb_tx_busy) {
			int bytes_left = usb_send_buffer_count - usb_send_bytes_sent;
			if (bytes_left == 0) usb_send_status = false;
			else usb_send_bytes_sent += usb_serial_send_buffer(usb_send_buffer + usb_send_bytes_sent, bytes_left);
		}

		// if read operation is in progress and data is ready to be read out
		if (usb_read_status && usb_rx_buffer_length != 0) {
			usb_read_bytes_read += usb_serial_read_buffer(usb_read_buffer + usb_read_bytes_read, usb_rx_buffer_length);
			int bytes_left = usb_read_buffer_count - usb_read_bytes_read;
			if (bytes_left == 0) usb_read_status = false;
		}
	}
}

void USB_Handler() {
	NVIC_DisableIRQ(USB_IRQn);
	
	usb_interrupt = true;
}

void rtos_usb_quit() {
	usb_detach();
	usb_interrupt = true;
	usb_quit = true;
	join_process(&usbproc);
}


void rtos_usb_start_send_buffer(uint8_t* buffer, int count) {
	// if send operation is currently in progress wait until it finishes
	if (usb_send_status) wait_until_false(&usb_send_status);
	usb_send_buffer = buffer;
	usb_send_buffer_count = count;
	usb_send_bytes_sent = 0;
	usb_send_status = true;
	// start send
	usb_send_bytes_sent += usb_serial_send_buffer(buffer, count);
}

int rtos_usb_wait_until_send_complete() {
	wait_until_false(&usb_send_status);
	return usb_send_bytes_sent;
}


void rtos_usb_start_read_buffer(uint8_t* buffer, int count) {
	// if read operation is currently in progress wait until it finishes
	if (usb_read_status) wait_until_false(&usb_read_status);
	usb_read_buffer = buffer;
	usb_read_buffer_count = count;
	usb_read_bytes_read = 0;
	usb_read_status = true;
}

int rtos_usb_wait_until_read_complete() {
	wait_until_false(&usb_read_status);
	return usb_read_bytes_read;
}