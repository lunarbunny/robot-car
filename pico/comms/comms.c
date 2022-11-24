#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include <string.h>
#include "comms.h"

#define UART_ID uart0
#define BAUD_RATE 115200

#define UART_TX_PIN 0
#define UART_RX_PIN 1

void COMMS_init(void)
{
    printf("[Comms] Init start \n");

    // Set up our UART with the required speed.
    uart_init(UART_ID, BAUD_RATE);

    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    printf("[Comms] Init done \n");
}

void COMMS_sendFloatToM5(float speed)
{
    char *buffer[255];
    snprintf(buffer, 255, "{\"speed\":%.2f}\n", speed);
    uart_puts(UART_ID, buffer);
}

void COMMS_sendCharToM5(int key, char *str)
{
    char *buffer[255]; // value to send over to m5
    // key 1 = sending hump
    if (key == 1)
    {
        snprintf(buffer, 255, "{\"hump\":%d}\n", str);
        // key 2 = sending barcode
    }
    else if (key == 2)
    {
        snprintf(buffer, 255, "{\"barcode\":%d}\n", str);
        // sending mapping value
    }
    uart_puts(UART_ID, buffer);
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
