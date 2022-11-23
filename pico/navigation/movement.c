#include "movement.h"

void moveTo(int *current_x, int *current_y, int *direction, int to_x, int to_y)
{

    int intended_dir, turn_operation;

    // Move south
    if (to_y - *current_y == 1)
        intended_dir = 2;
    // Move north
    else if (to_y - *current_y == -1)
        intended_dir = 0;
    // Move east
    else if (to_x - *current_x == 1)
        intended_dir = 1;
    // Move west
    else if (to_x - *current_x == -1)
        intended_dir = 3;

    turn_operation = *direction - intended_dir;
    if (turn_operation < 0)
        turn_operation += 4;

    switch (turn_operation)
    {
    case 0:
        MOTOR_moveFoward(ONEUNIT);
        (*current_y)--;
        break;
    case 1:
        MOTOR_spotTurn(MOTOR_TURN_CLOCKWISE, 90);
        (*direction)++;
        MOTOR_moveFoward(ONEUNIT);
        (current_x)++;
        break;
    case 2:
        MOTOR_moveBackward(ONEUNIT);
        (*current_y)++;
        break;
    case 3:
        MOTOR_spotTurn(MOTOR_TURN_ANTICLOCKWISE, 90);
        (*direction)--;
        MOTOR_moveFoward(ONEUNIT);
        (current_x)--;
        break;
    }
}

void moveDirection(mapping_struct *map, int *direction, int move_direction)
{
    int turn_operation = *direction - move_direction;
    if (turn_operation < 0)
        turn_operation += 4;

    printf("move direction operation: %d\n", turn_operation);

    switch (turn_operation)
    {
    case 0:
        MOTOR_moveFoward(ONEUNIT-8);
        // (map->current_position[1])--;
        printf("forward!\n");
        break;
    case 1:
        MOTOR_spotTurn(MOTOR_TURN_ANTICLOCKWISE, 90);
        printf("left face!\n");
        (*direction)--;
        if (*direction < 0)
            (*direction) += 4;
        sleep_ms(500);
        MOTOR_moveFoward(ONEUNIT-8);
        // (map->current_position[0])++;
        printf("forward!\n");
        break;
    case 2:
        MOTOR_moveBackward(ONEUNIT-8);
        // (map->current_position[1])++;
        printf("backward!\n");
        break;
    case 3:
        MOTOR_spotTurn(MOTOR_TURN_CLOCKWISE, 90);
        printf("right face!\n");
        (*direction)++;
        if (*direction > 3)
            (*direction) -= 4;
        sleep_ms(500);
        MOTOR_moveFoward(ONEUNIT-8);
        // (map->current_position[0])--;
        printf("forward!\n");
        break;
    }
}