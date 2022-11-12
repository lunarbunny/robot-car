#ifndef encoder_H_
#define encoder_H_

void ENCODER_init(void);
float ENCODER_getLeftWheelSpeed(void);
float ENCODER_getRightWheelSpeed(void);

void ENCODER_waitForISRInterrupts(uint target);
int ENCODER_cmToSteps(float cm);

#endif