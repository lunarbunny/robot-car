#include "movingAverageFilter.h"

float movingAverageFilter(float *ptrDataArray, float *ptrSum, int position, float nextData)
{
    // subtract the oldest number from the previous sum, add the new number
    *ptrSum = *ptrSum - ptrDataArray[position] + nextData;
    // assign the nextData to the position in the array
    ptrDataArray[position] = nextData;
    // return the average
    return *ptrSum / WINDOWLEN;
}