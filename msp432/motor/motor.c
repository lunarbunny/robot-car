#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

#include "motor.h"
#include "serial/serial.h"

// === PWM Configuration ===
// SMCLK 3MHz
// Divider 6, Clock 500 kHz, period = 2us
// Timer max 2500, 2us * 2500, 5ms period

#define TIMERPERIOD 2500

Timer_A_PWMConfig leftMotorPWMConfig = {
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_6,
        TIMERPERIOD,
        TIMER_A_CAPTURECOMPARE_REGISTER_1,
        TIMER_A_OUTPUTMODE_RESET_SET,
        0
};

Timer_A_PWMConfig rightMotorPWMConfig = {
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_6,
        TIMERPERIOD,
        TIMER_A_CAPTURECOMPARE_REGISTER_2,
        TIMER_A_OUTPUTMODE_RESET_SET,
        0
};

void MOTOR_setSpeed(float dutyCycle, int motor) {
    // Set the duty cycle in percentage
    if (motor & MOTOR_LEFT) {
        leftMotorPWMConfig.dutyCycle = (dutyCycle / 100) * TIMERPERIOD;
        Timer_A_generatePWM(TIMER_A0_BASE, &leftMotorPWMConfig);
    }
    if (motor & MOTOR_RIGHT) {
        rightMotorPWMConfig.dutyCycle = (dutyCycle / 100) * TIMERPERIOD;
        Timer_A_generatePWM(TIMER_A0_BASE, &rightMotorPWMConfig);
    }
}

float MOTOR_getSpeed(int motor) {
    // Return the duty cycle in percentage
    if (motor & MOTOR_LEFT) {
        return (leftMotorPWMConfig.dutyCycle / 100) * TIMERPERIOD;
    } else if (motor & MOTOR_RIGHT) {
        return (leftMotorPWMConfig.dutyCycle / 100) * TIMERPERIOD;
    } else {
        return 0.f;
    }
}

int motorDirLeft = -1;
int motorDirRight = -1;

void MOTOR_setDirection(int dir, int motor) {
    // Motor 1: PIN 5,4
    // 10 = Forward, 01 = Reverse
    // Motor 2: PIN 2,0
    // 01 = Forward, 10 = Reverse (Flipped because motor is also physically flipped)
    if (dir == MOTOR_DIR_FORWARD) {
        if (motor & MOTOR_LEFT) {
            GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN5);
            GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN4);
            motorDirLeft = MOTOR_DIR_FORWARD;
        }
        if (motor & MOTOR_RIGHT) {
            GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN2);
            GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN0);
            motorDirRight = MOTOR_DIR_FORWARD;
        }
    } else if (dir == MOTOR_DIR_REVERSE) {
        if (motor & MOTOR_LEFT) {
            GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN5);
            GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN4);
            motorDirLeft = MOTOR_DIR_REVERSE;
        }
        if (motor & MOTOR_RIGHT) {
            GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN2);
            GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN0);
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

void MOTOR_init(void)
{
    // Motor driver control pins
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN5); // MotorA IN1
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN4); // MotorA IN2
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN2); // MotorB IN3
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN0); // MotorB IN4
    // Set motor default state
    MOTOR_setDirection(MOTOR_DIR_FORWARD, MOTOR_LEFT | MOTOR_RIGHT);
    MOTOR_setSpeed(0.f, MOTOR_LEFT | MOTOR_RIGHT);

    // PWM control pins
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);

    Timer_A_generatePWM(TIMER_A0_BASE, &leftMotorPWMConfig);
    Timer_A_generatePWM(TIMER_A0_BASE, &rightMotorPWMConfig);

    SERIAL_printf("Motor init\r\n");
}
