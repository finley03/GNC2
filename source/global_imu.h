#ifndef GLOBAL_IMU_H
#define GLOBAL_IMU_H

#include <stdbool.h>

bool init_imu();
bool check_imu();
void request_imu_data();
void wait_for_imu_data();

#endif