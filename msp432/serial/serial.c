#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ctype.h>

// http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
const eUSCI_UART_ConfigV1 uartConfig =
{
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,                 // SMCLK Clock Source
        1,                                              // BRDIV
        10,                                             // UCxBRF
        0,                                              // UCxBRS
        EUSCI_A_UART_ODD_PARITY,                        // ODD Parity
        EUSCI_A_UART_LSB_FIRST,                         // LSB First
        EUSCI_A_UART_ONE_STOP_BIT,                      // One stop bit
        EUSCI_A_UART_MODE,                              // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,  // Oversampling
};

void SERIAL_printf(unsigned char * TxArray)
{
    unsigned short i = 0;
    while(*(TxArray+i))
    {
        UART_transmitData(EUSCI_A0_BASE, *(TxArray+i));  // Write the character at the location specified by pointer
        i++;                                             // Increment pointer to point to the next character
    }
}

void SERIAL_init(void)
{
    /* Selecting P1.2 and P1.3 in UART mode */
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configuring UART Module */
    UART_initModule(EUSCI_A0_BASE, &uartConfig);

    /* Enable UART module */
    UART_enableModule(EUSCI_A0_BASE);

    /* Enabling interrupts (Rx) */
    UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    Interrupt_enableInterrupt(INT_EUSCIA0);

    SERIAL_printf("UART init\r\n");
}

/* EUSCI A0 UART ISR */
void EUSCIA0_IRQHandler(void)
{
    uint32_t status = UART_getEnabledInterruptStatus(EUSCI_A0_BASE);
    UART_clearInterruptFlag(EUSCI_A0_BASE, status);

    if (status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG) {
        // Not implmented. Echo back whatever received
        uint8_t rec = UART_receiveData(EUSCI_A0_BASE);
        UART_transmitData(EUSCI_A0_BASE, rec);
    }
}
