#include <stdio.h>
#include "pico/stdlib.h"
//#include "hardware/pwm.h"

#define GPIO_PIN_MOTOR1_PWM 0
#define GPIO_PIN_MOTOR2_PWM 1

#define GPIO_PIN_MOTOR1_IN1 2
#define GPIO_PIN_MOTOR1_IN2 3
#define GPIO_PIN_MOTOR2_IN3 4
#define GPIO_PIN_MOTOR2_IN4 5

#define GPIO_PIN_LED 25

int moveFoward()
{
    gpio_put(GPIO_PIN_MOTOR2_IN3, 1);
    gpio_put(GPIO_PIN_MOTOR2_IN4, 0);
}

int moveBackward()
{
    gpio_put(GPIO_PIN_MOTOR2_IN3, 0);
    gpio_put(GPIO_PIN_MOTOR2_IN4, 1);
}

int main()
{
    stdio_init_all();

    gpio_init(GPIO_PIN_LED);
    gpio_init(GPIO_PIN_MOTOR2_IN3);
    gpio_init(GPIO_PIN_MOTOR2_IN4);

    gpio_set_dir(GPIO_PIN_LED, GPIO_OUT);
    gpio_set_dir(GPIO_PIN_MOTOR2_IN3, GPIO_OUT);
    gpio_set_dir(GPIO_PIN_MOTOR2_IN4, GPIO_OUT);

    // gpio_set_function(GPIO_PIN_MOTOR2_PWM, GPIO_FUNC_PWM);
    // Supposed to use PWM, for now just set pin to high
    gpio_init(GPIO_PIN_MOTOR2_PWM);
    gpio_set_dir(GPIO_PIN_MOTOR2_PWM, GPIO_OUT);
    gpio_put(GPIO_PIN_MOTOR2_PWM, 1);

    // Set initial state
    gpio_put(GPIO_PIN_MOTOR2_IN3, 0);
    gpio_put(GPIO_PIN_MOTOR2_IN4, 0);

    uint8_t led = 1;

    while (1)
    {
        sleep_ms(3000);

        led ^= 1;
        gpio_put(GPIO_PIN_LED, led);

        if (led) {
            moveFoward();
        } else {
            moveBackward();
        }

        bool in3 = gpio_get(GPIO_PIN_MOTOR2_IN3);
        bool in4 = gpio_get(GPIO_PIN_MOTOR2_IN4);
        printf("IN3: %d IN4: %d\n", in3, in4);
    }
}