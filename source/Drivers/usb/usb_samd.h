#ifndef USB_SAMD_H
#define USB_SAMD_H

//#include <parts.h>
//#include <io.h>
#include "usb.h"
#include <sam.h>

extern usb_descriptor_device_registers_t usb_endpoints[];
extern const uint8_t usb_num_endpoints;

#define USB_ALIGN __attribute__((__aligned__(4)))

#define USB_ENDPOINTS(NUM_EP) \
	const uint8_t usb_num_endpoints = (NUM_EP); \
	usb_descriptor_device_registers_t usb_endpoints[(NUM_EP)+1];

void* samd_serial_number_string_descriptor();

#endif