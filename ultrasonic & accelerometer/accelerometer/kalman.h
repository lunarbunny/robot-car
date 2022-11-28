#ifndef kalman_H
#define kalman_H

extern float kalmanFilter(float R, float Q, float measuredX, float *Xt_prev, float *Pt_prev);

#endif