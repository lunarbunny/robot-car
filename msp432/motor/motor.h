void MOTOR_init(void);
void MOTOR_initDebug(void);

void MOTOR_setSpeed(float dutyCycle);
float MOTOR_getSpeed();

void MOTOR_forward(void);
void MOTOR_reverse(void);
int MOTOR_getDirection(void);
