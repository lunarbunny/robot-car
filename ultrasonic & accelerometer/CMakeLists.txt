cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(pico_examples C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

set(PICO_EXAMPLES_PATH ${PROJECT_SOURCE_DIR})

# Initialize the SDK
pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )

# Hardware-specific examples in subdirectories:
# add_subdirectory(mpu6050)
add_subdirectory(accelerometer)
add_subdirectory(ultrasonic)

add_executable(pico_examples main.c)

pico_enable_stdio_uart(pico_examples 0)
pico_enable_stdio_usb(pico_examples 1)

# Add the standard library to the build
target_link_libraries(pico_examples accelerometer ultrasonic pico_stdlib)

pico_add_extra_outputs(pico_examples)