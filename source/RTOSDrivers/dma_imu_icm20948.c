#include "dma_imu_icm20948.h"
#include "Drivers/spi.h"
#include "RTOS/rtos.h"
#include "rtos_dma.h"
#include "util.h"
#include PORT_DRIVER
#include TIME_DRIVER

void dma_imu_icm20948_read(IMU_ICM20948_Descriptor* imudesc, DMA_Descriptor* dmadesc, uint8_t address, int count, void* writeback) {
	address |= IMU_READ_MASK;
	register sercom_registers_t* sercom = imudesc->sercom;
	
	// create descriptor for tx channel
	dma_create_descriptor(&(dma_descriptor[dmadesc->txchannel]), false, false, DMA_BEATSIZE_BYTE, 1,
	&address, &(sercom->SPIM.SERCOM_DATA), &txdesc2);
	dma_create_descriptor(&txdesc2, false, false, DMA_BEATSIZE_BYTE, count,
	&zero, &(sercom->SPIM.SERCOM_DATA), DMA_NEXTDESCRIPTOR_NONE);
	// create descriptor for rx channel
	dma_create_descriptor(&(dma_descriptor[dmadesc->rxchannel]), false, false, DMA_BEATSIZE_BYTE, 1,
	&(sercom->SPIM.SERCOM_DATA), &datasink, &rxdesc2);
	dma_create_descriptor(&rxdesc2, false, true, DMA_BEATSIZE_BYTE, count,
	&(sercom->SPIM.SERCOM_DATA), writeback, DMA_NEXTDESCRIPTOR_NONE);
	
	dma_spi_transaction(sercom, &(imudesc->sspin), dmadesc);
}

void dma_imu_icm20948_write(IMU_ICM20948_Descriptor* imudesc, DMA_Descriptor* dmadesc, uint8_t address, int count, void* data) {
	address &= IMU_WRITE_MASK;
	register sercom_registers_t* sercom = imudesc->sercom;
	
	// create descriptor for tx channel
	dma_create_descriptor(&(dma_descriptor[dmadesc->txchannel]), false, false, DMA_BEATSIZE_BYTE, 1,
	&address, &(sercom->SPIM.SERCOM_DATA), &txdesc2);
	dma_create_descriptor(&txdesc2, true, false, DMA_BEATSIZE_BYTE, count,
	data, &(sercom->SPIM.SERCOM_DATA), DMA_NEXTDESCRIPTOR_NONE);
	// create descriptor for rx channel
	dma_create_descriptor(&(dma_descriptor[dmadesc->rxchannel]), false, false, DMA_BEATSIZE_BYTE, 1,
	&(sercom->SPIM.SERCOM_DATA), &datasink, &rxdesc2);
	dma_create_descriptor(&rxdesc2, false, false, DMA_BEATSIZE_BYTE, count,
	&(sercom->SPIM.SERCOM_DATA), &datasink, DMA_NEXTDESCRIPTOR_NONE);
	
	dma_spi_transaction(sercom, &(imudesc->sspin), dmadesc);
}

bool dma_imu_icm20948_process_request(IMU_ICM20948_Descriptor* imudesc, DMA_Descriptor* dmadesc, IMU_Request* request, BusManager_Transaction_Request* busrequest) {
	IMU_ON_BUS();
	
	// set user bank
	uint8_t bank = (request->user_bank) << 4;
	// dma_imu_icm20948_write(imudesc, dmadesc, REG_BANK_SEL, 1, &bank);
	imu_icm20948_user_bank(imudesc, request->user_bank);
	
	// switch request type
	switch (request->type) {
		case IMU_Read:
		// dma_imu_icm20948_read(imudesc, dmadesc, request->address, request->count, request->data);
		imu_icm20948_read(imudesc, request->address, request->count, request->data);
		break;
		
		case IMU_Write:
		// dma_imu_icm20948_write(imudesc, dmadesc, request->address, request->count, request->data);
		imu_icm20948_write(imudesc, request->address, request->count, request->data);
		break;
	}
	
	IMU_OFF_BUS();
	
	return true;
}

void imu_icm20948_request(BusManager_Process* proc, IMU_Request* request, IMU_ICM20948_Descriptor* desc, int device_id, IMU_Transaction_Type type, uint8_t user_bank, uint32_t address, int count, void* data) {
	request->type = type;
	request->address = address;
	request->count = count;
	request->data = data;
	request->user_bank = user_bank;
	busmanager_request_transaction(proc, &(request->request), device_id, desc, request, dma_imu_icm20948_process_request);
}

void imu_icm20948_wait_until_done(IMU_Request* request) {
	wait_until(&(request->request.status), BusManager_Request_CompleteNotBusy, U8_MASK, Process_Wait_Until_Equal);
}

bool imu_icm20948_done(IMU_Request* request) {
	return request->request.status == BusManager_Request_CompleteNotBusy;
}

bool dma_mag_icm20948_process_request(IMU_ICM20948_Descriptor* imudesc, DMA_Descriptor* dmadesc, MAG_Request* request, BusManager_Transaction_Request* busrequest) {
	IMU_ON_BUS();
	
	uint8_t bank;
	
	switch (request->type) {
		case MAG_Read:
		// second time
		if (request->request_sent) {
			bank = 0 << 4;
			dma_imu_icm20948_write(imudesc, dmadesc, REG_BANK_SEL, 1, &bank);
			dma_imu_icm20948_read(imudesc, dmadesc, EXT_SLV_SENS_DATA_00, request->count, request->data);
			
			IMU_OFF_BUS();
			return true;
		}
		// first time
		else {
			bank = 3 << 4;
			uint8_t requestdata[3] = {0x0c | 0x80, request->address, 0x80 | request->count};
			dma_imu_icm20948_write(imudesc, dmadesc, REG_BANK_SEL, 1, &bank);
			dma_imu_icm20948_write(imudesc, dmadesc, I2C_SLV0_ADDR, 3, requestdata);
			// indicate request sent
			request->request_sent = true;
			// wait
			busrequest->busy_until = time_read_ticks() + (uint32_t)(1000) * TIME_TICKS_US_MULT;
			busrequest->busy = true;
			
			IMU_OFF_BUS();
			return false;
		}
		break;
		
		case MAG_Write:
		{
			bank = 3 << 4;
			uint8_t requestdata[3] = {0x0c, request->address, 0x80 | 0x01};
			dma_imu_icm20948_write(imudesc, dmadesc, REG_BANK_SEL, 1, &bank);
			dma_imu_icm20948_write(imudesc, dmadesc, I2C_SLV0_DO, 1, request->data);
			dma_imu_icm20948_write(imudesc, dmadesc, I2C_SLV0_ADDR, 3, requestdata);
			//// set slave address
			//imu_write_byte(imudesc, I2C_SLV0_ADDR, 0x0c);
			//// set address to write in magnetometer
			//imu_write_byte(imudesc, I2C_SLV0_REG, request->address);
			//// store data to write
			//imu_write_byte(imudesc, I2C_SLV0_DO, *(uint8_t*)(request->data));
			//// enable i2c and send byte
			//imu_write_byte(imudesc, I2C_SLV0_CTRL, 0x80 | 0x01);
			// wait
			busrequest->busy_until = time_read_ticks() + (uint32_t)(10000) * TIME_TICKS_US_MULT;
			busrequest->busy = true;
			
			//rtos_delay_us(10000);
			//mag_write_byte(imudesc, request->address, *(uint8_t*)(request->data));
			//mag_write_byte(imudesc, MAG_CNTL2, 0x08);
			
			IMU_OFF_BUS();
			return true;
		}
		break;
		
		default:
		break;
	}
	
	IMU_OFF_BUS();
	return true;
}

void mag_icm20948_request(BusManager_Process* proc, MAG_Request* request, IMU_ICM20948_Descriptor* desc, int device_id, MAG_Transaction_Type type, uint32_t address, int count, void* data) {
	request->type = type;
	request->address = address;
	request->count = count;
	request->data = data;
	request->request_sent = false;
	busmanager_request_transaction(proc, &(request->request), device_id, desc, request, dma_mag_icm20948_process_request);
}

void mag_icm20948_wait_until_done(MAG_Request* request) {
	wait_until(&(request->request.status), BusManager_Request_CompleteNotBusy, U8_MASK, Process_Wait_Until_Equal);
}