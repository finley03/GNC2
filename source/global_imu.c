#include "global_imu.h"
#include "util.h"
#include "RTOSDrivers/dma_imu_icm20948.h"

#define ACCEL_RAW_RANGE 32768
#define ACCEL_FS_SEL_HIGH_THRESHOLD (0.9f * ACCEL_RAW_RANGE)
#define ACCEL_FS_SEL_LOW_THRESHOLD (0.4f * ACCEL_RAW_RANGE)
#define ACCEL_FS_SEL_MIN 0
#define ACCEL_FS_SEL_MAX 3
#define ACCEL_MIN_RANGE_G 2
#define GYRO_RAW_RANGE 32768
#define GYRO_FS_SEL_HIGH_THRESHOLD (0.9f * GYRO_RAW_RANGE)
#define GYRO_FS_SEL_LOW_THRESHOLD (0.4f * GYRO_RAW_RANGE)
#define GYRO_FS_SEL_MIN 0
#define GYRO_FS_SEL_MAX 3
#define GYRO_MIN_RANGE_MICRORADIANS 4363323

IMU_ICM20948_Descriptor imu_desc;
IMU_Request data_request, accel_range_request, gyro_range_request;
IMU_ICM20948_Raw_Data raw_data;
int accel_fs_sel = 0;
int gyro_fs_sel = 0;
uint8_t accel_config_1 = 0;
uint8_t gyro_config_1 = 0;

// extern void imu_icm20948_read(IMU_ICM20948_Descriptor* desc, uint8_t address, int count, uint8_t* data);
#define SWAPI16(x) ((int16_t)(((x << 8) & 0xFF00) | ((x >> 8) & 0x00FF)))

bool imu_init() {
	port_set(IMU_SS_PORT, IMU_SS_PIN);
	port_set_output(IMU_SS_PORT, IMU_SS_PIN);
	IMU_OFF_BUS();
	port_set_output(IMU_BUSEN_PORT, IMU_BUSEN_PIN);

    bool ret = true;
    IMU_ON_BUS();
    Pin imu_ss = {.port = IMU_SS_PORT, .pin = IMU_SS_PIN};
    if (!imu_icm20948_init(&imu_desc, SPI_SERCOM, imu_ss) || !mag_icm20948_init(&imu_desc))
        ret = false;
    IMU_OFF_BUS();
    return ret;
}

bool check_imu() {
    bool pass = true;
    // check imu, and if good check internal magnetometer
	IMU_ON_BUS();
	if (!imu_icm20948_check(&imu_desc)) pass = false;
	if (pass)
		if (!mag_icm20948_check(&imu_desc)) pass = false;
	IMU_OFF_BUS();
    return pass;
}

void request_imu_data() {
    // if there is still a pending request, wait.
    // if (!imu_icm20948_done(&request)) imu_icm20948_wait_until_done(&request);
    imu_icm20948_request(&spiproc, &data_request, &imu_desc, SPI_DEVICE_IMU, IMU_Read, 0, ACCEL_XOUT_H, sizeof(raw_data), &raw_data);
}

void wait_for_imu_data() {
    imu_icm20948_wait_until_done(&data_request);

    // float accel_multiplier = __G__ * (ACCEL_MIN_RANGE_G << accel_fs_sel) / ACCEL_RAW_RANGE;
    int32_t accel_multiplier = (__G__ * ACCEL_MIN_RANGE_G) >> (15 - accel_fs_sel);
    globals.AccelerationXRaw = SWAPI16(raw_data.accel_x) * accel_multiplier;
    globals.AccelerationYRaw = SWAPI16(raw_data.accel_y) * accel_multiplier;
    globals.AccelerationZRaw = SWAPI16(raw_data.accel_z) * accel_multiplier;

    // float gyro_multiplier = (float)(GYRO_MIN_RANGE_DEGREES << gyro_fs_sel) / GYRO_RAW_RANGE;
    int32_t gyro_multiplier = (GYRO_MIN_RANGE_MICRORADIANS) >> (15 - gyro_fs_sel);
    globals.AngularVelocityXRaw = SWAPI16(raw_data.gyro_x) * gyro_multiplier;
    globals.AngularVelocityYRaw = SWAPI16(raw_data.gyro_y)* gyro_multiplier;
    globals.AngularVelocityZRaw = SWAPI16(raw_data.gyro_z) * gyro_multiplier;

    // adjust accelerometer range
    int16_t accel_max = UMAX_3(SWAPI16(raw_data.accel_x), SWAPI16(raw_data.accel_y), SWAPI16(raw_data.accel_z));
    if (accel_fs_sel != ACCEL_FS_SEL_MAX && accel_max > ACCEL_FS_SEL_HIGH_THRESHOLD) {
        ++accel_fs_sel;
        accel_config_1 = 0b00000110 & (uint8_t)(accel_fs_sel << 1);
        imu_icm20948_request(&spiproc, &accel_range_request, &imu_desc, SPI_DEVICE_IMU, IMU_Write, 2, ACCEL_CONFIG, 1, &accel_config_1);
    }
    else if (accel_fs_sel != ACCEL_FS_SEL_MIN && accel_max < ACCEL_FS_SEL_LOW_THRESHOLD) {
        --accel_fs_sel;
        accel_config_1 = 0b00000110 & (uint8_t)(accel_fs_sel << 1);
        imu_icm20948_request(&spiproc, &accel_range_request, &imu_desc, SPI_DEVICE_IMU, IMU_Write, 2, ACCEL_CONFIG, 1, &accel_config_1);
    }

    // adjust gyro range
    int16_t gyro_max = UMAX_3(SWAPI16(raw_data.gyro_x), SWAPI16(raw_data.gyro_y), SWAPI16(raw_data.gyro_z));
    if (gyro_fs_sel != GYRO_FS_SEL_MAX && gyro_max > GYRO_FS_SEL_HIGH_THRESHOLD) {
        ++gyro_fs_sel;
        gyro_config_1 = 0b00000110 & (uint8_t)(gyro_fs_sel << 1);
        imu_icm20948_request(&spiproc, &gyro_range_request, &imu_desc, SPI_DEVICE_IMU, IMU_Write, 2, GYRO_CONFIG_1, 1, &gyro_config_1);
        led_on();
    }
    else if (gyro_fs_sel != GYRO_FS_SEL_MIN && gyro_max < GYRO_FS_SEL_LOW_THRESHOLD) {
        --gyro_fs_sel;
        gyro_config_1 = 0b00000110 & (uint8_t)(gyro_fs_sel << 1);
        imu_icm20948_request(&spiproc, &gyro_range_request, &imu_desc, SPI_DEVICE_IMU, IMU_Write, 2, GYRO_CONFIG_1, 1, &gyro_config_1);
        led_off();
    }

}