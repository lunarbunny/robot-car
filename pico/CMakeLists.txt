cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Pull in Raspberry Pi Pico SDK
# include(pico_sdk_import.cmake)
set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
set(PICO_SDK_INIT_CMAKE_FILE ${PICO_SDK_PATH}/pico_sdk_init.cmake)
include(${PICO_SDK_INIT_CMAKE_FILE})

project(RobotCar C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

add_subdirectory(motor)
add_subdirectory(encoder)
add_subdirectory(infrared)
add_subdirectory(accelerometer)
add_subdirectory(ultrasonic)
add_subdirectory(comms)

add_executable(robotcar main.c)

pico_enable_stdio_uart(robotcar 0)
pico_enable_stdio_usb(robotcar 1)

# Link required library to the binary
target_link_libraries(robotcar motor pid encoder infrared accelerometer ultrasonic comms pico_stdlib)

pico_add_extra_outputs(robotcar)