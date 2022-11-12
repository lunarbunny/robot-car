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

#define PWM_CYCLE 254

#define MIN_TURN_ANGLE 45

uint pwm_slice;

uint8_t motorSpeedLeft = 0;
uint8_t motorSpeedRight = 0;

int motorDirLeft = -1;
int motorDirRight = -1;

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

    // Initialize PWM
    gpio_set_function(GPIO_PIN_PWM_EN1, GPIO_FUNC_PWM);
    gpio_set_function(GPIO_PIN_PWM_EN2, GPIO_FUNC_PWM);
    pwm_slice = pwm_gpio_to_slice_num(GPIO_PIN_PWM_EN1);
    // assert(pwm_slice == pwm_gpio_to_slice_num(GPIO_PIN_PWM_EN2));
    pwm_config c = pwm_get_default_config();
    pwm_config_set_clkdiv_int(&c, 5);       // clk_sys defaults at 125 MHz, PWM freq is (clk_sys / div)
    pwm_config_set_wrap(&c, PWM_CYCLE - 1); // Set period of PWM_CYCLE cycles (0 to PWM_CYCLE-1)

    // Set default state of motor
    MOTOR_stop(MOTOR_LEFT | MOTOR_RIGHT);
    MOTOR_setDirection(MOTOR_DIR_FORWARD, MOTOR_LEFT | MOTOR_RIGHT);

    pwm_init(pwm_slice, &c, true);

    printf("[Motor] Init done \n");
}

void MOTOR_setSpeed(uint8_t dutyCycle, int motor)
{
    if (dutyCycle < 0)
        dutyCycle = 0;
    else if (dutyCycle > 100)
        dutyCycle = 100;
    // Convert to its appropriate PWM level from percentage
    uint16_t level = dutyCycle / 100.f * PWM_CYCLE;
    if (motor & MOTOR_LEFT)
    {
        pwm_set_chan_level(pwm_slice, PWM_CHAN_A, level);
        motorSpeedLeft = dutyCycle;
    }
    if (motor & MOTOR_RIGHT)
    {
        pwm_set_chan_level(pwm_slice, PWM_CHAN_B, level);
        motorSpeedRight = dutyCycle;
    }
}

void MOTOR_stop(int motor)
{
    MOTOR_setSpeed(0, motor);
}

uint8_t MOTOR_getSpeed(int motor)
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

void MOTOR_spotTurn(int turnDirection, int angle)
{
    if (angle < MIN_TURN_ANGLE)
        angle = MIN_TURN_ANGLE;

    // Ensure angle is a multiple of MIN_TURN_ANGLE
    int angleError = angle % MIN_TURN_ANGLE;
    if (angleError > 0)
    {
        if (angleError > MIN_TURN_ANGLE / 2)
        {
            // Closer to next multiple of MIN_TURN_ANGLE, round up
            angle += MIN_TURN_ANGLE - angleError;
        }
        else
        {
            // Closer to last multiple of MIN_TURN_ANGLE, round down
            angle -= angleError;
        }
    }

    int interrupts = 4 * angle / MIN_TURN_ANGLE;
    int speed = 80;

    if (turnDirection == MOTOR_TURN_CLOCKWISE)
        MOTOR_setRightTurnMode();
    else if (turnDirection == MOTOR_TURN_ANTICLOCKWISE)
        MOTOR_setLeftTurnMode();

    MOTOR_setSpeed(speed, MOTOR_LEFT | MOTOR_RIGHT);
    ENCODER_waitForISRInterrupts(interrupts); // Wait until turn is done
    MOTOR_stop(MOTOR_LEFT | MOTOR_RIGHT);
}

void MOTOR_moveFoward(int cm)
{
    int interrupts = ENCODER_cmToSteps(cm);
    int speed = 80;

    // Set Motor Foward
    MOTOR_setDirection(MOTOR_DIR_FORWARD, MOTOR_LEFT | MOTOR_RIGHT);

    // Go forward until step value is reached
    MOTOR_setSpeed(speed, MOTOR_LEFT | MOTOR_RIGHT);
    ENCODER_waitForISRInterrupts(interrupts); // Wait until turn is done
    MOTOR_stop(MOTOR_LEFT | MOTOR_RIGHT);     // Stop when done
}