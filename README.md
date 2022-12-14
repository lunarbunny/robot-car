## Introduction

ICT2104 Team B2
An embedded systems project.

### Getting Started

##### Directory Structure

###### &#128193; graph
Python scripts used to plot graphs.

###### &#128193; individual
Contains code and test data from individual sub-module teams.

###### &#128193; pico
A CMake project containing the source code.
Divided into sub-folders each containing one sub-module, depicted below.
```bash
pico
├── accelerometer
├── comms
├── encoder
├── infrared
├── motor (PWM + PID controller)
├── navigation
├── ultrasonic
└── main.c (program entry point)
```

A `README.md` can be found in each sub-module folder with explaination for each module. 

##### Hardware
* Raspberry Pico
* 1x L298N Motor Driver
* 2x DC Motor
* 2x Encoder
* 1x Accelerometer
* 4x HC-SR04 Ultrasonic Sensor
* 1x M5StickC

##### Pin Layout
| Hardware | GPIO # |
|---|---|
| M5StickC | 0 - UART (TX) <br/> 1 - UART (RX) |
| Encoder | 2 - Left Motor <br/> 3 - Right Motor |
| Accelerometer | 4 - I2C (SDA) <br/> 5 - I2C (SDL) |
| Motor Control | 6 - IN1 <br/> 7 - IN2 <br/> 8 - IN3 <br/> 9 - IN4|
| Motor PWM | 14 - Left Motor <br/> 15 - Right Motor |
| Infrared | 26 - Digital IN |

##### Software
Follow the instructions for setting up PicoSDK for your OS.
https://github.com/raspberrypi/pico-sdk

## Members

<table>
  <tr>
    <td><strong>Module</strong></td>
    <td><strong>Members</strong></td>
  </tr>
  <tr>
    <td>Motor</td>
    <td>
    Wang Ting Wei
    <br/>
    Chow Sow Ying
    <br/>
    Loke Jia Xin
    </td>
  </tr>
  <tr>
    <td>Ultrasonic &<br/>Accelerometer</td>
    <td>
    Lee Yan Rong
    <br/>
    Khor Chai Hong
    </td>
  </tr>
  <tr>
    <td>Infrared &<br/>Encoder</td>
    <td>
    Kye Jordan O’Reilly
    <br/>
    Yeow Yen Yenn
    <br/>
    Nadzimushsyahar Bin Mohd Halel
    </td>
  </tr>
  <tr>
    <td>Communications</td>
    <td>
    Tan Ru Bin
    <br/>
    Chen Yan Jiun
    <br/>
    Ong Yi Meng
    </td>
  </tr>
  <tr>
    <td>Mapping &<br/>Navigation</td>
    <td>
    Maximus Lee Zhi Qin
    <br/>
    Jin Zheng Long
    </td>
  </tr>
</table>