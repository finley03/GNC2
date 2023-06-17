//----------WARNING----------//
// THIS FILE IS AUTO-GENERATED
// DO NOT MODIFY

#ifndef GLOBAL_VARIABLES_H
#define GLOBAL_VARIABLES_H

#include <stdint.h>
#include <stdbool.h>

#include <fixedpoint.h>

typedef enum {
    __I64,
    __U64,
    __F64,
    __I32,
    __U32,
    __FP32,
    __F32,
    __I16,
    __U16,
    __I8,
    __U8,
    __BOOL,
} __Global_Variable_Types;

typedef struct __attribute__((packed, aligned(8))) {
    int32_t AccelerationXRaw; // Raw X Direction Acceleration
    int32_t AccelerationYRaw; // Raw Y Direction Acceleration
    int32_t AccelerationZRaw; // Raw Z Direction Acceleration
    int32_t AngularVelocityXRaw; // Raw X Axis Angular Velocity
    int32_t AngularVelocityYRaw; // Raw Y Axis Angular Velocity
    int32_t AngularVelocityZRaw; // Raw Z Axis Angular Velocity
    int32_t AccelerationXCalibrated; // Calibrated X Direction Acceleration
    int32_t AccelerationYCalibrated; // Calibrated Y Direction Acceleration
    int32_t AccelerationZCalibrated; // Calibrated Z Direction Acceleration
    int32_t AngularVelocityXCalibrated; // Calibrated X Axis Angular Velocity
    int32_t AngularVelocityYCalibrated; // Calibrated Y Axis Angular Velocity
    int32_t AngularVelocityZCalibrated; // Calibrated Z Axis Angular Velocity
    uint32_t GlobalHash; // EEPROM Layout Hash
    uint32_t StartupCount; // Power Cycles
    bool DisableBuzzer; // Disable Buzzer
    bool DisableLED; // Disable LED
    bool CleanShutdown; // Clean Shutdown
} Global_Variables;

#define __GLOBAL_I64_COUNT 0
#define __GLOBAL_U64_COUNT 0
#define __GLOBAL_F64_COUNT 0
#define __GLOBAL_I32_COUNT 12
#define __GLOBAL_U32_COUNT 2
#define __GLOBAL_FP32_COUNT 0
#define __GLOBAL_F32_COUNT 0
#define __GLOBAL_I16_COUNT 0
#define __GLOBAL_U16_COUNT 0
#define __GLOBAL_I8_COUNT 0
#define __GLOBAL_U8_COUNT 0
#define __GLOBAL_BOOL_COUNT 3

typedef enum {
    Global_ID_INVALID,
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
    Global_ID_GlobalHash,
    Global_ID_StartupCount,
    Global_ID_DisableBuzzer,
    Global_ID_DisableLED,
    Global_ID_CleanShutdown,
} Global_Variable_IDs;

#define __GLOBAL_VARIABLE_NAMES {\
    "INVALID",\
    "Raw X Direction Acceleration",\
    "Raw Y Direction Acceleration",\
    "Raw Z Direction Acceleration",\
    "Raw X Axis Angular Velocity",\
    "Raw Y Axis Angular Velocity",\
    "Raw Z Axis Angular Velocity",\
    "Calibrated X Direction Acceleration",\
    "Calibrated Y Direction Acceleration",\
    "Calibrated Z Direction Acceleration",\
    "Calibrated X Axis Angular Velocity",\
    "Calibrated Y Axis Angular Velocity",\
    "Calibrated Z Axis Angular Velocity",\
    "EEPROM Layout Hash",\
    "Power Cycles",\
    "Disable Buzzer",\
    "Disable LED",\
    "Clean Shutdown",\
};

#define __GLOBAL_ID_START 1
#define __GLOBAL_I64_ID_START 1
#define __GLOBAL_I64_ID_END 1
#define __GLOBAL_U64_ID_START 1
#define __GLOBAL_U64_ID_END 1
#define __GLOBAL_F64_ID_START 1
#define __GLOBAL_F64_ID_END 1
#define __GLOBAL_I32_ID_START 1
#define __GLOBAL_I32_ID_END 13
#define __GLOBAL_U32_ID_START 13
#define __GLOBAL_U32_ID_END 15
#define __GLOBAL_FP32_ID_START 15
#define __GLOBAL_FP32_ID_END 15
#define __GLOBAL_F32_ID_START 15
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
#define __GLOBAL_BOOL_ID_END 18
#define __GLOBAL_ID_END 18

#define __GLOBAL_I32_START_VARIABLE AccelerationXRaw
#define __GLOBAL_U32_START_VARIABLE GlobalHash
#define __GLOBAL_BOOL_START_VARIABLE DisableBuzzer

// CRC32 of the source JSON used to create this file
// Used to identify when the globals structure changes
#define __GLOBAL_HASH 0x9fc0b1d3

#endif