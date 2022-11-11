#ifndef MOTOR_H_
#define MOTOR_H_

#define MOTOR_LEFT 1
#define MOTOR_RIGHT 2

#define MOTOR_DIR_FORWARD 0
#define MOTOR_DIR_REVERSE 1

void MOTOR_init(void);

void MOTOR_setSpeed(int8_t dutyCycle, int motor);
uint16_t MOTOR_getSpeed(int motor);

void MOTOR_setDirection(int dir, int motor);
int MOTOR_getDirection(int motor);

void MOTOR_turnLeft(void);
void MOTOR_turnRight(void);

int CMtoSteps(float cm);
void TurnLeft();
void TurnRight();
void TurnAround();
void MoveFoward(int interrupts);

#endif
