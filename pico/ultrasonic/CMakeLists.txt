add_library(ultrasonic ultrasonic.h ultrasonic.c)

target_link_libraries(ultrasonic kalman pico_stdlib)