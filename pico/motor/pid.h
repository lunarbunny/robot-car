#ifndef PID_H_
#define PID_H_

#define SPEED_NONE 0
#define SPEED_LOW 1
#define SPEED_MEDIUM 2
#define SPEED_HIGH 3

#define uint unsigned int

typedef struct _PID {
    float kP, kI, kD;
    float setPoint;
    float min, max;

    // Controller memory
    float p, i, d;
    float lastError;
} PID;

PID* PID_create(float kP, float kI, float kD, float setPoint, float min, float max);
void PID_setTarget(PID *pid, float setPoint);
void PID_setTargetSpeed(PID *pid, int speed);
uint PID_run(PID* pid, float input, float deltaTime);

#endif
