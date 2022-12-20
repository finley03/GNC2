//----------WARNING----------//
// THIS FILE IS AUTO-GENERATED
// DO NOT MODIFY

#ifndef GLOBAL_VARIABLES_H
#define GLOBAL_VARIABLES_H

#include <stdint.h>
#include <stdbool.h>

typedef struct __attribute__((packed, aligned(8))) {
    int32_t StartupCount; // Power Cycles
    float AccelerationXRaw; // Raw X Direction Acceleration
    float AccelerationYRaw; // Raw Y Direction Acceleration
    float AccelerationZRaw; // Raw Z Direction Acceleration
    float AngularVelocityXRaw; // Raw X Axis Angular Velocity
    float AngularVelocityYRaw; // Raw Y Axis Angular Velocity
    float AngularVelocityZRaw; // Raw X Axis Angular Velocity
    float AccelerationXCalibrated; // Calibrated X Direction Acceleration
    float AccelerationYCalibrated; // Calibrated Y Direction Acceleration
    float AccelerationZCalibrated; // Calibrated Z Direction Acceleration
    float AngularVelocityXCalibrated; // Calibrated X Axis Angular Velocity
    float AngularVelocityYCalibrated; // Calibrated Y Axis Angular Velocity
    float AngularVelocityZCalibrated; // Calibrated X Axis Angular Velocity
    bool boolTest; // Test Bool
    bool boolTest2 :1; // Test Bool
    bool boolTest3 :1; // Test Bool
} Global_Variables;

#define __GLOBAL_I64_COUNT 0
#define __GLOBAL_U64_COUNT 0
#define __GLOBAL_F64_COUNT 0
#define __GLOBAL_I32_COUNT 1
#define __GLOBAL_U32_COUNT 0
#define __GLOBAL_F32_COUNT 12
#define __GLOBAL_I16_COUNT 0
#define __GLOBAL_U16_COUNT 0
#define __GLOBAL_I8_COUNT 0
#define __GLOBAL_U8_COUNT 0
#define __GLOBAL_BOOL_COUNT 1
#define __GLOBAL_BOOL_PACKED_COUNT 2

// CRC32 of the source JSON used to create this file
// Used to identify when the globals structure changes
#define __GLOBAL_HASH 0x8cc2e26c

#endif