// Get readings from ultrasonic sensor
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/gpio.h"

#include "../accelerometer/movingAverageFilter.h"
#include "../accelerometer/kalman.h"
#include "kalman.h"
#include "ultrasonic.h"

#define GPIO_PIN_US_FRONT_TRIGGER 11
#define GPIO_PIN_US_FRONT_ECHO 20
#define GPIO_PIN_US_RIGHT_TRIGGER 13
#define GPIO_PIN_US_RIGHT_ECHO 18
#define GPIO_PIN_US_REAR_TRIGGER 12
#define GPIO_PIN_US_REAR_ECHO 19
#define GPIO_PIN_US_LEFT_TRIGGER 10
#define GPIO_PIN_US_LEFT_ECHO 21

float ultrasonic_Xt_prev;
float ultrasonic_Pt_prev = 1.0;
float ultrasonic_dataArray_3[3] = {0.0};
float ultrasonic_sum_3 = 0.0;
int ultrasonic_position_3 = 0;
float ultrasonic_dataArray_4[WINDOWLEN] = {0.0};
float ultrasonic_sum_4 = 0.0;
int ultrasonic_position_4 = 0;

kalman_filter_data_s kalman_data_R10 = {
    /* Transition matrix: 2x2 */
    /* float Phi_matrix[4]; */
    {1.0, 1.0, 0.0, 1.0},
    /* Q covariance plant noise matrix: 2x2 */
    /* float Q_matrix[4]; */
    {0.1, 0.0, 0.0, 0.1},
    /* Sensitivity matrix: 1X2 */
    /* float H_matrix[2]; */
    {1.0, 0.0},
    /* Observation noise: R covariance matrix 1x1 */
    /* float R_matrix; */
    10,
    /* P plus current covariance matrix 2x2: estimate error */
    /* float P_plus[4]; */
    {0.0, 0.0, 0.0, 0.0},
    /* x plus current state vector 2x1: value, speed */
    /* float x_plus[2]; */
    {0.0, 0.0},
};

void ULTRASONIC_init(void)
{
    printf("[Ultrasonic] Init start \n");

    gpio_init(GPIO_PIN_US_FRONT_TRIGGER);
    gpio_init(GPIO_PIN_US_RIGHT_TRIGGER);
    gpio_init(GPIO_PIN_US_REAR_TRIGGER);
    gpio_init(GPIO_PIN_US_LEFT_TRIGGER);
    gpio_init(GPIO_PIN_US_FRONT_ECHO);
    gpio_init(GPIO_PIN_US_RIGHT_ECHO);
    gpio_init(GPIO_PIN_US_REAR_ECHO);
    gpio_init(GPIO_PIN_US_LEFT_ECHO);
    gpio_set_dir(GPIO_PIN_US_FRONT_TRIGGER, GPIO_OUT);
    gpio_set_dir(GPIO_PIN_US_RIGHT_TRIGGER, GPIO_OUT);
    gpio_set_dir(GPIO_PIN_US_REAR_TRIGGER, GPIO_OUT);
    gpio_set_dir(GPIO_PIN_US_LEFT_TRIGGER, GPIO_OUT);
    gpio_set_dir(GPIO_PIN_US_FRONT_ECHO, GPIO_IN);
    gpio_set_dir(GPIO_PIN_US_RIGHT_ECHO, GPIO_IN);
    gpio_set_dir(GPIO_PIN_US_REAR_ECHO, GPIO_IN);
    gpio_set_dir(GPIO_PIN_US_LEFT_ECHO, GPIO_IN);

    printf("[Ultrasonic] Init done \n");
}

float getPulse(uint trigPin, uint echoPin)
{
    absolute_time_t startTime, endTime;
    gpio_put(trigPin, 1);
    sleep_us(10);
    gpio_put(trigPin, 0);

    // printf("[a] %i | %u %u \n", gpio_get(echoPin), trigPin, echoPin);

    while (gpio_get(echoPin) == 0)
        ;
    startTime = get_absolute_time();
    absolute_time_t timeout = delayed_by_us(startTime, MAXPULSELEN);

    // printf("[b] %i \n", gpio_get(echoPin));

    while (gpio_get(echoPin) == 1)
    {
        // printf("[c] %i \n", gpio_get(echoPin));
        if (time_reached(timeout))
            return 0;
    }

    endTime = get_absolute_time();

    return absolute_time_diff_us(startTime, endTime);
}

float getCM(uint trigPin, uint echoPin, bool filter)
{
    float pulseLength = getPulse(trigPin, echoPin) / 58;
    printf("measured:\t%.2f\n", pulseLength);
    if (pulseLength > 0)
    {
        float kalman_accelerometer, kalman_ultrasonic, movingAverage_3, movingAverage_4;
        if (filter)
        {
            // filter algorithms
            kalman_accelerometer = kalmanFilter(10, 0.1, pulseLength, &ultrasonic_Xt_prev, &ultrasonic_Pt_prev);
            kalman_ultrasonic = ultrasonic_kalmanFilter(pulseLength, &kalman_data_R10);
            movingAverage_3 = movingAverageFilter(ultrasonic_dataArray_3, &ultrasonic_sum_3, ultrasonic_position_3, pulseLength);
            movingAverage_4 = movingAverageFilter(ultrasonic_dataArray_4, &ultrasonic_sum_4, ultrasonic_position_4, pulseLength);
            printf("kalman_accelerometer:\t%.2f\n", kalman_accelerometer);
            printf("kalman_ultrasonic:\t%.2f\n", kalman_ultrasonic);
            printf("movingAverage_3:\t%.2f\n", movingAverage_3);
            printf("movingAverage_4:\t%.2f\n", movingAverage_4);
            // increment position index of window
            ultrasonic_position_3++;
            ultrasonic_position_4++;
            // reset window index position when it hits the length
            if (ultrasonic_position_3 >= 3)
            {
                ultrasonic_position_3 = 0;
            }
            if (ultrasonic_position_4 >= WINDOWLEN)
            {
                ultrasonic_position_4 = 0;
            }
            // using kalman_ultrasonic as final algorithm
            return kalman_ultrasonic;
        }
        else 
            return pulseLength;
    }
    else
        return pulseLength;
}

float ULTRASONIC_getCM(int ultrasonic)
{
    bool filter = true;
    if (ultrasonic == ULTRASONIC_FRONT)
    {
        return getCM(GPIO_PIN_US_FRONT_TRIGGER, GPIO_PIN_US_FRONT_ECHO, filter);
    }
    else if (ultrasonic == ULTRASONIC_RIGHT)
    {
        return getCM(GPIO_PIN_US_RIGHT_TRIGGER, GPIO_PIN_US_RIGHT_ECHO, filter);
    }
    else if (ultrasonic == ULTRASONIC_REAR)
    {
        return getCM(GPIO_PIN_US_REAR_TRIGGER, GPIO_PIN_US_REAR_ECHO, filter);
    }
    else if (ultrasonic == ULTRASONIC_LEFT)
    {
        return getCM(GPIO_PIN_US_LEFT_TRIGGER, GPIO_PIN_US_LEFT_ECHO, filter);
    }
}

void ULTRASONIC_resetFilter() {
    // reset window array
    for (int i = 0; i < WINDOWLEN; i++) {
        ultrasonic_dataArray_4[i] = 0.0;
    }
    // reset variables
    ultrasonic_sum_4 = 0.0;
    ultrasonic_position_4 = 0;
}

int ULTRASONIC_hitWall()
{
    return 0;
}