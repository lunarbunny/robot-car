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
// record start time when boot pico
absolute_time_t startTime;

void ACCELEROMETER_init(void)
{
    stdio_init_all();

    i2c_init(i2c_default, 400 * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(SDA_PIN, SCL_PIN, GPIO_FUNC_I2C));

    mpu6050_reset();

    startTime = get_absolute_time();
}

float filterMeasurement(void)
{
    // find difference in time of start time and end time
    float diffTime, diffSeconds;
    // measurement and prediction variables
    float measuredX, kalmanX, movingAverageX;

    // kalman initialization
    float SensorData;
    float Xt_prev;
    float Pt_prev = 1.0;

    // complementary initialization
    // record previous time
    absolute_time_t prevTime;

    // moving average initialization
    // array to store data every n times (as defined as window size) and calculate the average
    float dataArray[WINDOWLEN] = {0.0};
    float sum = 0.0;
    int position = 0;

    absolute_time_t endTime = get_absolute_time();
    // find time difference in micro seconds and convert to seconds
    diffTime = absolute_time_diff_us(startTime, endTime);
    diffSeconds = diffTime / 1000000;

    // run for 30s -- FOR NOW --
    // if (diffSeconds <= (float)10000)
    // {
    // read raw data from accelerometer
    mpu6050_read_raw(acc, gyro);
    // convert to angle
    measuredX = atan2(acc[1], acc[2]) * 180 / M_PI;

    // kalman filter algorithm
    kalmanX = kalmanFilter(1, 0.01, measuredX, &Xt_prev, &Pt_prev);
    float kalmanX2 = kalmanFilter(10, 0.1, measuredX, &Xt_prev, &Pt_prev);
    // moving average filter algorithm
    movingAverageX = movingAverageFilter(dataArray, &sum, position, measuredX);

    printf("------------------\n");
    printf("time: %.2f\n", diffSeconds);
    printf("int seconds: %d\n", (int)diffSeconds);
    printf("\n");
    printf("measuredX:\t%.2f\n", measuredX);
    printf("kalmanX:\t%.2f\n", kalmanX);
    printf("kalmanX2:\t%.2f\n", kalmanX2);
    // printf("complementaryX:\t%.2f\n", complementaryX);
    // printf("movingAverageX:\t%.2f\n", movingAverageX);

    prevTime = get_absolute_time();
    position++;
    if (position >= WINDOWLEN)
    {
        position = 0;
    }

    return kalmanX;
}

float ACCELEROMETER_detectHump(void)
{
    // get predicted angle
    float predictedX;
    // to detect amout of h
    absolute_time_t startTime1, endTime1;
    bool slope_detect = false;
    float triangle_h;
    float height;
    float speed = 30;
    float speed_t;
    float radianHighest, degreeHighest;

    predictedX = filterMeasurement();

    if (predictedX > 0 && predictedX > FIRSTRANGE)
    {
        // inclination occur
        if (slope_detect == false)
        {
            printf("slope detected\n");
            slope_detect = true;
            startTime1 = get_absolute_time();
        }
        if (predictedX > degreeHighest)
        {
            degreeHighest = predictedX;
            endTime1 = get_absolute_time();
        }
    }
    else if (predictedX < 0 && predictedX < -(FIRSTRANGE))
    {
        // declination occur
        if (slope_detect == true)
        {
            radianHighest = degreeHighest * M_PI / 180;
            printf("go down slope: %.2f\n", radianHighest);
            slope_detect = false;
            endTime1 = get_absolute_time();
            speed_t = absolute_time_diff_us(startTime1, endTime1);
            printf("t before: %.2f", speed_t);

            speed_t = speed_t / 1000000;
            printf("t after: %.2f", speed_t);
            triangle_h = speed_t * speed;
            printf("triangle_h (1.5): %.4f\n", triangle_h);
            printf("sin radian (0.6): %.4f\n", sin(radianHighest));
            height = triangle_h * sin(radianHighest);
            printf("height (0.9): %.1f \n", height);

            // reset highest degree
            degreeHighest = 0.0;
        }
    }

    if (!slope_detect && degreeHighest) {
        return -1;
    }
    else return height;
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

int hitWall()
{
    return 0;
}