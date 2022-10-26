#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/types.h" // Timers

// Define use of Pico LED
#define GPIO_PIN_LED 25

// Define Infrared Sensor Needs
#define GPIO_PIN_INFRARED_IN1 15

int main() {
    stdio_init_all(); // For printing into PUTTY program

    // Declarations
    bool infraFlag = true;
    int no1 = 1;
    int no10 = 10;
    int arrayVar = 0;           // Used in main loop to check 1 char scanned // To-do: Make it dynamic 
    int charStartEndCheck[10];  // Used in checking for * symbol
    int timeChanges[9];         // Stores the differences in timings

    // Initialization
    gpio_init(GPIO_PIN_LED);
    gpio_init(GPIO_PIN_INFRARED_IN1);

    // Set if its input or output
    gpio_set_dir(GPIO_PIN_LED, GPIO_OUT);           // LED out
    gpio_set_dir(GPIO_PIN_INFRARED_IN1, GPIO_IN);   // S1 in

    // Setting initial state
    gpio_put(GPIO_PIN_LED, 0);  // Off

    // Main loop
    while (true) {

        if (arrayVar == 10) // 1 char was read 
        {
            int temp = charStartEndCheck[0];
            for (int i = 0; i < 10; i++) {
                charStartEndCheck[i] -= temp;
                // printf("T%i in miliseconds corrected: %i \n", i, charStartEndCheck[i]);
            }

            /* To sort and find out the diff between each time interval */
            for (int i = 0; i < 9; i++) {
                timeChanges[i] = charStartEndCheck[i + 1] - charStartEndCheck[i]; // Differences in each timing captured
                printf("T%i in miliseconds corrected: %i (%i) \n", i, charStartEndCheck[i], timeChanges[i]);
            }

            printf("\n");
            arrayVar = 0; // Reset var to start new char reading 

            /* Find if its long or short, black or white ooO ooA */
            int h1 = INT32_MIN; //TOP 1st, index of top 1st
            int h2 = INT32_MIN; //TOP 2nd, index of top 2nd
            int h3 = INT32_MIN; //TOP 3rd, index of top 3rd
            int h1i, h2i, h3i = 0;
            for (int i = 0; i < 9; i++)
            {
                if (timeChanges[i] > h1) {
                    h1i = i;
                    h3 = h2;
                    h2 = h1;
                    h1 = timeChanges[i];
                }
                else if (timeChanges[i] > h2) {
                    h2i = i;
                    h3 = h2;
                    h2 = timeChanges[i];
                }
                else if (timeChanges[i] > h3) {
                    h3i = i;
                    h3 = timeChanges[i];
                }
            }
            printf("TOP 1st is %i from T%i - T%i \n", h1, h1i, h1i + 1);
            printf("TOP 2nd is %i from T%i - T%i \n", h2, h2i, h2i + 1);
            printf("TOP 3rd is %i from T%i - T%i \n\n", h3, h3i, h3i + 1);


            char black = 'B'; //black
            char white = 'W'; //white

            printf("0 short, 1 long, here are the results of the scan: \n");
            for (int i = 0; i < 9; i++) {
                if (i % 2) {
                    if (i == h1i || i == h2i || i == h3i) { printf("T%i-T%i (%c1)\n", i, i + 1, white); }
                    else { printf("T%i-T%i (%c0) \n", i, i + 1, white); }
                }
                else {
                    if (i == h1i || i == h2i || i == h3i) { printf("T%i-T%i (%c1)\n", i, i + 1, black); }
                    else { printf("T%i-T%i (%c0) \n", i, i + 1, black); }
                }


            }
        }



        /** SCANNING OF B/W **/
        if (gpio_get(GPIO_PIN_INFRARED_IN1) == 1) // BLACK BAR
        {
            if (infraFlag == true) {
                infraFlag = false;
                printf("T%i: %i\n", arrayVar, time_us_32() / 1000);
                charStartEndCheck[arrayVar] = time_us_32() / 1000;
                arrayVar += 1;

            }
            gpio_put(GPIO_PIN_LED, 1);  // On
            // printf("Here is the current time in miliseconds: %i\n", time_us_32()/1000);
        }
        else {
            if (infraFlag == false) {
                infraFlag = true;
                printf("T%i: %i\n", arrayVar, time_us_32() / 1000);
                charStartEndCheck[arrayVar] = time_us_32() / 1000;
                arrayVar += 1;
            }
            gpio_put(GPIO_PIN_LED, 0);  // Off
        }

    }

}

