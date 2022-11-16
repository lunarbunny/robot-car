#include <math.h>
#include <stdio.h>
#include <stdlib.h>

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
        newPID->p = newPID->i = newPID->d = 0;
    }
    return newPID;
}

int PID_run(PID* pid, float input) {
    float error = pid->setPoint - input;

    pid->p = pid->kP * error;

    // Check if saturating and if sign bit of integral and error is the same.
    // If saturating it means the output is higher than specified limit of actuator.
    // If sign bits are the same, it means the integral is causing oversaturation and making it worse.
    // char antiWindupCheck = pid->saturating && (signbit(error) == signbit(pid->integral));
    // if (!antiWindupCheck) {
    //     pid->integral += pid->kI * error;
    //     output = proportional + pid->integral;
    // } else {
    //     // If saturating and integral causing more saturation, ignore integral
    //     output = proportional;
    // }

    pid->i += pid->kI * error;

    if (pid->i > pid->max) pid->i = pid->max;
    else if (pid->i < pid->min) pid->i = pid->min;

    pid->d = pid->kD * (input - pid->lastInput);

    if (error < 0.01f)
        pid->i = 0;

    int output = round(pid->p + pid->i - pid->d);

    // Saturation check (output too low/high)
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
    pid->lastInput = input;

    return output;
}

