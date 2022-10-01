#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

#include "motor/motor.h"

int main(void) {
	WDT_A_holdTimer(); // Stop watchdog timer

	// Initialize modules
    MOTOR_init();

	// For testing: Motor control using buttons
	GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN4);
	GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0); // LED 1 indicate motor direction
	GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN4);
	GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN4);

	/* Enabling interrupts and starting the watchdog timer */
	Interrupt_enableInterrupt(INT_PORT1);
	Interrupt_enableMaster();

	while (1) {
	    PCM_gotoLPM0();
	}
}

void PORT1_IRQHandler(void) {
    uint32_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);

    int motorOn = MOTOR_getSpeed() > 0.f;

    if (status & GPIO_PIN1) {
        // Button 1: Toggle motor
        if (motorOn) {
            MOTOR_setSpeed(0.f);
        } else {
            MOTOR_setSpeed(100.f);
        }
    } else if (status & GPIO_PIN4) {
        // Button 2: Toggle direction (forward/reverse)
        if (MOTOR_getDirection()) {
            MOTOR_reverse();
            GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
        } else {
            MOTOR_forward();
            GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
        }
    }

    GPIO_clearInterruptFlag(GPIO_PORT_P1, status);
}
