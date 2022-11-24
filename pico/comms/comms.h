#ifndef comms_H_
#define comms_H_

#define KEY_SPEED 1
#define KEY_HUMP 2
#define KEY_MAPPING 3
#define KEY_BARCODE 4

void COMMS_init(void);
void COMMS_sendToM5(int key, char *str);

void COMMS_sendFloatToM5(float speed);
void COMMS_sendCharToM5(int key, char *str);

#endif