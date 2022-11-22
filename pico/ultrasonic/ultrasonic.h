#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

#define ULTRASONIC_FRONT 1
#define ULTRASONIC_RIGHT 2
#define ULTRASONIC_REAR 4
#define ULTRASONIC_LEFT 8

void ULTRASONIC_init(void);
uint64_t getPulse(uint, uint);
uint64_t getCM(uint, uint);
uint64_t ULTRASONIC_getCM(int);

#endif
