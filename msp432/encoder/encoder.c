#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdint.h>

#include "encoder.h"
#include "serial/serial.h"

int rightRotation = 0;
int leftRotation = 0;

const Timer_A_UpModeConfig upConfig = {
        TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock Source = 3Mhz
        TIMER_A_CLOCKSOURCE_DIVIDER_64,         // TACLK = 3MHz / 64
        46875,                                  // 46875 ticks (CCR0)
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE ,    // Enable CCR0 interrupt
        TIMER_A_DO_CLEAR                        // Clear value
};

void ENCODER_init(void)
{
    // Left Encoder P2.7
    // Right Encoder P2.6
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P2, GPIO_PIN7 | GPIO_PIN6);
    GPIO_interruptEdgeSelect(GPIO_PORT_P2, GPIO_PIN7 | GPIO_PIN6, GPIO_HIGH_TO_LOW_TRANSITION); // Trigger when high to low

    GPIO_clearInterruptFlag(GPIO_PORT_P2, GPIO_PIN7 | GPIO_PIN6);
    GPIO_enableInterrupt(GPIO_PORT_P2, GPIO_PIN7 | GPIO_PIN6);

    // Setup timer (1 second)
    Timer_A_configureUpMode(TIMER_A0_BASE, &upConfig);
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);
    Timer_A_enableInterrupt(TIMER_A0_BASE);

    // Enable interrupt
    Interrupt_enableInterrupt(INT_PORT2);
    Interrupt_enableInterrupt(INT_TA0_0);

    SERIAL_printf("Encoder init\r\n");
}

int ENCODER_getLeftRotation(void)
{
    return leftRotation;
}

int ENCODER_getRightRotation(void)
{
    return rightRotation;
}

void ENCODER_resetLeft(void) {
    leftRotation = 0;
}

void ENCODER_resetRight(void) {
    rightRotation = 0;
}

void PORT2_IRQHandler(void)
{
    uint32_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P2);
    GPIO_clearInterruptFlag(GPIO_PORT_P2, status);

    // Left Encoder
    if (status & GPIO_PIN7) {
        leftRotation++;
        SERIAL_printf("L encoder interrupt\r\n");
    }

    // Right Encoder
    if (status & GPIO_PIN6) {
        rightRotation++;
        SERIAL_printf("R encoder interrupt\r\n");
    }
}

void TA0_0_IRQHandler(void)
{
    Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
    SERIAL_printf("1 second up\r\n");
}
