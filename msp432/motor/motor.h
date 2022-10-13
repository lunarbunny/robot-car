#ifndef MOTOR_H_
#define MOTOR_H_

#define MOTOR_LEFT 1
#define MOTOR_RIGHT 2

#define MOTOR_DIR_FORWARD 0
#define MOTOR_DIR_REVERSE 1

void MOTOR_init(void);
void MOTOR_initDebug(void);

void MOTOR_setSpeed(float dutyCycle, int motor);
float MOTOR_getSpeed(int motor);

void MOTOR_setDirection(int dir, int motor);
int MOTOR_getDirection(int motor);

void MOTOR_turnLeft(void);
void MOTOR_turnRight(void);

#endif
