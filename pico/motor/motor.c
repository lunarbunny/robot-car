#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

#include "motor.h"

#define GPIO_PIN_MOTOR1_PWM 0
#define GPIO_PIN_MOTOR2_PWM 1
#define GPIO_PIN_MOTOR1_IN1 2
#define GPIO_PIN_MOTOR1_IN2 3
#define GPIO_PIN_MOTOR2_IN3 4
#define GPIO_PIN_MOTOR2_IN4 5

#define PWM_CYCLE 10

uint pwm_slice;

float motorSpeedLeft = 0.f;
float motorSpeedRight = 0.f;

int motorDirLeft = -1;
int motorDirRight = -1;

void MOTOR_init(void)
{
    // Initialize GPIO pins
    gpio_init(GPIO_PIN_MOTOR1_IN1);
    gpio_init(GPIO_PIN_MOTOR1_IN2);
    gpio_init(GPIO_PIN_MOTOR2_IN3);
    gpio_init(GPIO_PIN_MOTOR2_IN4);
    gpio_set_dir(GPIO_PIN_MOTOR1_IN1, GPIO_OUT);
    gpio_set_dir(GPIO_PIN_MOTOR1_IN2, GPIO_OUT);
    gpio_set_dir(GPIO_PIN_MOTOR2_IN3, GPIO_OUT);
    gpio_set_dir(GPIO_PIN_MOTOR2_IN4, GPIO_OUT);

    // Setup PWM pins
    gpio_set_function(GPIO_PIN_MOTOR1_PWM, GPIO_FUNC_PWM);
    gpio_set_function(GPIO_PIN_MOTOR2_PWM, GPIO_FUNC_PWM);
    pwm_slice = pwm_gpio_to_slice_num(GPIO_PIN_MOTOR1_PWM); // GPIO 0 and 1 is slice 0
    pwm_set_wrap(pwm_slice, PWM_CYCLE - 1); // Set period of 10 cycles (0-9)
    pwm_set_enabled(pwm_slice, true); // Enable PWM

    // Set default state of motor
    MOTOR_setDirection(MOTOR_DIR_FORWARD, MOTOR_LEFT | MOTOR_RIGHT);
    MOTOR_setSpeed(0.f, MOTOR_LEFT | MOTOR_RIGHT);

    printf("Motor Init \r\n");
}

void MOTOR_setSpeed(float dutyCycle, int motor) {
    // Set the duty cycle in percentage
    if (motor & MOTOR_LEFT) {
        pwm_set_chan_level(pwm_slice, PWM_CHAN_A, dutyCycle / (100.f / PWM_CYCLE));
        motorSpeedLeft = dutyCycle;
    }
    if (motor & MOTOR_RIGHT) {
        pwm_set_chan_level(pwm_slice, PWM_CHAN_B, dutyCycle / (100.f / PWM_CYCLE));
        motorSpeedRight = dutyCycle;
    }
}

float MOTOR_getSpeed(int motor) {
    // Return the duty cycle in percentage
    if (motor & MOTOR_LEFT) {
        return motorSpeedLeft;
    } else if (motor & MOTOR_RIGHT) {
        return motorDirRight;
    } else {
        return 0.f;
    }
}

void MOTOR_setDirection(int dir, int motor) {
    // Motor 1: IN1 IN2
    // 10 = Forward, 01 = Reverse
    // Motor 2: IN3 IN4
    // 01 = Forward, 10 = Reverse (Flipped because motor is also physically flipped)
    if (dir == MOTOR_DIR_FORWARD) {
        if (motor & MOTOR_LEFT) {
            gpio_put(GPIO_PIN_MOTOR1_IN1, 1);
            gpio_put(GPIO_PIN_MOTOR1_IN2, 0);
            motorDirLeft = MOTOR_DIR_FORWARD;
        }
        if (motor & MOTOR_RIGHT) {
            gpio_put(GPIO_PIN_MOTOR2_IN3, 0);
            gpio_put(GPIO_PIN_MOTOR2_IN4, 1);
            motorDirRight = MOTOR_DIR_FORWARD;
        }
    } else if (dir == MOTOR_DIR_REVERSE) {
        if (motor & MOTOR_LEFT) {
            gpio_put(GPIO_PIN_MOTOR1_IN1, 0);
            gpio_put(GPIO_PIN_MOTOR1_IN2, 1);
            motorDirLeft = MOTOR_DIR_REVERSE;
        }
        if (motor & MOTOR_RIGHT) {
            gpio_put(GPIO_PIN_MOTOR2_IN3, 1);
            gpio_put(GPIO_PIN_MOTOR2_IN4, 0);
            motorDirRight = MOTOR_DIR_REVERSE;
        }
    }
}

int MOTOR_getDirection(int motor) {
    if (motor == MOTOR_LEFT) {
        return motorDirLeft;
    } else if (motor == MOTOR_RIGHT) {
        return motorDirRight;
    }
    return -1;
}

void MOTOR_turnLeft(void) {
    MOTOR_setDirection(MOTOR_DIR_REVERSE, MOTOR_LEFT);
    MOTOR_setDirection(MOTOR_DIR_FORWARD, MOTOR_RIGHT);
}

void MOTOR_turnRight(void) {
    MOTOR_setDirection(MOTOR_DIR_FORWARD, MOTOR_LEFT);
    MOTOR_setDirection(MOTOR_DIR_REVERSE, MOTOR_RIGHT);
}
