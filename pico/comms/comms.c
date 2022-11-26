#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include <string.h>
#include "comms.h"

#define UART_ID uart0
#define BAUD_RATE 115200

#define UART_TX_PIN 0
#define UART_RX_PIN 1

#define BUFFER_SIZE 256

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

void COMMS_sendToM5(int key, void *data)
{
    char buffer[BUFFER_SIZE]; // value to send over to m5

    switch (key)
    {
    case TX_KEY_SPEED:
        snprintf(buffer, 255, "{\"speed\":%.2f}\n", *((float *)data));
        break;
    case TX_KEY_HUMP_HEIGHT:
        snprintf(buffer, 255, "{\"hump\":%d}\n", *((float *)data));
        break;
    case TX_KEY_BARCODE:
        snprintf(buffer, 255, "{\"barcode\":%c}\n", *((char *)data));
        break;
    case TX_KEY_MAPPING:
        break;
    }

    uart_puts(UART_ID, buffer); // send over to m5
}
