#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/util/datetime.h"
#include "motor/motor.h"
#include "motor/pid.h"
#include "encoder/encoder.h"
#include "infrared/infrared.h"
#include "accelerometer/accelerometer.h"
#include "comms/comms.h"
#include "ultrasonic/ultrasonic.h"
#include "hardware/clocks.h"

#define DEBUG

#define PID_Kp 2.f
#define PID_Ki 2.f
#define PID_Kd 0.f

void togglePid(uint8_t *flag)
{
    *flag ^= 1;
    printf("> [PID] Enabled: %i \n", *flag);
}

int main()
{
    stdio_init_all();
    while (!stdio_usb_connected())
        sleep_ms(100); // Wait for USB to initialize

    printf("===== Starting pico ===== \n");

    // Initialize modules
    MOTOR_init();
    ENCODER_init();
    //INFRARED_init();
    ACCELEROMETER_init();
    ULTRASONIC_init();
    COMMS_init();

    // Create PID contoller for motors
    PID *leftMotorPID = PID_create(PID_Kp, PID_Ki, PID_Kd, 0, 0, 100);
    PID *rightMotorPID = PID_create(PID_Kp, PID_Ki, PID_Kd, 0, 0, 100);

    absolute_time_t currentTime, previousTime, bootTime;
    bootTime = get_absolute_time();

    float leftWheelSpeed, rightWheelSpeed;
    int leftWheelInterruptSpeed, rightWheelInterruptSpeed;
    uint leftMotorDutyCycle, rightMotorDutyCycle;

    float deltaTime; // Time since last loop in seconds

    uint8_t usePid = 1;

    while (1)
    {
        // Read stdin input (via serial terminal over usb)
        uint8_t c = getchar_timeout_us(0);
        switch (c)
        {
        case 'z': // Toggle PID controller
            togglePid(&usePid);
            break;
        case 'q': // Slow down (through PID)
            PID_setTargetSpeed(leftMotorPID, SPEED_MEDIUM);
            PID_setTargetSpeed(rightMotorPID, SPEED_MEDIUM);
            printf("> [Motor] PID Target Setpoint %.0f \n", leftMotorPID->setPoint);
            break;
        case 'e': // Speed up (through PID)
            PID_setTargetSpeed(leftMotorPID, SPEED_HIGH);
            PID_setTargetSpeed(rightMotorPID, SPEED_HIGH);
            printf("> [Motor] PID Target Setpoint %.0f \n", leftMotorPID->setPoint);
            break;
        case '`': // Stop
            if (usePid)
            {
                PID_setTargetSpeed(leftMotorPID, SPEED_NONE);
                PID_setTargetSpeed(rightMotorPID, SPEED_NONE);
                printf("> [Motor] PID Target Setpoint %.0f \n", leftMotorPID->setPoint);
            }
            else
            {
                MOTOR_stop(MOTOR_LEFT | MOTOR_RIGHT);
            }
            break;
        case '1': // 60% Duty cycle
            if (usePid)
                togglePid(&usePid);
            MOTOR_setSpeed(60, MOTOR_LEFT | MOTOR_RIGHT);
            break;
        case '2': // 70% Duty cycle
            if (usePid)
                togglePid(&usePid);
            MOTOR_setSpeed(70, MOTOR_LEFT | MOTOR_RIGHT);
            break;
        case '3': // 80% Duty cycle
            if (usePid)
                togglePid(&usePid);
            MOTOR_setSpeed(80, MOTOR_LEFT | MOTOR_RIGHT);
            break;
        case '4': // 90% Duty cycle
            if (usePid)
                togglePid(&usePid);
            MOTOR_setSpeed(90, MOTOR_LEFT | MOTOR_RIGHT);
            break;
        case '5': // 100% Duty cycle
            if (usePid)
                togglePid(&usePid);
            MOTOR_setSpeed(100, MOTOR_LEFT | MOTOR_RIGHT);
            break;
        case 'Q': // Slow down (direct motor control)
            if (usePid)
                togglePid(&usePid);
            uint8_t prevSpeed = MOTOR_getSpeed(MOTOR_LEFT) - 10;
            MOTOR_setSpeed(prevSpeed, MOTOR_LEFT | MOTOR_RIGHT);
            break;
        case 'E': // Speed up (direct motor control)
            if (usePid)
                togglePid(&usePid);
            uint8_t nextSpeed = MOTOR_getSpeed(MOTOR_LEFT) + 10;
            MOTOR_setSpeed(nextSpeed, MOTOR_LEFT | MOTOR_RIGHT);
            break;
        case 'w': // Forward
            MOTOR_setDirection(MOTOR_DIR_FORWARD, MOTOR_LEFT | MOTOR_RIGHT);
            printf("> [Motor] Forward \n");
            break;
        case 's': // Reverse
            MOTOR_setDirection(MOTOR_DIR_REVERSE, MOTOR_LEFT | MOTOR_RIGHT);
            printf("> [Motor] Reverse \n");
            break;
        case 'a': // Left turn (in place)
            MOTOR_spotTurn(MOTOR_TURN_ANTICLOCKWISE, 90);
            printf("> [Motor] Left Turn \n");
            break;
        case 'd': // Right turn (in place)
            MOTOR_spotTurn(MOTOR_TURN_CLOCKWISE, 90);
            printf("> [Motor] Right Turn \n");
            break;
        case 'A': // Turn 180 (in place)
            MOTOR_spotTurn(MOTOR_TURN_ANTICLOCKWISE, 180);
            printf("> [Motor] Turn 180 ANTI-CLK \n");
            break;
        case 'D': // Turn 180 (in place)
            MOTOR_spotTurn(MOTOR_TURN_CLOCKWISE, 180);
            printf("> [Motor] Turn 180 CLK \n");
            break;
        case 'x': // Move Foward (10 cm)
            MOTOR_moveFoward(10);
            printf("> [Motor] Move Foward \n");
            break;
        case 'i': // Front US
            printf("> [US] FRONT: %.2f \n", ULTRASONIC_getCM(ULTRASONIC_FRONT));
            break;
        case 'k': // Rear US
            printf("> [US] REAR: %.2f \n", ULTRASONIC_getCM(ULTRASONIC_REAR));
            break;
        case 'j': // Left US
            printf("> [US] LEFT: %.2f \n", ULTRASONIC_getCM(ULTRASONIC_LEFT));
            break;
        case 'l': // Right US
            printf("> [US] RIGHT: %.2f \n", ULTRASONIC_getCM(ULTRASONIC_RIGHT));
            break;
        }

        currentTime = get_absolute_time();
        deltaTime = absolute_time_diff_us(previousTime, currentTime) / 1000000.f;
        previousTime = currentTime;

        leftWheelSpeed = ENCODER_getWheelSpeed(ENCODER_LEFT);
        rightWheelSpeed = ENCODER_getWheelSpeed(ENCODER_RIGHT);

        if (usePid)
        {
            // leftWheelInterruptSpeed = ENCODER_getWheelInterruptSpeed(ENCODER_LEFT);
            // rightWheelInterruptSpeed = ENCODER_getWheelInterruptSpeed(ENCODER_RIGHT);

            leftMotorDutyCycle = PID_run(leftMotorPID, leftWheelSpeed, deltaTime);
            rightMotorDutyCycle = PID_run(rightMotorPID, rightWheelSpeed, deltaTime);

            if (leftMotorDutyCycle != MOTOR_getSpeed(MOTOR_LEFT))
            {
                MOTOR_setSpeed(leftMotorDutyCycle, MOTOR_LEFT);
            }
            if (rightMotorDutyCycle != MOTOR_getSpeed(MOTOR_RIGHT))
            {
                MOTOR_setSpeed(rightMotorDutyCycle, MOTOR_RIGHT);
            }

            // printf("PID Delta Time: %.6f (s) \n", deltaTime);
            printf("T: %.2fs | SP: %.2f | SPD L: %.2f | DUTY L: %u | [P]%.2f [I]%.2f [D]%.2f (Err: %.2f) \n", absolute_time_diff_us(bootTime, currentTime) / 1000000.f, leftMotorPID->setPoint, leftWheelSpeed, leftMotorDutyCycle, leftMotorPID->p, leftMotorPID->i, leftMotorPID->d, leftMotorPID->lastError);
            printf("T: %.2fs | SP: %.2f | SPD R: %.2f | DUTY R: %u | [P]%.2f [I]%.2f [D]%.2f (Err: %.2f) \n", absolute_time_diff_us(bootTime, currentTime) / 1000000.f, rightMotorPID->setPoint, rightWheelSpeed, rightMotorDutyCycle, rightMotorPID->p, rightMotorPID->i, rightMotorPID->d, rightMotorPID->lastError);
        }
        else
        {
            printf("SPD L: %.2f | DUTY L: %i \n", leftWheelSpeed, MOTOR_getSpeed(MOTOR_LEFT));
            printf("SPD R: %.2f | DUTY R: %i \n", rightWheelSpeed, MOTOR_getSpeed(MOTOR_RIGHT));
        }

        switch (c)
        {
        case 'v':
            COMMS_sendFloatToM5(leftWheelSpeed);
            break;
        case 'b':
            COMMS_sendFloatToM5(rightWheelSpeed);
            break;
        }

        sleep_ms(200);
    }
}