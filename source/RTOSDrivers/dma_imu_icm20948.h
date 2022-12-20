#ifndef DMA_IMU_ICM20948_H
#define DMA_IMU_ICM20948_H

#include <stdbool.h>
#include <sam.h>
#include "Drivers/imu_icm20948.h"
#include "dma_spi.h"
#include "bus_manager.h"

typedef enum {
	IMU_Read,
	IMU_Write
} IMU_Transaction_Type;

typedef struct {
	BusManager_Transaction_Request request;
	uint32_t address;
	int count;
	void* data;
	IMU_Transaction_Type type;
	uint8_t user_bank;
} IMU_Request;

typedef enum {
	MAG_Read,
	MAG_Write
} MAG_Transaction_Type;

typedef struct {
	BusManager_Transaction_Request request;
	uint32_t address;
	int count;
	void* data;
	MAG_Transaction_Type type;
	bool request_sent;
} MAG_Request;


bool dma_imu_icm20948_process_request(IMU_ICM20948_Descriptor* imudesc, DMA_Descriptor* dmadesc, IMU_Request* request, BusManager_Transaction_Request* busrequest);

void imu_icm20948_request(BusManager_Process* proc, IMU_Request* request, IMU_ICM20948_Descriptor* desc, int device_id, IMU_Transaction_Type type, uint8_t user_bank, uint32_t address, int count, void* data);
void imu_icm20948_wait_until_done(IMU_Request* request);
bool imu_icm20948_done(IMU_Request* request);

bool dma_mag_icm20948_process_request(IMU_ICM20948_Descriptor* imudesc, DMA_Descriptor* dmadesc, MAG_Request* request, BusManager_Transaction_Request* busrequest);

// only one byte is supported for MAG_Write transfers
void mag_icm20948_request(BusManager_Process* proc, MAG_Request* request, IMU_ICM20948_Descriptor* desc, int device_id, MAG_Transaction_Type type, uint32_t address, int count, void* data);
void mag_icm20948_wait_until_done(MAG_Request* request);
bool mag_icm20948_done(MAG_Request* request);

#endif