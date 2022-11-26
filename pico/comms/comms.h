#ifndef comms_H_
#define comms_H_

#define TX_KEY_SPEED 1
#define TX_KEY_HUMP_HEIGHT 2
#define TX_KEY_BARCODE 3
#define TX_KEY_MAPPING 4

void COMMS_init(void);
void COMMS_sendToM5(int key, void *data);

#endif