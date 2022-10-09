#ifndef PID_H_
#define PID_H_

typedef struct _PID {
    float kP, kI, kD;
    float setPoint;
    float min, max;

    // Controller memory
    float integral;
    float lastError;
    float lastInput;
    unsigned char saturating; // Boolean
} PID;

PID* PID_create(float kP, float kI, float kD, float setPoint, float min, float max);
float PID_run(PID* pid, float input);

#endif
