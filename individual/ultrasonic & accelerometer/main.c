#include <stdio.h>

#include "pico/stdlib.h" 
#include "pico/binary_info.h" 
#include "hardware/gpio.h" 

#include "accelerometer/accelerometer.h"
#include "ultrasonic/ultrasonic.h"

#define GPIO_PIN_US_TRIGGER 17
#define GPIO_PIN_US_ECHO 16

int main()
{
    stdio_init_all();

    // Initialize modules
    ACCELEROMETER_init();
    // ULTRASONIC_init();

    while (1)
    {
        // printf("> [US] FRONT: %.2f \n", getCM(GPIO_PIN_US_TRIGGER, GPIO_PIN_US_ECHO, true));
        float val = ACCELEROMETER_detectHump();

        sleep_ms(400);
    }
}