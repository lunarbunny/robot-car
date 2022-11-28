## Infrared Module

### In a nutshell
Timings are gathered and stored in charStartEndCheck, differences between each charStartEndCheck will be stored in timeChanges. This will be sorted into 1's and 0's representing a character in varcharStartEndStar to be used for decoding.
![image](https://user-images.githubusercontent.com/23163333/204268668-e41cc500-3357-4995-8677-cd736ee64443.png)


## Functions

### INFRARED_init()
Function used in main.c to initialize GPIO pins necessary for the barcode reader to function.

### compareArray()
Function commonly used throughout the infraredMain.c to compare 2 arrays.

### readyToStartBarcode()
Function used to detect when to start the barcode. Used in main.c to start the barcode state when this functions returns 1;

### INFRARED_scanning()
Function to get the system time when color changes from white to black OR black to white. The time is assigned into 1 index in an array, saving a total of 9 timings

### INFRARED_readyToReturnChar()
Function returns true if detection if the whole barcode has been read from start to finish. This is done by detecting 2 asterisk characters - One at the beginning and one at the end, encapsulating the data.

### INFRARED_returnChar()
Once readyToReturnChar() function returns true, sannitize the finalString by removing the asterisk characters and setting the finalString to contain just the data from the barcode.

### INFRARED_resetForNewString
Function resets all the variables responsible for getting the character from the raw values of timings, so that it is ready to read a new barcode.

### INFRARED_decodeCharTree
Function used to decode the 1's and 0's in varcharStartEndStar into the actual letters. Uses a searching tree algorithm to make searching for the character significantly faster. Once decoded the character, it is then concatenated into the variable finalString, to be used in INFRARED_returnChar();

### INFRARED_oneCharRead
Function returns true if 10 timings have come in, (1 character read).

### INFRARED_sortingTimings
Function sorts the raw timings received in readyToStartBarcode() into 1's and 0's used in INFRARED_decodeCharTree(). This is done following these steps:

1. First it gets the differences between each value and stores them in timeChanges array. [] *Note that if the time changes is more than timeout (e.g. 1.2s), then INFRARED_resetForNewString() runs, resetting all the variables. This is to prevent a softlock and inaccurate reading.
2. Once it has found all the changes between each timing of reading, it will find the top 3 timings. This is because each character in the BARCODE 39 standard has 3 long bars. 
3. Once we get the top 3 variables, it then converts where the long timings are to 1's and 0's. Where 1's are the long bars and 0's are the short bars. Since the barcode always starts with black, no extra code is needed for this.
4. We next check if the character is an asterisk or a backward asterisk (P char). If it is, a flag arrayStarNotMatch is changed to false. If it's a backward asterisk, another flag isBackwards is changed to true. Whether it is an asterisk or char P as the first character, it will ensure that there is only 2 such instances. One at the beginning of the barcode and one at the end.
5. To denoise, before the IR sensor reads the first value, it might pick up unnecessary noise. If arrayStarNotMatch is true, meaning that the first character is not yet a star, it will set the arrayVar to 9, and shift charStartEndCheck indexes to the left by 1, leaving a new empty index at the end of the array, ready for the new timing to come in. Once a new value comes in, it will check if its a star, if not repeat. If it's a star things will be processed as in step 4.
6. If arrayStarNotMatch is false, then set arrayVar to 0. This is because if we continue to use the queue to check eachh time a new variable comes in, the finalString will contain a lot of unnecessary characters, making it inaccurate. Setting it to 0 ensures that the character is only compared to the tree when one char has been read. 
