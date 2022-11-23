#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include <string.h>
#include "comms.h"

#define UART_ID uart0
#define BAUD_RATE 115200

#define UART_TX_PIN 0
#define UART_RX_PIN 1

#define KEY_SPEED 1
#define KEY_HUMP 2
#define KEY_MAPPING 3
#define KEY_BARCODE 4

void COMMS_init()
{
    // Set up our UART with the required speed.
    uart_init(UART_ID, BAUD_RATE);

    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
}

void COMMS_sendToM5(int key, char *str)
{
    char value[256]; // value to send over to m5
    // sending speed value
    if (key == 1)
    {
        char speed[256] = "{\"speed\":";
        strcat(speed, str);
        char nextline[10] = "}\n";
        strcat(speed, nextline);
        strcpy(value, speed);
        // sending hump value
    }
    else if (key == KEY_HUMP)
    {
        char hump[256] = "{\"hump\":";
        strcat(hump, str);
        char nextline[10] = "}\n";
        strcat(hump, nextline);
        strcpy(value, hump);
        // sending mapping value
    }
    else if (key == KEY_MAPPING)
    {
        char mapping[256] = "{\"mapping\":";
        strcat(mapping, str);
        char nextline[10] = "}\n";
        strcat(mapping, nextline);
        strcpy(value, mapping);
        // sending barcode value
    }
    else if (key == KEY_BARCODE)
    {
        char barcode[256] = "{\"barcode\":";
        strcat(barcode, str);
        char nextline[10] = "}\n";
        strcat(barcode, nextline);
        strcpy(value, barcode);
    }
    // send over to m5
    uart_puts(UART_ID, value);
}

int main()
{
    // how it sends over
    sendToM5(1, "123");
    sendToM5(2, "50");
    sendToM5(3, "010");
    sendToM5(4, "*A*");
}
