#include "kalman.h"

float kalmanFilter(float R, float Q, float measuredX, float *Xt_prev, float *Pt_prev) 
{
    float Xt_update = *Xt_prev;
    float Pt_update = *Pt_prev + Q;
    float Kt = Pt_update / (Pt_update + R);
    float Xt = Xt_update + ( Kt * (measuredX - Xt_update));
    float Pt = (1 - Kt) * Pt_update;
    *Xt_prev = Xt;
    *Pt_prev = Pt;
    return Xt;
}