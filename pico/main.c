#include <stdio.h>

#include "pico/stdlib.h"
#include "motor/motor.h"
#include "motor/pid.h"
#include "encoder/encoder.h"

#define GPIO_PIN_LED 25

PID* leftMotorPID = NULL;
PID* rightMotorPID = NULL;

int main()
{
    stdio_init_all();

    // Initialize modules
    MOTOR_init();
    ENCODER_init();

    // Create PID contoller for motors
    leftMotorPID = PID_create(5.0f, 5.f, 0.5f, 0, 0, 100);
    rightMotorPID = PID_create(5.0f, 5.f, 0.5f, 0, 0, 100);

    printf("Initialization done\r\n");

    uint8_t led = 1;

    while (1)
    {
        led ^= 1;
        gpio_put(GPIO_PIN_LED, led);

        float leftWheelSpeed = ENCODER_getLeftWheelSpeed();
	    float rightWheelSpeed = ENCODER_getRightWheelSpeed();

	    float leftMotorDutyCycle = PID_run(leftMotorPID, leftWheelSpeed);
	    float rightMotorDutyCycle = PID_run(rightMotorPID, rightWheelSpeed);

	    MOTOR_setSpeed(leftMotorDutyCycle, MOTOR_LEFT);
	    MOTOR_setSpeed(rightMotorDutyCycle, MOTOR_RIGHT);

	    char printBuffer[50];
	    snprintf(printBuffer, sizeof(printBuffer), "1) SPD L: %.2f | SPD R: %.2f \r\n", leftWheelSpeed, rightWheelSpeed);
	    printf(printBuffer);
	    snprintf(printBuffer, sizeof(printBuffer), "2) PID L: %.2f | PID R: %.2f \r\n", leftMotorDutyCycle, rightMotorDutyCycle);
	    printf(printBuffer);
        
        sleep_ms(50);
    }
}