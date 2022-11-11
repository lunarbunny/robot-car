#include <stdio.h>

#include "pico/stdlib.h"
#include "motor/motor.h"
#include "motor/pid.h"
#include "encoder/encoder.h"

#define DEBUG

int main()
{
    stdio_init_all();
    while (!stdio_usb_connected())
        sleep_ms(100); // Wait for USB to initialize

    printf("===== Starting pico ===== \n");

    // Initialize modules
    MOTOR_init();
    ENCODER_init();

    // Create PID contoller for motors
    PID *leftMotorPID = PID_create(5.0f, 5.f, 0.5f, 0, 0, 100);
    PID *rightMotorPID = PID_create(5.0f, 5.f, 0.5f, 0, 0, 100);

    float leftWheelSpeed, rightWheelSpeed;
    int leftMotorDutyCycle, rightMotorDutyCycle;

    uint8_t usePid = 1;

    while (1)
    {
        // Read stdin input (via serial terminal over usb)
        uint8_t c = getchar_timeout_us(0);
        switch (c)
        {
        case 'z': // Toggle PID controller
            usePid ^= 1;
            printf("> [PID] Enabled: %i \n", usePid);
            break;
        case 'q': // Slow down (through PID)
            leftMotorPID->setPoint -= 2;
            if (leftMotorPID->setPoint < 0.f)
                leftMotorPID->setPoint = 0.f;
            rightMotorPID->setPoint -= 2;
            if (rightMotorPID->setPoint < 0.f)
                rightMotorPID->setPoint = 0.f;
            printf("> [Motor] Target Speed %f \n", leftMotorPID->setPoint * 10);
            break;
        case 'e': // Speed up (through PID)
            leftMotorPID->setPoint += 2;
            if (leftMotorPID->setPoint > 10.f)
                leftMotorPID->setPoint = 10.f;
            rightMotorPID->setPoint += 2;
            if (rightMotorPID->setPoint > 10.f)
                rightMotorPID->setPoint = 10.f;
            printf("> [Motor] Target Speed %.0f \n", leftMotorPID->setPoint * 10);
            break;
        case 'Q': // Slow down (direct motor control, need to disable PID!)
            MOTOR_setSpeed(MOTOR_getSpeed(MOTOR_LEFT) - 20, MOTOR_LEFT | MOTOR_RIGHT);
            printf("> [Motor] Target Speed %u \n", MOTOR_getSpeed(MOTOR_LEFT));
            break;
        case 'E': // Speed up (direct motor control, need to disable PID!)
            MOTOR_setSpeed(MOTOR_getSpeed(MOTOR_LEFT) + 20, MOTOR_LEFT | MOTOR_RIGHT);
            printf("> [Motor] Target Speed %u \n", MOTOR_getSpeed(MOTOR_LEFT));
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
            TurnLeft();
            printf("> [Motor] Left Turn \n");
            break;
        case 'd': // Right turn (in place)
            TurnRight();
            printf("> [Motor] Right Turn \n");
            break;
         case 'x': // Move Foward (10 cm)
            MoveFoward(CMtoSteps(10));
            printf("> [Motor] Move Foward \n");
            break;
         case 't': // Turn Around
            TurnAround();
            printf("> [Motor] Move Foward \n");
            break;
        }

        if (usePid)
        {
            leftWheelSpeed = ENCODER_getLeftWheelSpeed();
            rightWheelSpeed = ENCODER_getRightWheelSpeed();

            leftMotorDutyCycle = PID_run(leftMotorPID, leftWheelSpeed);
            rightMotorDutyCycle = PID_run(rightMotorPID, rightWheelSpeed);

            MOTOR_setSpeed(leftMotorDutyCycle, MOTOR_LEFT);
            MOTOR_setSpeed(rightMotorDutyCycle, MOTOR_RIGHT);

#ifdef DEBUG
            printf("SPD L: %.2f PID L: %i | [P]%.2f [I]%.2f [D]%.2f \r\n", leftWheelSpeed, leftMotorDutyCycle, leftMotorPID->p, leftMotorPID->i, leftMotorPID->d);
            printf("SPD R: %.2f PID R: %i | [P]%.2f [I]%.2f [D]%.2f \r\n", rightWheelSpeed, rightMotorDutyCycle, rightMotorPID->p, rightMotorPID->i, rightMotorPID->d);
#endif
        }

        sleep_ms(500);
    }
}