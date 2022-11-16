#ifndef encoder_H_
#define encoder_H_

void ENCODER_init(void);
float ENCODER_getLeftWheelSpeed(void);
float ENCODER_getRightWheelSpeed(void);

//Global Variables
extern volatile int leftEncoderISR;
extern volatile int rightEncoderISR;

void resetEncoderISRCount();
int getLeftISRCount();
int getRightISRCount();

#endif