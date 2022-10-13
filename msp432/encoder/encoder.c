#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdint.h>

#include "encoder.h"
#include "serial/serial.h"

volatile int encIndex = 0;
int leftRotation[4];
int rightRotation[4];

const Timer_A_UpModeConfig upConfig = {
        TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock Source = 3Mhz
        TIMER_A_CLOCKSOURCE_DIVIDER_16,         // TACLK = 3MHz / 16 = 187500
        46875,                                  // 46875 ticks (CCR0)
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,    // Enable CCR0 interrupt
        TIMER_A_DO_CLEAR                        // Clear value
};

void ENCODER_init(void)
{
    // Left Encoder P2.7, Right Encoder P2.3
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P2, GPIO_PIN7 | GPIO_PIN3);
    GPIO_interruptEdgeSelect(GPIO_PORT_P2, GPIO_PIN7 | GPIO_PIN3, GPIO_HIGH_TO_LOW_TRANSITION); // Trigger when high to low

    GPIO_clearInterruptFlag(GPIO_PORT_P2, GPIO_PIN7 | GPIO_PIN3);
    GPIO_enableInterrupt(GPIO_PORT_P2, GPIO_PIN7 | GPIO_PIN3);

    // Setup timer (0.25 second)
    Timer_A_configureUpMode(TIMER_A1_BASE, &upConfig);
    Timer_A_enableInterrupt(TIMER_A1_BASE);

    // Enable interrupt
    Interrupt_enableInterrupt(INT_PORT2);
    Interrupt_enableInterrupt(INT_TA1_0);

    Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);

    SERIAL_printf("Encoder init\r\n");
}

float ENCODER_getLeftWheelSpeed(void)
{
    int total = 0;
    int i;
    for (i = 0; i < 4; i++) {
        total += leftRotation[i];
    }
    return total / 4;
}

float ENCODER_getRightWheelSpeed(void)
{
    int total = 0;
    int i;
    for (i = 0; i < 4; i++) {
        total += rightRotation[i];
    }
    return total / 4;
}

void PORT2_IRQHandler(void)
{
    uint32_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P2);
    GPIO_clearInterruptFlag(GPIO_PORT_P2, status);

    // Left Encoder
    if (status & GPIO_PIN7) {
        leftRotation[encIndex]++;
        //SERIAL_printf("L encoder interrupt\r\n");
    }

    // Right Encoder
    if (status & GPIO_PIN3) {
        rightRotation[encIndex]++;
        //SERIAL_printf("R encoder interrupt\r\n");
    }
}

void TA1_0_IRQHandler(void)
{
    Timer_A_clearCaptureCompareInterrupt(TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
    leftRotation[(encIndex + 1) % 4] = 0;
    rightRotation[(encIndex + 1) % 4] = 0;
    encIndex++;
    if (encIndex == 4) encIndex = 0;
    //SERIAL_printf("1/4 second timer up\r\n");
}
