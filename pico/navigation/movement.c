#include "movement.h"

void moveDirection(mapping_struct *map, int *direction, int *state, int *shared_buffer)
{
    int turn_operation = *direction - *shared_buffer;
    if (turn_operation < 0)
        turn_operation += 4;

    printf("move direction operation: %d\n", turn_operation);

    switch (turn_operation)
    {
    case 0:
        MOTOR_moveFoward(ONEUNIT);
        printf("forward!\n");
        break;
    case 1:
        MOTOR_spotTurn(MOTOR_TURN_ANTICLOCKWISE, 90);
        printf("left face!\n");
        (*direction)--;
        if (*direction < 0)
            (*direction) += 4;
        sleep_ms(500);
        MOTOR_moveFoward(ONEUNIT);
        printf("forward!\n");
        break;
    case 2:
        MOTOR_moveBackward(ONEUNIT);
        printf("backward!\n");
        break;
    case 3:
        MOTOR_spotTurn(MOTOR_TURN_CLOCKWISE, 90);
        printf("right face!\n");
        (*direction)++;
        if (*direction > 3)
            (*direction) -= 4;
        sleep_ms(500);
        MOTOR_moveFoward(ONEUNIT);
        printf("forward!\n");
        break;
    }

    // Go to scan surroundings
    // If there was a backstep, no need to scan surroundings, just go to mapping
    if (!map->backstepped)
    {
        *state = STATE_SRNDIND;
    }
    else
    {
        *state = STATE_MAPPING;
    }
}

void moveLong(compute_path path, int *direction, int *state)
{
    int chosen_direction, x_diff, y_diff;

    for (int i = 0; i < path.move_count - 1; i++)
    {
        int steps = 0;
        x_diff = 0;
        y_diff = 0;

        x_diff = path.actual_path[i][0] - path.actual_path[i + 1][0];
        if (x_diff != 0)
        {
            chosen_direction = x_diff > 0 ? 3 : 1;
        }

        y_diff = path.actual_path[i][1] - path.actual_path[i + 1][1];
        if (y_diff != 0)
        {
            chosen_direction = y_diff > 0 ? 0 : 2;
        }

        steps++;

        while (1)
        {
            if (i + 2 == path.move_count - 1)
            {
                break;
            }

            if ((x_diff != 0 && x_diff == path.actual_path[i + 1][0] - path.actual_path[i + 2][0]) ||
                (y_diff != 0 && y_diff == path.actual_path[i + 1][1] - path.actual_path[i + 2][1]))
            {
                steps++;
                i++;
            }
            else
            {
                break;
            }
        }

        printf("dir: %d dist: %d\n", chosen_direction, steps);

        int turn_operation = *direction - chosen_direction;
        if (turn_operation < 0)
            turn_operation += 4;

        switch (turn_operation)
        {
        case 0:
            MOTOR_moveFoward(ONEUNIT * steps);
            printf("forward!\n");
            break;
        case 1:
            MOTOR_spotTurn(MOTOR_TURN_ANTICLOCKWISE, 90);
            printf("left face!\n");
            (*direction)--;
            if (*direction < 0)
                (*direction) += 4;
            sleep_ms(500);
            MOTOR_moveFoward(ONEUNIT * steps);
            printf("forward!\n");
            break;
        case 2:
            MOTOR_moveBackward(ONEUNIT * steps);
            printf("backward!\n");
            break;
        case 3:
            MOTOR_spotTurn(MOTOR_TURN_CLOCKWISE, 90);
            printf("right face!\n");
            (*direction)++;
            if (*direction > 3)
                (*direction) -= 4;
            sleep_ms(500);
            MOTOR_moveFoward(ONEUNIT * steps);
            printf("forward!\n");
            break;
        }
    }

    printf("navig done!\n");
    *state = STATE_IDLE;
}