#ifndef kalman_ultrasonic_H
#define kalman_ultrasonic_H

/* Export structures */
typedef struct kalman_filter_data
{
	/* Transition matrix: 2x2 */
    float Phi_matrix[4];
	/* Q covariance plant noise matrix: 2x2 */
    float Q_matrix[4];
	/* Sensitivity matrix: 1X2 */
    float H_matrix[2];
	/* Observation noise: R covariance matrix 1x1 */
    float R_matrix;
	/* P plus current covariance matrix 2x2: estimate error */
    float P_plus[4];
	/* x plus current state vector 2x1: value, speed */
    float x_plus[2];
} kalman_filter_data_s;


/* Export function */
extern float ultrasonic_kalmanFilter(float, kalman_filter_data_s*);

#endif