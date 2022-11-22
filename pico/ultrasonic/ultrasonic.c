// Get readings from ultrasonic sensor

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"

#include "ultrasonic.h"

#define GPIO_PIN_US_FRONT_TRIGGER 11
#define GPIO_PIN_US_FRONT_ECHO 20
#define GPIO_PIN_US_RIGHT_TRIGGER 13
#define GPIO_PIN_US_RIGHT_ECHO 18
#define GPIO_PIN_US_REAR_TRIGGER 12
#define GPIO_PIN_US_REAR_ECHO 19
#define GPIO_PIN_US_LEFT_TRIGGER 10
#define GPIO_PIN_US_LEFT_ECHO 21

#define TIMEOUT 26100

void ULTRASONIC_init(void)
{
    printf("[Ultrasonic] Init start \n");

    gpio_init(GPIO_PIN_US_FRONT_TRIGGER);
    gpio_init(GPIO_PIN_US_RIGHT_TRIGGER);
    gpio_init(GPIO_PIN_US_REAR_TRIGGER);
    gpio_init(GPIO_PIN_US_LEFT_TRIGGER);
    gpio_init(GPIO_PIN_US_FRONT_ECHO);
    gpio_init(GPIO_PIN_US_RIGHT_ECHO);
    gpio_init(GPIO_PIN_US_REAR_ECHO);
    gpio_init(GPIO_PIN_US_LEFT_ECHO);
    gpio_set_dir(GPIO_PIN_US_FRONT_TRIGGER, GPIO_OUT);
    gpio_set_dir(GPIO_PIN_US_RIGHT_TRIGGER, GPIO_OUT);
    gpio_set_dir(GPIO_PIN_US_REAR_TRIGGER, GPIO_OUT);
    gpio_set_dir(GPIO_PIN_US_LEFT_TRIGGER, GPIO_OUT);
    gpio_set_dir(GPIO_PIN_US_FRONT_ECHO, GPIO_IN);
    gpio_set_dir(GPIO_PIN_US_RIGHT_ECHO, GPIO_IN);
    gpio_set_dir(GPIO_PIN_US_REAR_ECHO, GPIO_IN);
    gpio_set_dir(GPIO_PIN_US_LEFT_ECHO, GPIO_IN);

    printf("[Ultrasonic] Init done \n");
}

int64_t getPulse(uint trigPin, uint echoPin)
{
    absolute_time_t startTime, endTime;
    gpio_put(trigPin, 1);
    sleep_us(10);
    gpio_put(trigPin, 0);

    uint64_t width = 0;

    // printf("[a] %i | %u %u \n", gpio_get(echoPin), trigPin, echoPin);

    while (gpio_get(echoPin) == 0)
    {
        startTime = get_absolute_time();
    }

    // printf("[b] %i \n", gpio_get(echoPin));

    while (gpio_get(echoPin) == 1)
    {
        // printf("[c] %i \n", gpio_get(echoPin));
        width++;
        sleep_us(1);
        if (width > TIMEOUT)
            return -1;
    }

    endTime = get_absolute_time();

    return absolute_time_diff_us(startTime, endTime);
}

float getCM(uint trigPin, uint echoPin)
{
    int64_t pulseLength = getPulse(trigPin, echoPin);
    if(pulseLength < 0){
        return 50;
    }
    return pulseLength / 58.f;
}

float ULTRASONIC_getCM(int ultrasonic)
{
    if (ultrasonic == ULTRASONIC_FRONT)
    {
        return getCM(GPIO_PIN_US_FRONT_TRIGGER, GPIO_PIN_US_FRONT_ECHO);
    }
    else if (ultrasonic == ULTRASONIC_RIGHT)
    {
        return getCM(GPIO_PIN_US_RIGHT_TRIGGER, GPIO_PIN_US_RIGHT_ECHO);
    }
    else if (ultrasonic == ULTRASONIC_REAR)
    {
        return getCM(GPIO_PIN_US_REAR_TRIGGER, GPIO_PIN_US_REAR_ECHO);
    }
    else if (ultrasonic == ULTRASONIC_LEFT)
    {
        return getCM(GPIO_PIN_US_LEFT_TRIGGER, GPIO_PIN_US_LEFT_ECHO);
    }
}