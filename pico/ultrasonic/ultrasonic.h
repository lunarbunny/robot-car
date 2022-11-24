#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

#define ULTRASONIC_FRONT 1
#define ULTRASONIC_RIGHT 2
#define ULTRASONIC_REAR 4
#define ULTRASONIC_LEFT 8

void ULTRASONIC_init(void);
float ULTRASONIC_getCM(int ultrasonic);

#endif
