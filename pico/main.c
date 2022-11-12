#include <stdio.h>

#include "pico/stdlib.h"
#include "motor/motor.h"
#include "motor/pid.h"
#include "encoder/encoder.h"

#include "hardware/clocks.h"

#define DEBUG

void show_clock_freqs(void)
{
    uint f_pll_sys = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_PLL_SYS_CLKSRC_PRIMARY);
    uint f_pll_usb = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_PLL_USB_CLKSRC_PRIMARY);
    uint f_rosc = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_ROSC_CLKSRC);
    uint f_clk_sys = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_SYS);
    uint f_clk_peri = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_PERI);
    uint f_clk_usb = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_USB);
    uint f_clk_adc = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_ADC);

    printf("pll_sys  = %dkHz\n"
           "pll_usb  = %dkHz\n"
           "rosc     = %dkHz\n"
           "clk_sys  = %dkHz\n"
           "clk_peri = %dkHz\n"
           "clk_usb  = %dkHz\n"
           "clk_adc  = %dkHz\n",
           f_pll_sys, f_pll_usb, f_rosc, f_clk_sys, f_clk_peri, f_clk_usb, f_clk_adc);
}

void togglePid(uint8_t* flag) {
    *flag ^= 1;
    printf("> [PID] Enabled: %i \n", *flag);
}

int main()
{
    stdio_init_all();
    while (!stdio_usb_connected())
        sleep_ms(100); // Wait for USB to initialize

    printf("===== Starting pico ===== \n");

    // Print clocks sources and their frequencies
    show_clock_freqs();

    // Initialize modules
    MOTOR_init();
    ENCODER_init();

    // Create PID contoller for motors
    PID *leftMotorPID = PID_create(5.0f, 5.f, 0.5f, 0, 0, 100);
    PID *rightMotorPID = PID_create(5.0f, 5.f, 0.5f, 0, 0, 100);

    float leftWheelSpeed, rightWheelSpeed;
    int leftMotorDutyCycle, rightMotorDutyCycle;

    uint8_t usePid = 1;

    while (1)
    {
        // Read stdin input (via serial terminal over usb)
        uint8_t c = getchar_timeout_us(0);
        switch (c)
        {
        case 'z': // Toggle PID controller
            togglePid(&usePid);
            break;
        case 'q': // Slow down (through PID)
            leftMotorPID->setPoint -= 2;
            if (leftMotorPID->setPoint < 0.f)
                leftMotorPID->setPoint = 0.f;
            rightMotorPID->setPoint -= 2;
            if (rightMotorPID->setPoint < 0.f)
                rightMotorPID->setPoint = 0.f;
            printf("> [Motor] PID Target Speed %f \n", leftMotorPID->setPoint * 10);
            break;
        case 'e': // Speed up (through PID)
            leftMotorPID->setPoint += 2;
            if (leftMotorPID->setPoint > 10.f)
                leftMotorPID->setPoint = 10.f;
            rightMotorPID->setPoint += 2;
            if (rightMotorPID->setPoint > 10.f)
                rightMotorPID->setPoint = 10.f;
            printf("> [Motor] PID Target Speed %.0f \n", leftMotorPID->setPoint * 10);
            break;
        case 'Q': // Slow down (direct motor control, need to disable PID!)
            if (usePid) togglePid(&usePid);
            MOTOR_setSpeed(MOTOR_getSpeed(MOTOR_LEFT) - 20, MOTOR_LEFT | MOTOR_RIGHT);
            printf("> [Motor] Set Speed %u \n", MOTOR_getSpeed(MOTOR_LEFT));
            break;
        case 'E': // Speed up (direct motor control, need to disable PID!)
            if (usePid) togglePid(&usePid);
            MOTOR_setSpeed(MOTOR_getSpeed(MOTOR_LEFT) + 20, MOTOR_LEFT | MOTOR_RIGHT);
            printf("> [Motor] Set Speed %u \n", MOTOR_getSpeed(MOTOR_LEFT));
            break;
        case 'w': // Forward
            MOTOR_setDirection(MOTOR_DIR_FORWARD, MOTOR_LEFT | MOTOR_RIGHT);
            printf("> [Motor] Forward \n");
            break;
        case 's': // Reverse
            MOTOR_setDirection(MOTOR_DIR_REVERSE, MOTOR_LEFT | MOTOR_RIGHT);
            printf("> [Motor] Reverse \n");
            break;
        case 'a': // Left turn (in place)
            MOTOR_spotTurn(MOTOR_TURN_ANTICLOCKWISE, 90);
            printf("> [Motor] Left Turn \n");
            break;
        case 'd': // Right turn (in place)
            MOTOR_spotTurn(MOTOR_TURN_CLOCKWISE, 90);
            printf("> [Motor] Right Turn \n");
            break;
        case 'x': // Move Foward (10 cm)
            MOTOR_moveFoward(10);
            printf("> [Motor] Move Foward \n");
            break;
        case 't': // Turn Around
            MOTOR_spotTurn(MOTOR_TURN_CLOCKWISE, 180);
            printf("> [Motor] Move Foward \n");
            break;
        case 'c':
            show_clock_freqs();
            break;
        }

        if (usePid)
        {
            leftWheelSpeed = ENCODER_getLeftWheelSpeed();
            rightWheelSpeed = ENCODER_getRightWheelSpeed();

            leftMotorDutyCycle = PID_run(leftMotorPID, leftWheelSpeed);
            rightMotorDutyCycle = PID_run(rightMotorPID, rightWheelSpeed);

            MOTOR_setSpeed(leftMotorDutyCycle, MOTOR_LEFT);
            MOTOR_setSpeed(rightMotorDutyCycle, MOTOR_RIGHT);

#ifdef DEBUG
            printf("SPD L: %.2f PID L: %i | [P]%.2f [I]%.2f [D]%.2f \r\n", leftWheelSpeed, leftMotorDutyCycle, leftMotorPID->p, leftMotorPID->i, leftMotorPID->d);
            printf("SPD R: %.2f PID R: %i | [P]%.2f [I]%.2f [D]%.2f \r\n", rightWheelSpeed, rightMotorDutyCycle, rightMotorPID->p, rightMotorPID->i, rightMotorPID->d);
#endif
        }

        sleep_ms(500);
    }
}