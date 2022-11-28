add_library(ultrasonic ultrasonic.h ultrasonic.c)
add_library(kalman_ultrasonic kalman.c kalman.h)

target_link_libraries(ultrasonic kalman_ultrasonic movingAverageFilter pico_stdlib hardware_gpio pico_binary_info )