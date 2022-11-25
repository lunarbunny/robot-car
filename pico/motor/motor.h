#ifndef MOTOR_H_
#define MOTOR_H_

#include "pid.h"

#define MOTOR_LEFT 1
#define MOTOR_RIGHT 2

#define MOTOR_DIR_FORWARD 0
#define MOTOR_DIR_REVERSE 1

#define MOTOR_TURN_CLOCKWISE 0
#define MOTOR_TURN_ANTICLOCKWISE 1

void MOTOR_init(PID *left, PID *right);

void MOTOR_setSpeed(uint dutyCycle, int motor);
void MOTOR_stop(int motor);
uint MOTOR_getSpeed(int motor);

void MOTOR_setDirection(int dir, int motor);
int MOTOR_getDirection(int motor);

void MOTOR_spotTurn(int turnDirection, int angle);
void MOTOR_moveFoward(int cm);
void MOTOR_spotTurnPID(PID *pidLeft, PID *pidRight, int turnDirection, int angle);
void MOTOR_moveFowardPID(PID *pidLeft, PID *pidRight, int cm);

int CMtoSteps(float cm);
#endif
