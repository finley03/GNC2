//----------WARNING----------//
// THIS FILE IS AUTO-GENERATED
// DO NOT MODIFY

#ifndef GLOBAL_VARIABLES_H
#define GLOBAL_VARIABLES_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    __I64,
    __U64,
    __F64,
    __I32,
    __U32,
    __F32,
    __I16,
    __U16,
    __I8,
    __U8,
    __BOOL,
    __BOOL_PACKED,
} __Global_Variable_Types;

typedef struct __attribute__((packed, aligned(8))) {
    uint32_t GlobalHash; // EEPROM Layout Hash
    uint32_t StartupCount; // Power Cycles
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
    bool EnableBuzzer; // Enable Buzzer
    bool CleanShutdown; // Clean Shutdown
} Global_Variables;

#define __GLOBAL_I64_COUNT 0
#define __GLOBAL_U64_COUNT 0
#define __GLOBAL_F64_COUNT 0
#define __GLOBAL_I32_COUNT 0
#define __GLOBAL_U32_COUNT 2
#define __GLOBAL_F32_COUNT 12
#define __GLOBAL_I16_COUNT 0
#define __GLOBAL_U16_COUNT 0
#define __GLOBAL_I8_COUNT 0
#define __GLOBAL_U8_COUNT 0
#define __GLOBAL_BOOL_COUNT 2
#define __GLOBAL_BOOL_PACKED_COUNT 0

typedef enum {
    Global_ID_INVALID,
    Global_ID_GlobalHash,
    Global_ID_StartupCount,
    Global_ID_AccelerationXRaw,
    Global_ID_AccelerationYRaw,
    Global_ID_AccelerationZRaw,
    Global_ID_AngularVelocityXRaw,
    Global_ID_AngularVelocityYRaw,
    Global_ID_AngularVelocityZRaw,
    Global_ID_AccelerationXCalibrated,
    Global_ID_AccelerationYCalibrated,
    Global_ID_AccelerationZCalibrated,
    Global_ID_AngularVelocityXCalibrated,
    Global_ID_AngularVelocityYCalibrated,
    Global_ID_AngularVelocityZCalibrated,
    Global_ID_EnableBuzzer,
    Global_ID_CleanShutdown,
} Global_Variable_IDs;

#define __GLOBAL_ID_START 1
#define __GLOBAL_I64_ID_START 1
#define __GLOBAL_I64_ID_END 1
#define __GLOBAL_U64_ID_START 1
#define __GLOBAL_U64_ID_END 1
#define __GLOBAL_F64_ID_START 1
#define __GLOBAL_F64_ID_END 1
#define __GLOBAL_I32_ID_START 1
#define __GLOBAL_I32_ID_END 1
#define __GLOBAL_U32_ID_START 1
#define __GLOBAL_U32_ID_END 3
#define __GLOBAL_F32_ID_START 3
#define __GLOBAL_F32_ID_END 15
#define __GLOBAL_I16_ID_START 15
#define __GLOBAL_I16_ID_END 15
#define __GLOBAL_U16_ID_START 15
#define __GLOBAL_U16_ID_END 15
#define __GLOBAL_I8_ID_START 15
#define __GLOBAL_I8_ID_END 15
#define __GLOBAL_U8_ID_START 15
#define __GLOBAL_U8_ID_END 15
#define __GLOBAL_BOOL_ID_START 15
#define __GLOBAL_BOOL_ID_END 17
#define __GLOBAL_BOOL_PACKED_ID_START 17
#define __GLOBAL_BOOL_PACKED_ID_END 17
#define __GLOBAL_ID_END 17

#define __GLOBAL_U32_START_VARIABLE GlobalHash
#define __GLOBAL_F32_START_VARIABLE AccelerationXRaw
#define __GLOBAL_BOOL_START_VARIABLE EnableBuzzer

// CRC32 of the source JSON used to create this file
// Used to identify when the globals structure changes
#define __GLOBAL_HASH 0xb7177d51

#endif