#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

    // Character Logic Variables
    int startReading = 0; 
    int varcharStartEndStar[9]; // Used for only comparing * (starting/ ending characters)
    int countStar = 0;
    char varCharASCII = '~'; // Used for storing main data character
    char finalString[22];   // 1 used for *, 1 to store ending char '/0' if not buffer overflow; 20 char storage

    // CODE39 Char Arrays. 1 is LONG bar. 0 is SHORT.
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
                // printf("T%i in miliseconds corrected: %i (%i) \n", i, charStartEndCheck[i], timeChanges[i]);
            }

            printf("New char detected\n");
            arrayVar = 0; // Reset var to start new char reading 

            /* Find if its long or short bars, black or white ooO ooA */
            int h1 = INT32_MIN; //TOP 1st, index of top 1st
            int h2 = INT32_MIN; //TOP 2nd, index of top 2nd
            int h3 = INT32_MIN; //TOP 3rd, index of top 3rd
            int h1i, h2i, h3i = 0;
            for(int i = 0; i < 9; i++)
            {
                if(timeChanges[i] > h1) { 
                    h3i = h2i;
                    h2i = h1i;
                    h1i = i;

                    h3 = h2;                
                    h2 = h1;                
                    h1 = timeChanges[i];
                }
                else if (timeChanges[i] > h2){ 
                    h3i = h2i;
                    h2i = i;

                    h3 = h2;                
                    h2 = timeChanges[i];  
                }
                else if (timeChanges[i] > h3){ 
                    h3i = i;
                    h3 = timeChanges[i];
                }
            }

            /* Remembers the position of the long bars in respect to how the timings came in */
            for (int i = 0; i < 9; i++) {
                if (i % 2) {
                    if (i == h1i || i == h2i || i == h3i) {varcharStartEndStar[i] = 1;}
                    else { varcharStartEndStar[i] = 0;}
                } 
                else {
                    if (i == h1i || i == h2i || i == h3i) { varcharStartEndStar[i] = 1;}
                    else { varcharStartEndStar[i] = 0;}
                }
            }

            // If character = * || Used to know when to deliver message - By looking at if got starting and ending character
            if (compareArray(varcharStartEndStar, charStartEndStar)==0 && varCharASCII != '*')
            {
                varCharASCII = '*'; // Prevent looping in case of slow reading
                if (countStar < 2) {
                    countStar ++;
                    // printf("=====ANNEYONG IDK KOREAN=====> %i", countStar);
                }
            }

            /* START CHARACTER SORTING */
            // If first number in data section of array starts with 1 
            // KRAHOE185MC3U.YW
            if (varcharStartEndStar[0]){ // Currently using varcharStartEndStar, 
                if (varcharStartEndStar[8]) 
                {
                    // U1AK
                    if (compareArray(varcharStartEndStar, letterU)==0) {varCharASCII = 'U';}
                    else if (compareArray(varcharStartEndStar, no1)==0) {varCharASCII = '1';}
                    else if (compareArray(varcharStartEndStar, letterA)==0) {varCharASCII = 'A';}
                    else {varCharASCII = 'K';}
                } 
                
                else if (varcharStartEndStar[2])
                {
                    // W3CM
                    if (compareArray(varcharStartEndStar, letterW)==0) {varCharASCII = 'W';}
                    else if (compareArray(varcharStartEndStar, no3)==0) {varCharASCII = '3';}
                    else if (compareArray(varcharStartEndStar, letterC)==0) {varCharASCII = 'C';}
                    else {varCharASCII = 'M';}
                }
                else if (varcharStartEndStar[6])
                {
                    //.8HR
                    if (compareArray(varcharStartEndStar, charPeriod)==0) {varCharASCII = '.';}
                    else if (compareArray(varcharStartEndStar, no8)==0) {varCharASCII = '8';}
                    else if (compareArray(varcharStartEndStar, letterH)==0) {varCharASCII = 'H';}
                    else {varCharASCII = 'R';}
                }
                else 
                {
                    // OE5Y
                    if (compareArray(varcharStartEndStar, letterO)==0) {varCharASCII = 'O';}
                    else if (compareArray(varcharStartEndStar, letterE)==0) {varCharASCII = 'E';}
                    else if (compareArray(varcharStartEndStar, no5)==0) {varCharASCII = '5';}
                    else {varCharASCII = 'Y';}
                }
            } 
            
            // If first number in data section of array starts with 0
            // QGNTDJ7%40LSBIPF296-+X*/$VZ SPACE
            else if (!varcharStartEndStar[0]) {
                // VX-2BL47DNGQ
                if (varcharStartEndStar[8]) // Ending number 1
                {
                    if (varcharStartEndStar[2]) {
                        //V2BL
                        if (compareArray(varcharStartEndStar, letterV)==0) {varCharASCII = 'V';}
                        else if (compareArray(varcharStartEndStar, no2)==0) {varCharASCII = '2';}
                        else if (compareArray(varcharStartEndStar, letterB)==0) {varCharASCII = 'B';}
                        else {varCharASCII = 'L';}
                    }
                    else if (varcharStartEndStar[6]) {
                        //-7GQ
                        if (compareArray(varcharStartEndStar, charDash)==0) {varCharASCII = '-';}
                        else if (compareArray(varcharStartEndStar, no7)==0) {varCharASCII = '7';}
                        else if (compareArray(varcharStartEndStar, letterG)==0) {varCharASCII = 'G';}
                        else {varCharASCII = 'Q';}
                    }
                    else {
                        //X4DN
                        if (compareArray(varcharStartEndStar, letterX)==0) {varCharASCII = 'X';}
                        else if (compareArray(varcharStartEndStar, no4)==0) {varCharASCII = '4';}
                        else if (compareArray(varcharStartEndStar, letterD)==0) {varCharASCII = 'D';}
                        else {varCharASCII = 'N';}
                    }
                    
                }
                else // Ending number 0
                {
                    // Z$/*+69FPIS0%JT SPACE <0>
                    if (varcharStartEndStar[1]) 
                    {
                        // Z$/+ SPACE
                        if (varcharStartEndStar[2]) {
                            // Z SPACE
                            if (compareArray(varcharStartEndStar, letterZ)==0) {varCharASCII = 'Z';}
                            else {varCharASCII = ' ';}
                        }
                        else {
                            // $/*+
                            if (compareArray(varcharStartEndStar, charStartEndStar)==0) {varCharASCII = '*';}
                            else if (compareArray(varcharStartEndStar, charDollar)==0) {varCharASCII = '$';}
                            else if (compareArray(varcharStartEndStar, charBackslash)==0) {varCharASCII = '/';}
                            else {varCharASCII = '+';}
                        } 
                    }
                    else if (varcharStartEndStar[2])
                    {
                        // 69FPIS
                        if (varcharStartEndStar[4]){
                            //6FP
                            if (compareArray(varcharStartEndStar, no6)==0) {varCharASCII = '6';}
                            else if (compareArray(varcharStartEndStar, letterF)==0) {varCharASCII = 'F';}
                            else {varCharASCII = 'P';}
                        }
                        else {
                            //9IS
                            if (compareArray(varcharStartEndStar, no9)==0) {varCharASCII = '9';}
                            else if (compareArray(varcharStartEndStar, letterI)==0) {varCharASCII = 'I';}
                            else {varCharASCII = 'S';}
                        }
                    }
                    else {
                        // 0%JT
                        if (compareArray(varcharStartEndStar, no0)==0) {varCharASCII = '0';}
                        else if (compareArray(varcharStartEndStar, charPercentage)==0) {varCharASCII = '%';}
                        else if (compareArray(varcharStartEndStar, letterJ)==0) {varCharASCII = 'J';}
                        else {varCharASCII = 'T';}
                    }
                }
            }
            /* END CHARACTER SORTING */

            strncat(finalString, &varCharASCII, 1); // Concatenating the characters into var finalString
        }

        /* WHEN COUNTSTAR = 2, PRINT THE FINAL STRING HERE */
        if (countStar >= 2) {
            printf("\n\n YOUR FINAL READING IS ON THE FOLLOWING LINE: \n \033[1;31m %s \033[0m", finalString);
            countStar = 0; // Reset count star
            varCharASCII = '~'; // Reset variable
            strcpy(finalString, ""); // Clear contents of string
        }

        /** SCANNING OF B/W **/
        if (gpio_get(GPIO_PIN_INFRARED_IN1) == 1) // BLACK BAR
        {
            if (infraFlag == true) {
                infraFlag = false; 
                sleep_us(100); // Delay of at least 10 microseconds NEEDS to be here otherwise it wont work
                charStartEndCheck[arrayVar] = time_us_32()/1000;
                arrayVar += 1;
                
            }
            gpio_put(GPIO_PIN_LED, 1);  // On
            // printf("Here is the current time in miliseconds: %i\n", time_us_32()/1000);
        }
        else {
            if (infraFlag == false) {
                infraFlag = true; 
                sleep_us(100); // Delay of at least 10 microseconds NEEDS to be here otherwise it wont work
                charStartEndCheck[arrayVar] = time_us_32()/1000;
                arrayVar += 1;
            }
            gpio_put(GPIO_PIN_LED, 0);  // Off
        }
    
    }

}