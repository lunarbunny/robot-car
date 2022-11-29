#ifndef movingAverageFilter_H
#define movingAverageFilter_H

#define WINDOWLEN 4

extern float movingAverageFilter(float *ptrDataArray, float *ptrSum, int position, float nextData);

#endif