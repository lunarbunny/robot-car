#include <stdlib.h>
#include <math.h>

#include "pid.h"

PID* PID_create(float kP, float kI, float kD, float setPoint, float min, float max) {
    PID* newPID = malloc(sizeof(PID));
    if (newPID != NULL) {
        newPID->kP = kP; // Proportional
        newPID->kI = kI; // Integral
        newPID->kD = kD; // Deriative
        newPID->setPoint = setPoint;
        newPID->min = min; // Minimum output
        newPID->max = max; // Maximum output
        newPID->integral = 0;
    }
    return newPID;
}

float PID_run(PID* pid, float input) {
    float error = pid->setPoint - input;

    float proportional = pid->kP * error;

    // Check if saturating and if sign bit of integral and error is the same.
    // If saturating it means the output is higher than specified limit of actuator.
    // If sign bits are the same, it means the integral is causing oversaturation and making it worse.
    char antiWindupCheck = pid->saturating && (signbit(error) == signbit(pid->integral));
    if (!antiWindupCheck) {
        pid->integral += pid->kI * error;
    }

    // Saturation check (integral output too high)
    float output = proportional + pid->integral;
    if (output > pid->max) {
        output = pid->max;
        pid->saturating = 1;
    } else if (output < pid->min) {
        output = pid->min;
        pid->saturating = 1;
    } else {
        pid->saturating = 0;
    }

    pid->lastError = error;

    return output;
}
