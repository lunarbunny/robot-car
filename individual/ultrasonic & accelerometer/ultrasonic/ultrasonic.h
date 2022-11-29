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
