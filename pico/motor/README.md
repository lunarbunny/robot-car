## Motor Module

### Introduction

This sub-module library contains:

1. Motor control logic `motor.c`
2. PID Controller `pid.c`

### Explanation

This library provides an interface to the motors through a L298N motor driver.

Definitions in `motor.h`:
```c
#define MOTOR_LEFT 1
#define MOTOR_RIGHT 2

#define MOTOR_DIR_FORWARD 0
#define MOTOR_DIR_REVERSE 1

#define MOTOR_TURN_CLOCKWISE 0
#define MOTOR_TURN_ANTICLOCKWISE 1

// Initialize motor
void MOTOR_init(PID *left, PID *right);

// Set left or right motor's PWM duty cycle.
void MOTOR_setSpeed(uint dutyCycle, int motor);
// Shorthand for MOTOR_setSpeed(0.f, motor);
void MOTOR_stop(int motor);
// Get currently configured duty cycle.
uint MOTOR_getSpeed(int motor);

// Set direction of left or right motor (forward/reverse).
void MOTOR_setDirection(int dir, int motor);
// Get direction of left or right motor.
int MOTOR_getDirection(int motor);

// Move forward by X cm. Blocks until completed.
void MOTOR_moveFoward(int cm);
// Move forward by X cm using PID controller. Non blocking.
void MOTOR_moveFowardPID(PID *pidLeft, PID *pidRight, int cm);
// Move backward by X cm. Blocks until completed.
void MOTOR_moveBackward(int cm);
// Turn on the spot, either clockwise or anticlockwise. Blocks until completed. Angle must be multiple of 45.
void MOTOR_spotTurn(int turnDirection, int angle);
// Turn on the spot using PID. Non blocking. Angle must be multiple of 45.
void MOTOR_spotTurnPID(PID *pidLeft, PID *pidRight, int turnDirection, int angle);

// Helper function to convert from CM to number of interrupts.
int CMtoSteps(float cm);
```

Definitions in `pid.h`:
```c
#define SPEED_NONE 0
#define SPEED_LOW 1
#define SPEED_MEDIUM 2
#define SPEED_HIGH 3

// PID controller memory
typedef struct _PID {
    float kP, kI, kD;
    float setPoint;
    float min, max;

    float p, i, d;
    float lastError;
} PID;

// Create a new PID controller
PID* PID_create(float kP, float kI, float kD, float setPoint, float min, float max);
// Set the new setpoint for a PID controller.
void PID_setTarget(PID *pid, float setPoint);
// Shorthand for PID_setTarget using the SPEED_NONE, SPEED_MEDIUM, etc for predefined speeds.
void PID_setTargetSpeed(PID *pid, int speed);
// PID control loop
uint PID_run(PID* pid, float input, float deltaTime);
```