#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "accelerometer.h"

// get raw data of acceleration and gyroscope
int16_t acc[MAXLEN], gyro[MAXLEN];
int addr = 0x68;

// measurement and prediction variables
float measuredX, movingAverageX;

// get predicted angle
float predictedX;

// kalman initialization
float Xt_prev;
float Pt_prev = 1.0;

// moving average initialization
// array to store data every n times (as defined as window size) and calculate the average
float dataArray[WINDOWLEN] = {0.0};
float sum = 0.0;
int position = 0;

// to detect amout of h
absolute_time_t startTime1;
absolute_time_t endTime1;
bool slope_detect = false;
float triangle_h;
float height;
float speed = 3;
double timeDiff_us = 0;
double timeDiff_s = 0;
float radianHighest = 0;
float degreeHighest = 0;

void ACCELEROMETER_init(void)
{
    printf("[Accelerometer] Init start \n");

    i2c_init(i2c_default, 400 * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    mpu6050_reset();

    printf("[Accelerometer] Init done \n");
}

float filterMeasurement(void)
{
    // read raw data from accelerometer
    mpu6050_read_raw(acc, gyro);
    // convert to angle
    measuredX = atan2(acc[1], acc[2]) * 180 / M_PI;

    // moving average filter algorithm
    movingAverageX = movingAverageFilter(dataArray, &sum, position, measuredX);

    printf("------------------\n");
    printf("measuredX:\t%.2f\n", measuredX);
    printf("movingAverageX:\t%.2f\n", movingAverageX);

    // increment index position of window
    position++;
    // reset window index position when it hits the length
    if (position >= WINDOWLEN)
    {
        position = 0;
    }

    return movingAverageX;
}

float ACCELEROMETER_detectHump(void)
{
    // get predicted reading of angle X
    predictedX = filterMeasurement();

    // when angle exceeds threshold
    while (predictedX > FIRSTRANGE)
    {
        // inclination occur
        if (slope_detect == false)
        {
            printf("slope detected\n");
            slope_detect = true;
            // get start time of first inclination
            startTime1 = get_absolute_time();
        }
        else
        {
            // get highest angle with comparison to current angle
            if (predictedX >= degreeHighest)
            {
                degreeHighest = predictedX;
            }
            else
            {
                // get end time of end inclination
                endTime1 = get_absolute_time();
                break;
            }
        }

        // get raw data again
        mpu6050_read_raw(acc, gyro);
        // convert to angle
        measuredX = atan2(acc[1], acc[2]) * 180 / M_PI;
        predictedX = movingAverageFilter(dataArray, &sum, position, measuredX);
    }

    // when angle less than minimum threshold and when slope is finished
    if (predictedX < 0 && slope_detect == true)
    {
        // convert highest angle to radian
        radianHighest = degreeHighest * M_PI / 180;

        // get difference of end time and start time in microseconds
        timeDiff_us = absolute_time_diff_us(startTime1, endTime1);
        // convert time difference to seconds 
        timeDiff_s = timeDiff_us / 1000000.0;

        // --- start of trigonometry function --- //
        // get hypothenuse
        triangle_h = timeDiff_s * speed;
        // get height of hump using sin
        height = triangle_h * sin(radianHighest);
        printf("height: %.1f \n", height);
        // --- end of trigonometry function --- //
        // reset variables
        slope_detect = false;
        degreeHighest = 0;
    }
    return height;
}

// #ifdef i2c_default
void mpu6050_reset(void)
{
    // Two byte reset. First byte register, second byte data
    // There are a load more options to set up the device in different ways that could be added here
    uint8_t buf[] = {0x6B, 0x00};
    i2c_write_blocking(i2c_default, addr, buf, 2, false);
}

void mpu6050_read_raw(int16_t acc[MAXLEN], int16_t gyro[MAXLEN])
{
    // For this particular device, we send the device the register we want to read
    // first, then subsequently read from the device. The register is auto incrementing
    // so we don't need to keep sending the register we want, just the first.

    uint8_t buffer[6];

    // Start reading acceleration registers from register 0x3B for 6 bytes
    uint8_t val = 0x3B;
    i2c_write_blocking(i2c_default, addr, &val, 1, true); // true to keep master control of bus
    i2c_read_blocking(i2c_default, addr, buffer, 6, false);

    for (int i = 0; i < 3; i++)
    {
        acc[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);
    }

    // Now gyro data from reg 0x43 for 6 bytes
    // The register is auto incrementing on each read
    val = 0x43;
    i2c_write_blocking(i2c_default, addr, &val, 1, true);
    i2c_read_blocking(i2c_default, addr, buffer, 6, false); // False - finished with bus

    for (int i = 0; i < 3; i++)
    {
        gyro[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);
    }
}

float kalmanFilter(float R, float Q, float measuredX, float *Xt_prev, float *Pt_prev)
{
    float Xt_update = *Xt_prev;
    float Pt_update = *Pt_prev + Q;
    float Kt = Pt_update / (Pt_update + R);
    float Xt = Xt_update + (Kt * (measuredX - Xt_update));
    float Pt = (1 - Kt) * Pt_update;
    *Xt_prev = Xt;
    *Pt_prev = Pt;
    return Xt;
}

float movingAverageFilter(float *ptrDataArray, float *ptrSum, int position, float nextData)
{
    // subtract the oldest number from the previous sum, add the new number
    *ptrSum = *ptrSum - ptrDataArray[position] + nextData;
    // assign the nextData to the position in the array
    ptrDataArray[position] = nextData;
    // return the average
    return *ptrSum / WINDOWLEN;
}