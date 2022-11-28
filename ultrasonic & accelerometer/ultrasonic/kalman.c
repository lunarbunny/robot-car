#include "kalman.h"

float ultrasonic_kalmanFilter(float input, kalman_filter_data_s* kalman_data)
{
	float P_minus[4]; /* matrix 2x2 */
	float x_minus[2]; /* vector 2x1 */
	float K_gain[2];  /* matrix 2x1 */
	float temp_help;
	
	/* Prediction Step */
	x_minus[0] = kalman_data->Phi_matrix[0]*kalman_data->x_plus[0] + kalman_data->Phi_matrix[1]*kalman_data->x_plus[1];
	x_minus[1] = kalman_data->Phi_matrix[2]*kalman_data->x_plus[0] + kalman_data->Phi_matrix[3]*kalman_data->x_plus[1];
	P_minus[0] = (kalman_data->Phi_matrix[0]*kalman_data->P_plus[0] + kalman_data->Phi_matrix[1]*kalman_data->P_plus[2])*kalman_data->Phi_matrix[0];
	P_minus[0] += (kalman_data->Phi_matrix[0]*kalman_data->P_plus[1] + kalman_data->Phi_matrix[1]*kalman_data->P_plus[3])*kalman_data->Phi_matrix[1];
	P_minus[0] += kalman_data->Q_matrix[0];
	P_minus[1] = (kalman_data->Phi_matrix[0]*kalman_data->P_plus[0] + kalman_data->Phi_matrix[1]*kalman_data->P_plus[2])*kalman_data->Phi_matrix[2];
	P_minus[1] += (kalman_data->Phi_matrix[0]*kalman_data->P_plus[1] + kalman_data->Phi_matrix[1]*kalman_data->P_plus[3])*kalman_data->Phi_matrix[3];
	P_minus[1] += kalman_data->Q_matrix[1];
	P_minus[2] = (kalman_data->Phi_matrix[2]*kalman_data->P_plus[0] + kalman_data->Phi_matrix[3]*kalman_data->P_plus[2])*kalman_data->Phi_matrix[0];
	P_minus[2] += (kalman_data->Phi_matrix[2]*kalman_data->P_plus[1] + kalman_data->Phi_matrix[3]*kalman_data->P_plus[3])*kalman_data->Phi_matrix[1];
	P_minus[2] += kalman_data->Q_matrix[2];
	P_minus[3] = (kalman_data->Phi_matrix[2]*kalman_data->P_plus[0] + kalman_data->Phi_matrix[3]*kalman_data->P_plus[2])*kalman_data->Phi_matrix[2];
	P_minus[3] += (kalman_data->Phi_matrix[2]*kalman_data->P_plus[1] + kalman_data->Phi_matrix[3]*kalman_data->P_plus[3])*kalman_data->Phi_matrix[3];
	P_minus[3] += kalman_data->Q_matrix[3];
	/* Kalman Gain */
	temp_help = (kalman_data->H_matrix[0]*P_minus[0] + kalman_data->H_matrix[1]*P_minus[2])*kalman_data->H_matrix[0];
	temp_help += (kalman_data->H_matrix[0]*P_minus[1] + kalman_data->H_matrix[1]*P_minus[3])*kalman_data->H_matrix[1];
	temp_help += kalman_data->R_matrix;
	K_gain[0] = (kalman_data->H_matrix[0]*P_minus[0] + kalman_data->H_matrix[1]*P_minus[1])/temp_help; /* temp_help shall be !=0 */
	K_gain[1] = (kalman_data->H_matrix[0]*P_minus[2] + kalman_data->H_matrix[1]*P_minus[3])/temp_help;
	/* Correction Step */
	kalman_data->P_plus[0] = (1.0 - K_gain[0]*kalman_data->H_matrix[0])*P_minus[0] - K_gain[0]*kalman_data->H_matrix[1]*P_minus[2];
	kalman_data->P_plus[1] = (1.0 - K_gain[0]*kalman_data->H_matrix[0])*P_minus[1] - K_gain[0]*kalman_data->H_matrix[1]*P_minus[3];
	kalman_data->P_plus[2] = -K_gain[1]*kalman_data->H_matrix[0]*P_minus[0] + (1.0 - K_gain[1]*kalman_data->H_matrix[1])*P_minus[2];
	kalman_data->P_plus[3] = -K_gain[1]*kalman_data->H_matrix[0]*P_minus[1] + (1.0 - K_gain[1]*kalman_data->H_matrix[1])*P_minus[3];
	kalman_data->x_plus[0] = x_minus[0] + K_gain[0]*(input - x_minus[0]);
	kalman_data->x_plus[1] = x_minus[1] + K_gain[1]*(input - x_minus[0]);
	
	return kalman_data->x_plus[0];
}