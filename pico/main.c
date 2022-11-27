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

typedef struct state
{
    absolute_time_t currentTime, previousTime, bootTime;

    float deltaTime; // Time since last loop in seconds
    float leftWheelSpeed, rightWheelSpeed;
    uint leftMotorDutyCycle, rightMotorDutyCycle;

    PID *leftPID, *rightPID;    // PID controller data
    uint8_t usePid;             // Flag, PID enabled or disabled
    repeating_timer_t pidTimer; // Repeating timer that runs PID controller
} State;

void togglePid(State *state)
{
    state->usePid ^= 1;
    printf("> [PID] Enabled: %u \n", state->usePid);
}

bool pidCallback(repeating_timer_t *timer)
{
    State state = *((State *)timer->user_data);

    if (state.usePid)
    {
        state.leftMotorDutyCycle = PID_run(state.leftPID, state.leftWheelSpeed, state.deltaTime);
        state.rightMotorDutyCycle = PID_run(state.rightPID, state.rightWheelSpeed, state.deltaTime);

        if (state.leftMotorDutyCycle != MOTOR_getSpeed(MOTOR_LEFT))
        {
            MOTOR_setSpeed(state.leftMotorDutyCycle, MOTOR_LEFT);
        }
        if (state.rightMotorDutyCycle != MOTOR_getSpeed(MOTOR_RIGHT))
        {
            MOTOR_setSpeed(state.rightMotorDutyCycle, MOTOR_RIGHT);
        }

        printf("T: %.2fs | SP: %.2f | SPD L: %.2f | DUTY L: %u | [P]%.2f [I]%.2f [D]%.2f (Err: %.2f) \n", absolute_time_diff_us(state.bootTime, state.currentTime) / 1000000.f, state.leftPID->setPoint, state.leftWheelSpeed, state.leftMotorDutyCycle, state.leftPID->p, state.leftPID->i, state.leftPID->d, state.leftPID->lastError);
        printf("T: %.2fs | SP: %.2f | SPD R: %.2f | DUTY R: %u | [P]%.2f [I]%.2f [D]%.2f (Err: %.2f) \n", absolute_time_diff_us(state.bootTime, state.currentTime) / 1000000.f, state.rightPID->setPoint, state.rightWheelSpeed, state.rightMotorDutyCycle, state.rightPID->p, state.rightPID->i, state.rightPID->d, state.rightPID->lastError);
    }
    else
    {
        printf("SPD L: %.2f | DUTY L: %i \n", state.leftWheelSpeed, MOTOR_getSpeed(MOTOR_LEFT));
        printf("SPD R: %.2f | DUTY R: %i \n", state.rightWheelSpeed, MOTOR_getSpeed(MOTOR_RIGHT));
    }

    return true;
}

int main()
{
    stdio_init_all();
    while (!stdio_usb_connected())
        sleep_ms(100); // Wait for USB to initialize

    printf("===== Starting pico ===== \n");

    State state;

    // Create PID contoller for motors
    state.leftPID = PID_create(PID_Kp, PID_Ki, PID_Kd, 0, 0.f, 100.f);
    state.rightPID = PID_create(PID_Kp, PID_Ki, PID_Kd, 0, 0.f, 100.f);

    // Initialize modules
    MOTOR_init(state.leftPID, state.rightPID);
    ENCODER_init();
    // INFRARED_init();
    ACCELEROMETER_init();
    ULTRASONIC_init();
    COMMS_init();

    state.bootTime = get_absolute_time();
    state.usePid = 1;
    add_repeating_timer_ms(-200, pidCallback, &state, &state.pidTimer);

    while (1)
    {
        // Read stdin input (via serial terminal over usb)
        uint8_t c = getchar_timeout_us(0);
        switch (c)
        {
        case 'z': // Toggle PID controller
            togglePid(&state);
            break;
        case 'q': // Slow down (through PID)
            PID_setTargetSpeed(state.leftPID, SPEED_MEDIUM);
            PID_setTargetSpeed(state.rightPID, SPEED_MEDIUM);
            printf("> [Motor] PID Target Setpoint %.0f \n", state.leftPID->setPoint);
            break;
        case 'e': // Speed up (through PID)
            PID_setTargetSpeed(state.leftPID, SPEED_HIGH);
            PID_setTargetSpeed(state.rightPID, SPEED_HIGH);
            printf("> [Motor] PID Target Setpoint %.0f \n", state.leftPID->setPoint);
            break;
        case '`': // Stop
            if (state.usePid)
            {
                PID_setTargetSpeed(state.leftPID, SPEED_NONE);
                PID_setTargetSpeed(state.rightPID, SPEED_NONE);
                printf("> [Motor] PID Target Setpoint %.0f \n", state.leftPID->setPoint);
            }
            else
            {
                MOTOR_stop(MOTOR_LEFT | MOTOR_RIGHT);
            }
            break;
        case '1': // 60% Duty cycle
            if (state.usePid)
                togglePid(&state);
            MOTOR_setSpeed(60, MOTOR_LEFT | MOTOR_RIGHT);
            break;
        case '2': // 70% Duty cycle
            if (state.usePid)
                togglePid(&state);
            MOTOR_setSpeed(70, MOTOR_LEFT | MOTOR_RIGHT);
            break;
        case '3': // 80% Duty cycle
            if (state.usePid)
                togglePid(&state);
            MOTOR_setSpeed(80, MOTOR_LEFT | MOTOR_RIGHT);
            break;
        case '4': // 90% Duty cycle
            if (state.usePid)
                togglePid(&state);
            MOTOR_setSpeed(90, MOTOR_LEFT | MOTOR_RIGHT);
            break;
        case '5': // 100% Duty cycle
            if (state.usePid)
                togglePid(&state);
            MOTOR_setSpeed(100, MOTOR_LEFT | MOTOR_RIGHT);
            break;
        case 'Q': // Slow down (direct motor control)
            if (state.usePid)
                togglePid(&state);
            uint8_t prevSpeed = MOTOR_getSpeed(MOTOR_LEFT) - 10;
            MOTOR_setSpeed(prevSpeed, MOTOR_LEFT | MOTOR_RIGHT);
            break;
        case 'E': // Speed up (direct motor control)
            if (state.usePid)
                togglePid(&state);
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
        case 'A': // Left turn (in place) using PID
            MOTOR_spotTurnPID(state.leftPID, state.rightPID, MOTOR_TURN_ANTICLOCKWISE, 90);
            printf("> [Motor] Left Turn (PID) \n");
            break;
        case 'D': // Right turn (in place) using PID
            MOTOR_spotTurnPID(state.leftPID, state.rightPID, MOTOR_TURN_CLOCKWISE, 90);
            printf("> [Motor] Right Turn (PID) \n");
            break;
        case 'x': // Move Foward (10 cm)
            MOTOR_moveFoward(10);
            printf("> [Motor] Move Foward \n");
            break;
        case 'X': // Move Foward (10 cm) using PID
            MOTOR_moveFowardPID(state.leftPID, state.rightPID, 10);
            printf("> [Motor] Move Foward (PID) \n");
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
        case '[': // Decrease I factor by 0.1
            state.leftPID->kI -= 0.1f;
            state.rightPID->kI -= 0.1f;
            printf("> [PID] I Factor: %.2f \n", state.leftPID->kI);
            break;
        case ']': // Increase I factor by 0.1
            state.leftPID->kI += 0.1f;
            state.rightPID->kI += 0.1f;
            printf("> [PID] I Factor: %.2f \n", state.leftPID->kI);
            break;
        }

        state.currentTime = get_absolute_time();
        state.deltaTime = absolute_time_diff_us(state.previousTime, state.currentTime) / 1000000.f;
        state.previousTime = state.currentTime;

        state.leftWheelSpeed = ENCODER_getWheelSpeed(ENCODER_LEFT);
        state.rightWheelSpeed = ENCODER_getWheelSpeed(ENCODER_RIGHT);

        switch (c)
        {
        case 'v':
            COMMS_sendToM5(TX_KEY_SPEED, &state.leftWheelSpeed);
            break;
        case 'b':
            COMMS_sendToM5(TX_KEY_SPEED, &state.rightWheelSpeed);
            break;
        }

        sleep_ms(10);
    }
}