#include <stdio.h>
#include <stdlib.h>
// Pico libraries
#include "pico/types.h" // Timers
#include "pico/stdlib.h"
#include "pico/stdio.h"

// Define use of Pico LED
#define GPIO_PIN_LED 25

// Define Infrared Sensor Needs
#define GPIO_PIN_INFRARED_IN1 15

int compareArray(int a[],int b[])	{
	int i;
	for(i = 0; i < 9; i++){
		if(a[i]!=b[i])
			return 1; // If array not equal, return 1
	}
	return 0;   // If array equal, return 0
}

int main () {
    stdio_init_all(); // For printing into PUTTY program

    // Declarations
    bool infraFlag = true;
    int arrayVar = 0;           // Used in main loop to check 1 char scanned // To-do: Make it dynamic 
    int charStartEndCheck[9];  // Used in checking for * symbol
    int timeChanges[9];         // Stores the differences in timings
    int charASCII[8];

    // Char maps
    int varcharStartEndStar[9];

    int charStartEndStar[9] = {0, 1, 0, 0, 1, 0, 1, 0, 0};
    int charSpace[9] = {0, 1, 1, 0, 0, 0, 1, 0, 0};
    int letterA[9] = {1, 0, 0, 0, 0, 1, 0, 0, 1};

    int letterB[9] = {0, 0, 1, 0, 0, 1, 0, 0, 1};
    int letterC[9] = {1, 0, 1, 0, 0, 1, 0, 0, 0};
    int letterD[9] = {0, 0, 0, 0, 1, 1, 0, 0, 1};
    int letterE[9] = {1, 0, 0, 0, 1, 1, 0, 0, 0};
    int letterF[9] = {0, 0, 1, 0, 1, 1, 0 ,0 ,0};
    int letterG[9] = {0, 0, 0, 0, 0, 1, 1, 0, 1};
    int letterH[9] = {1, 0, 0, 0, 0, 1, 1, 0, 0};
    int letterI[9] = {0, 0, 1, 0, 0, 1, 1, 0 ,0};
    int letterJ[9] = {0, 0, 0, 0, 1, 1, 1, 0, 0};
    int letterK[9] = {1, 0, 0, 0, 0, 0, 0, 1, 1};
    int letterL[9] = {0, 0, 1, 0, 0, 0, 0, 1, 1};
    int letterM[9] = {1, 0, 1, 0, 0, 0, 0, 1, 0};
    int letterN[9] = {0, 0, 0, 0, 1, 0, 0, 1, 1};
    int letterO[9] = {1, 0, 0, 0, 1, 0, 0, 1, 0};
    int letterP[9] = {0, 0, 1, 0, 1, 0, 0, 1, 0};
    int letterQ[9] = {0, 0, 0, 0, 0, 0, 1, 1, 1};
    int letterR[9] = {1, 0, 0, 0, 0, 0, 1, 1, 0};
    int letterS[9] = {0, 0, 1, 0, 0, 0, 1, 1, 0};
    int letterT[9] = {0, 0, 0, 0, 1, 0, 1, 1, 0};
    int letterU[9] = {1, 1, 0, 0, 0, 0, 0, 0, 1};
    int letterV[9] = {0, 1, 1, 0, 0, 0, 0, 0, 1};
    int letterW[9] = {1, 1, 1, 0, 0, 0, 0, 0, 0};
    int letterX[9] = {0, 1, 0, 0, 1, 0, 0, 0, 1};
    int letterY[9] = {1, 1, 0, 0, 1, 0, 0, 0, 0};
    int letterZ[9] = {0, 1, 1, 0, 1, 0, 0, 0, 0};
    int charDash[9] = {0, 1, 0, 0, 0, 0, 1, 0, 1};
    int charPeriod[9] = {1, 1, 0, 0, 0, 0, 1, 0, 0};
    int charDollar[9] = {0, 1, 0, 1, 0, 1, 0, 0, 0};
    int charBackslash[9] = {0, 1, 0, 1, 0, 0, 0, 1, 0};
    int charPlus[9] = {0, 1, 0, 0, 0, 1, 0, 1, 0};
    int charPercentage[9] = {0, 0, 0, 1, 0, 1, 0, 1, 0};
    int no0[9] = {0, 0, 0, 1, 1, 0, 1, 0, 0};
    int no1[9] = {1, 0, 0, 1, 0, 0, 0, 0, 1};
    int no2[9] = {0, 0, 1, 1, 0, 0, 0, 0, 1};
    int no3[9] = {1, 0, 1, 1, 0, 0, 0, 0, 0};
    int no4[9] = {0, 0, 0, 1, 1, 0, 0, 0, 1};
    int no5[9] = {1, 0, 0, 1, 1, 0, 0, 0, 0};
    int no6[9] = {0, 0, 1, 1, 1, 0, 0, 0, 0};
    int no7[9] = {0, 0, 0, 1, 0, 0, 1, 0, 1};
    int no8[9] = {1, 0, 0, 1, 0, 0, 1, 0, 0};
    int no9[9] = {0, 0, 1, 1, 0, 0, 1, 0, 0};

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
                timeChanges[i] = charStartEndCheck[i+1] - charStartEndCheck[i]; // Differences in each timing captured
                printf("T%i in miliseconds corrected: %i (%i) \n", i, charStartEndCheck[i], timeChanges[i]);
            }

            printf("\n");
            arrayVar = 0; // Reset var to start new char reading 

            /* Find if its long or short, black or white ooO ooA */
            int h1 = INT32_MIN; //TOP 1st, index of top 1st
            int h2 = INT32_MIN; //TOP 2nd, index of top 2nd
            int h3 = INT32_MIN; //TOP 3rd, index of top 3rd
            int h1i, h2i, h3i = 0;
            for(int i = 0; i < 9; i++)
            {
                if(timeChanges[i] > h1) { 
                    h1i = i;
                    h3 = h2;                
                    h2 = h1;                
                    h1 = timeChanges[i];
                }
                else if (timeChanges[i] > h2){ 
                    h2i = i;
                    h3 = h2;                
                    h2 = timeChanges[i];  
                }
                else if (timeChanges[i] > h3){ 
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
                    if (i == h1i || i == h2i || i == h3i) {printf("T%i-T%i (%c1)\n", i, i+1, white); varcharStartEndStar[i] = 1;}
                    else {printf("T%i-T%i (%c0) \n", i, i+1, white); varcharStartEndStar[i] = 0;}
                } 
                else {
                    if (i == h1i || i == h2i || i == h3i) {printf("T%i-T%i (%c1)\n", i, i+1, black); varcharStartEndStar[i] = 1;}
                    else {printf("T%i-T%i (%c0) \n", i, i+1, black); varcharStartEndStar[i] = 0;}
                }
            }

            // PRINTING CHARBINARY 
            if (compareArray(varcharStartEndStar, charStartEndStar)==0) {printf("MATCHES START END \n");} else {printf("DOESNT MATCH START END \n");}
            if (compareArray(varcharStartEndStar, charSpace)==0) {printf("MATCHES SPACE \n");} else {printf("DOESNT MATCH SPACE \n");}
            if (compareArray(varcharStartEndStar, letterA)==0) {printf("MATCHES A \n");} else {printf("DOESNT MATCH A \n");}
        }

        

        /** SCANNING OF B/W **/
        if (gpio_get(GPIO_PIN_INFRARED_IN1) == 1) // BLACK BAR
        {
            if (infraFlag == true) {
                infraFlag = false; 
                printf("T%i: %i\n", arrayVar, time_us_32()/1000);
                charStartEndCheck[arrayVar] = time_us_32()/1000;
                arrayVar += 1;
                
            }
            gpio_put(GPIO_PIN_LED, 1);  // On
            // printf("Here is the current time in miliseconds: %i\n", time_us_32()/1000);
        }
        else {
            if (infraFlag == false) {
                infraFlag = true; 
                printf("T%i: %i\n", arrayVar, time_us_32()/1000);
                charStartEndCheck[arrayVar] = time_us_32()/1000;
                arrayVar += 1;
            }
            gpio_put(GPIO_PIN_LED, 0);  // Off
        }
    
    }

}