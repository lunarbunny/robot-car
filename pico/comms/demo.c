#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#define UART_ID uart0
#define BAUD_RATE 115200

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#define UART_TX_PIN 0
#define UART_RX_PIN 1



void COMMS_init ();
void COMMS_sendCharToM5( int key, char* str );
void COMMS_sendFloatToM5(int key, float speed);

void COMMS_init(){
            // Set up our UART with the required speed.
        uart_init(UART_ID, BAUD_RATE);

        // Set the TX and RX pins by using the function select on the GPIO
        // Set datasheet for more information on function select
        gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
        gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

}

void COMMS_sendFloatToM5(int key, float value){
    
    char* buffer[255];
    if (key == 1){
        snprintf(buffer, 255, "{\"speed\":%.2f}\n", value);
    }else if(key == 2){
        snprintf(buffer, 255, "{\"hump\":%.2f}\n", value);
    }
    
    uart_puts(UART_ID,buffer);
}

void COMMS_sendCharToM5( int key, char* str){

    //COMMS_init(); // setup pin info0
    char* buffer[255];//value to send over to m5
    // key 1 = sending hump
    if (key == 1){
                
            // key 2 = sending barcode
            }else if (key == 2){
                snprintf(buffer, 255, "{\"barcode\":%c}\n", str);
            // sending mapping value
            uart_puts(UART_ID,buffer);
            }
            
}

int main() {
        // how it sends over
            COMMS_init();
            while (1){
             int r = rand()%100;
            float speed=(float)rand()/((float)RAND_MAX/r);
            COMMS_sendFloatToM5(1,speed);
            sleep(2);
            float hump=(float)rand()/((float)RAND_MAX/r);
            COMMS_sendFloatToM5(2,hump); 
            COMMS_sendCharToM5(2,'A');     

            }
            
            
        
        
 
}
