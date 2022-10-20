
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

// Define use of Pico LED
#define GPIO_PIN_LED 25

// Define Infrared Sensor Needs
#define GPIO_PIN_INFRARED_IN1 15

int main () {
    stdio_init_all();

    // Initialization
    gpio_init(GPIO_PIN_LED); 
    gpio_init(GPIO_PIN_INFRARED_IN1);

    // Set if its input or output
    gpio_set_dir(GPIO_PIN_LED, GPIO_OUT);           // LED out
    gpio_set_dir(GPIO_PIN_INFRARED_IN1, GPIO_IN);   // S1 in

    // Setting initial state
    gpio_put(GPIO_PIN_LED, 0);  // Off

    // Main loop
    while (true) {
        if (gpio_get(GPIO_PIN_INFRARED_IN1) == 1) {
            printf("YOU GOTA ONE!!");
            gpio_put(GPIO_PIN_LED, 1); // On
        } else {
            gpio_put(GPIO_PIN_LED, 0); // Off
        }
    }

}

