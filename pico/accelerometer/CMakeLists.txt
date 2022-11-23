add_library(accelerometer accelerometer.c accelerometer.h)
add_library(kalman kalman.c kalman.h)

# pull in common dependencies and additional i2c hardware support
target_link_libraries(accelerometer pico_stdlib hardware_i2c kalman)