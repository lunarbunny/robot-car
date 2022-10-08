cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initialize pico-sdk from GIT
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_FETCH_FROM_GIT on)

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(motorcontrol C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(motorcontrol motorcontrol.c )

pico_set_program_name(motorcontrol "motorcontrol")
pico_set_program_version(motorcontrol "0.1")

# pico_enable_stdio_uart(motorcontrol 1)
pico_enable_stdio_usb(motorcontrol 1)

# Add the standard library to the build
target_link_libraries(motorcontrol pico_stdlib)

pico_add_extra_outputs(motorcontrol)
