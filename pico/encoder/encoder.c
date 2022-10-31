#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"

#include "encoder.h"

#define GPIO_PIN_ENC_LEFT 6
#define GPIO_PIN_ENC_RIGHT 7

volatile int encIndex = 0;
int leftRotation[4];
int rightRotation[4];

void IRQHandlerEncoder(uint gpio, uint32_t events);
bool WheelSpeedTimerCallback(struct repeating_timer* timer);

void ENCODER_init(void)
{
    // Setup timer (0.25 second), used to calculate wheel speed
    // Create a repeating timer that calls repeating_timer_callback.
    // If the delay is > 0 then this is the delay between the previous callback ending and the next starting.
    // If the delay is negative then the next call to the callback will be exactly 500ms after the start of the call to the last callback
    struct repeating_timer timer;
    add_repeating_timer_ms(-250, &WheelSpeedTimerCallback, NULL, &timer);

    // Setup interrupt, trigger during high to low transition
    gpio_set_irq_enabled_with_callback(GPIO_PIN_ENC_LEFT, GPIO_IRQ_EDGE_FALL, true, &IRQHandlerEncoder);
    gpio_set_irq_enabled_with_callback(GPIO_PIN_ENC_RIGHT, GPIO_IRQ_EDGE_FALL, true, &IRQHandlerEncoder);

    printf("Encoder init\r\n");
}

float ENCODER_getLeftWheelSpeed(void)
{
    int total = 0;
    int i;
    for (i = 0; i < 4; i++)
    {
        total += leftRotation[i];
    }
    return total / 4;
}

float ENCODER_getRightWheelSpeed(void)
{
    int total = 0;
    int i;
    for (i = 0; i < 4; i++)
    {
        total += rightRotation[i];
    }
    return total / 4;
}

void IRQHandlerEncoder(uint gpio, uint32_t events)
{
    if (gpio == GPIO_PIN_ENC_LEFT) {
        leftRotation[encIndex]++;
        //printf("L encoder interrupt\r\n");
    }

    if (gpio == GPIO_PIN_ENC_RIGHT) {
        rightRotation[encIndex]++;
        //printf("R encoder interrupt\r\n");
    }
}

bool WheelSpeedTimerCallback(struct repeating_timer* timer)
{
    leftRotation[(encIndex + 1) % 4] = 0;
    rightRotation[(encIndex + 1) % 4] = 0;
    encIndex++;
    if (encIndex == 4)
        encIndex = 0;
    // printf("0.25s timer up\r\n");
    return true;
}