#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

#include "motor.h"
#include "../encoder/encoder.h"

/*
====== Motor Control ======
IN1 IN2, EN1 -> Right Motor
IN3 IN4, EN2 -> Left Motor
====== ============= ======
*/

#define GPIO_PIN_PWM_EN1 4
#define GPIO_PIN_PWM_EN2 5
#define GPIO_PIN_MOTOR_IN1 6
#define GPIO_PIN_MOTOR_IN2 7
#define GPIO_PIN_MOTOR_IN3 8
#define GPIO_PIN_MOTOR_IN4 9

#define PWM_CYCLE 10

uint pwm_slice;

float motorSpeedLeft = 0.f;
float motorSpeedRight = 0.f;

int motorDirLeft = -1;
int motorDirRight = -1;

const int slotCount = 20;     // 20 Slots in disk
const float wheelDiameter = 66.1f; // Wheel diameter in millimeters

void MOTOR_init(void)
{
    printf("[Motor] Init start \n");

    // Initialize GPIO pins
    gpio_init(GPIO_PIN_MOTOR_IN1);
    gpio_init(GPIO_PIN_MOTOR_IN2);
    gpio_init(GPIO_PIN_MOTOR_IN3);
    gpio_init(GPIO_PIN_MOTOR_IN4);
    gpio_set_dir(GPIO_PIN_MOTOR_IN1, GPIO_OUT);
    gpio_set_dir(GPIO_PIN_MOTOR_IN2, GPIO_OUT);
    gpio_set_dir(GPIO_PIN_MOTOR_IN3, GPIO_OUT);
    gpio_set_dir(GPIO_PIN_MOTOR_IN4, GPIO_OUT);

    // Setup PWM pins
    gpio_set_function(GPIO_PIN_PWM_EN1, GPIO_FUNC_PWM);
    gpio_set_function(GPIO_PIN_PWM_EN2, GPIO_FUNC_PWM);
    pwm_slice = pwm_gpio_to_slice_num(GPIO_PIN_PWM_EN1);
    pwm_set_wrap(pwm_slice, PWM_CYCLE - 1); // Set period of PWM_CYCLE cycles (0 to PWM_CYCLE-1)

    // Set default state of motor
    MOTOR_stop(MOTOR_LEFT | MOTOR_RIGHT);
    MOTOR_setDirection(MOTOR_DIR_FORWARD, MOTOR_LEFT | MOTOR_RIGHT);

    pwm_set_enabled(pwm_slice, true);

    printf("[Motor] Init done \n");
}

void MOTOR_setSpeed(uint16_t dutyCycle, int motor)
{
    if (dutyCycle < 0)
        dutyCycle = 0;
    else if (dutyCycle > 100)
        dutyCycle = 100;
    // Set the duty cycle in percentage
    if (motor & MOTOR_LEFT)
    {
        pwm_set_chan_level(pwm_slice, PWM_CHAN_A, dutyCycle / PWM_CYCLE);
        motorSpeedLeft = dutyCycle;
    }
    if (motor & MOTOR_RIGHT)
    {
        pwm_set_chan_level(pwm_slice, PWM_CHAN_B, dutyCycle / PWM_CYCLE);
        motorSpeedRight = dutyCycle;
    }
}

void MOTOR_stop(int motor)
{
    if (motor & MOTOR_LEFT)
    {
        pwm_set_chan_level(pwm_slice, PWM_CHAN_A, 0);
        motorSpeedLeft = 0.f;
    }
    if (motor & MOTOR_RIGHT)
    {
        pwm_set_chan_level(pwm_slice, PWM_CHAN_B, 0);
        motorSpeedRight = 0.f;
    }
}

uint16_t MOTOR_getSpeed(int motor)
{
    // Return the duty cycle in percentage
    if (motor & MOTOR_LEFT)
    {
        return motorSpeedLeft;
    }
    else if (motor & MOTOR_RIGHT)
    {
        return motorSpeedRight;
    }
    else
    {
        return -1;
    }
}

void MOTOR_setDirection(int dir, int motor)
{
    // Motor 1: IN1 IN2
    // 10 = Forward, 01 = Reverse
    // Motor 2: IN3 IN4
    // 01 = Forward, 10 = Reverse (Flipped because motor is also physically flipped)
    if (dir == MOTOR_DIR_FORWARD)
    {
        if (motor & MOTOR_LEFT)
        {
            gpio_put(GPIO_PIN_MOTOR_IN3, 1);
            gpio_put(GPIO_PIN_MOTOR_IN4, 0);
            motorDirLeft = MOTOR_DIR_FORWARD;
        }
        if (motor & MOTOR_RIGHT)
        {
            gpio_put(GPIO_PIN_MOTOR_IN1, 1);
            gpio_put(GPIO_PIN_MOTOR_IN2, 0);
            motorDirRight = MOTOR_DIR_FORWARD;
        }
    }
    else if (dir == MOTOR_DIR_REVERSE)
    {
        if (motor & MOTOR_LEFT)
        {
            gpio_put(GPIO_PIN_MOTOR_IN3, 0);
            gpio_put(GPIO_PIN_MOTOR_IN4, 1);
            motorDirLeft = MOTOR_DIR_REVERSE;
        }
        if (motor & MOTOR_RIGHT)
        {
            gpio_put(GPIO_PIN_MOTOR_IN1, 0);
            gpio_put(GPIO_PIN_MOTOR_IN2, 1);
            motorDirRight = MOTOR_DIR_REVERSE;
        }
    }
}

int MOTOR_getDirection(int motor)
{
    if (motor == MOTOR_LEFT)
    {
        return motorDirLeft;
    }
    else if (motor == MOTOR_RIGHT)
    {
        return motorDirRight;
    }
    return -1;
}

void MOTOR_setLeftTurnMode(void)
{
    MOTOR_setDirection(MOTOR_DIR_REVERSE, MOTOR_LEFT);
    MOTOR_setDirection(MOTOR_DIR_FORWARD, MOTOR_RIGHT);
}

void MOTOR_setRightTurnMode(void)
{
    MOTOR_setDirection(MOTOR_DIR_FORWARD, MOTOR_LEFT);
    MOTOR_setDirection(MOTOR_DIR_REVERSE, MOTOR_RIGHT);
}

// Convert from centimeters to steps
int CMtoSteps(float cm)
{
    int result;                                        // Final calculation result
    float circumference = (wheelDiameter * 3.14) / 10; // Calculate wheel circumference in cm
    float cm_step = circumference / slotCount;         // CM per Step

    float f_result = cm / cm_step; // Calculate result as a float
    result = (int)f_result;        // Convert to an integer (note this is NOT rounded)

    return result; // End and return result
}

void MOTOR_spotTurn(int turnDirection, int angle)
{
    int interrupts = 4 * angle / 45;
    int mSpeed = 60;

    if (turnDirection == MOTOR_TURN_CLOCKWISE)
        MOTOR_setRightTurnMode();
    else if (turnDirection == MOTOR_TURN_ANTICLOCKWISE)
        MOTOR_setLeftTurnMode();

    resetEncoderISRCount();
    MOTOR_setSpeed(mSpeed, MOTOR_LEFT | MOTOR_RIGHT);

    // Wait until turn is done
    while (getLeftISRCount() < interrupts || getRightISRCount() < interrupts);
    MOTOR_stop(MOTOR_LEFT | MOTOR_RIGHT);
}

void MOTOR_moveFoward(int interrupts)
{
    int mSpeed = 80;

    resetEncoderISRCount();

    // Set Left Motor Foward
    MOTOR_setDirection(MOTOR_DIR_FORWARD, MOTOR_LEFT);
    // Set Right Motor Foward
    MOTOR_setDirection(MOTOR_DIR_FORWARD, MOTOR_RIGHT);

    // Go forward until step value is reached
    while (interrupts > getLeftISRCount() && interrupts > getRightISRCount())
    {

        if (interrupts > getLeftISRCount())
        {
            MOTOR_setSpeed(mSpeed, MOTOR_LEFT);
        }
        else
        {
            MOTOR_setSpeed(0.f, MOTOR_LEFT);
        }
        if (interrupts > getRightISRCount())
        {
            MOTOR_setSpeed(mSpeed, MOTOR_RIGHT);
        }
        else
        {
            MOTOR_setSpeed(0.f, MOTOR_RIGHT);
        }
    }

    MOTOR_setSpeed(0.f, MOTOR_LEFT | MOTOR_RIGHT); // Stop when done
}