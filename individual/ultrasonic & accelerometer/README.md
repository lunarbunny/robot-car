## Ultrasonic & Accelerometer module

### Introduction

This submodule contains two components:
1. Accelerometer `accelerometer.c`, `accelerometer.h`, `kalman.c`, `kalman.h`, `movingAverageFilter.c`, `movingAverageFilter.h`
2. Ultrasonic `kalman.c`,  `kalman.h`, `ultrasonic.c`, `ultrasonic.h`
3. Main combined with accelerometer and ultrasonic `main.c`

### Explanation

This submodule contains two components which is the ultrasonic and accelerometer. The ultrasonic data is filtered using kalman algorithm and the accelerometer is filtered using the moving Average Filter. The kalman in accelerometer library is used to make comparison with the moving average filter algorithm.


![accelerometer filter algo analysis_R10_Q0 1](https://user-images.githubusercontent.com/66397701/204321584-c063e1d5-3ded-4b87-8c19-19834dbea8ad.png)
![moving average filter algo analysis](https://user-images.githubusercontent.com/66397701/204321688-353f5a21-4266-4f67-a328-c91d3a7f5e44.png)
![ultrasonic filter algo analysis_R10_Q0 1](https://user-images.githubusercontent.com/66397701/204321757-039bc808-44ce-44dc-941f-47b4e4555d5c.png)

Definitions in `ultrasonic.h`:
```c
#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

#define ULTRASONIC_FRONT 1
#define ULTRASONIC_RIGHT 2
#define ULTRASONIC_REAR 4
#define ULTRASONIC_LEFT 8
#define MAXPULSELEN 1044 // 18cm * 58 = 1044us 

void ULTRASONIC_init(void);
float ULTRASONIC_getCM(int ultrasonic);
float getCM(uint trigPin, uint echoPin, bool filter);
void ULTRASONIC_resetFilter();
int ULTRASONIC_hitWall();

#endif
```

Definitions in `kalman.h` in ultrasonic component:
```c
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
```

Definitions in `movingAverageFilter.h`:
```c
#ifndef movingAverageFilter_H
#define movingAverageFilter_H

#define WINDOWLEN 4

extern float movingAverageFilter(float *ptrDataArray, float *ptrSum, int position, float nextData);

#endif
```

Definitions in `kalman.h` in accelerometer component:
```c
#ifndef kalman_H
#define kalman_H

extern float kalmanFilter(float R, float Q, float measuredX, float *Xt_prev, float *Pt_prev);

#endif
```

Definitions in `movingAverageFilter.h` :
```c
#ifndef movingAverageFilter_H
#define movingAverageFilter_H

#define WINDOWLEN 4

extern float movingAverageFilter(float *ptrDataArray, float *ptrSum, int position, float nextData);

#endif
```

