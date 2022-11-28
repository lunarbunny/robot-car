## COMMUNICATION
### Summary:
For communication, sensor values are gathered from the other modules of the car, and function to send data over to M5StickCPlus will be called.
The current protocol that we are using is UART. Once communication via UART is established with M5StickCPlus, information is send over to front-end API called Thingsboard where we can display these information in Graphical visuals.

## M5StickCPlus 
For programming for M5StickCPlus, we are using UIFlow from M5Stack.

To run the code in M5StickCPlus:
1. The file which stores the code can be found in the Comms folder
2. The link can be found here: https://flow.m5stack.com/ 
3. Copy and paste the code into UIFlow
4. Make sure M5StickCPlus is powered on and connected to cloud
5. Enter the API key displayed on the M5StickCPlus and ensure that it is connected.
6. Run the UIFlow code before running the C program from Raspberry Pico.

![image](https://user-images.githubusercontent.com/90168655/204292772-dfe4f6b0-6d3f-4b70-a6b9-5456f93f4940.png)


## Raspberry Pico
To run the code in Raspberry Pico:
1. Go to the build the code and connect the Raspberry Pico to your laptop.
2. drag the .uf2 file into the Raspberry Pico folder

## Functions:
### COMMS_init():
This function is called to initialise communication between Pico and M5StickCPlus via UART and set up the TX and RX pins required (GPIO0 and GPIO1).

### COMMS_sendToM5(int key, void \*data):
This function takes in any data from the other modules from the car.
The data is differentiated using a key so that we know which data is being transferred over
After that, the data is sent over to M5StickCPlus as a JSON string. The JSON String is to help us to over to the front-end api via MQTT from the M5StickCPlus.

## Thingsboard
Thingsboard is the front-end api that we are using to display our sensor values in visuals. As data is transmitted over, they will be constantly updated in the telemetry and the visuals as well.

![image](https://user-images.githubusercontent.com/90168655/204291777-d5e0a8df-2068-4e63-973b-b2aef8945831.png)

