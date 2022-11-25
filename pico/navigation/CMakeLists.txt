add_library(navigation mappinggod.h movement.c mapping.c navigation.c)

target_link_libraries(navigation pico_stdlib motor pid ultrasonic)