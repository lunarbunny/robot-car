cmake_minimum_required(VERSION 3.13)

project(PICO C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initialize pico-sdk from GIT
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_FETCH_FROM_GIT on)

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(robotcar C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

add_subdirectory(motor)
add_subdirectory(encoder)
add_subdirectory(InfraredSensor)

# Add executable.
add_executable(robotcar main.c)

# pico_enable_stdio_uart(robotcar 1)
pico_enable_stdio_usb(robotcar 1)

# Add the standard library to the build
target_link_libraries(robotcar motor encoder pid pico_stdlib)

pico_add_extra_outputs(robotcar)