#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/util/datetime.h"
#include "motor/motor.h"
#include "motor/pid.h"
#include "encoder/encoder.h"
#include "navigation/movement.h"
#include "navigation/mappinggod.h"
#include "hardware/clocks.h"
#include "ultrasonic/ultrasonic.h"

#define DEBUG
// #define FUNCTIONS
#define NAVIG

#define PID_Kp 2.f
#define PID_Ki 2.f
#define PID_Kd 0.f

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

void togglePid(uint8_t *flag)
{
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
    ULTRASONIC_init();

    // Create PID contoller for motors
    PID *leftMotorPID = PID_create(PID_Kp, PID_Ki, PID_Kd, 0, 0, 100);
    PID *rightMotorPID = PID_create(PID_Kp, PID_Ki, PID_Kd, 0, 0, 100);

    absolute_time_t currentTime, previousTime;

    float leftWheelSpeed, rightWheelSpeed;
    int leftWheelInterruptSpeed, rightWheelInterruptSpeed;
    uint leftMotorDutyCycle, rightMotorDutyCycle;

    float deltaTime; // Time since last loop in seconds

    uint8_t usePid = 1;

#ifdef FUNCTIONS
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
            PID_setTargetSpeed(leftMotorPID, SPEED_MEDIUM);
            PID_setTargetSpeed(rightMotorPID, SPEED_MEDIUM);
            printf("> [Motor] PID Target Setpoint %.0f \n", leftMotorPID->setPoint);
            break;
        case 'e': // Speed up (through PID)
            PID_setTargetSpeed(leftMotorPID, SPEED_HIGH);
            PID_setTargetSpeed(rightMotorPID, SPEED_HIGH);
            printf("> [Motor] PID Target Setpoint %.0f \n", leftMotorPID->setPoint);
            break;
        case '`': // Stop
            if (usePid)
            {
                PID_setTargetSpeed(leftMotorPID, SPEED_NONE);
                PID_setTargetSpeed(rightMotorPID, SPEED_NONE);
                printf("> [Motor] PID Target Setpoint %.0f \n", leftMotorPID->setPoint);
            }
            else
            {
                MOTOR_stop(MOTOR_LEFT | MOTOR_RIGHT);
            }
            break;
        case '1': // 60% Duty cycle
            if (usePid)
                togglePid(&usePid);
            MOTOR_setSpeed(60, MOTOR_LEFT | MOTOR_RIGHT);
            break;
        case '2': // 70% Duty cycle
            if (usePid)
                togglePid(&usePid);
            MOTOR_setSpeed(70, MOTOR_LEFT | MOTOR_RIGHT);
            break;
        case '3': // 80% Duty cycle
            if (usePid)
                togglePid(&usePid);
            MOTOR_setSpeed(80, MOTOR_LEFT | MOTOR_RIGHT);
            break;
        case '4': // 90% Duty cycle
            if (usePid)
                togglePid(&usePid);
            MOTOR_setSpeed(90, MOTOR_LEFT | MOTOR_RIGHT);
            break;
        case '5': // 100% Duty cycle
            if (usePid)
                togglePid(&usePid);
            MOTOR_setSpeed(100, MOTOR_LEFT | MOTOR_RIGHT);
            break;
        case 'Q': // Slow down (direct motor control)
            if (usePid)
                togglePid(&usePid);
            uint8_t prevSpeed = MOTOR_getSpeed(MOTOR_LEFT) - 10;
            MOTOR_setSpeed(prevSpeed, MOTOR_LEFT | MOTOR_RIGHT);
            break;
        case 'E': // Speed up (direct motor control)
            if (usePid)
                togglePid(&usePid);
            uint8_t nextSpeed = MOTOR_getSpeed(MOTOR_LEFT) + 10;
            MOTOR_setSpeed(nextSpeed, MOTOR_LEFT | MOTOR_RIGHT);
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
        case 'A': // Turn 180 (in place)
            MOTOR_spotTurn(MOTOR_TURN_ANTICLOCKWISE, 180);
            printf("> [Motor] Turn 180 ANTI-CLK \n");
            break;
        case 'D': // Turn 180 (in place)
            MOTOR_spotTurn(MOTOR_TURN_CLOCKWISE, 180);
            printf("> [Motor] Turn 180 CLK \n");
            break;
        case 'x': // Move Foward (10 cm)
            MOTOR_moveFoward(10);
            printf("> [Motor] Move Foward \n");
            break;
        case 'c':
            show_clock_freqs();
            break;
        case 'i': // Front US
            printf("> [US] FRONT: %.2f \n", ULTRASONIC_getCM(ULTRASONIC_FRONT));
            break;
        case 'k': // Rear US
            printf("> [US] REAR: %.2f \n", ULTRASONIC_getCM(ULTRASONIC_REAR));
            break;
        case 'j': // Left US
            printf("> [US] LEFT: %.2f \n", ULTRASONIC_getCM(ULTRASONIC_LEFT));
            break;
        case 'l': // Right US
            printf("> [US] RIGHT: %.2f \n", ULTRASONIC_getCM(ULTRASONIC_RIGHT));
            break;
        }

        currentTime = get_absolute_time();
        deltaTime = absolute_time_diff_us(previousTime, currentTime) / 1000000.f;
        previousTime = currentTime;

        leftWheelSpeed = ENCODER_getWheelSpeed(ENCODER_LEFT);
        rightWheelSpeed = ENCODER_getWheelSpeed(ENCODER_RIGHT);

        if (usePid)
        {
            // leftWheelInterruptSpeed = ENCODER_getWheelInterruptSpeed(ENCODER_LEFT);
            // rightWheelInterruptSpeed = ENCODER_getWheelInterruptSpeed(ENCODER_RIGHT);

            leftMotorDutyCycle = PID_run(leftMotorPID, leftWheelSpeed, deltaTime);
            rightMotorDutyCycle = PID_run(rightMotorPID, rightWheelSpeed, deltaTime);

            if (leftMotorDutyCycle != MOTOR_getSpeed(MOTOR_LEFT))
            {
                MOTOR_setSpeed(leftMotorDutyCycle, MOTOR_LEFT);
            }
            if (rightMotorDutyCycle != MOTOR_getSpeed(MOTOR_RIGHT))
            {
                MOTOR_setSpeed(rightMotorDutyCycle, MOTOR_RIGHT);
            }

#ifdef DEBUG
            // printf("PID Delta Time: %.6f (s) \n", deltaTime);
            printf("SPD L: %.2f | DUTY L: %i | [P]%.2f [I]%.2f [D]%.2f (Err: %.2f) \n", leftWheelSpeed, leftMotorDutyCycle, leftMotorPID->p, leftMotorPID->i, leftMotorPID->d, leftMotorPID->lastError);
            printf("SPD R: %.2f | DUTY R: %i | [P]%.2f [I]%.2f [D]%.2f (Err: %.2f) \n", rightWheelSpeed, rightMotorDutyCycle, rightMotorPID->p, rightMotorPID->i, rightMotorPID->d, rightMotorPID->lastError);
#endif
        }
        else
        {
#ifdef DEBUG
            printf("SPD L: %.2f | DUTY L: %i \n", leftWheelSpeed, MOTOR_getSpeed(MOTOR_LEFT));
            printf("SPD R: %.2f | DUTY R: %i \n", rightWheelSpeed, MOTOR_getSpeed(MOTOR_RIGHT));
#endif
        }

        sleep_ms(500);
    }
#endif

#ifdef NAVIG
    // Initialise map struct
    mapping_struct map;
    map.mapped_count = 0;
    map.backstepped = 0;
    map.visited_coords = (int **)malloc(MAZE_SIZE * sizeof(int *));
    mallocChecker(map.visited_coords);
    for (int i = 0; i < MAZE_SIZE; i++)
    {
        map.visited_coords[i] = (int *)malloc(2 * sizeof(int));
        mallocChecker(map.visited_coords[i]);
    }
    map.visited_coords_size = 0;
    map.bot_path = (int **)malloc(MAZE_SIZE * sizeof(int *));
    mallocChecker(map.bot_path);
    for (int i = 0; i < MAZE_SIZE; i++)
    {
        map.bot_path[i] = (int *)malloc(2 * sizeof(int));
        mallocChecker(map.bot_path[i]);
    }
    map.bot_path_size = 0;
    map.start_position[0] = 0;
    map.start_position[1] = 0;
    map.current_position[0] = 0;
    map.current_position[1] = 0;

    // Initialise compute_path for navigation
    compute_path path;
    path.current_grid[0] = 0;
    path.current_grid[1] = 0;
    path.ending_grid[0] = 3;
    path.ending_grid[1] = 1;

    // Initialise bot direction, which ever direction it started in is its "north"
    int direction = 0;
    // Start in state 1, scan surroundings
    int state = STATE_SRNDIND;
    // Buffer will be used to share info throughout mapping
    int shared_buffer = 0;

    // int state = STATE_NORMALISING;
    // grid_stats mazeSetup[MAZE_SIZE] = {
    //     {{0, 0}, {0, 1, 1, 0}},
    //     {{1, 0}, {0, 1, 1, 1}},
    //     {{2, 0}, {0, 1, 1, 1}},
    //     {{3, 0}, {0, 1, 0, 1}},
    //     {{4, 0}, {0, 0, 1, 1}},
    //     {{0, 1}, {1, 0, 1, 0}},
    //     {{1, 1}, {1, 0, 1, 0}},
    //     {{2, 1}, {1, 1, 0, 0}},
    //     {{3, 1}, {0, 0, 0, 1}},
    //     {{4, 1}, {1, 0, 1, 0}},
    //     {{0, 2}, {1, 0, 1, 0}},
    //     {{1, 2}, {1, 1, 0, 0}},
    //     {{2, 2}, {0, 1, 0, 1}},
    //     {{3, 2}, {0, 1, 0, 1}},
    //     {{4, 2}, {1, 0, 1, 1}},
    //     {{0, 3}, {1, 1, 0, 0}},
    //     {{1, 3}, {0, 1, 0, 1}},
    //     {{2, 3}, {0, 1, 0, 1}},
    //     {{3, 3}, {0, 1, 0, 1}},
    //     {{4, 3}, {1, 0, 0, 1}}};

    // int counter = 0;
    // map.start_position[0] = 0;
    // map.start_position[1] = 0;
    // for (int y = 0; y < 4; y++)
    // {
    //     for (int x = 0; x < 5; x++)
    //     {
    //         map.maze_info[counter].coordinates[0] = mazeSetup[counter].coordinates[0];
    //         map.maze_info[counter].coordinates[1] = mazeSetup[counter].coordinates[1];
    //         map.maze_info[counter].connections[0] = mazeSetup[counter].connections[0];
    //         map.maze_info[counter].connections[1] = mazeSetup[counter].connections[1];
    //         map.maze_info[counter].connections[2] = mazeSetup[counter].connections[2];
    //         map.maze_info[counter].connections[3] = mazeSetup[counter].connections[3];
    //         counter++;
    //     }
    // }

    while (1)
    {
        sleep_ms(2000);
        switch (state)
        {
        case STATE_IDLE:
            printf("Sleeping for 10s, prepare to navigate\n");
            sleep_ms(10000);
            state = STATE_COMPUTING_PATH;
            break;
        case STATE_SRNDIND:
            getTileInfo(&map, &direction, &state);
            break;
        case STATE_MAPPING:
            chooseMovement(&map, &state, &shared_buffer);
            break;
        case STATE_MOVING:
            moveDirection(&map, &direction, &state, &shared_buffer);
            break;
        case STATE_NORMALISING:
            sortMappedMaze(&map, &state);
            printf("final position: %d, %d\n", map.current_position[0], map.current_position[1]);
            for (int i = 0; i < MAZE_SIZE; i++)
            {
                printf("(%d,%d) %d-%d-%d-%d\n", map.maze_info[i].coordinates[0], map.maze_info[i].coordinates[1],
                       map.maze_info[i].connections[0], map.maze_info[i].connections[1], map.maze_info[i].connections[2], map.maze_info[i].connections[3]);
            }
            break;
        case STATE_COMPUTING_PATH:
            computeShortestPath(map, &path, &state);
            break;
        case STATE_EXECUTING_PATH:
            printf("computed path: \n");
            for (int i = 0; i < path.move_count; i++)
            {
                printf("(%d,%d)\n", path.actual_path[i][0], path.actual_path[i][1]);
            }
            moveLong(path, &direction, &state);
            break;
        }
    }
#endif
}