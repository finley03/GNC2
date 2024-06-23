cmake_minimum_required(VERSION 3.25)

# no specified environment
set(CMAKE_SYSTEM_NAME Generic)
# specify arm processor
set(CMAKE_SYSTEM_PROCESSR arm-none-eabi)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "output")

# set(CMAKE_MAKE_PROGRAM ${CMAKE_CURRENT_SOURCE_DIR}/toolchain/ninja/ninja.exe)
set(CMAKE_GENERATOR Ninja)

# set compilers
set(TOOLCHAIN_PATH ${CMAKE_CURRENT_SOURCE_DIR}/toolchain/arm-none-eabi-gcc/bin)
set(CMAKE_C_COMPILER ${TOOLCHAIN_PATH}/arm-none-eabi-gcc.exe)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PATH}/arm-none-eabi-g++.exe)
# set(CMAKE_OBJCOPY ${TOOLCHAIN_PATH}/arm-none-eabi-objcopy.exe)
# set(CMAKE_OBJDUMP ${TOOLCHAIN_PATH}/arm-none-eabi-objdump.exe)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(LINKER_SCRIPT "lib/SAMD21_DFP/samd21d/gcc/gcc/samd21j17d_flash.ld")

project(GNC0200A)

set(GNC2_SOURCE_FILES
    lib/SAMD21_DFP/samd21d/gcc/system_samd21j17d.c
    lib/SAMD21_DFP/samd21d/gcc/gcc/startup_samd21j17d.c

    source/comms.c
    source/globals.c
    source/global_eeprom.c
    source/global_imu.c
    source/global_ports.c
    source/main.c
    source/navigation.c
    source/util.c

    source/Drivers/baro_ms56.c
    source/Drivers/dma.c
    source/Drivers/eeprom_cat25.c
    source/Drivers/flash_at25.c
    source/Drivers/imu_icm20948.c
    source/Drivers/port.c
    source/Drivers/pwm.c
    source/Drivers/sercom.c
    source/Drivers/spi.c
    source/Drivers/time.c
    source/Drivers/uart.c

    source/Drivers/usb/usb_requests.c
    source/Drivers/usb/usb_samd.c
    source/Drivers/usb/usb_serial.c
    source/Drivers/usb/usb.c

    source/RTOS/rtos_main.c
    source/RTOS/rtos_preemption.c
    source/RTOS/rtos_process.c
    source/RTOS/rtos_time.c
    source/RTOS/rtos_util.c

    source/RTOSDrivers/bus_manager.c
    source/RTOSDrivers/dma_baro_ms56.c
    source/RTOSDrivers/dma_eeprom_cat25.c
    source/RTOSDrivers/dma_flash_at25.c
    source/RTOSDrivers/dma_imu_icm20948.c
    source/RTOSDrivers/dma_spi.c
    source/RTOSDrivers/rtos_dma.c
    source/RTOSDrivers/rtos_uart.c
    source/RTOSDrivers/rtos_usb.c

    C:/Users/$ENV{USERNAME}/OneDrive/Documents/GNC2/software/GNClink/source/gnclink.c
    C:/Users/$ENV{USERNAME}/OneDrive/Documents/GNC2/software/FixedPoint/include/source/fixedpoint.c
)

set(EXECUTABLE ${PROJECT_NAME}.elf)

add_executable(${EXECUTABLE} ${GNC2_SOURCE_FILES})

target_compile_definitions(${EXECUTABLE} PRIVATE
    -D__SAMD21J17D__
    -D__ATSAMD21J17D__
    -D__GNC0200__
    -DDEBUG
    -DHARDWARE_DEFINITION="Hardware/GNC0200A.h"
)

target_include_directories(${EXECUTABLE} PRIVATE
    source
    # source/RTOS
    # source/RTOS/Drivers
    # source/RTOS/Drivers/usb
    lib/CMSIS_5/CMSIS/Core/Include
    toolchain/arm-none-eabi-gcc/arm-none-eabi/include
    lib/SAMD21_DFP/samd21d/include
    lib/SAMD21_DFP/samd21d/include/component
    lib/SAMD21_DFP/samd21d/include/instance
    C:/Users/$ENV{USERNAME}/OneDrive/Documents/GNC2/software/GNClink/source
    C:/Users/$ENV{USERNAME}/OneDrive/Documents/GNC2/software/FixedPoint/include
    C:/Users/$ENV{USERNAME}/OneDrive/Documents/GNC2/software/FixedPoint/include/source
)

target_compile_options(${EXECUTABLE} PRIVATE
    -xc
    -mthumb
    -Og
    -ffunction-sections
    -mlong-calls
    -g3
    -Wall
    -mcpu=cortex-m0plus
    -c
    -std=gnu11
    -fstack-usage
)

target_link_options(${EXECUTABLE} PRIVATE
    -mthumb
    -Wl,-Map=output/${PROJECT_NAME}.map
    # -Wl,--start-group
    # -lm
    # -Wl,--end-group
    -Wl,--gc-sections
    -Wl,--print-memory-usage
    -mcpu=cortex-m0plus
    -T${LINKER_SCRIPT}
)

add_custom_command(TARGET ${EXECUTABLE}
    POST_BUILD
    COMMAND ${TOOLCHAIN_PATH}/arm-none-eabi-objcopy.exe -O binary output/${EXECUTABLE} output/${PROJECT_NAME}.bin
    COMMAND ${TOOLCHAIN_PATH}/arm-none-eabi-objcopy.exe -O ihex  -R .eeprom -R .fuse -R .lock -R .signature output/${EXECUTABLE} output/${PROJECT_NAME}.hex
    COMMAND ${TOOLCHAIN_PATH}/arm-none-eabi-objdump.exe -h -S output/${EXECUTABLE} > output/${PROJECT_NAME}.lss
    COMMAND ${TOOLCHAIN_PATH}/arm-none-eabi-size.exe output/${EXECUTABLE}
)