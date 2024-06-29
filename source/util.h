#ifndef UTIL_H
#define UTIL_H

// includes

#include "RTOS/rtos.h"
#include "Drivers/dma.h"
#include "Drivers/eeprom_cat25.h"
// #include "Drivers/imu_icm20948.h"
#include "Drivers/flash_at25.h"
#include "Drivers/baro_ms56.h"
#include "RTOSDrivers/bus_manager.h"

#include "globals.h"
#include "fixedpoint.h"

// include hardware definition
#ifndef HARDWARE_H
#define HARDWARE_H
#include HARDWARE_DEFINITION
#endif

#define ABS(a) ((a < 0) ? -(a) : a)
#define MAX_2(a, b) ((a > b) ? a : b)
#define MIN_2(a, b) ((b > a) ? a : b);
#define MAX_3(a, b, c) (MAX_2(MAX_2(a, b), c))
#define MIN_3(a, b, c) (MIN_2(MIN_2(a, b), c))
#define UMAX_2(a, b) MAX_2(ABS(a), ABS(b))
#define UMIN_2(a, b) MIN_2(ABS(a), ABS(b))
#define UMAX_3(a, b, c) MAX_3(ABS(a), ABS(b), ABS(c))
#define UMIN_3(a, b, c) MIN_3(ABS(a), ABS(b), ABS(c))
#define ARRLEN(a) (sizeof(a)/sizeof(a[0]))
#define STRLEN(a) (ARRLEN(a)-1)
// #define __G__ 9.80665f
#define __G__ 9806650 // um/s^2

#define USB_STACK_BASE USER_STACK_BASE
#define BUZZER_STACK_BASE (USB_STACK_BASE+USB_STACK_SIZE)
#define SPIMGR_STACK_BASE (BUZZER_STACK_BASE+BUZZER_STACK_SIZE)
#define RC_RECEIVER_STACK_BASE (SPIMGR_STACK_BASE+SPIMGR_STACK_SIZE)
#define NAVIGATION_STACK_BASE (RC_RECEIVER_STACK_BASE+RC_RECEIVER_STACK_SIZE)

// external declarations
// extern IMU_ICM20948_Descriptor imu_desc;
// extern EEPROM_Descriptor eeprom_desc;
extern FLASH_Descriptor flash_desc;
extern Barometer_Descriptor baro_desc;

// extern DMA_Descriptor_Type txdesc2;
// extern DMA_Descriptor_Type rxdesc2;

extern BusManager_Process spiproc;

extern const uint8_t zero;
extern uint8_t datasink;

#endif