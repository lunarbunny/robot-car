cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(2104proj)

pico_sdk_init()

add_executable(i2cstuff
        i2cstuff.c
        )

# pull in common dependencies
target_link_libraries(i2cstuff 
        pico_stdlib
        hardware_i2c)

# create map/bin/hex file etc.
pico_add_extra_outputs(i2cstuff)

#pico_enable_stdio_usb(i2cstuff 1)
#pico_enable_stdio_uart(i2cstuff 0)