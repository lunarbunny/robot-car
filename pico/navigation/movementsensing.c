#include <stdio.h>
#include "mappinggod.h"
#include "pico/stdlib.h"

int moveTo(int *current_x, int *current_y, int *direction, int coords[2])
{

    int intended_dir, turn_operation;

    // Move south
    if (coords[1] - *current_y == 1)
    {
        intended_dir = 2;
        (*current_y)++;
    }
    // Move north
    else if (coords[1] - *current_y == -1)
    {
        intended_dir = 0;
        (*current_y)--;
    }
    // Move east
    else if (coords[0] - *current_x == 1)
    {
        intended_dir = 1;
        (*current_x)++;
    }
    // Move west
    else if (coords[0] - *current_x == -1)
    {
        intended_dir = 3;
        (*current_x)--;
    }

    turn_operation = *direction - intended_dir;
    if (turn_operation < 0)
        turn_operation += 4;

    switch (turn_operation)
    {
    case 0:
        MOTOR_moveFoward(ONEUNIT);
        break;
    case 1:
        MOTOR_spotTurn(MOTOR_TURN_ANTICLOCKWISE, 90);
        (*direction)--;
        MOTOR_moveFoward(ONEUNIT);
        break;
    case 2:
        MOTOR_moveBackward(ONEUNIT);
        break;
    case 3:
        MOTOR_spotTurn(MOTOR_TURN_CLOCKWISE, 90);
        (*direction)++;
        MOTOR_moveFoward(ONEUNIT);
        break;
    }

    return turn_operation;
}