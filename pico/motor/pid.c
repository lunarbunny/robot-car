#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "pid.h"

PID *PID_create(float kP, float kI, float kD, float setPoint, float min, float max)
{
    PID *newPID = malloc(sizeof(PID));
    if (newPID != NULL)
    {
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

int clamp(int input, int min, int max)
{
    if (input > max)
        return max;
    if (input < min)
        return min;
    return input;
}

float clampF(float input, float min, float max)
{
    if (input > max)
        return max;
    if (input < min)
        return min;
    return input;
}

uint normalize(uint input)
{
    // Motors only have enough torque to start moving at ~80% duty cycle
    // So we normalize the speed so that the original PID output of 0-100% is
    // transformed into 80%-100% to fit expected real life speeds.
    if (input > 0)
    {
        float step = (100 - 80) / 100.f;
        return 80 + roundf(input * step);
    }
    else
    {
        return 0;
    }
}

void PID_setTarget(PID *pid, float setPoint)
{
    pid->setPoint = clamp(setPoint, 0.f, 25.f);
}

void PID_setTargetSpeed(PID *pid, int speed)
{
    switch (speed)
    {
    case SPEED_NONE:
        PID_setTarget(pid, 0.f);
        break;
    case SPEED_LOW:
        PID_setTarget(pid, 12.f);
        break;
    case SPEED_MEDIUM:
        PID_setTarget(pid, 18.f);
        break;
    case SPEED_HIGH:
        PID_setTarget(pid, 25.f);
        break;
    }
}

uint PID_run(PID *pid, float input, float deltaTime)
{
    float error = pid->setPoint - input;

    pid->p = pid->kP * error;

    pid->i += pid->kI * error * deltaTime;
    pid->i = clampF(pid->i, -pid->min, pid->max);

    pid->d = pid->kD * (error - pid->lastError) / deltaTime;

    uint output = pid->p + pid->i + pid->d;
    output = clamp(output, pid->min, pid->max);

    pid->lastError = error;

    return normalize(output);
}
