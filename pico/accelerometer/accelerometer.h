#ifndef accelerometer_H_
#define accelerometer_H_

#include "pico/stdlib.h"

#define SDA_PIN 4
#define SCL_PIN 5
#define MAXLEN 3
#define WINDOWLEN 4
#define FIRSTRANGE 1.5

void ACCELEROMETER_init(void);
float ACCELEROMETER_detectHump(void);
float filterMeasurement(void);
void mpu6050_reset(void);
void mpu6050_read_raw(int16_t acc[MAXLEN], int16_t gyro[MAXLEN]);
float kalmanFilter(float R, float Q, float measuredX, float *Xt_prev, float *Pt_prev);
float movingAverageFilter(float *ptrDataArray, float *ptrSum, int position, float nextData);
int hitWall();

#endif