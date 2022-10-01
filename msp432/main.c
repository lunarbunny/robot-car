#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

#include "motor/motor.h"

int main(void) {
	WDT_A_holdTimer(); // Stop watchdog timer

	// Initialize modules
    MOTOR_init();

	// For testing: Motor control using buttons and LED indicators
	GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0); // LED 1 indicate motor direction, ON = Forward ; OFF = Reverse
	GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0); // Default forward, so enable LED 1
	GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2); // LED 2 indicate button mode, R = Motor On/OFF ; G = Motor Direction
	GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0); // Default is On/Off mode
	GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1 | GPIO_PIN2); // Disable other 2 LED

    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN4); // Button 1 toggle mode, Button 2 configure parameters for current mode
	GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN4);
	GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN4);

	/* Enabling interrupts and starting the watchdog timer */
	Interrupt_enableInterrupt(INT_PORT1);
	Interrupt_enableMaster();

	while (1) {
	    PCM_gotoLPM0();
	}
}

int mode = 0;

void PORT1_IRQHandler(void) {
    uint32_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);

    int motorOn = MOTOR_getSpeed() > 0.f;

    if (status & GPIO_PIN1) {
        mode = ++mode % 2;
        switch(mode) {
            case 0:
                GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0); // Default is On/Off mode
                GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1 | GPIO_PIN2); // Disable other 2 LED
                break;
            case 1:
                GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1); // Default is On/Off mode
                GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN2); // Disable other 2 LED
                break;
        }
    } else if (status & GPIO_PIN4) {
        switch (mode) {
            case 0:
                // Button 1: Toggle motor
                if (motorOn) {
                    MOTOR_setSpeed(0.f);
                } else {
                    MOTOR_setSpeed(100.f);
                }
                break;
            case 1:
                // Button 2: Toggle direction (forward/reverse)
                if (MOTOR_getDirection()) {
                    MOTOR_reverse();
                    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
                } else {
                    MOTOR_forward();
                    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
                }
                break;
        }
    }

    GPIO_clearInterruptFlag(GPIO_PORT_P1, status);
}
