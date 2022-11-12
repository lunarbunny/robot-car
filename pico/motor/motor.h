#ifndef MOTOR_H_
#define MOTOR_H_

#define MOTOR_LEFT 1
#define MOTOR_RIGHT 2

#define MOTOR_DIR_FORWARD 0
#define MOTOR_DIR_REVERSE 1

#define MOTOR_TURN_CLOCKWISE 0
#define MOTOR_TURN_ANTICLOCKWISE 1

void MOTOR_init(void);

void MOTOR_setSpeed(uint8_t dutyCycle, int motor);
void MOTOR_stop(int motor);
uint8_t MOTOR_getSpeed(int motor);

void MOTOR_setDirection(int dir, int motor);
int MOTOR_getDirection(int motor);

void MOTOR_spotTurn(int turnDirection, int angle);
void MOTOR_moveFoward(int cm);

int CMtoSteps(float cm);
#endif
