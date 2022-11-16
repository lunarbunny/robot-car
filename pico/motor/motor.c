#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

#include "motor.h"
#include "../encoder/encoder.h"

#define GPIO_PIN_MOTOR1_PWM 4
#define GPIO_PIN_MOTOR2_PWM 5
#define GPIO_PIN_MOTOR1_IN1 6
#define GPIO_PIN_MOTOR1_IN2 7
#define GPIO_PIN_MOTOR2_IN3 8
#define GPIO_PIN_MOTOR2_IN4 9

#define PWM_CYCLE 10

uint pwm_slice;

float motorSpeedLeft = 0.f;
float motorSpeedRight = 0.f;

int motorDirLeft = -1;
int motorDirRight = -1;

const float slotCount = 20.00;  // 20 Slots in disk, change if different
const float wheelDiameter = 66.10; // Wheel diameter in millimeters, change if different

void MOTOR_init(void)
{
    printf("[Motor] Init start \n");

    // Initialize GPIO pins
    gpio_init(GPIO_PIN_MOTOR1_IN1);
    gpio_init(GPIO_PIN_MOTOR1_IN2);
    gpio_init(GPIO_PIN_MOTOR2_IN3);
    gpio_init(GPIO_PIN_MOTOR2_IN4);
    gpio_set_dir(GPIO_PIN_MOTOR1_IN1, GPIO_OUT);
    gpio_set_dir(GPIO_PIN_MOTOR1_IN2, GPIO_OUT);
    gpio_set_dir(GPIO_PIN_MOTOR2_IN3, GPIO_OUT);
    gpio_set_dir(GPIO_PIN_MOTOR2_IN4, GPIO_OUT);

    // Setup PWM pins
    gpio_set_function(GPIO_PIN_MOTOR1_PWM, GPIO_FUNC_PWM);
    gpio_set_function(GPIO_PIN_MOTOR2_PWM, GPIO_FUNC_PWM);
    pwm_slice = pwm_gpio_to_slice_num(GPIO_PIN_MOTOR1_PWM); // GPIO 0 and 1 is slice 0
    pwm_set_wrap(pwm_slice, PWM_CYCLE - 1); // Set period of 10 cycles (0-9)
    pwm_set_enabled(pwm_slice, true); // Enable PWM

    // Set default state of motor
    MOTOR_setDirection(MOTOR_DIR_FORWARD, MOTOR_LEFT | MOTOR_RIGHT);
    MOTOR_setSpeed(0.f, MOTOR_LEFT | MOTOR_RIGHT);

    printf("[Motor] Init done \n");
}

void MOTOR_setSpeed(int8_t dutyCycle, int motor) {
    if (dutyCycle < 0) dutyCycle = 0;
    else if (dutyCycle > 100) dutyCycle = 100;
    // Set the duty cycle in percentage
    if (motor & MOTOR_LEFT) {
        pwm_set_chan_level(pwm_slice, PWM_CHAN_A, dutyCycle / PWM_CYCLE);
        motorSpeedLeft = dutyCycle;
    }
    if (motor & MOTOR_RIGHT) {
        pwm_set_chan_level(pwm_slice, PWM_CHAN_B, dutyCycle / PWM_CYCLE);
        motorSpeedRight = dutyCycle;
    }
}

uint16_t MOTOR_getSpeed(int motor) {
    // Return the duty cycle in percentage
    if (motor & MOTOR_LEFT) {
        return motorSpeedLeft;
    } else if (motor & MOTOR_RIGHT) {
        return motorSpeedRight;
    } else {
        return -1;
    }
}

void MOTOR_setDirection(int dir, int motor) {
    // Motor 1: IN1 IN2
    // 10 = Forward, 01 = Reverse
    // Motor 2: IN3 IN4
    // 01 = Forward, 10 = Reverse (Flipped because motor is also physically flipped)
    if (dir == MOTOR_DIR_FORWARD) {
        if (motor & MOTOR_LEFT) {
            gpio_put(GPIO_PIN_MOTOR1_IN1, 1);
            gpio_put(GPIO_PIN_MOTOR1_IN2, 0);
            motorDirLeft = MOTOR_DIR_FORWARD;
        }
        if (motor & MOTOR_RIGHT) {
            gpio_put(GPIO_PIN_MOTOR2_IN3, 0);
            gpio_put(GPIO_PIN_MOTOR2_IN4, 1);
            motorDirRight = MOTOR_DIR_FORWARD;
        }
    } else if (dir == MOTOR_DIR_REVERSE) {
        if (motor & MOTOR_LEFT) {
            gpio_put(GPIO_PIN_MOTOR1_IN1, 0);
            gpio_put(GPIO_PIN_MOTOR1_IN2, 1);
            motorDirLeft = MOTOR_DIR_REVERSE;
        }
        if (motor & MOTOR_RIGHT) {
            gpio_put(GPIO_PIN_MOTOR2_IN3, 1);
            gpio_put(GPIO_PIN_MOTOR2_IN4, 0);
            motorDirRight = MOTOR_DIR_REVERSE;
        }
    }
}

int MOTOR_getDirection(int motor) {
    if (motor == MOTOR_LEFT) {
        return motorDirLeft;
    } else if (motor == MOTOR_RIGHT) {
        return motorDirRight;
    }
    return -1;
}

void MOTOR_turnLeft(void) {
    MOTOR_setDirection(MOTOR_DIR_REVERSE, MOTOR_LEFT);
    MOTOR_setDirection(MOTOR_DIR_FORWARD, MOTOR_RIGHT);
}

void MOTOR_turnRight(void) {
    MOTOR_setDirection(MOTOR_DIR_FORWARD, MOTOR_LEFT);
    MOTOR_setDirection(MOTOR_DIR_REVERSE, MOTOR_RIGHT);
}

// Function to convert from centimeters to steps
int CMtoSteps(float cm) {

  int result;  // Final calculation result
  float circumference = (wheelDiameter * 3.14) / 10; // Calculate wheel circumference in cm
  float cm_step = circumference / slotCount;  // CM per Step
  
  float f_result = cm / cm_step;  // Calculate result as a float
  result = (int) f_result; // Convert to an integer (note this is NOT rounded)
  
  return result;  // End and return result
}

void TurnRight() 
{
    int interrupts = 10;
    int mSpeed = 60;

   resetEncoderISRCount();

   // Set Left Motor Backward
   MOTOR_setDirection(MOTOR_DIR_REVERSE, MOTOR_LEFT);
   // Set Right Motor Foward
   MOTOR_setDirection(MOTOR_DIR_FORWARD, MOTOR_RIGHT);

  // Go forward until step value is reached
  while (interrupts > getLeftISRCount() && interrupts > getRightISRCount()) {
   
    if (interrupts > getLeftISRCount()) {
        MOTOR_setSpeed(mSpeed, MOTOR_LEFT);
    } 
	else {
    	MOTOR_setSpeed(0.f, MOTOR_LEFT);
    }
    if (interrupts >  getRightISRCount()) {
    	MOTOR_setSpeed(mSpeed, MOTOR_RIGHT);
    } 
	else {
   		MOTOR_setSpeed(0.f, MOTOR_RIGHT);
    }
   }
    
  MOTOR_setSpeed(0.f, MOTOR_LEFT);
  MOTOR_setSpeed(0.f, MOTOR_RIGHT);  // Stop when done
  resetEncoderISRCount();
 
}

void TurnLeft() 
{
    int interrupts = 10;
    int mSpeed = 60;

   resetEncoderISRCount();

   // Set Left Motor Foward
   MOTOR_setDirection(MOTOR_DIR_FORWARD, MOTOR_LEFT);
   // Set Right Motor Backward
   MOTOR_setDirection(MOTOR_DIR_REVERSE, MOTOR_RIGHT);

  // Go forward until step value is reached
  while (interrupts > getLeftISRCount() && interrupts > getRightISRCount()) {
   
    if (interrupts > getLeftISRCount()) {
        MOTOR_setSpeed(mSpeed, MOTOR_LEFT);
    } 
	else {
    	MOTOR_setSpeed(0.f, MOTOR_LEFT);
    }
    if (interrupts >  getRightISRCount()) {
    	MOTOR_setSpeed(mSpeed, MOTOR_RIGHT);
    } 
	else {
   		MOTOR_setSpeed(0.f, MOTOR_RIGHT);
    }
   }
    
  MOTOR_setSpeed(0.f, MOTOR_LEFT);
  MOTOR_setSpeed(0.f, MOTOR_RIGHT);  // Stop when done
  resetEncoderISRCount();
 
}

void MoveFoward(int interrupts) 
{
 
    int mSpeed = 80;

   resetEncoderISRCount();

   // Set Left Motor Foward
   MOTOR_setDirection(MOTOR_DIR_FORWARD, MOTOR_LEFT);
   // Set Right Motor Foward
   MOTOR_setDirection(MOTOR_DIR_FORWARD, MOTOR_RIGHT);

  // Go forward until step value is reached
  while (interrupts > getLeftISRCount() && interrupts > getRightISRCount()) {
   
    if (interrupts > getLeftISRCount()) {
        MOTOR_setSpeed(mSpeed, MOTOR_LEFT);
    } 
	else {
    	MOTOR_setSpeed(0.f, MOTOR_LEFT);
    }
    if (interrupts >  getRightISRCount()) {
    	MOTOR_setSpeed(mSpeed, MOTOR_RIGHT);
    } 
	else {
   		MOTOR_setSpeed(0.f, MOTOR_RIGHT);
    }
   }
    
  MOTOR_setSpeed(0.f, MOTOR_LEFT);
  MOTOR_setSpeed(0.f, MOTOR_RIGHT);  // Stop when done
  resetEncoderISRCount();
 
}

void TurnAround() 
{
    int interrupts = 20;
    int mSpeed = 60;

   resetEncoderISRCount();

   // Set Left Motor Foward
   MOTOR_setDirection(MOTOR_DIR_FORWARD, MOTOR_LEFT);
   // Set Right Motor Backward
   MOTOR_setDirection(MOTOR_DIR_REVERSE, MOTOR_RIGHT);

  // Go forward until step value is reached
  while (interrupts > getLeftISRCount() && interrupts > getRightISRCount()) {
   
    if (interrupts > getLeftISRCount()) {
        MOTOR_setSpeed(mSpeed, MOTOR_LEFT);
    } 
	else {
    	MOTOR_setSpeed(0.f, MOTOR_LEFT);
    }
    if (interrupts >  getRightISRCount()) {
    	MOTOR_setSpeed(mSpeed, MOTOR_RIGHT);
    } 
	else {
   		MOTOR_setSpeed(0.f, MOTOR_RIGHT);
    }
   }
    
  MOTOR_setSpeed(0.f, MOTOR_LEFT);
  MOTOR_setSpeed(0.f, MOTOR_RIGHT);  // Stop when done
  resetEncoderISRCount();
 
}