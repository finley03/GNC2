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
    int32_t DebugInt1; // Debug Int 1
    uint32_t GlobalHash; // EEPROM Layout Hash
    uint32_t StartupCount; // Power Cycles
    uint16_t RCchannel1; // RC Channel 1
    uint16_t RCchannel2; // RC Channel 2
    uint16_t RCchannel3; // RC Channel 3
    uint16_t RCchannel4; // RC Channel 4
    uint16_t RCchannel5; // RC Channel 5
    uint16_t RCchannel6; // RC Channel 6
    uint16_t RCchannel7; // RC Channel 7
    uint16_t RCchannel8; // RC Channel 8
    uint16_t RCchannel9; // RC Channel 9
    uint16_t RCchannel10; // RC Channel 10
    uint16_t RCchannel11; // RC Channel 11
    uint16_t RCchannel12; // RC Channel 12
    uint16_t RCchannel13; // RC Channel 13
    uint16_t RCchannel14; // RC Channel 14
    uint16_t RCchannel15; // RC Channel 15
    uint16_t RCchannel16; // RC Channel 16
    bool DisableBuzzer; // Disable Buzzer
    bool DisableLED; // Disable LED
    bool CleanShutdown; // Clean Shutdown
    bool RunNavProcess; // Run Navigation Process
    bool RunControlProcess; // Run Control Process
    bool RunGuidanceProcess; // Run Guidance Process
    bool RCRXFailsafe; // RC Receiver failsafe
    bool DebugBool1; // Debug Bool 1
    bool DebugBool2; // Debug Bool 2
} Global_Variables;

#define __GLOBAL_I64_COUNT 0
#define __GLOBAL_U64_COUNT 0
#define __GLOBAL_F64_COUNT 0
#define __GLOBAL_I32_COUNT 13
#define __GLOBAL_U32_COUNT 2
#define __GLOBAL_FP32_COUNT 0
#define __GLOBAL_F32_COUNT 0
#define __GLOBAL_I16_COUNT 0
#define __GLOBAL_U16_COUNT 16
#define __GLOBAL_I8_COUNT 0
#define __GLOBAL_U8_COUNT 0
#define __GLOBAL_BOOL_COUNT 9

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
    Global_ID_DebugInt1,
    Global_ID_GlobalHash,
    Global_ID_StartupCount,
    Global_ID_RCchannel1,
    Global_ID_RCchannel2,
    Global_ID_RCchannel3,
    Global_ID_RCchannel4,
    Global_ID_RCchannel5,
    Global_ID_RCchannel6,
    Global_ID_RCchannel7,
    Global_ID_RCchannel8,
    Global_ID_RCchannel9,
    Global_ID_RCchannel10,
    Global_ID_RCchannel11,
    Global_ID_RCchannel12,
    Global_ID_RCchannel13,
    Global_ID_RCchannel14,
    Global_ID_RCchannel15,
    Global_ID_RCchannel16,
    Global_ID_DisableBuzzer,
    Global_ID_DisableLED,
    Global_ID_CleanShutdown,
    Global_ID_RunNavProcess,
    Global_ID_RunControlProcess,
    Global_ID_RunGuidanceProcess,
    Global_ID_RCRXFailsafe,
    Global_ID_DebugBool1,
    Global_ID_DebugBool2,
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
    "Debug Int 1",\
    "EEPROM Layout Hash",\
    "Power Cycles",\
    "RC Channel 1",\
    "RC Channel 2",\
    "RC Channel 3",\
    "RC Channel 4",\
    "RC Channel 5",\
    "RC Channel 6",\
    "RC Channel 7",\
    "RC Channel 8",\
    "RC Channel 9",\
    "RC Channel 10",\
    "RC Channel 11",\
    "RC Channel 12",\
    "RC Channel 13",\
    "RC Channel 14",\
    "RC Channel 15",\
    "RC Channel 16",\
    "Disable Buzzer",\
    "Disable LED",\
    "Clean Shutdown",\
    "Run Navigation Process",\
    "Run Control Process",\
    "Run Guidance Process",\
    "RC Receiver failsafe",\
    "Debug Bool 1",\
    "Debug Bool 2",\
};

#define __GLOBAL_ID_START 1
#define __GLOBAL_I64_ID_START 1
#define __GLOBAL_I64_ID_END 1
#define __GLOBAL_U64_ID_START 1
#define __GLOBAL_U64_ID_END 1
#define __GLOBAL_F64_ID_START 1
#define __GLOBAL_F64_ID_END 1
#define __GLOBAL_I32_ID_START 1
#define __GLOBAL_I32_ID_END 14
#define __GLOBAL_U32_ID_START 14
#define __GLOBAL_U32_ID_END 16
#define __GLOBAL_FP32_ID_START 16
#define __GLOBAL_FP32_ID_END 16
#define __GLOBAL_F32_ID_START 16
#define __GLOBAL_F32_ID_END 16
#define __GLOBAL_I16_ID_START 16
#define __GLOBAL_I16_ID_END 16
#define __GLOBAL_U16_ID_START 16
#define __GLOBAL_U16_ID_END 32
#define __GLOBAL_I8_ID_START 32
#define __GLOBAL_I8_ID_END 32
#define __GLOBAL_U8_ID_START 32
#define __GLOBAL_U8_ID_END 32
#define __GLOBAL_BOOL_ID_START 32
#define __GLOBAL_BOOL_ID_END 41
#define __GLOBAL_ID_END 41

#define __GLOBAL_I32_START_VARIABLE AccelerationXRaw
#define __GLOBAL_U32_START_VARIABLE GlobalHash
#define __GLOBAL_U16_START_VARIABLE RCchannel1
#define __GLOBAL_BOOL_START_VARIABLE DisableBuzzer

// CRC32 of the source JSON used to create this file
// Used to identify when the globals structure changes
#define __GLOBAL_HASH 0x5006f37a

#endif